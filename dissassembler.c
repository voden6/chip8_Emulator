#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ch8.h"


void ch8Dissassemble(uint8_t* buffer, int pc, chip8 ch8);

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		printf("You did not enter a chip8 file to dissassemble\n");
		return 1;
	}

	chip8 ch8;
	int fileSize;
	FILE* chip8_Rom = fopen(argv[1], "rb");


	if (chip8_Rom == NULL)
	{
		printf("Error: File %s could not be opened\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	//get fileSize of ROM
	fseek(chip8_Rom, 0, SEEK_END);
	fileSize = ftell(chip8_Rom);
	fseek(chip8_Rom, 0, SEEK_SET);

	uint8_t* buffer = (uint8_t*)malloc(fileSize + 0x200);
	fread(buffer + 0x200, fileSize, 1, chip8_Rom);

	ch8.pc = 0x200;
	while (ch8.pc < (fileSize + 0x200))
	{
		ch8Dissassemble(buffer, ch8.pc, ch8);
		ch8.pc += 2;
		puts("");
	}
	
	fclose(chip8_Rom);
	free(buffer);

	return 0;
}

void ch8Dissassemble(uint8_t* buffer, int pc, chip8 ch8)
{
	uint8_t* code = &buffer[pc];
	uint8_t firstNib = (code[0] & 0xf0);
        uint8_t regX = (code[0] & 0x0f);
	uint8_t regY = ((code[1] >> 4) & 0x0f);

	printf("%04x %02x %02x ", pc, code[0], code[1]);
	switch (firstNib)
	{
	case 0x00:
	       	   {
			
			 if(code[1] == 0xE0)
			 {
				 printf("%-10s", "CLS"); break;
			 }
			 else if(code[1] == 0xEE)
			 {
				 printf("%-10s", "RET"); break;
			 }
			 else
			 {
				 printf("%-14s #$%01X%02X", "SYS", code[0] & 0x0f, code[1]); break;
			 }
		   };
		   break;
	case 0x10: 
		   {
			 printf("%-14s #$%01X%02X", "JMP", code[0] & 0x0f , code[1]);	  
		   };
		   break;
	case 0x20: {
			 printf("%-14s #$%01X%02X", "CALL", code[0] & 0x0f, code[1]); 
		   };
		   break;
	case 0x30: 
		   {
			   ch8.reg[regX] = (code[0] & 0x0f);
			   printf("%-10s V%01X, #$%02X", "SE", ch8.reg[regX], code[1]);
		   };
		   break;
	case 0x40: 
		   {
			   ch8.reg[regX] = (code[0] & 0x0f);
			   printf("%-10s V%01X, #$%02X", "SNE", ch8.reg[regX], code[1]);
		   };
		   break;
	case 0x50: 
		   {
			   ch8.reg[regX] = (code[0] & 0x0f);
			   printf("%-10s V%01x, V%01X", "SE", ch8.reg[regX], ch8.reg[regY]);
		   };
		   break;
	case 0x60:
		   {
			   ch8.reg[regX] = (code[0] & 0x0f);
			   printf("%-10s V%01X, #$%02X", "LD", ch8.reg[regX], code[1]);
		   };
		   break;
	case 0x70:
		   {
			   ch8.reg[regX] = (code[0] & 0x0f);
			   printf("%-10s V%01X, #$%02X", "ADD", ch8.reg[regX], code[1]);
		   };
		   break;
	case 0x80: 
		   {
			  if((code[1] & 0x0f) == 0x00)
			  {
				  ch8.reg[regX] = (code[0] & 0x0f);
				  ch8.reg[regY] = ((code[1] >> 4) & 0x0f);
				  printf("%-6s V%01X, V%01X", "LD", ch8.reg[regX], ch8.reg[regY]);
			 	  break;
			  }
			  else if((code[1] & 0x0f) == 0x01)
			  {
				  ch8.reg[regX] = (code[0] & 0x0f);
				  ch8.reg[regY] = ((code[1] >> 4) & 0x0f);
				  printf("%-10s V%01X, V%01X", "OR", ch8.reg[regX], ch8.reg[regY]);
			 	  break;
			  }
			  else  if((code[1] & 0x0f) == 0x02)
			  {
				  ch8.reg[regX] = (code[0] & 0x0f);
				  ch8.reg[regY] = ((code[1] >> 4) & 0x0f);
				  printf("%-6s V%01X, V%01X,", "AND", ch8.reg[regX], ch8.reg[regY]);
			 	  break;
			  }
			  else if((code[1] & 0x0f) == 0x03)
			  {
				  ch8.reg[regX] = (code[0] & 0x0f);
				  ch8.reg[regY] = ((code[1] >> 4) & 0x0f);
				  printf("%-10s V%01X, V%01X", "XOR", ch8.reg[regX], ch8.reg[regY]);
			 	  break;
			  }
			  else if((code[1] & 0x0f) == 0x04)
			  {
				  ch8.reg[regX] = (code[0] & 0x0f);
				  ch8.reg[regY] = ((code[1] >> 4) & 0x0f);
				  printf("%-10s V%01X, V%01X", "ADD", ch8.reg[regX], ch8.reg[regY]);
			 	  break;
			  }
			  else if((code[1] & 0x0f) == 0x05)
			  {
				  ch8.reg[regX] = (code[0] & 0x0f);
				  ch8.reg[regY] = ((code[1] >> 4) & 0x0f);
				  printf("%-10s V%01X, V%01X", "SUB", ch8.reg[regX], ch8.reg[regY]);
			 	  break;
			  }
			  else if((code[1] & 0x0f) == 0x06)
			  {
				  ch8.reg[regX] = (code[0] & 0x0f);
				  ch8.reg[regY] = ((code[1] >> 4) & 0x0f);
				  printf("%-10s V%01X, {,V%01X}", "SMR", ch8.reg[regX], ch8.reg[regY]);
			 	  break;
			  }
			  else if((code[1] & 0x0f) == 0x07)
			  {
				  ch8.reg[regX] = (code[0] & 0x0f);
				  ch8.reg[regY] = ((code[1] >> 4) & 0x0f);
				  printf("%-10s V%01X, V%01X", "SUBN", ch8.reg[regX], ch8.reg[regY]);
			 	  break;
			  }
			  else if((code[1] & 0x0f) == 0x0E)
			  {
				  ch8.reg[regX] = (code[0] & 0x0f);
				  ch8.reg[regY] = ((code[1] >> 4) & 0x0f);
				  printf("%-6s V%01X, {,V%01X}", "SML", ch8.reg[regX], ch8.reg[regY]);
			 	  break;
			  }		  
		   };
		   break;
	case 0x90: 
		   {
			   ch8.reg[regX] = (code[0] & 0x0f);
		      	   ch8.reg[regY] = ((code[1] >> 4) & 0x0f);
			   printf("%-6s V%01X, V%01X,", "SNE", ch8.reg[regX], ch8.reg[regY]);
			   break;

		   };
		   break;
	case 0xa0:
		   {
		   	   ch8.I = (((code[0] << 8) + code[1]) & 0x0fff);
			   printf("%-11s I, #$%03x", "LD", ch8.I);
		   };
		   break;
	case 0xb0:
		   {
			   printf("%-10s V0, #$%03x", "JP", (((code[0] << 8) + code[1]) & 0x0fff));
		   };
		   break;
	case 0xc0: 
		   {
			   ch8.reg[regX] = (code[0] & 0x0f);
			   printf("%-10s V%01X, #$%02X", "RND", ch8.reg[regX], code[1]);
		   };
		   break;
	case 0xd0:
		   {
			   ch8.reg[regX] = (code[0] & 0x0f);
		 	   ch8.reg[regY] = ((code[1] >> 4) & 0x0f);
			   printf("%-6s V%01X, V%01X, #$%01X", "DRW", ch8.reg[regX], ch8.reg[regY], (code[1] & 0x0f));
		   };
		   break;
	case 0xe0:
		   {
			   if(code[1] == 0x9E)
			   {
			   	ch8.reg[regX] = (code[0] & 0x0f);
			   	printf("%-10s V%01X", "SKP", ch8.reg[regX]);
			   }
			   else if(code[1] == 0xA1)
			   {
			   	ch8.reg[regX] = (code[0] & 0x0f);
			   	printf("%-10s V%01X", "SKNP", ch8.reg[regX]);
			   }
		   };
		   break;
	case 0xf0:
		   {
			if(code[1] == 0x07)
			{
		 		ch8.reg[regX] = (code[0] & 0x0f);
		   		printf("%-6s V%01X, DT", "LD", ch8.reg[regX]);
			}
			if(code[1] == 0x0A)
			{
		 		ch8.reg[regX] = (code[0] & 0x0f);
		   		printf("%-10s V%01X, K", "LD", ch8.reg[regX]);
			}
			if(code[1] == 0x15)
			{
		 		ch8.reg[regX] = (code[0] & 0x0f);
		   		printf("%-6s DT, V%01X,", "LD", ch8.reg[regX]);
			}
			if(code[1] == 0x18)
			{
		 		ch8.reg[regX] = (code[0] & 0x0f);
		   		printf("%-10s ST, V%01X", "LD", ch8.reg[regX]);
			}
			if(code[1] == 0x1E)
			{
		 		ch8.reg[regX] = (code[0] & 0x0f);
		   		printf("%-7s I, V%01X,", "ADD", ch8.reg[regX]);
			}
			if(code[1] == 0x29)
			{
		 		ch8.reg[regX] = (code[0] & 0x0f);
		   		printf("%-7s F, V%01X,", "LD", ch8.reg[regX]);
			}
			if(code[1] == 0x33)
			{
		 		ch8.reg[regX] = (code[0] & 0x0f);
		   		printf("%-7s B, V%01X,", "LD", ch8.reg[regX]);
			}
			if(code[1] == 0x55)
			{
		 		ch8.reg[regX] = (code[0] & 0x0f);
		   		printf("%-5s [I], V%01X,", "LD", ch8.reg[regX]);
			}
			if(code[1] == 0x65)
			{
		 		ch8.reg[regX] = (code[0] & 0x0f);
		   		printf("%-5s V%01X, [I],", "LD", ch8.reg[regX]);
			}

		   };
		   break;
	default:
		   printf("Not handled yet"); break;
	}
}
