#include "ppu.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "rom.h"

uint8_t ppudata[0x820];

uint8_t oamdata[0x100];

uint8_t ppuctrl;
uint8_t ppumask;
uint8_t oamaddr;
struct {
    uint8_t x;
    uint8_t y;
} ppuscroll;
uint16_t ppuaddr;

uint8_t ppuaddr_latch;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

color_t screen[0xf000];

color_t palette[] = {
    {.r=0x66, .g=0x66, .b=0x66},
    {.r=0x00, .g=0x2a, .b=0x88},
    {.r=0x14, .g=0x12, .b=0xa7},
    {.r=0x3b, .g=0x00, .b=0xa4},
    {.r=0x5c, .g=0x00, .b=0x7e},
    {.r=0x6e, .g=0x00, .b=0x40},
    {.r=0x6c, .g=0x06, .b=0x00},
    {.r=0x56, .g=0x1d, .b=0x00},
    {.r=0x33, .g=0x35, .b=0x00},
    {.r=0x0b, .g=0x48, .b=0x00},
    {.r=0x00, .g=0x52, .b=0x00},
    {.r=0x00, .g=0x4f, .b=0x08},
    {.r=0x00, .g=0x40, .b=0x4d},
    {.r=0x00, .g=0x00, .b=0x00},
    {.r=0x00, .g=0x00, .b=0x00},
    {.r=0x00, .g=0x00, .b=0x00},

    {.r=0xad, .g=0xad, .b=0xad},
    {.r=0x15, .g=0x5f, .b=0xd9},
    {.r=0x42, .g=0x40, .b=0xff},
    {.r=0x75, .g=0x27, .b=0xfe},
    {.r=0xa0, .g=0x1a, .b=0xcc},
    {.r=0xb7, .g=0x1e, .b=0x7b},
    {.r=0xb5, .g=0x31, .b=0x20},
    {.r=0x99, .g=0x4e, .b=0x00},
    {.r=0x6b, .g=0x6d, .b=0x00},
    {.r=0x38, .g=0x87, .b=0x00},
    {.r=0x0c, .g=0x93, .b=0x00},
    {.r=0x00, .g=0x8f, .b=0x32},
    {.r=0x00, .g=0x7c, .b=0x8d},
    {.r=0x00, .g=0x00, .b=0x00},
    {.r=0x00, .g=0x00, .b=0x00},
    {.r=0x00, .g=0x00, .b=0x00},

    {.r=0xff, .g=0xfe, .b=0xff},
    {.r=0x64, .g=0xb0, .b=0xff},
    {.r=0x92, .g=0x90, .b=0xff},
    {.r=0xc6, .g=0x76, .b=0xff},
    {.r=0xf3, .g=0x6a, .b=0xff},
    {.r=0xfe, .g=0x6e, .b=0xcc},
    {.r=0xfe, .g=0x81, .b=0x70},
    {.r=0xea, .g=0x9e, .b=0x22},
    {.r=0xbc, .g=0xbe, .b=0x00},
    {.r=0x88, .g=0xd8, .b=0x00},
    {.r=0x5c, .g=0xe4, .b=0x30},
    {.r=0x45, .g=0xe0, .b=0x82},
    {.r=0x48, .g=0xcd, .b=0xde},
    {.r=0x4f, .g=0x4f, .b=0x4f},
    {.r=0x00, .g=0x00, .b=0x00},
    {.r=0x00, .g=0x00, .b=0x00},

    {.r=0xff, .g=0xfe, .b=0xff},
    {.r=0xc0, .g=0xdf, .b=0xff},
    {.r=0xd3, .g=0xd2, .b=0xff},
    {.r=0xe8, .g=0xc8, .b=0xff},
    {.r=0xfb, .g=0xc2, .b=0xff},
    {.r=0xfe, .g=0xc4, .b=0xea},
    {.r=0xfe, .g=0xcc, .b=0xc5},
    {.r=0xf7, .g=0xd8, .b=0xa5},
    {.r=0xe4, .g=0xe5, .b=0x94},
    {.r=0xcf, .g=0xef, .b=0x96},
    {.r=0xbd, .g=0xf4, .b=0xab},
    {.r=0xb3, .g=0xf3, .b=0xcc},
    {.r=0xb5, .g=0xeb, .b=0xf2},
    {.r=0xb8, .g=0xb8, .b=0xb8},
    {.r=0x00, .g=0x00, .b=0x00},
    {.r=0x00, .g=0x00, .b=0x00}
};

void ppu_draw_sprite(uint16_t start, uint16_t end, uint8_t spr) {
    uint16_t sy_begin = oamdata[(spr<<2)]+1;
    uint16_t sy_end = sy_begin+8;
    if (sy_end > start && sy_begin < end) {
        uint8_t tile = oamdata[(spr<<2)|0x1];
        uint8_t pal_index = oamdata[(spr<<2)|0x2] & 0x3;
        uint16_t sx_begin = oamdata[(spr<<2)|0x3];
        uint16_t x_end = sx_begin <= 0xf8 ? 8 : 0x100 - sx_begin;
        uint16_t y_begin = sy_begin >= start ? 0 : start - sy_begin;
        uint16_t y_end = sy_end <= end ? 8 : end - sy_begin;
        sy_begin = sy_begin >= start ? sy_begin : start;
        
        for (uint16_t y = y_begin, sy = sy_begin; y < y_end; ++y, ++sy) {
            uint8_t gfx_low, gfx_high;
            if (oamdata[(spr<<2)|0x2]&0x80) {
                gfx_low = chr[(tile<<4)|((~y)&07)];
                gfx_high = chr[(tile<<4)|0x8|((~y)&0x7)];
            } else {
                gfx_low = chr[(tile<<4)|(y&0x7)];
                gfx_high = chr[(tile<<4)|0x8|(y&0x7)];
            }
            for (uint16_t x = 0, sx = sx_begin; x < x_end; ++x, ++sx) {
                uint8_t pal_color;
                if (oamdata[(spr<<2)|0x2]&0x40) {
                    pal_color = (((gfx_high>>(x&0x7))<<1)&0x2)|((gfx_low>>(x&0x7))&0x1);
                } else {
                    pal_color = (((gfx_high<<(x&0x7))>>6)&0x2)|(((gfx_low<<(x&0x7))>>7)&0x1);
                }
                if (pal_color) {
                    screen[(sy<<8)|sx] = palette[ppudata[0x810|(pal_index<<2)|pal_color]];
                }
            }
        }
    }
}

void ppu_draw(uint16_t end) {
    static uint16_t start = 0;
    if (end > 0xf0) {
        end = 0xf0;
    }

    // clear screen
    for (uint16_t i = (start << 8); i < (end << 8); ++i) {
        screen[i] = palette[ppudata[0x800]];
    }

    // draw sprites behind background
    if (ppumask & 0x10) {
        for (uint8_t spr = 0x3f; spr < 0x80; --spr) {
            if (oamdata[(spr<<2)|0x2]&0x20) {
                ppu_draw_sprite(start, end, spr);
            }
        }
    }

    // draw background
    if (ppumask & 0x08) {
        for (uint8_t nt = 0; nt < 2; ++nt) {
            for (uint16_t y = start; y < end; ++y) {
                uint16_t x_begin, x_end;
                if (nt == (ppuctrl & 0x1)) {
                    x_begin = ppuscroll.x;
                    x_end = 0x100;
                } else {
                    x_begin = 0;
                    x_end = ppuscroll.x;
                }

                uint8_t gfx_low;
                uint8_t gfx_high;
                uint8_t pal_index;
                for (uint16_t x = x_begin, sx = 0x100 - x_end; x < x_end; ++x, ++sx) {
                    if (!(x & 0x7) || !sx) {
                        if (!(x & 0xf) || !sx) {
                            pal_index = ppudata[(nt<<10)|0x3c0|((y>>2)&0x38)|(x>>5)];
                            pal_index >>= ((y>>2)&0x4)|((x>>3)&0x2);
                            pal_index &= 0x3;
                        }

                        uint8_t tile = ppudata[(nt<<10)|((y<<2)&0x3e0)|(x>>3)];
                        gfx_low = chr[0x1000|(tile<<4)|(y&0x7)];
                        gfx_high = chr[0x1000|(tile<<4)|0x8|(y&0x7)];
                    }

                    uint8_t pal_color = (((gfx_high<<(x&0x7))>>6)&0x2)|(((gfx_low<<(x&0x7))>>7)&0x1);
                    if (pal_color) {
                        screen[(y<<8)|sx] = palette[ppudata[0x800|(pal_index<<2)|pal_color]];
                    }
                }
            }
        }
    }

    // draw sprites in front of background
    if (ppumask & 0x10) {
        for (uint8_t spr = 0x3f; spr < 0x80; --spr) {
            if (!(oamdata[(spr<<2)|0x2]&0x20)) {
                ppu_draw_sprite(start, end, spr);
            }
        }
    }

    start = end < 0xf0 ? end : 0;
}

const uint8_t *ppu_screen(void) {
    return (const uint8_t *)screen;
}

uint8_t ppu_read(uint16_t address) {
    address &= 0x3fff;
    if (address < 0x2000) {
        return chr[address];
    } else if (address < 0x3f00) {
        address &= 0x7ff;
        return ppudata[address];
    } else {
        address &= 0x81f;
        if ((address & 0x10) && !(address & 0x3)) {
            address &= 0x80c;
        }
        return ppudata[address];
    }
}

void ppu_write(uint16_t address, uint8_t value) {
    address &= 0x3fff;
    if (address < 0x2000) {
        fprintf(stderr, "error: ppu write to $%x!\n", address);
    } else if (address < 0x3f00) {
        address &= 0x7ff;
        ppudata[address] = value;
    } else {
        address &= 0x81f;
        if ((address & 0x10) && !(address & 0x3)) {
            address &= 0x80c;
        }
        ppudata[address] = value;
    }
}

uint8_t ppu_read_ppustatus(void) {
    ppuaddr_latch = 0;
    // timing related: hard code values
    if (pc == 0x800d || pc == 0x8012) {
        // waiting for vblank
        return 0x80;
    } else if (pc == 0x80a9 || pc == 0x817b || pc == 0x8e1c || pc == 0x8ee0) {
        // value discarded
        return 0;
    } else if (pc == 0x8140) {
        // sprite 0 hit clear
        ppu_draw(0x1f);
        return 0;
    } else if (pc == 0x8153) {
        // sprite 0 hit set
        return 0x40;
    } else {
        // fprintf(stderr, "error: PPUSTATUS read unimplemented!\n");
        // fprintf(stderr, "pc = $%04x\n", pc);
        // exit(1);
    }
}

uint8_t ppu_read_oamdata(void) {
    return oamdata[oamaddr];
}

uint8_t ppu_read_ppudata(void) {
    static uint8_t next_value;

    uint8_t value;
    if ((ppuaddr & 0x3fff) < 0x3f00) {
        value = next_value;
        next_value = ppu_read(ppuaddr);
    } else {
        // palettes
        value = ppu_read(ppuaddr);
        next_value = ppu_read(ppuaddr & 0x2fff);
    }

    if (ppuctrl & 0x04) {
        ppuaddr += 0x20;
    } else {
        ppuaddr++;
    }
    return value;
}

void ppu_write_ppuctrl(uint8_t value) {
    // don't update nametable address if
    // screen is off (prevents a glitch)
    if (ppumask & 0x8) {
        ppuctrl = value;
    } else {
        ppuctrl = (value & 0xfc) | (ppuctrl & 0x3);
    }
}

void ppu_write_ppumask(uint8_t value) {
    ppumask = value;
}

void ppu_write_oamaddr(uint8_t value) {
    oamaddr = value;
}

void ppu_write_oamdata(uint8_t value) {
    oamdata[oamaddr] = value;
    ++oamaddr;
}

void ppu_write_ppuscroll(uint8_t value) {
    if (ppuaddr_latch == 0) {
        ppuscroll.x = value;
        ppuaddr_latch++;
    } else if (ppuaddr_latch == 1) {
        ppuscroll.y = value;
        ppuaddr_latch--;
    }
}

void ppu_write_ppuaddr(uint8_t value) {
    if (ppuaddr_latch == 0) {
        ppuaddr &= 0x00ff;
        ppuaddr |= value << 8;
        ppuaddr_latch++;
    } else if (ppuaddr_latch == 1) {
        ppuaddr &= 0xff00;
        ppuaddr |= value;
        ppuaddr_latch--;
    }
}

void ppu_write_ppudata(uint8_t value) {
    ppu_write(ppuaddr, value);
    if (ppuctrl & 0x04) {
        ppuaddr += 0x20;
    } else {
        ppuaddr++;
    }
}
