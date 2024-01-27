#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>

#include "smb1.h"
#include "apu.h"
#include "rom.h"
#include "ppu.h"

#include <stdbool.h>

uint16_t pc;
uint8_t a;
uint8_t x;
uint8_t y;
uint8_t sr;
uint8_t sp;

bool interactive;

bool joy_strobe;
uint8_t joy1_index;
uint8_t joy2_index;

void cpu_print_registers(void) {
    fprintf(stderr, "pc: $%04x, a: $%02x, x: $%02x, y: $%02x, sr: %%%x%x%x%x%x%x%x%x, sp: $%02x\n",
        pc,
        a,
        x,
        y,
        (sr & SR_NEGATIVE) >> 7,
        (sr & SR_OVERFLOW) >> 6,
        (sr & 0x20) >> 5,
        (sr & SR_BREAK) >> 4,
        (sr & SR_DECIMAL) >> 3,
        (sr & SR_INTERRUPT) >> 2,
        (sr & SR_ZERO) >> 1,
        sr & SR_CARRY,
        sp
    );
}

#define PPUCTRL     ((uint16_t)0x2000)
#define PPUMASK     ((uint16_t)0x2001)
#define PPUSTATUS   ((uint16_t)0x2002)
#define OAMADDR     ((uint16_t)0x2003)
#define OAMDATA     ((uint16_t)0x2004)
#define PPUSCROLL   ((uint16_t)0x2005)
#define PPUADDR     ((uint16_t)0x2006)
#define PPUDATA     ((uint16_t)0x2007)

#define SQ1_VOL     ((uint16_t)0x4000)
#define SQ1_SWEEP   ((uint16_t)0x4001)
#define SQ1_LO      ((uint16_t)0x4002)
#define SQ1_HI      ((uint16_t)0x4003)
#define SQ2_VOL     ((uint16_t)0x4004)
#define SQ2_SWEEP   ((uint16_t)0x4005)
#define SQ2_LO      ((uint16_t)0x4006)
#define SQ2_HI      ((uint16_t)0x4007)
#define TRI_LINEAR  ((uint16_t)0x4008)
#define TRI_LO      ((uint16_t)0x400a)
#define TRI_HI      ((uint16_t)0x400b)
#define NOISE_VOL   ((uint16_t)0x400c)
#define NOISE_LO    ((uint16_t)0x400e)
#define NOISE_HI    ((uint16_t)0x400f)
#define DMC_FREQ    ((uint16_t)0x4010)
#define DMC_RAW     ((uint16_t)0x4011)
#define DMC_START   ((uint16_t)0x4012)
#define DMC_LEN     ((uint16_t)0x4013)
#define OAMDMA      ((uint16_t)0x4014)
#define SND_CHN     ((uint16_t)0x4015)
#define JOY1        ((uint16_t)0x4016)
#define JOY2        ((uint16_t)0x4017)

uint8_t cpu_read(uint16_t address) {
    if (interactive && address < 0x8000) {
        fprintf(stderr, "read from $%04x\n", address);
    }

    if (address < 0x2000) {
        return memory[address & 0x7ff];
    } else if (address < 0x4000) {
        // PPU
        address &= 0x2007;
        switch (address) {
            case PPUSTATUS: {
                return ppu_read_ppustatus();
            } case OAMDATA: {
                return ppu_read_oamdata();
            } case PPUDATA: {
                return ppu_read_ppudata();
            } default: {
                fprintf(stderr, "read from $%x!\n", address);
                exit(1);
            }
        }
    } else if (address < 0x4020) {
        // APU/IO
        switch (address) {
            case SND_CHN: {
                return apu_read_snd_chn();
            } case JOY1: {
                return cpu_read_joy1();
            } case JOY2: {
                return cpu_read_joy2();
            } default: {
                fprintf(stderr, "read from $%x!\n", address);
                exit(1);
            }
        }
    } else if (address < 0x8000) {
        // unused
        fprintf(stderr, "read from $%x!\n", address);
        exit(1);
    } else {
        // ROM
        return prg[address & 0x7fff];
    }
}

uint16_t cpu_read16(uint16_t address) {
    return cpu_read(address) | (cpu_read(address+1) << 8);
}

void cpu_write(uint16_t address, uint8_t value) {
    if (interactive && address < 0x8000) {
        fprintf(stderr, "wrote #$%02x to $%04x\n", value, address);
    }

    if (address < 0x2000) {
        memory[address & 0x7ff] = value;
    } else if (address < 0x4000) {
        // PPU
        address &= 0x2007;
        switch (address) {
            case PPUCTRL: {
                ppu_write_ppuctrl(value);
                break;
            } case PPUMASK: {
                ppu_write_ppumask(value);
                break;
            } case OAMADDR: {
                ppu_write_oamaddr(value);
                break;
            } case OAMDATA: {
                ppu_write_oamdata(value);
                break;
            } case PPUSCROLL: {
                ppu_write_ppuscroll(value);
                break;
            } case PPUADDR: {
                ppu_write_ppuaddr(value);
                break;
            } case PPUDATA: {
                ppu_write_ppudata(value);
                break;
            } default: {
                fprintf(stderr, "write to $%x!\n", address);
                break;
            }
        }
    } else if (address < 0x4020) {
        // APU/IO
        switch (address) {
            case SQ1_VOL: {
                apu_write_sq1_vol(value);
                break;
            } case SQ1_SWEEP: {
                apu_write_sq1_sweep(value);
                break;
            } case SQ1_LO: {
                apu_write_sq1_lo(value);
                break;
            } case SQ1_HI: {
                apu_write_sq1_hi(value);
                break;
            } case SQ2_VOL: {
                apu_write_sq2_vol(value);
                break;
            } case SQ2_SWEEP: {
                apu_write_sq2_sweep(value);
                break;
            } case SQ2_LO: {
                apu_write_sq2_lo(value);
                break;
            } case SQ2_HI: {
                apu_write_sq2_hi(value);
                break;
            } case TRI_LINEAR: {
                apu_write_tri_linear(value);
                break;
            } case TRI_LO: {
                apu_write_tri_lo(value);
                break;
            } case TRI_HI: {
                apu_write_tri_hi(value);
                break;
            } case NOISE_VOL: {
                apu_write_noise_vol(value);
                break;
            } case NOISE_LO: {
                apu_write_noise_lo(value);
                break;
            } case NOISE_HI: {
                apu_write_noise_hi(value);
                break;
            } case DMC_FREQ: {
                apu_write_dmc_freq(value);
                break;
            } case DMC_RAW: {
                apu_write_dmc_raw(value);
                break;
            } case DMC_START: {
                apu_write_dmc_start(value);
                break;
            } case DMC_LEN: {
                apu_write_dmc_len(value);
                break;
            } case OAMDMA: {
                cpu_write_oamdma(value);
                break;
            } case SND_CHN: {
                apu_write_snd_chn(value);
                break;
            } case JOY1: {
                cpu_write_joy1(value);
                break;
            } case JOY2: {
                apu_write_joy2(value);
                break;
            } default: {
                // fprintf(stderr, "write to $%x!\n", address);
                break;
            }
        }
    } else {
        // cartridge
        fprintf(stderr, "write to $%x!\n", address);
    }
}

void _default_input_callback(smb1_input_t *input) {
    input->a = false;
    input->b = false;
    input->select = false;
    input->start = false;
    input->up = false;
    input->down = false;
    input->left = false;
    input->right = false;
}

smb1_input_callback_t cpu_input_callback = _default_input_callback;
smb1_input_t input;

uint8_t cpu_read_joy1(void) {
    if (joy_strobe) {
        cpu_input_callback(&input);
        return 0x40 | (input.a ? 1 : 0);
    } else {
        uint8_t value = 0x40;
        switch (joy1_index) {
            case 0:
                value |= (input.a ? 1 : 0);
                break;
            case 1:
                value |= (input.b ? 1 : 0);
                break;
            case 2:
                value |= (input.select ? 1 : 0);
                break;
            case 3:
                value |= (input.start ? 1 : 0);
                break;
            case 4:
                value |= (input.up && !input.down ? 1 : 0);
                break;
            case 5:
                value |= (input.down && !input.up ? 1 : 0);
                break;
            case 6:
                value |= (input.left && !input.right ? 1 : 0);
                break;
            case 7:
                value |= (input.right && !input.left ? 1 : 0);
                break;
            default:
                value |= 1;
                break;
        }
        ++joy1_index;
        return value;
    }
}

uint8_t cpu_read_joy2(void) {
    if (joy_strobe) {
        return 0;
    } else {
        if (joy2_index < 8) {
            ++joy2_index;
            return 0;
        } else {
            return 1;
        }
    }
}

void cpu_write_oamdma(uint8_t value) {
    uint16_t base_addr = value << 8;
    for (int i = 0; i < 0x100; ++i) {
        ppu_write_oamdata(cpu_read(base_addr | i));
    }
}

void cpu_write_joy1(uint8_t value) {
    if (value & 1) {
        joy_strobe = true;
    } else {
        joy_strobe = false;
        joy1_index = 0;
        joy2_index = 0;
        cpu_input_callback(&input);
    }
}

void cpu_adc(uint8_t value) {
    uint16_t result = a + value + (sr & 0x01);

    // flags
    if (result & 0x100) {
        sr |= SR_CARRY;
    } else {
        sr &= ~SR_CARRY;
    }

    if (!(result & 0xff)) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (!((a & 0x80) ^ (value & 0x80))
        && ((a & 0x80) ^ (result & 0x80))) {
        sr |= SR_OVERFLOW;
    } else {
        sr &= ~SR_OVERFLOW;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    a = result;
}

void cpu_and(uint8_t value) {
    uint8_t result = a & value;

    // flags
    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    a = result;
}

uint8_t cpu_asl(uint8_t value) {
    uint8_t result = value << 1;

    // flags
    if (value & 0x80) {
        sr |= SR_CARRY;
    } else {
        sr &= ~SR_CARRY;
    }

    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    return result;
}

void cpu_bcc(uint8_t value) {
    if (!(sr & SR_CARRY)) {
        pc += (int8_t)value;
    }
}

void cpu_bcs(uint8_t value) {
    if (sr & SR_CARRY) {
        pc += (int8_t)value;
    }
}

void cpu_beq(uint8_t value) {
    if (sr & SR_ZERO) {
        pc += (int8_t)value;
    }
}

void cpu_bit(uint8_t value) {
    // flags
    if (!(a & value)) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (value & 0x40) {
        sr |= SR_OVERFLOW;
    } else {
        sr &= ~SR_OVERFLOW;
    }

    if (value & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }
}

void cpu_bmi(uint8_t value) {
    if (sr & SR_NEGATIVE) {
        pc += (int8_t)value;
    }
}

void cpu_bne(uint8_t value) {
    if (!(sr & SR_ZERO)) {
        pc += (int8_t)value;
    }
}

void cpu_bpl(uint8_t value) {
    if (!(sr & SR_NEGATIVE)) {
        pc += (int8_t)value;
    }
}

void cpu_brk(void) {
    // unimplemented
    fprintf(stderr, "unimplemented: brk\n");
    fprintf(stderr, "pc = $%04x\n", pc);
    exit(1);
}

void cpu_bvc(uint8_t value) {
    if (!(sr & SR_OVERFLOW)) {
        pc += (int8_t)value;
    }
}

void cpu_bvs(uint8_t value) {
    if (sr & SR_OVERFLOW) {
        pc += (int8_t)value;
    }
}

void cpu_clc(void) {
    sr &= ~SR_CARRY;
}

void cpu_cld(void) {
    sr &= ~SR_DECIMAL;
}

void cpu_cli(void) {
    sr &= ~SR_INTERRUPT;
}

void cpu_clv(void) {
    sr &= ~SR_OVERFLOW;
}

void cpu_cmp(uint8_t value) {
    uint16_t result = a + ((~value) & 0xff) + 1;

    // flags
    if (result & 0x100) {
        sr |= SR_CARRY;
    } else {
        sr &= ~SR_CARRY;
    }

    if (!(result & 0xff)) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }
}

void cpu_cpx(uint8_t value) {
    uint16_t result = x + ((~value) & 0xff) + 1;

    // flags
    if (result & 0x100) {
        sr |= SR_CARRY;
    } else {
        sr &= ~SR_CARRY;
    }

    if (!(result & 0xff)) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }
}

void cpu_cpy(uint8_t value) {
    uint16_t result = y + ((~value) & 0xff) + 1;

    // flags
    if (result & 0x100) {
        sr |= SR_CARRY;
    } else {
        sr &= ~SR_CARRY;
    }

    if (!(result & 0xff)) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }
}

uint8_t cpu_dec(uint8_t value) {
    uint8_t result = value - 1;

    // flags
    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    return result;
}

void cpu_dex(void) {
    uint8_t result = x - 1;

    // flags
    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    x = result;
}

void cpu_dey(void) {
    uint8_t result = y - 1;

    // flags
    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    y = result;
}

void cpu_eor(uint8_t value) {
    uint8_t result = a ^ value;

    // flags
    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    a = result;
}

uint8_t cpu_inc(uint8_t value) {
    uint8_t result = value + 1;

    // flags
    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    return result;
}

void cpu_inx(void) {
    uint8_t result = x + 1;

    // flags
    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    x = result;
}

void cpu_iny(void) {
    uint8_t result = y + 1;

    // flags
    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    y = result;
}

void cpu_jmp(uint16_t address) {
    pc = address;
}

void cpu_jsr(uint16_t address) {
    --pc;
    cpu_write(0x100 | sp--, (uint8_t)(pc >> 8));
    cpu_write(0x100 | sp--, (uint8_t)(pc));
    pc = address;
}

void cpu_lda(uint8_t value) {
    // flags
    if (!value) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (value & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    a = value;
}

void cpu_ldx(uint8_t value) {
    // flags
    if (!value) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (value & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    x = value;
}

void cpu_ldy(uint8_t value) {
    // flags
    if (!value) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (value & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    y = value;
}

uint8_t cpu_lsr(uint8_t value) {
    uint8_t result = value >> 1;

    // flags
    if (value & 0x01) {
        sr |= SR_CARRY;
    } else {
        sr &= ~SR_CARRY;
    }

    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    sr &= ~SR_NEGATIVE;

    return result;
}

void cpu_nop(void) {
}

void cpu_ora(uint8_t value) {
    uint8_t result = a | value;

    // flags
    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    a = result;
}

void cpu_pha(void) {
    cpu_write(0x100 | sp--, a);
}

void cpu_php(void) {
    cpu_write(0x100 | sp--, sr | SR_BREAK | 0x20);
}

void cpu_pla(void) {
    uint8_t result = cpu_read(0x100 | ++sp);

    // flags
    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    a = result;
}

void cpu_plp(void) {
    sr = (cpu_read(0x100 | ++sp) & ~(SR_BREAK | 0x20)) | (sr & (SR_BREAK | 0x20));
}

uint8_t cpu_rol(uint8_t value) {
    uint8_t result = (value << 1) | (sr & 0x01);

    // flags
    if (value & 0x80) {
        sr |= SR_CARRY;
    } else {
        sr &= ~SR_CARRY;
    }

    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    return result;
}

uint8_t cpu_ror(uint8_t value) {
    uint8_t result = (value >> 1) | (sr << 7);

    // flags
    if (value & 0x01) {
        sr |= SR_CARRY;
    } else {
        sr &= ~SR_CARRY;
    }

    if (!result) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    return result;
}

void cpu_rti(void) {
    // unimplemented
    fprintf(stderr, "unimplemented: rti\n");
    exit(1);
}

void cpu_rts(void) {
    pc = cpu_read(0x100 | ++sp);
    pc |= (cpu_read(0x100 | ++sp) << 8);
    ++pc;
}

void cpu_sbc(uint8_t value) {
    uint16_t result = a + ((~value) & 0xff) + (sr & 0x01);

    // flags
    if (result & 0x100) {
        sr |= SR_CARRY;
    } else {
        sr &= ~SR_CARRY;
    }

    if (!(result & 0xff)) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (!((a & 0x80) ^ ((~value) & 0x80))
        && ((a & 0x80) ^ (result & 0x80))) {
        sr |= SR_OVERFLOW;
    } else {
        sr &= ~SR_OVERFLOW;
    }

    if (result & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    a = result;
}

void cpu_sec(void) {
    sr |= SR_CARRY;
}

void cpu_sed(void) {
    sr |= SR_DECIMAL;
}

void cpu_sei(void) {
    sr |= SR_INTERRUPT;
}

uint8_t cpu_sta(void) {
    return a;
}

uint8_t cpu_stx(void) {
    return x;
}

uint8_t cpu_sty(void) {
    return y;
}

void cpu_tax(void) {
    // flags
    if (!a) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (a & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    x = a;
}

void cpu_tay(void) {
    // flags
    if (!a) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (a & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    y = a;
}

void cpu_tsx(void) {
    // flags
    if (!sp) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (sp & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    x = sp;
}

void cpu_txa(void) {
    // flags
    if (!x) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (x & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    a = x;
}

void cpu_txs(void) {
    // flags
    if (!x) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (x & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    sp = x;
}

void cpu_tya(void) {
    // flags
    if (!y) {
        sr |= SR_ZERO;
    } else {
        sr &= ~SR_ZERO;
    }

    if (y & 0x80) {
        sr |= SR_NEGATIVE;
    } else {
        sr &= ~SR_NEGATIVE;
    }

    a = y;
}

void cpu_print_instruction(void) {
    uint8_t instruction = cpu_read(pc);

    uint8_t group = instruction & 0x3;
    uint8_t addr_mode = (instruction >> 2) & 0x7;
    uint8_t opcode = instruction >> 5;

    if (group == 1) {
        if (opcode == 0) {
            fprintf(stderr, "ora ");
        } else if (opcode == 1) {
            fprintf(stderr, "and ");
        } else if (opcode == 2) {
            fprintf(stderr, "eor ");
        } else if (opcode == 3) {
            fprintf(stderr, "adc ");
        } else if (opcode == 4) {
            if (addr_mode != 2) {
                fprintf(stderr, "sta ");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else if (opcode == 5) {
            fprintf(stderr, "lda ");
        } else if (opcode == 6) {
            fprintf(stderr, "cmp ");
        } else {
            fprintf(stderr, "sbc ");
        }

        if (addr_mode == 0) {
            uint8_t operand = cpu_read(pc + 1);
            fprintf(stderr, "($%02x,x)\n", operand);
        } else if (addr_mode == 1) {
            uint8_t operand = cpu_read(pc + 1);
            fprintf(stderr, "$%02x\n", operand);
        } else if (addr_mode == 2) {
            if (opcode != 4) {
                uint8_t operand = cpu_read(pc + 1);
                fprintf(stderr, "#$%02x\n", operand);
            }
        } else if (addr_mode == 3) {
            uint16_t operand = cpu_read16(pc + 1);
            fprintf(stderr, "$%04x\n", operand);
        } else if (addr_mode == 4) {
            uint8_t operand = cpu_read(pc + 1);
            fprintf(stderr, "($%02x),y\n", operand);
        } else if (addr_mode == 5) {
            uint8_t operand = cpu_read(pc + 1);
            fprintf(stderr, "$%02x,x\n", operand);
        } else if (addr_mode == 6) {
            uint16_t operand = cpu_read16(pc + 1);
            fprintf(stderr, "$%04x,y\n", operand);
        } else {
            uint16_t operand = cpu_read16(pc + 1);
            fprintf(stderr, "$%04x,x\n", operand);
        }
    } else if (group == 2) {
        if (opcode == 0) {
            if (addr_mode != 0 && addr_mode != 4 && addr_mode != 6) {
                fprintf(stderr, "asl ");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else if (opcode == 1) {
            if (addr_mode != 0 && addr_mode != 4 && addr_mode != 6) {
                fprintf(stderr, "rol ");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else if (opcode == 2) {
            if (addr_mode != 0 && addr_mode != 4 && addr_mode != 6) {
                fprintf(stderr, "lsr ");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else if (opcode == 3) {
            if (addr_mode != 0 && addr_mode != 4 && addr_mode != 6) {
                fprintf(stderr, "ror ");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else if (opcode == 4) {
            if (addr_mode == 2) {
                fprintf(stderr, "txa\n");
            } else if (addr_mode == 6) {
                fprintf(stderr, "txs\n");
            } else if (addr_mode != 0 && addr_mode != 4 && addr_mode != 7) {
                fprintf(stderr, "stx ");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else if (opcode == 5) {
            if (addr_mode == 2) {
                fprintf(stderr, "tax\n");
            } else if (addr_mode == 6) {
                fprintf(stderr, "tsx\n");
            } else if (addr_mode != 4) {
                fprintf(stderr, "ldx ");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else if (opcode == 6) {
            if (addr_mode == 2) {
                fprintf(stderr, "dex\n");
            } else if (addr_mode != 0 && addr_mode != 4 && addr_mode != 6) {
                fprintf(stderr, "dec ");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else {
            if (addr_mode == 2) {
                fprintf(stderr, "nop\n");
            } else if (addr_mode != 0 && addr_mode != 4 && addr_mode != 6) {
                fprintf(stderr, "inc ");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        }

        if (addr_mode == 0) {
            if (opcode == 5) {
                uint8_t operand = cpu_read(pc + 1);
                fprintf(stderr, "#$%02x\n", operand);
            }
        } else if (addr_mode == 1) {
            uint8_t operand = cpu_read(pc + 1);
            fprintf(stderr, "$%02x\n", operand);
        } else if (addr_mode == 2) {
            if (opcode == 0 || opcode == 1 || opcode == 2 || opcode == 3) {
                fprintf(stderr, "a\n");
            }
        } else if (addr_mode == 3) {
            uint16_t operand = cpu_read16(pc + 1);
            fprintf(stderr, "$%04x\n", operand);
        } else if (addr_mode == 5) {
            uint8_t operand = cpu_read(pc + 1);
            if (opcode == 4 || opcode == 5) {
                fprintf(stderr, "$%02x,y\n", operand);
            } else {
                fprintf(stderr, "$%02x,x\n", operand);
            }
        } else {
            if (opcode == 5) {
                uint16_t operand = cpu_read16(pc + 1);
                fprintf(stderr, "$%04x,y\n", operand);
            } else if (opcode != 4) {
                uint16_t operand = cpu_read16(pc + 1);
                fprintf(stderr, "$%04x,x\n", operand);
            }
        }
    }  else if (group == 0) {
        if (opcode == 0) {
            if (addr_mode == 0) {
                fprintf(stderr, "brk\n");
            } else if (addr_mode == 2) {
                fprintf(stderr, "php\n");
            } else if (addr_mode == 4) {
                fprintf(stderr, "bpl ");
            } else if (addr_mode == 6) {
                fprintf(stderr, "clc\n");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else if (opcode == 1) {
            if (addr_mode == 0) {
                fprintf(stderr, "jsr ");
            } else if (addr_mode == 2) {
                fprintf(stderr, "plp\n");
            } else if (addr_mode == 4) {
                fprintf(stderr, "bmi ");
            } else if (addr_mode == 6) {
                fprintf(stderr, "sec\n");
            } else if (addr_mode == 1 || addr_mode == 3) {
                fprintf(stderr, "bit ");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else if (opcode == 2) {
            if (addr_mode == 0) {
                fprintf(stderr, "rti\n");
            } else if (addr_mode == 2) {
                fprintf(stderr, "pha\n");
            } else if (addr_mode == 3) {
                fprintf(stderr, "jmp ");
            } else if (addr_mode == 4) {
                fprintf(stderr, "bvc ");
            } else if (addr_mode == 6) {
                fprintf(stderr, "cli\n");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else if (opcode == 3) {
            if (addr_mode == 0) {
                fprintf(stderr, "rts\n");
            } else if (addr_mode == 2) {
                fprintf(stderr, "pla\n");
            } else if (addr_mode == 3) {
                fprintf(stderr, "jmp ");
            } else if (addr_mode == 4) {
                fprintf(stderr, "bvs ");
            } else if (addr_mode == 6) {
                fprintf(stderr, "sei\n");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else if (opcode == 4) {
            if (addr_mode == 2) {
                fprintf(stderr, "dey\n");
            } else if (addr_mode == 4) {
                fprintf(stderr, "bcc ");
            } else if (addr_mode == 6) {
                fprintf(stderr, "tya\n");
            } else if (addr_mode != 0 && addr_mode != 7) {
                fprintf(stderr, "sty ");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else if (opcode == 5) {
            if (addr_mode == 2) {
                fprintf(stderr, "tay\n");
            } else if (addr_mode == 4) {
                fprintf(stderr, "bcs ");
            } else if (addr_mode == 6) {
                fprintf(stderr, "clv\n");
            } else {
                fprintf(stderr, "ldy ");
            }
        } else if (opcode == 6) {
            if (addr_mode == 2) {
                fprintf(stderr, "iny\n");
            } else if (addr_mode == 4) {
                fprintf(stderr, "bne ");
            } else if (addr_mode == 6) {
                fprintf(stderr, "cld\n");
            } else if (addr_mode != 5 && addr_mode != 7) {
                fprintf(stderr, "cpy ");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        } else {
            if (addr_mode == 2) {
                fprintf(stderr, "inx\n");
            } else if (addr_mode == 4) {
                fprintf(stderr, "beq ");
            } else if (addr_mode == 6) {
                fprintf(stderr, "sed\n");
            } else if (addr_mode != 5 && addr_mode != 7) {
                fprintf(stderr, "cpx ");
            } else {
                fprintf(stderr, "illegal instruction\n");
                return;
            }
        }

        if (addr_mode == 0) {
            if (opcode == 1) {
                uint16_t operand = cpu_read16(pc + 1);
                fprintf(stderr, "$%04x\n", operand);
            } else if (opcode == 5 || opcode == 6 || opcode == 7) {
                uint8_t operand = cpu_read(pc + 1);
                fprintf(stderr, "#$%02x\n", operand);
            }
        } else if (addr_mode == 1) {
            if (opcode != 0 && opcode != 2 && opcode != 3) {
                uint8_t operand = cpu_read(pc + 1);
                fprintf(stderr, "$%02x\n", operand);
            }
        } else if (addr_mode == 3) {
            if (opcode == 3) {
                uint16_t operand = cpu_read16(pc + 1);
                fprintf(stderr, "($%04x)\n", operand);
            } else if (opcode != 0) {
                uint16_t operand = cpu_read16(pc + 1);
                fprintf(stderr, "$%04x\n", operand);
            }
        } else if (addr_mode == 4) {
            uint8_t operand = cpu_read(pc + 1);
            fprintf(stderr, "$%04x\n", pc + 2 + (int8_t)operand);
        } else if (addr_mode == 5) {
            if (opcode == 4 || opcode == 5) {
                uint8_t operand = cpu_read(pc + 1);
                fprintf(stderr, "$%02x,x\n", operand);
            }
        } else if (addr_mode == 7) {
            if (opcode == 5) {
                uint16_t operand = cpu_read16(pc + 1);
                fprintf(stderr, "$%04x,x\n", operand);
            }
        }
    }
}

void cpu_execute(void) {

    uint8_t prev_sp = sp;
    bool interactive_pause = false;
    uint8_t interactive_prev_sp;
    while (true) {

        if (interactive && !interactive_pause) {
            cpu_print_registers();
            cpu_print_instruction();
            int c = getchar();
            if (c == 'n' && getchar() == '\n') {
                if (cpu_read(pc) == 0x20) {
                    interactive_pause = true;
                    interactive_prev_sp = sp - 2;
                }
            } else if (c != '\n') {
                while (getchar() != '\n') {}
            }
        }

        // hardcoded
        if (pc == 0x8057) {
            // EndlessLoop
            return;
        }

        // fetch
        uint8_t instruction = cpu_read(pc++);

        // decode
        uint8_t group = instruction & 0x3;
        uint8_t addr_mode = (instruction >> 2) & 0x7;
        uint8_t opcode = instruction >> 5;

        // execute
        if (group == 1) {
            uint16_t address;
            if (addr_mode == 0) {
                // indx
                address = cpu_read16((uint8_t)(cpu_read(pc) + x));
                ++pc;
            } else if (addr_mode == 1) {
                // zpg
                address = cpu_read(pc);
                ++pc;
            } else if (addr_mode == 2) {
                if (opcode != 4) {
                    // imm
                    address = pc;
                    ++pc;
                }
            } else if (addr_mode == 3) {
                // abs
                address = cpu_read16(pc);
                pc += 2;
            } else if (addr_mode == 4) {
                // indy
                address = cpu_read16(cpu_read(pc)) + y;
                ++pc;
            } else if (addr_mode == 5) {
                // zpgx
                address = (uint8_t)(cpu_read(pc) + x);
                ++pc;
            } else if (addr_mode == 6) {
                // absy
                address = cpu_read16(pc) + y;
                pc += 2;
            } else if (addr_mode == 7) {
                // absx
                address = cpu_read16(pc) + x;
                pc += 2;
            }

            if (opcode == 0) {
                // ora
                cpu_ora(cpu_read(address));
            } else if (opcode == 1) {
                // and
                cpu_and(cpu_read(address));
            } else if (opcode == 2) {
                // eor
                cpu_eor(cpu_read(address));
            } else if (opcode == 3) {
                // adc
                cpu_adc(cpu_read(address));
            } else if (opcode == 4) {
                if (addr_mode != 2) {
                    // sta
                    cpu_write(address, cpu_sta());
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 5) {
                // lda
                cpu_lda(cpu_read(address));
            } else if (opcode == 6) {
                // cmp
                cpu_cmp(cpu_read(address));
            } else if (opcode == 7) {
                // sbc
                cpu_sbc(cpu_read(address));
            }
        } else if (group == 2) {
            uint16_t address;
            if (addr_mode == 0) {
                if (opcode == 5) {
                    // imm
                    address = pc;
                    ++pc;
                }
            } else if (addr_mode == 1) {
                // zpg
                address = cpu_read(pc);
                ++pc;
            } else if (addr_mode == 3) {
                // abs
                address = cpu_read16(pc);
                pc += 2;
            } else if (addr_mode == 5) {
                if (opcode == 4 || opcode == 5) {
                    // zpgy
                    address = (uint8_t)(cpu_read(pc) + y);
                    ++pc;
                } else {
                    // zpgx
                    address = (uint8_t)(cpu_read(pc) + x);
                    ++pc;
                }
            } else if (addr_mode == 7) {
                if (opcode == 5) {
                    // absy
                    address = cpu_read16(pc) + y;
                    pc += 2;
                } else if (opcode != 4) {
                    // absx
                    address = cpu_read16(pc) + x;
                    pc += 2;
                }
            }

            if (opcode == 0) {
                // asl
                if (addr_mode == 2) {
                    a = cpu_asl(a);
                } else if (addr_mode != 0 && addr_mode != 4 && addr_mode != 6) {
                    cpu_write(address, cpu_asl(cpu_read(address)));
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 1) {
                // rol
                if (addr_mode == 2) {
                    a = cpu_rol(a);
                } else if (addr_mode != 0 && addr_mode != 4 && addr_mode != 6) {
                    cpu_write(address, cpu_rol(cpu_read(address)));
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 2) {
                // lsr
                if (addr_mode == 2) {
                    a = cpu_lsr(a);
                } else if (addr_mode != 0 && addr_mode != 4 && addr_mode != 6) {
                    cpu_write(address, cpu_lsr(cpu_read(address)));
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 3) {
                // ror
                if (addr_mode == 2) {
                    a = cpu_ror(a);
                } else if (addr_mode != 0 && addr_mode != 4 && addr_mode != 6) {
                    cpu_write(address, cpu_ror(cpu_read(address)));
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 4) {
                if (addr_mode == 2) {
                    // txa
                    cpu_txa();
                } else if (addr_mode == 6) {
                    // txs
                    cpu_txs();
                } else if (addr_mode == 1 || addr_mode == 3 || addr_mode == 5) {
                    // stx
                    cpu_write(address, cpu_stx());
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 5) {
                if (addr_mode == 2) {
                    // tax
                    cpu_tax();
                } else if (addr_mode == 6) {
                    // tsx
                    cpu_tsx();
                } else if (addr_mode != 4) {
                    // ldx
                    cpu_ldx(cpu_read(address));
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 6) {
                if (addr_mode == 2) {
                    // dex
                    cpu_dex();
                } else if (addr_mode != 0 && addr_mode != 4 && addr_mode != 6) {
                    // dec
                    cpu_write(address, cpu_dec(cpu_read(address)));
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 7) {
                if (addr_mode == 2) {
                    // nop
                    cpu_nop();
                } else if (addr_mode != 0 && addr_mode != 4 && addr_mode != 6) {
                    // inc
                    cpu_write(address, cpu_inc(cpu_read(address)));
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            }
        } else if (group == 0) {
            uint16_t address;
            if (addr_mode == 0) {
                if (opcode == 1) {
                    // abs
                    address = cpu_read16(pc);
                    pc += 2;
                } else if (opcode == 5 || opcode == 6 || opcode == 7) {
                    // imm
                    address = pc;
                    ++pc;
                }
            } else if (addr_mode == 1) {
                if (opcode != 0 && opcode != 2 && opcode != 3) {
                    // zpg
                    address = cpu_read(pc);
                    ++pc;
                }
            } else if (addr_mode == 3) {
                if (opcode == 3) {
                    // ind
                    address = cpu_read16(cpu_read16(pc));
                    pc += 2;
                } else if (opcode != 0) {
                    // abs
                    address = cpu_read16(pc);
                    pc += 2;
                }
            } else if (addr_mode == 4) {
                // rel
                address = pc;
                ++pc;
            } else if (addr_mode == 5) {
                if (opcode == 4 || opcode == 5) {
                    // zpgx
                    address = (uint8_t)(cpu_read(pc) + x);
                    ++pc;
                }
            } else if (addr_mode == 7) {
                if (opcode == 5) {
                    // absx
                    address = cpu_read16(pc) + x;
                    pc += 2;
                }
            }

            if (opcode == 0) {
                if (addr_mode == 0) {
                    // brk
                    cpu_brk();
                } else if (addr_mode == 2) {
                    // php
                    cpu_php();
                } else if (addr_mode == 4) {
                    // bpl
                    cpu_bpl(cpu_read(address));
                } else if (addr_mode == 6) {
                    // clc
                    cpu_clc();
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 1) {
                if (addr_mode == 0) {
                    // jsr
                    cpu_jsr(address);
                } else if (addr_mode == 2) {
                    // plp
                    cpu_plp();
                } else if (addr_mode == 4) {
                    // bmi
                    cpu_bmi(cpu_read(address));
                } else if (addr_mode == 6) {
                    // sec
                    cpu_sec();
                } else if (addr_mode == 1 || addr_mode == 3) {
                    // bit
                    cpu_bit(cpu_read(address));
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 2) {
                if (addr_mode == 0) {
                    // rti
                    return;
                } else if (addr_mode == 2) {
                    // pha
                    cpu_pha();
                } else if (addr_mode == 3) {
                    // jmp
                    cpu_jmp(address);
                } else if (addr_mode == 4) {
                    // bvc
                    cpu_bvc(cpu_read(address));
                } else if (addr_mode == 6) {
                    // cli
                    cpu_cli();
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 3) {
                if (addr_mode == 0) {
                    // rts
                    if (sp == prev_sp) {
                        return;
                    }
                    if (interactive && interactive_pause &&
                            sp == interactive_prev_sp) {
                        interactive_pause = false;
                    }
                    cpu_rts();
                } else if (addr_mode == 2) {
                    // pla
                    cpu_pla();
                } else if (addr_mode == 3) {
                    // jmp
                    cpu_jmp(address);
                } else if (addr_mode == 4) {
                    // bvs
                    cpu_bvs(cpu_read(address));
                } else if (addr_mode == 6) {
                    // sei
                    cpu_sei();
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 4) {
                if (addr_mode == 2) {
                    // dey
                    cpu_dey();
                } else if (addr_mode == 4) {
                    // bcc
                    cpu_bcc(cpu_read(address));
                } else if (addr_mode == 6) {
                    // tya
                    cpu_tya();
                } else if (addr_mode != 0 && addr_mode != 7) {
                    // sty
                    cpu_write(address, cpu_sty());
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 5) {
                if (addr_mode == 2) {
                    // tay
                    cpu_tay();
                } else if (addr_mode == 4) {
                    // bcs
                    cpu_bcs(cpu_read(address));
                } else if (addr_mode == 6) {
                    // clv
                    cpu_clv();
                } else {
                    // ldy
                    cpu_ldy(cpu_read(address));
                }
            } else if (opcode == 6) {
                if (addr_mode == 2) {
                    // iny
                    cpu_iny();
                } else if (addr_mode == 4) {
                    // bne
                    cpu_bne(cpu_read(address));
                } else if (addr_mode == 6) {
                    // cld
                    cpu_cld();
                } else if (addr_mode != 5 && addr_mode != 7) {
                    // cpy
                    cpu_cpy(cpu_read(address));
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            } else if (opcode == 7) {
                if (addr_mode == 2) {
                    // inx
                    cpu_inx();
                } else if (addr_mode == 4) {
                    // beq
                    cpu_beq(cpu_read(address));
                } else if (addr_mode == 6) {
                    // sed
                    cpu_sed();
                } else if (addr_mode != 5 && addr_mode != 7) {
                    // cpx
                    cpu_cpx(cpu_read(address));
                } else {
                    fprintf(stderr, "illegal instruction: a=%x,b=%x,c=%x\n", opcode, addr_mode, group);
                }
            }
        }
    }
}
