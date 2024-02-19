#ifndef APU_H_
#define APU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

// ~60.0988
#define FPS_NUMER 39375000ULL
#define FPS_DENOM 655171ULL

#define APU_CHANNEL_COUNT 2
#define APU_SAMPLE_RATE 44100
#define APU_SAMPLES_SIZE ((APU_SAMPLE_RATE * FPS_DENOM / FPS_NUMER)*APU_CHANNEL_COUNT*sizeof(int16_t))
extern const int16_t *apu_samples;

void apu_init(void);
void apu_run(void);

uint8_t apu_read_snd_chn(void);

void apu_write_sq1_vol(uint8_t value);
void apu_write_sq1_sweep(uint8_t value);
void apu_write_sq1_lo(uint8_t value);
void apu_write_sq1_hi(uint8_t value);
void apu_write_sq2_vol(uint8_t value);
void apu_write_sq2_sweep(uint8_t value);
void apu_write_sq2_lo(uint8_t value);
void apu_write_sq2_hi(uint8_t value);
void apu_write_tri_linear(uint8_t value);
void apu_write_tri_lo(uint8_t value);
void apu_write_tri_hi(uint8_t value);
void apu_write_noise_vol(uint8_t value);
void apu_write_noise_lo(uint8_t value);
void apu_write_noise_hi(uint8_t value);
void apu_write_dmc_freq(uint8_t value);
void apu_write_dmc_raw(uint8_t value);
void apu_write_dmc_start(uint8_t value);
void apu_write_dmc_len(uint8_t value);
void apu_write_snd_chn(uint8_t value);
void apu_write_joy2(uint8_t value);

#ifdef __cplusplus
}
#endif

#endif
