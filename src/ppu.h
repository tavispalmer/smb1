#ifndef PPU_H_
#define PPU_H_

#include <stdint.h>

void ppu_draw(uint16_t y_end);
const uint8_t *ppu_screen(void);

uint8_t ppu_read_ppustatus(void);
uint8_t ppu_read_oamdata(void);
uint8_t ppu_read_ppudata(void);

void ppu_write_ppuctrl(uint8_t value);
void ppu_write_ppumask(uint8_t value);
void ppu_write_oamaddr(uint8_t value);
void ppu_write_oamdata(uint8_t value);
void ppu_write_ppuscroll(uint8_t value);
void ppu_write_ppuaddr(uint8_t value);
void ppu_write_ppudata(uint8_t value);

#endif
