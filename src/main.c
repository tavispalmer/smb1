#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _MSC_VER
# define MIN min
# define ssize_t long long
# include <windows.h>
#else
# include <pthread.h>
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

#ifdef _MSC_VER
#define INLINE __forceinline
#else
#define INLINE extern __inline__ __attribute__((__always_inline__,__gnu_inline__))
#endif

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

void queue_init(queue_t *queue, size_t capacity) {
    memset(queue, 0, sizeof(queue_t));
    queue->array = malloc(capacity);
    queue->capacity = capacity;
}

void queue_destroy(queue_t *queue) {
    free(queue->array);
}

ssize_t queue_enqueue(queue_t *queue, const void *buf, size_t count) {
    count = MIN(count, queue->capacity - queue->size);
    if (count == 0) {
        return 0;
    }

    size_t first_part = MIN(queue->capacity - queue->tail, count);
    memcpy(queue->array + queue->tail, buf, first_part);
    size_t second_part = count - first_part;
    if (second_part > 0) {
        memcpy(queue->array, (const uint8_t *)buf + first_part, second_part);
    }

    queue->tail = (queue->tail + count) % queue->capacity;
    queue->size += count;

    return count;
}

ssize_t queue_dequeue(queue_t *queue, void *buf, size_t count) {
    count = MIN(count, queue->size);
    if (count == 0) {
        return 0;
    }

    size_t first_part = MIN(queue->capacity - queue->head, count);
    memcpy(buf, queue->array + queue->head, first_part);
    size_t second_part = count - first_part;
    if (second_part > 0) {
        memcpy((uint8_t *)buf + first_part, queue->array, second_part);
    }

    queue->head = (queue->head + count) % queue->capacity;
    queue->size -= count;

    return count;
}

#ifndef _WIN32
typedef pthread_mutex_t mutex_t;
typedef pthread_cond_t cond_t;
#endif

INLINE void mutex_init(mutex_t *mutex) {
    pthread_mutex_init((pthread_mutex_t *)mutex, NULL);
}

INLINE void mutex_destroy(mutex_t *mutex) {
    pthread_mutex_destroy((pthread_mutex_t *)mutex);
}

INLINE void mutex_lock(mutex_t *mutex) {
    pthread_mutex_lock((pthread_mutex_t *)mutex);
}

INLINE void mutex_unlock(mutex_t *mutex) {
    pthread_mutex_unlock((pthread_mutex_t *)mutex);
}

INLINE void cond_init(cond_t *cond) {
    pthread_cond_init((pthread_cond_t *)cond, NULL);
}

INLINE void cond_destroy(cond_t *cond) {
    pthread_cond_destroy((pthread_cond_t *)cond);
}

INLINE void cond_signal(cond_t *cond) {
    pthread_cond_signal((pthread_cond_t *)cond);
}

INLINE void cond_wait(cond_t *cond, mutex_t *mutex) {
    pthread_cond_wait((pthread_cond_t *)cond, (pthread_mutex_t *)mutex);
}

typedef struct {
    queue_t queue;
    mutex_t mutex;
    cond_t cond;
} callback_data_t;

void sdl_audio_callback(void *userdata, Uint8* stream, int len) {
    callback_data_t *callback_data = (callback_data_t *)userdata;
    
    size_t count = MIN(len, callback_data->queue.size);
    queue_dequeue(&callback_data->queue, stream, count);
    cond_signal(&callback_data->cond);
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

INLINE int64_t gettime(void) {
#ifdef _WIN32
    static LARGE_INTEGER frequency;
    if (!frequency.QuadPart) {
        QueryPerformanceFrequency(&frequency);
    }
    LARGE_INTEGER performanceCount;
    QueryPerformanceCounter(&performanceCount);
    return performanceCount.QuadPart * 1000000000LL / frequency.QuadPart;
#else
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_sec * 1000000000LL + tp.tv_nsec;
#endif
}

void sleep(int64_t ns) {
    // code based on https://github.com/python/cpython
#ifdef _WIN32
    static DWORD flags = -1;
    if (flags == -1) {
        HANDLE timer = CreateWaitableTimerExW(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
        if (!timer) {
            flags = 0;
        } else {
            flags = CREATE_WAITABLE_TIMER_HIGH_RESOLUTION;
            CloseHandle(timer);
        }
    }
    LARGE_INTEGER dueTime = { .QuadPart = -(ns / 100LL) };
    HANDLE timer = CreateWaitableTimerExW(NULL, NULL, flags, TIMER_ALL_ACCESS);
    SetWaitableTimerEx(timer, &dueTime, 0, NULL, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
#else
    int64_t request_ns = gettime() + ns;
    struct timespec request = {
        .tv_sec = request_ns / 1000000000LL,
        .tv_nsec = request_ns % 1000000000LL
    };
    while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &request, NULL) == EINTR);
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
    callback_data_t callback_data;
    queue_init(&callback_data.queue, 16384 * sizeof(int16_t));
    mutex_init(&callback_data.mutex);
    cond_init(&callback_data.cond);

    uint8_t *tmp = calloc(16384 * sizeof(int16_t), 1);
    queue_enqueue(&callback_data.queue, tmp, 16384 * sizeof(int16_t));
    free(tmp);

    SDL_AudioSpec out;
    SDL_AudioSpec spec;
    memset(&spec, 0, sizeof(spec));
    spec.freq = APU_SAMPLE_RATE;
    spec.format = AUDIO_S16SYS;
    spec.channels = 2;
    spec.samples = 4096;
    spec.callback = sdl_audio_callback;
    spec.userdata = &callback_data;

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

        poll_events(&should_quit);

        smb1_run();

        size_t written = 0;
        while (written < APU_SAMPLES_SIZE) {
            SDL_LockAudioDevice(audio_device);
            if (callback_data.queue.capacity == callback_data.queue.size) {
                SDL_UnlockAudioDevice(audio_device);
                mutex_lock(&callback_data.mutex);
                cond_wait(&callback_data.cond, &callback_data.mutex);
                mutex_unlock(&callback_data.mutex);
            } else {
                size_t write_amt = queue_enqueue(&callback_data.queue, apu_samples + written, APU_SAMPLES_SIZE - written);
                SDL_UnlockAudioDevice(audio_device);
                written += write_amt;
            }
        }

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

        int64_t remaining = (1048273600ULL / 63ULL) + start - stop;
        if (remaining > 0) {
            sleep(remaining);
        }
    }

    SDL_CloseAudioDevice(audio_device);
    cond_destroy(&callback_data.cond);
    mutex_destroy(&callback_data.mutex);
    queue_destroy(&callback_data.queue);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
