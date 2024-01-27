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

#ifdef _WIN32
# include <SDL.h>
#else
# include <SDL2/SDL.h>
#endif

#include "rom.h"

#include "apu.h"
#include "cpu.h"
#include "ppu.h"

#include "smb1.h"

smb1_input_t saved_inputs;

void input_callback(smb1_input_t *input) {
    *input = saved_inputs;
}

typedef struct {
    uint8_t*array;
    size_t head;
    size_t tail;
    size_t capacity;
    size_t size;
} queue_t;

void queue_init(queue_t *queue) {
    memset(queue, 0, sizeof(queue_t));
}

void queue_destroy(queue_t *queue) {
    if (queue->array) {
        free(queue->array);
    }
}

void queue_enqueue(queue_t *queue, const void *buf, size_t count) {
    if (count <= 0) {
        return;
    }

    if (queue->size + count > queue->capacity) {
        size_t newcapacity = queue->capacity ? queue->capacity : 4;
        while (queue->size + count > newcapacity) {
            newcapacity *= 2;
        }

        uint8_t *newarray = malloc(newcapacity);
        if (queue->size > 0) {
            if (queue->head < queue->tail) {
                memcpy(newarray, queue->array + queue->head, queue->size);
            }
            else {
                memcpy(newarray, queue->array + queue->head, queue->capacity - queue->head);
                memcpy(newarray + queue->capacity - queue->head, queue->array, queue->tail);
            }
        }

        if (queue->array) {
            free(queue->array);
        }
        queue->capacity = newcapacity;
        queue->array = newarray;
        queue->head = 0;
        queue->tail = (queue->size == newcapacity) ? 0 : queue->size;
    }

    size_t first_part = MIN(queue->capacity - queue->tail, count);
    memcpy(queue->array + queue->tail, buf, first_part);
    size_t second_part = count - first_part;
    if (second_part > 0) {
        memcpy(queue->array, (const uint8_t *)buf + first_part, second_part);
    }

    queue->tail = (queue->tail + count) % queue->capacity;
    queue->size += count;
}

void queue_dequeue(queue_t *queue, void *buf, size_t count) {
    count = MIN(count, queue->size);
    if (count <= 0) {
        return;
    }

    size_t first_part = MIN(queue->capacity - queue->head, count);
    memcpy(buf, queue->array + queue->head, first_part);
    size_t second_part = count - first_part;
    if (second_part > 0) {
        memcpy((uint8_t *)buf + first_part, queue->array, second_part);
    }

    queue->head = (queue->head + count) % queue->capacity;
    queue->size -= count;
}

void sdl_audio_callback(void *userdata, Uint8* stream, int len) {
    queue_t *audio_queue = (queue_t *)userdata;
    
    size_t count = MIN(len, audio_queue->size);
    queue_dequeue(audio_queue, stream, count);
    memset(stream + count, 0, len - count);
}

void poll_events(bool *should_quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                *should_quit = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_X:
                        saved_inputs.a = true;
                        break;
                    case SDL_SCANCODE_Z:
                        saved_inputs.b = true;
                        break;
                    case SDL_SCANCODE_RSHIFT:
                        saved_inputs.select = true;
                        break;
                    case SDL_SCANCODE_RETURN:
                        saved_inputs.start = true;
                        break;
                    case SDL_SCANCODE_UP:
                        saved_inputs.up = true;
                        break;
                    case SDL_SCANCODE_DOWN:
                        saved_inputs.down = true;
                        break;
                    case SDL_SCANCODE_LEFT:
                        saved_inputs.left = true;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        saved_inputs.right = true;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_X:
                        saved_inputs.a = false;
                        break;
                    case SDL_SCANCODE_Z:
                        saved_inputs.b = false;
                        break;
                    case SDL_SCANCODE_RSHIFT:
                        saved_inputs.select = false;
                        break;
                    case SDL_SCANCODE_RETURN:
                        saved_inputs.start = false;
                        break;
                    case SDL_SCANCODE_UP:
                        saved_inputs.up = false;
                        break;
                    case SDL_SCANCODE_DOWN:
                        saved_inputs.down = false;
                        break;
                    case SDL_SCANCODE_LEFT:
                        saved_inputs.left = false;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        saved_inputs.right = false;
                        break;
                    default:
                        break;
                }
                break;
        }
    }
}

int64_t gettime(void) {
#ifdef _WIN32
    static int64_t frequency;
    static bool init = false;
    if (!init) {
        LARGE_INTEGER lpFrequency;
        QueryPerformanceFrequency(&lpFrequency);
        frequency = lpFrequency.QuadPart;
    }

    LARGE_INTEGER lpPerformanceCount;
    QueryPerformanceCounter(&lpPerformanceCount);
    int64_t performance_count = lpPerformanceCount.QuadPart;
    return performance_count * 1000000000LL / frequency;
#else
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_sec * 1000000000LL + tp.tv_nsec;
#endif
}

void sleep(int64_t ns) {
#ifdef _WIN32
    Sleep((DWORD)(ns / 1000000LL));
#else
    struct timespec tp = {
        .tv_sec = ns / 1000000000LL,
        .tv_nsec = ns % 1000000000LL
    };
    while (nanosleep(&tp, &tp) == -1 && errno == EINTR) {}
#endif
}

int main(int argc, char **argv) {

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("smb1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 768, 720, 0);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff)) {
        fprintf(stderr, "SDL_SetRenderDrawColor: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 256, 240);
    if (!texture) {
        fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // audio
    queue_t audio_queue;
    queue_init(&audio_queue);

    SDL_AudioSpec out;
    SDL_AudioSpec spec;
    memset(&spec, 0, sizeof(spec));
    spec.freq = APU_SAMPLE_RATE;
    spec.format = AUDIO_S16SYS;
    spec.channels = 2;
    spec.samples = 1024;
    spec.callback = sdl_audio_callback;
    spec.userdata = &audio_queue;

    SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(NULL, false, &spec, &out, 0);
    if (!audio_device) {
        fprintf(stderr, "SDL_OpenAudioDevice: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_PauseAudioDevice(audio_device, false);

    smb1_set_input_callback(input_callback);
    smb1_init();

    bool should_quit = false;
    while (!should_quit) {

        int64_t start = gettime();

        smb1_run();

        SDL_LockAudioDevice(audio_device);
        queue_enqueue(&audio_queue, apu_samples, APU_SAMPLES_SIZE);
        SDL_UnlockAudioDevice(audio_device);

        uint8_t *pixels;
        int pitch;
        if (SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch)) {
            fprintf(stderr, "SDL_LockTexture: %s\n", SDL_GetError());
            SDL_Quit();
            return EXIT_FAILURE;
        }
        const uint8_t *screen = ppu_screen();
        for (size_t row = 0; row < 240; ++row) {
            for (size_t column = 0; column < 256; ++column) {
                pixels[row*pitch+column*4+2] = screen[(row*256+column)*3];
                pixels[row*pitch+column*4+1] = screen[(row*256+column)*3+1];
                pixels[row*pitch+column*4] = screen[(row*256+column)*3+2];
                pixels[row*pitch+column*4+3] = 0xff;
            }
        }
        SDL_UnlockTexture(texture);

        if (SDL_RenderClear(renderer)) {
            fprintf(stderr, "SDL_RenderClear: %s\n", SDL_GetError());
            SDL_Quit();
            return EXIT_FAILURE;
        }
        if (SDL_RenderCopy(renderer, texture, NULL, NULL)) {
            fprintf(stderr, "SDL_RenderCopy: %s\n", SDL_GetError());
            SDL_Quit();
            return EXIT_FAILURE;
        }

        SDL_RenderPresent(renderer);

        int64_t stop = gettime();

        int64_t time_remaining = (1048273600ULL / 63ULL) + start - stop;
        if (time_remaining > 0) {
            sleep(time_remaining);
        }

        poll_events(&should_quit);
    }

    SDL_CloseAudioDevice(audio_device);
    queue_destroy(&audio_queue);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
