#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ch8.h"

void chip8_INIT();
void chip8_CYCLE();

int main (int argc, char *argv[])
{
	
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window;
	SDL_Texture* texture;
	SDL_Renderer* renderer;
	SDL_Event event;

	FILE* game = fopen(argv[1], "rb");
	if(argc < 2)
	{
		printf("Please enter a game to Emulate");
		return 0;
	}
	//fread(s->memory+0x200, 1, 
        chip8_INIT();

	while(1)
	{
		
	}


	SDL_DestroyRenderer(renderer);

	return 0;
}

void  chip8_INIT()
{	
	CH8_STATE* s = calloc(sizeof(CH8_STATE),1);

	s->memory = calloc(4096,1);
	s->screen = &s->memory[0xF00];
	s->pc = 0x200;
	s->sp = 0xEA00;
	
}

void chip8_CYCLE(CH8_STATE* state)
{
	uint8_t* code = &state->memory[state->pc];
	uint8_t firstnib = (code[0] & 0xF0);
	
	switch(firstnib)
	{
		case 0x00:
			 switch(code[1])
			 {
				case 0xE0: break;
			 }
		default:
			puts("Opcode Uninitialized"); break;
	}
}

