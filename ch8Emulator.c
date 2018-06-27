#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "SDL_log.h"


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

int main (int argc, char *argv[])
{
	SDL_Window *window = NULL;
	SDL_Surface *surface = NULL;
	SDL_Renderer *renderer = NULL;

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0);
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL wasn't initialized: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	window = SDL_CreateWindow("Chip8 Emulator",
				   SDL_WINDOWPOS_CENTERED,
				   SDL_WINDOWPOS_CENTERED,
				   64,
				   32,
				   SDL_WINDOW_RESIZABLE);
	if(window == NULL)
	{
		printf("Could not create window: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
		
	SDL_Quit();
	return 0;
}
