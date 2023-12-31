#ifndef CPU_H_
#define CPU_H_

#include <stdbool.h>
#include <stdint.h>

#include "smb1.h"

#define SR_NEGATIVE     ((uint8_t)0x80)
#define SR_OVERFLOW     ((uint8_t)0x40)
#define SR_BREAK        ((uint8_t)0x10)
#define SR_DECIMAL      ((uint8_t)0x08)
#define SR_INTERRUPT    ((uint8_t)0x04)
#define SR_ZERO         ((uint8_t)0x02)
#define SR_CARRY        ((uint8_t)0x01)

extern uint16_t pc;
extern uint8_t a;
extern uint8_t x;
extern uint8_t y;
extern uint8_t sr;
extern uint8_t sp;

extern bool interactive;

void cpu_execute(void);

extern smb1_input_callback_t cpu_input_callback;

uint8_t cpu_read_joy1(void);
uint8_t cpu_read_joy2(void);

void cpu_write_oamdma(uint8_t value);
void cpu_write_joy1(uint8_t value);
void cpu_write_joy2(uint8_t value);

#define adc(value) { \
    uint16_t result = a + (uint8_t)(value) + (c ? 1 : 0); \
    c = (result & 0x100) != 0; \
    z = result == 0; \
    v = !((a & 0x80) ^ ((uint8_t)(value) & 0x80)) && ((a & 0x80) ^ (result & 0x80)); \
    n = (int8_t)result < 0; \
    a = result; \
}

#define and(value) { \
    uint8_t result = a & (uint8_t)(value); \
    z = result == 0; \
    n = (int8_t)result < 0; \
    a = result; \
}

#define asl(value) { \
    uint8_t result = (uint8_t)(value) << 1; \
    c = ((uint8_t)(value) & 0x80) != 0; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    (value) = result; \
}

#define bcc(value) { \
    if (!c) goto value; \
}

#define bcs(value) { \
    if (c) goto value; \
}

#define beq(value) { \
    if (z) goto value; \
}

#define bit(value) { \
    z = (a & (uint8_t)(value)) == 0; \
    v = ((uint8_t)(value) & 0x40) != 0; \
    n = (int8_t)(value) < 0; \
}

#define bmi(value) { \
    if (n) goto value; \
}

#define bne(value) { \
    if (!z) goto value; \
}

#define bpl(value) { \
    if (!n) goto value; \
}

#define bvc(value) { \
    if (!v) goto value; \
}

#define bvs(value) { \
    if (v) goto value; \
}

#define clc() { \
    c = false; \
}

#define clv() { \
    v = false; \
}

#define cmp(value) { \
    uint8_t result = a + (uint8_t)(~(value)) + 1; \
    c = a >= (uint8_t)(value); \
    z = a == (uint8_t)(value); \
    n = (int8_t)result < 0; \
}

#define cpx(value) { \
    uint8_t result = x + (uint8_t)(~(value)) + 1; \
    c = x >= (uint8_t)(value); \
    z = x == (uint8_t)(value); \
    n = (int8_t)result < 0; \
}

#define cpy(value) { \
    uint8_t result = y + (uint8_t)(~(value)) + 1; \
    c = y >= (uint8_t)(value); \
    z = y == (uint8_t)(value); \
    n = (int8_t)result < 0; \
}

#define dec(value) { \
    uint8_t result = (uint8_t)(value) - 1; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    (value) = result; \
}

#define dex() { \
    uint8_t result = x - 1; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    x = result; \
}

#define dey() { \
    uint8_t result = y - 1; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    y = result; \
}

#define eor(value) { \
    uint8_t result = a ^ (uint8_t)(value); \
    z = result == 0; \
    n = (int8_t)result < 0; \
    a = result; \
}

#define inc(value) { \
    uint8_t result = (uint8_t)(value) + 1; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    (value) = result; \
}

#define inx() { \
    uint8_t result = x + 1; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    x = result; \
}

#define iny() { \
    uint8_t result = y + 1; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    y = result; \
}

#define jmp(value) { \
    goto value; \
}

#define lda(value) { \
    uint8_t result = (uint8_t)(value); \
    z = result == 0; \
    n = (int8_t)result < 0; \
    a = result; \
}

#define ldx(value) { \
    uint8_t result = (uint8_t)(value); \
    z = result == 0; \
    n = (int8_t)result < 0; \
    x = result; \
}

#define ldy(value) { \
    uint8_t result = (uint8_t)(value); \
    z = result == 0; \
    n = (int8_t)result < 0; \
    y = result; \
}

#define lsr(value) { \
    uint8_t result = (uint8_t)(value) >> 1; \
    c = ((uint8_t)(value) & 0x01) != 0; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    (value) = result; \
}

#define nop() { \
}

#define ora(value) { \
    uint8_t result = a | (uint8_t)(value); \
    z = result == 0; \
    n = (int8_t)result < 0; \
    a = result; \
}

#define pha() { \
    *(sp--) = a; \
}

#define pla() { \
    uint8_t result = *(++sp); \
    z = result == 0; \
    n = (int8_t)result < 0; \
    a = result; \
}

#define rol(value) { \
    uint8_t result = ((uint8_t)(value) << 1) | (c ? 0x01 : 0); \
    c = ((uint8_t)(value) & 0x80) != 0; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    (value) = result; \
}

#define ror(value) { \
    uint8_t result = ((uint8_t)(value) >> 1) | (c ? 0x80 : 0); \
    c = ((uint8_t)(value) & 0x01) != 0; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    (value) = result; \
}

#define sbc(value) { \
    uint16_t result = a + (uint8_t)(~(value)) + (c ? 1 : 0); \
    c = (result & 0x100) != 0; \
    z = result == 0; \
    v = !((a & 0x80) ^ ((uint8_t)(value) & 0x80)) && ((a & 0x80) ^ (result & 0x80)); \
    n = (int8_t)result < 0; \
    a = result; \
}

#define sec() { \
    c = true; \
}

#define sta(value) { \
    (value) = a; \
}

#define stx(value) { \
    (value) = x; \
}

#define sty(value) { \
    (value) = y; \
}

#define tax() { \
    uint8_t result = a; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    x = a; \
}

#define tay() { \
    uint8_t result = a; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    y = a; \
}

#define txa() { \
    uint8_t result = x; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    a = x; \
}

#define tya() { \
    uint8_t result = y; \
    z = result == 0; \
    n = (int8_t)result < 0; \
    a = y; \
}

#endif
