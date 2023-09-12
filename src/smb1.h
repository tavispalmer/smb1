#ifndef SMB1_H_
#define SMB1_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool a;
    bool b;
    bool select;
    bool start;
    bool up;
    bool down;
    bool left;
    bool right;
} smb1_input_t;

typedef void (*smb1_input_callback_t)(smb1_input_t *input);

extern uint8_t memory[0x800];

void smb1_init(void);
void smb1_run(void);
void smb1_set_input_callback(smb1_input_callback_t callback);

#endif
