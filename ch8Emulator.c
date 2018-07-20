#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ch8.h"

#define ch8Mem 4096

void graphics_INIT();
CH8_STATE* chip8_INIT();
void loadGame(int argc, char* argv[], CH8_STATE* s);
void chip8_CYCLE();

int main (int argc, char* argv[])
{
	SDL_Event event;
	int running = 1;
	CH8_STATE* state = chip8_INIT(); 

	graphics_INIT();
	loadGame(argc, argv, state);


	while(running)
	{
		chip8_CYCLE(state);
	}


	//SDL_DestroyRenderer(renderer);

	return 0;
}

void graphics_INIT()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window;
	SDL_Texture* texture;
	SDL_Renderer* renderer;
	SDL_Event event;
}

CH8_STATE*  chip8_INIT()
{
	CH8_STATE* s = calloc(sizeof(CH8_STATE),1);
	
	s->screen = &s->memory[0xF00];
	s->pc = 0x200;
	s->sp = 0xEA0;
	
	return s;
}

void loadGame(int argc, char* argv[], CH8_STATE* s)
{
	FILE* game = fopen(argv[1], "rb");
	if(argc < 2)
	{
		printf("Please enter a Chip8 ROM...");
		exit(EXIT_FAILURE);
	}
	fread(s->memory+0x200, 1, ch8Mem, game);
        fclose(game);	
}

void chip8_CYCLE(CH8_STATE* state)
{
	uint8_t* code = &state->memory[state->pc];
	uint8_t firstnib = (code[0] & 0xF0);
	uint8_t regX = (code[0] & 0x0F);
	uint8_t regY = ((code[1] >> 4) & 0x0F);

	printf("Executing %02X%02X, PC: %04X, SP %04X\n", code[0], code[1], state->pc, state->sp);	
	switch(firstnib)
	{
		case 0x00:
			 switch(code[1])
			 {
				case 0xE0: 
					{
						memset(state->graphics, 0, sizeof(state->graphics));
						state->pc += 2;
					}
					break;
				case 0xEE:
					{
						state->pc = state->stack[state->sp];
						state->pc += 2;
					}
					break;
				default: break;
			 }
		case 0x01:
			 {
				uint16_t jmp_addr = ((code[0] & 0xF) << 8 | code[1]);
				state->pc = jmp_addr;
				break;
		 	 }
		case 0x20: 
			 {
				state->pc += 2;
			 }
			   break;
		case 0x30: 
			 {
				if(state->reg[regX] == code[1])
					state->pc += 2;
				state->pc += 2;
			 }	 
			   break;
		case 0x40:
			 {
				if(state->reg[regX] != code[1])
					state->pc += 2;
				state->pc += 2;
			 }
			   break;
		case 0x50: 
			 {
			       	if(state->reg[regX] == state->reg[regY])
					state->pc += 2;
				state->pc += 2;			
			 }
			   break;
		case 0x60: 
			 {
				state->reg[regX] = code[1];
				state->pc += 2;
			 }
			   break;
		case 0x70:
			 {
				 state->reg[regX] += code[1];
				 state->pc += 2;
			 }
			   break;
		case 0x80: 
			   switch(code[1] & 0x0F)
			   {
			   	case 0x00: 
					 {
					 	state->reg[regX] = state->reg[regY];
				       		state->pc += 2;		
					 }
					   break;
				case 0x01: 
					 {
						 state->reg[regX] = state->reg[regX] | state->reg[regY];
						 state->pc += 2;
					 }
					   break;
				case 0x02:
					 {
					 	state->reg[regX] = state->reg[regX] & state->reg[regY];
						state->pc += 2;
					 }
					   break;
				case 0x03:
					 {
					   	state->reg[regX] = state->reg[regX] ^ state->reg[regY];
						state->pc += 2;
					 }
					   break;
				case 0x04: 
					 {
						 state->reg[regX] += state->reg[regY];
						 if(((int)state->reg[regX] + (int)state->reg[regY]) < 256)
							state->reg[0xF] &= 0;
						 else
							state->reg[0xF] = 1; 
						 state->pc += 2;
					 }
					   break;
				case 0x05: 
					 {
						
						 state->reg[regX] -= state->reg[regY];
						 if(((int)state->reg[regX] - (int)state->reg[regY]) >= 0)
							state->reg[0xF] = 1;
						 else
							state->reg[0xF] &= 0; 
						 state->pc += 2;
					 }
					   break;
				case 0x06:
					 {
						 state->reg[0xF] = state->reg[regY] & 0x0F; 
						 state->reg[regX] = state->reg[regY] >> 1;
						 state->pc += 2;
					 }
					   break;
				case 0x07:
					 {
						 state->reg[regX] = state->reg[regY] - state->reg[regX];
						 if(((int)state->reg[regY] - (int)state->reg[regX]) >= 0)
							state->reg[0xF] = 1;
						 else
							state->reg[0xF] &= 0; 
						 state->pc += 2;
					 }
					   break;
				case 0x0E:
					 {
						 state->reg[0xF] = state->reg[regY] & 0xF0;
						 state->reg[regY] <<= 1;
						 state->reg[regX] = state->reg[regY];
						 state->pc += 2;
					 }
					   break;
			   }
			   break;
		case 0x90: 
			 {
				if(state->reg[regX] != state-> reg[regY])
			       		state->pc += 2;
				state->pc += 2;		
			 }
			   break;
		case 0xA0: 
			 {
				state->I = ((code[0] & 0xf) << 8) | code[1];
				state->pc += 2;
			 }
			   break;
		case 0xB0: 
			 {
				state->pc = (state->reg[0] + ((code[0] & 0x0F) << 4) | code[1]);	
			 }
			   break;
		case 0xC0:
			 {
				state->reg[regX] = (rand() % 255) & code[1]; 
				state->pc += 2;
			 }
			   break;
		case 0xD0: 
			 {
				state->pc += 2;
			 }
			   break;
		case 0xE0: 
			 {
				 switch(code[1])
				 {
					 case 0x9E:
						  {
							  state->pc += 2;
						  }
						    break;
					 case 0xA1:
						  {
							  state->pc += 2;
						  }
						    break;
					 default:
						    printf("Unregistered opcode");
				 }
			 }
			   break;
		case 0xF0: 
			 {
				 switch(code[1])
				 {
					 case 0x07:
						  {
							  state->pc += 2;
						  }
						    break;
					 case 0x0A:
						  {
							  state->pc += 2;
						  }
						    break;
					 case 0x15: 
						  {
							  state->pc += 2;
						  }
						    break;
					 case 0x18:
						  {
							  state->pc += 2;
						  }
						    break;
					 case 0x1E: 
						  {
							  state->I += state->reg[regX];
							  state->pc += 2;
						  }
						    break;
					 case 0x29: 
						  {

							  state->pc += 2;
						  }
						    break;
				 	 case 0x33: 
						  {

							  state->pc += 2;
						  }
						    break;
					 case 0x55: 
						  {
							  for(int i = 0; i <= regX; i++)
							  {
								 state->memory[state->I+i] = state->reg[i];
							  }
							  state->pc += 2;
						  }
						    break;
					 case 0x65:
						  {
							  for(int i = 0; i <= regX; i++)
							  {
								  state->reg[i] = state->memory[state->I + i];
							  }
						  	  state->pc += 2;
						  }
						    break;
					 default:
						    printf("Unregistered opcode");
				 }
			 }
			   break;
		default:
			puts("Opcode Uninitialized");
		        exit(EXIT_FAILURE);
			break;
	}
}

