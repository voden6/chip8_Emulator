#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "ch8.h"

#define ch8Mem 4096
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32	
#define PIXEL_ON 0x000000
#define PIXEL_OFF 0xFFFFFF
#define SCALE 10
#define FRAMES_PER_SECOND 60
#define MILLISECONDS_PER_CYCLE ((1000)/ (FRAMES_PER_SECOND))

//temporary location
uint16_t keymap[16] = 
{ SDLK_1, SDLK_2, SDLK_3, SDLK_4,
  SDLK_q, SDLK_w, SDLK_e, SDLK_r,
  SDLK_a, SDLK_s, SDLK_d, SDLK_f,
  SDLK_z, SDLK_x, SDLK_c, SDLK_v
};
  
typedef struct 
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
}screen;

CH8_STATE* chip8_INIT();
screen* ch8_graphicsINIT();
void ch8_loadGame(int argc, char* argv[], CH8_STATE* s);
void ch8_printState(CH8_STATE* state);
void ch8_CYCLE();
void ch8_updateTimers(CH8_STATE* state);
void ch8_drawGraphics(screen* display, CH8_STATE* state);
void ch8_Quit(screen* display);

int main (int argc, char* argv[])
{	
	uint32_t start_tick;
	uint32_t frame_speed;

	SDL_Event event;
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	bool running = true;
	
	CH8_STATE* state = chip8_INIT(); 	
	screen* display = ch8_graphicsINIT();

	ch8_loadGame(argc, argv, state);

	
	while(running)
	{
		start_tick = SDL_GetTicks();
		if(SDL_PollEvent(&event))
			if(event.type == SDL_QUIT || keyState[SDLK_ESCAPE])
			{
				running = false;
				exit(EXIT_SUCCESS);	
			}
		ch8_CYCLE(state);
		if(state->draw == true) 
		{
			ch8_drawGraphics(display, state);
			state->draw = false;
		}
		
		frame_speed = SDL_GetTicks() - start_tick;
		if(frame_speed < MILLISECONDS_PER_CYCLE)
		{
			SDL_Delay(MILLISECONDS_PER_CYCLE - frame_speed);
		}
	}


	free(display);
	free(state);	
	ch8_Quit(display);

	return 0;
}

void ch8_Quit(screen* display)
{
	SDL_DestroyRenderer(display->renderer);
	SDL_DestroyWindow(display->window);
	SDL_DestroyTexture(display->texture);
	
	SDL_Quit();
}

screen* ch8_graphicsINIT()
{
	screen* disp = malloc(sizeof(screen));
	
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);	
	}
	
	disp->window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DISPLAY_WIDTH * SCALE, DISPLAY_HEIGHT * SCALE, SDL_WINDOW_SHOWN); 
	if(disp->window == NULL)
	{
		printf("SDL Error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	
	disp->renderer = SDL_CreateRenderer(disp->window, -1, SDL_RENDERER_ACCELERATED);
	if(disp->renderer == NULL)
	{
		printf("SDL Error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}	

	disp->texture = SDL_CreateTexture(disp->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	if(disp->texture == NULL)
	{
		printf("SDL_Error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	return disp;
}

void ch8_drawGraphics(screen* display, CH8_STATE* state)
{
	SDL_UpdateTexture(display->texture, NULL, state->gfx, DISPLAY_WIDTH * sizeof(uint32_t));
	SDL_RenderCopy(display->renderer, display->texture, NULL, NULL);
	SDL_RenderPresent(display->renderer);
	
}

CH8_STATE*  chip8_INIT()
{
	CH8_STATE* s = calloc(sizeof(CH8_STATE), 1);

	s->pc = 0x200;
	s->sp = 0;
	s->I = 0;
	
	memset(s->reg, 0, 0x10);
	memset(s->stack, 0, 32);
	memcpy(&s->memory[0x0], ch8_fontset, 80);
	memset(s->gfx, 0, 0x800);
	
	return s;
}

void ch8_UpdateTimers(CH8_STATE* state)
{
	if(state->delayTimer > 0)
		--state->delayTimer;
	if(state->soundTimer > 0)
		if(state->soundTimer == 1)
			puts("beep!");
		--state->soundTimer;
}

void ch8_loadGame(int argc, char* argv[], CH8_STATE* s)
{
	FILE* game = fopen(argv[1], "rb");
	if(argc < 2)
	{
		printf("Please enter a Chip8 ROM...\n");
		exit(EXIT_FAILURE);
	}
	fread(s->memory+0x200, 1, ch8Mem, game);
        fclose(game);	
}

void ch8_printState(CH8_STATE* state)
{
	uint8_t* code = &state->memory[state->pc];
	uint8_t opNib = code[0] & 0xF0;
	uint8_t regX = (code[0] & 0x0F);
	uint8_t regY = ((code[1] >> 4) & 0x0F);
	printf("Executing %02X%02X, PC: %04X, Stack %04X, SP %02X, I %04X, ", code[0], code[1], state->pc, state->sp == 0? state->stack[state->sp]: state->stack[state->sp - 1], state->sp, state->I);
	if(opNib == 0x00 || opNib == 0x10 || opNib == 0x20 || opNib == 0xA0 || opNib == 0xB0)
	{
		printf(" ");
	}
	else
	{
		printf("Reg%01X: %02X, ", regX, state->reg[regX]);
	}
	if(opNib == 0x50 || opNib == 0x80 || opNib == 0x90 || opNib == 0xD0)
	{
		printf("Reg%01X: %02X", regY, state->reg[regY]);
	}
	puts("");	
}

void ch8_CYCLE(CH8_STATE* state)
{
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	uint8_t* code = &state->memory[state->pc];
	uint8_t firstnib = (code[0] & 0xF0);
	uint8_t regX = (code[0] & 0x0F);
	uint8_t regY = ((code[1] >> 4) & 0x0F);

	ch8_printState(state);
	switch(firstnib)
	{
		case 0x00:
			switch(code[1])
			{
				case 0xE0: 
					memset(state->gfx, 0, 0x800);
					state->pc += 2;
					break;
				case 0xEE:
					state->pc = state->stack[state->sp - 1] + 2;
					state->sp--;
					break;
				default: break;
			}
			break;
		case 0x10:
			{
				uint16_t jmp_addr = ((code[0] & 0xF) << 8 | code[1]);
				state->pc = jmp_addr;
			} break;
		case 0x20: 
			state->stack[state->sp] = state->pc;
			state->sp++;
			state->pc = ((code[0] & 0xF) << 8 | code[1]);
			break;
		case 0x30: 
			if(state->reg[regX] == code[1])
				state->pc += 2;
			state->pc += 2;	 
			break;
		case 0x40:
			if(state->reg[regX] != code[1])
				state->pc += 2;
			state->pc += 2;
			break;
		case 0x50: 
			if(state->reg[regX] == state->reg[regY])
				state->pc += 2;
			state->pc += 2;			
			break;
		case 0x60: 
			state->reg[regX] = code[1];
			state->pc += 2;
			break;
		case 0x70:
			state->reg[regX] += code[1];
			state->pc += 2;
			break;
		case 0x80: 
			switch(code[1] & 0x0F)
			{
				case 0x00: 
					state->reg[regX] = state->reg[regY];
					state->pc += 2;		
					break;
				case 0x01: 
					state->reg[regX] = state->reg[regX] | state->reg[regY];
					state->pc += 2;
					break;
				case 0x02:
					state->reg[regX] = state->reg[regX] & state->reg[regY];
					state->pc += 2;
					break;
				case 0x03:
					state->reg[regX] = state->reg[regX] ^ state->reg[regY];
					state->pc += 2;
					break;
				case 0x04: 
					state->reg[regX] += state->reg[regY];
					if(((int)state->reg[regX] + (int)state->reg[regY]) < 256)
						state->reg[0xF] &= 0;
					else
						state->reg[0xF] = 1; 
					state->pc += 2;
					break;
				case 0x05: 
					state->reg[regX] -= state->reg[regY];
					if(((int)state->reg[regX] - (int)state->reg[regY]) >= 0)
						state->reg[0xF] = 1;
					else
						state->reg[0xF] &= 0; 
					state->pc += 2;
					break;
				case 0x06:
					state->reg[0xF] = state->reg[regY] & 0x0F; 
					state->reg[regX] = state->reg[regY] >> 1;
					state->pc += 2;
					break;
				case 0x07:
					state->reg[regX] = state->reg[regY] - state->reg[regX];
					if(((int)state->reg[regY] - (int)state->reg[regX]) >= 0)
						state->reg[0xF] = 1;
					else
					state->reg[0xF] &= 0; 
						state->pc += 2;
					break;
				case 0x0E:
					state->reg[0xF] = state->reg[regY] & 0xF0;
					state->reg[regY] <<= 1;
					state->reg[regX] = state->reg[regY];
					state->pc += 2;
					break;
					default:
						 printf("Unregistered Opcode: 8***\n");
						 exit(EXIT_FAILURE);
			} break;
		case 0x90: 
			if(state->reg[regX] != state-> reg[regY])
				state->pc += 2;
			state->pc += 2;		
			break;
		case 0xA0: 
			state->I = ((code[0] & 0xF) << 8) | code[1];
			state->pc += 2;
			break;
		case 0xB0: 
			state->pc = (state->reg[0] + ((code[0] & 0x0F) << 4) | code[1]);	
			break;
		case 0xC0:
			state->reg[regX] = (rand() % 255) & code[1]; 
			state->pc += 2;
			break;
		case 0xD0: 
			{
				uint8_t height = (code[1] & 0x0F);
				uint8_t pixel;
				state->reg[0xF] = 0x0;
				for(int y = 0; y < height; y++)
				{
					pixel = state->memory[state->I + y];
					for(int x = 0; x < 8; x++)
					{
						if(pixel & (0x80 >> x))
						{
							int index = (state->reg[regX] + x) % DISPLAY_WIDTH + ((state->reg[regY] + y) % DISPLAY_HEIGHT) * DISPLAY_WIDTH;
							if(state->gfx[index] == PIXEL_ON)
							{
								state->reg[0xF] = 0x1;
								state->gfx[index] = PIXEL_OFF;
							}
							else
							{
								state->gfx[index] = PIXEL_ON;
							}
							state->draw = true;
						}	
					}
				}
			} 
			state->pc += 2;
			break;
		case 0xE0: 
			switch(code[1])
			{
				case 0x9E:
					if(keyState[keymap[state->reg[regX]]])
						state->pc += 2;
					state->pc += 2;
					break;
				case 0xA1:	
					if(!keyState[keymap[state->reg[regX]]])
						state->pc += 2;
					state->pc += 2;
					break;
				default:
					printf("Unregistered opcode: E***\n");
					exit(EXIT_FAILURE);
					break;
			} break;
		case 0xF0: 
			switch(code[1])
			{
				case 0x07:
					state->reg[regX] = state->delayTimer;
					state->pc += 2;
					break;
				case 0x0A:
					for(int i = 0; i < 0x10; i++)
					{
						if (keyState[keymap[i]])
						{
							state->reg[regX] = i;
							state->pc += 2;
						}
					}
					break;
				case 0x15: 
					state->delayTimer = state->reg[regX];
					state->pc += 2;
					break;
				case 0x18:
					state->soundTimer = state->reg[regX];
					state->pc += 2;
					break;
				case 0x1E: 
					state->I += state->reg[regX];
					state->pc += 2;
					break;
				case 0x29:
				        state->I = (state->reg[regX] * 5);	
					state->pc += 2;
					break;
				case 0x33: 
					state->memory[state->I] = state->reg[regX] / 100;
					state->memory[state->I + 1] = (state->reg[regX] / 10) % 10;
					state->memory[state->I + 2] = (state->reg[regX] % 100) % 10;
					state->pc += 2;
					break;
				case 0x55: 
					for(int i = 0; i <= regX; i++)
						state->memory[state->I+i] = state->reg[i];
					state->pc += 2;
					break;
				case 0x65:
					for(int i = 0; i <= regX; i++)
						state->reg[i] = state->memory[state->I + i];
					state->pc += 2;
					break;
				default:
					printf("Unregistered opcode: F***\n");
					exit(EXIT_FAILURE);
					break;
			} break;
		default:
		puts("Opcode Uninitialized");
		exit(EXIT_FAILURE);
	}
	ch8_UpdateTimers(state);
}


