#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct chip8
{
        uint8_t memory[4096];
        uint8_t reg[16];
        uint8_t key[16];
        uint8_t gfx[64 * 32];
        uint8_t delayTimer;
        uint8_t soundTimer;
        uint16_t I;
        uint16_t pc;
        uint16_t opcode[35];
        uint16_t stack[16];
        uint16_t sp;

}chip8;


