#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct chip8
{
        uint8_t reg[16];
        uint8_t key[16];	
        uint8_t delayTimer;
        uint8_t soundTimer;
        uint16_t I;
        uint16_t opcode;
        uint16_t sp;
        uint16_t pc;
        uint8_t* memory;
	uint8_t* screen;

}chip8, CH8_STATE;


