#define _POSIX_C_SOURCE 199309L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _MSC_VER
# define MIN min
# define ssize_t long long
# include <windows.h>
#else
# include <sys/param.h>
# include <time.h>
#endif

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <AL/al.h>
#include <AL/alc.h>

#include "rom.h"

#include "apu.h"
#include "cpu.h"
#include "ppu.h"

#include "smb1.h"

smb1_input_t saved_inputs;

int16_t empty_buffer[APU_SAMPLES_SIZE/sizeof(int16_t)];

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_X:
                saved_inputs.a = true;
                break;
            case GLFW_KEY_Z:
                saved_inputs.b = true;
                break;
            case GLFW_KEY_RIGHT_SHIFT:
                saved_inputs.select = true;
                break;
            case GLFW_KEY_ENTER:
                saved_inputs.start = true;
                break;
            case GLFW_KEY_UP:
                saved_inputs.up = true;
                break;
            case GLFW_KEY_DOWN:
                saved_inputs.down = true;
                break;
            case GLFW_KEY_LEFT:
                saved_inputs.left = true;
                break;
            case GLFW_KEY_RIGHT:
                saved_inputs.right = true;
                break;
            default:
                break;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_X:
                saved_inputs.a = false;
                break;
            case GLFW_KEY_Z:
                saved_inputs.b = false;
                break;
            case GLFW_KEY_RIGHT_SHIFT:
                saved_inputs.select = false;
                break;
            case GLFW_KEY_ENTER:
                saved_inputs.start = false;
                break;
            case GLFW_KEY_UP:
                saved_inputs.up = false;
                break;
            case GLFW_KEY_DOWN:
                saved_inputs.down = false;
                break;
            case GLFW_KEY_LEFT:
                saved_inputs.left = false;
                break;
            case GLFW_KEY_RIGHT:
                saved_inputs.right = false;
                break;
            default:
                break;
        }
    }
}

void input_callback(smb1_input_t *input) {
    *input = saved_inputs;
}

/*
 * START OPENAL CODE
 * 
 * OpenAL code taken from RetroArch:
 * https://github.com/libretro/RetroArch/blob/master/audio/drivers/openal.c
*/

#define OPENAL_BUFSIZE 1024

typedef struct al {
    ALuint source;
    ALuint *buffers;
    ALuint *res_buf;
    ALCdevice *handle;
    ALCcontext *ctx;
    size_t res_ptr;
    ALsizei num_buffers;
    size_t tmpbuf_ptr;
    int rate;
    ALenum format;
    uint8_t tmpbuf[OPENAL_BUFSIZE];
    bool nonblock;
    bool is_paused;
} al_t;

void al_free(al_t *al) {
    if (!al)
        return;
    
    alSourceStop(al->source);
    alDeleteSources(1, &al->source);

    if (al->buffers)
        alDeleteBuffers(al->num_buffers, al->buffers);
    
    free(al->buffers);
    free(al->res_buf);
    alcMakeContextCurrent(NULL);

    if (al->ctx)
        alcDestroyContext(al->ctx);
    if (al->handle)
        alcCloseDevice(al->handle);
    free(al);
}

al_t *al_init(unsigned rate, unsigned latency) {
    al_t *al;

    al = (al_t*)calloc(1, sizeof(al_t));
    if (!al)
        return NULL;
    
    al->handle = alcOpenDevice(NULL);
    if (!al->handle)
        goto error;
    
    al->ctx = alcCreateContext(al->handle, NULL);
    if (!al->ctx)
        goto error;
    
    alcMakeContextCurrent(al->ctx);

    al->rate = rate;

    /* We already use one buffer for tmpbuf. */
    al->num_buffers = (latency * rate * 2 * sizeof(int16_t)) / (1000 * OPENAL_BUFSIZE) - 1;
    if (al->num_buffers < 2)
        al->num_buffers = 2;
    
    al->buffers = (ALuint*)calloc(al->num_buffers, sizeof(ALuint));
    al->res_buf = (ALuint*)calloc(al->num_buffers, sizeof(ALuint));
    if (!al->buffers || !al->res_buf)
        goto error;
    
    alGenSources(1, &al->source);
    alGenBuffers(al->num_buffers, al->buffers);

    memcpy(al->res_buf, al->buffers, al->num_buffers * sizeof(ALuint));
    al->res_ptr = al->num_buffers;

    return al;

error:
    al_free(al);
    return NULL;
}

bool al_unqueue_buffers(al_t *al) {
    ALint val;

    alGetSourcei(al->source, AL_BUFFERS_PROCESSED, &val);

    if (val <= 0)
        return false;
    
    alSourceUnqueueBuffers(al->source, val, &al->res_buf[al->res_ptr]);
    al->res_ptr += val;
    return true;
}

bool al_get_buffer(al_t *al, ALuint *buffer) {
    if (!al->res_ptr) {
        for (;;) {
            if (al_unqueue_buffers(al))
                break;
            
            if (al->nonblock)
                return false;
            
            /* Must sleep as there is no proper blocking method. */
#ifdef _WIN32
            Sleep(1);
#else
            struct timespec time = {
                .tv_sec = 0L,
                .tv_nsec = 1000000L // 1ms
            };
            while (nanosleep(&time, &time) == -1 && errno == EINTR) {}
#endif
        }
    }

    *buffer = al->res_buf[--al->res_ptr];
    return true;
}

size_t al_fill_internal_buf(al_t *al, const void *buf, size_t size) {
    size_t read_size = MIN(OPENAL_BUFSIZE - al->tmpbuf_ptr, size);
    memcpy(al->tmpbuf + al->tmpbuf_ptr, buf, read_size);
    al->tmpbuf_ptr += read_size;
    return read_size;
}

ssize_t al_write(al_t *al, const uint8_t *buf, size_t size) {
    size_t written = 0;

    while (size) {
        ALint val;
        ALuint buffer;
        size_t rc = al_fill_internal_buf(al, buf, size);

        written += rc;
        buf += rc;
        size -= rc;

        if (al->tmpbuf_ptr != OPENAL_BUFSIZE)
            break;
        
        if (!al_get_buffer(al, &buffer))
            break;
        
        alBufferData(buffer, AL_FORMAT_STEREO16, al->tmpbuf, OPENAL_BUFSIZE, al->rate);
        al->tmpbuf_ptr = 0;
        alSourceQueueBuffers(al->source, 1, &buffer);
        if (alGetError() != AL_NO_ERROR)
            return -1;
        
        alGetSourcei(al->source, AL_SOURCE_STATE, &val);
        if (val != AL_PLAYING)
            alSourcePlay(al->source);
        
        if (alGetError() != AL_NO_ERROR)
            return -1;
    }

    return written;
}

void al_set_nonblock_state(al_t *al, bool state) {
    if (al)
        al->nonblock = state;
}

/*
 * END OPENAL CODE
*/

int main(int argc, char **argv) {

    GLFWwindow *window;

    if (!glfwInit())
        return -1;
    
    window = glfwCreateWindow(1536, 1440, "smb1", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    gladLoadGL((GLADloadfunc) glfwGetProcAddress);

    glfwSwapInterval(1);

    // init screen

    // shader
    const char *vertex_code =
            "#version 330 core\n"
            "layout (location = 0) in vec2 a_pos;\n"
            "layout (location = 1) in vec2 a_tex_coord;\n"
            "\n"
            "out vec2 tex_coord;\n"
            "\n"
            "void main() {\n"
            "    gl_Position = vec4(a_pos, 0.0, 1.0);\n"
            "    tex_coord = a_tex_coord;\n"
            "}\n";
    const char *fragment_code =
            "#version 330 core\n"
            "out vec4 frag_color;\n"
            "\n"
            "in vec2 tex_coord;\n"
            "\n"
            "uniform sampler2D tex;\n"
            "\n"
            "void main() {\n"
            "    frag_color = texture(tex, tex_coord);\n"
            "}\n";
    
    GLuint shader, vertex, fragment;
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_code, NULL);
    glCompileShader(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_code, NULL);
    glCompileShader(fragment);

    shader = glCreateProgram();
    glAttachShader(shader, vertex);
    glAttachShader(shader, fragment);
    glLinkProgram(shader);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // vertices
    float vertices[] = {
        -1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    GLuint vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 240, 0, GL_RGB, GL_UNSIGNED_BYTE, ppu_screen());
    
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "tex"), 0);

    // audio
    al_t *al = al_init(APU_SAMPLE_RATE, 64);
    al_set_nonblock_state(al, true);

    smb1_set_input_callback(input_callback);
    smb1_init();

    while (!glfwWindowShouldClose(window)) {

        // struct timespec start;
        // clock_gettime(CLOCK_MONOTONIC, &start);
        // uint64_t start_nanos = (uint64_t)start.tv_sec * 1000000000LL + (uint64_t)start.tv_nsec;

        smb1_run();

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 240, GL_RGB, GL_UNSIGNED_BYTE, ppu_screen());

        glUseProgram(shader);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        al_write(al, (const uint8_t *)apu_samples, APU_SAMPLES_SIZE);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // struct timespec stop;
        // clock_gettime(CLOCK_MONOTONIC, &stop);
        // uint64_t stop_nanos = (uint64_t)stop.tv_sec * 1000000000LL + (uint64_t)stop.tv_nsec;

        // uint64_t time_remaining = (1000000000ULL / 60ULL) + start_nanos - stop_nanos;
        // //fprintf(stderr, "%llu\n", time_remaining);
        // if (time_remaining > 0) {
        //     struct timespec remaining = {
        //         .tv_sec = time_remaining / 1000000000ULL,
        //         .tv_nsec = time_remaining % 1000000000ULL
        //     };
        //     while (nanosleep(&remaining, &remaining) == -1 && errno == EINTR) {}
        // }
    }

    al_free(al);

    glfwTerminate();
}
