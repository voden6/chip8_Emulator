#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
#define clearDisplay 0x00E0
#define sub_return 0x00EE
#define jmp_addr 0x10
#define call_subr 0x20
#define regEQNN 0x30
#define regNENN 0x40
#define setRegNN 0x50
*/

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

void ch8Dissassemble(uint8_t* buffer, int pc, chip8 ch8);

int main(int argc, char** argv)
{
	if(argc < 1)
	{
		printf("You did not enter a chip8 file to dissassemble");
	}
	chip8 ch8;
	int fileSize;
	FILE* chip8_Rom = fopen(argv[1], "rb");
	if (chip8_Rom == NULL)
	{
		printf("Error: File %s could not be opened", argv[1]);
	}

	fseek(chip8_Rom, 0, SEEK_END);
	fileSize = ftell(chip8_Rom);
	fseek(chip8_Rom, 0, SEEK_SET);

	uint8_t* buffer = (char*)malloc(fileSize);
	fread(buffer, fileSize, 1, chip8_Rom);

	ch8.pc = 0x0;
	while (ch8.pc < fileSize)
	{
		ch8Dissassemble(buffer, ch8.pc, ch8);
		ch8.pc += 2;
		puts("");
	}
	
	fclose(chip8_Rom);
	free(buffer);

	system("PAUSE");
	return 0;
}

void ch8Dissassemble(uint8_t* buffer, int pc, chip8 ch8)
{
	uint8_t* code = &buffer[pc];
	uint8_t firstNib = (code[0] & 0xf0);
        uint8_t regNum = (code[0] & 0x0f);

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
				 printf("%-10s #$%01X%02X", "SYS", code[0] & 0x0f, code[0]); break;
			 }
		   }
	case 0x10: 
		   {
			 printf("%-10s #$%01X%02X", "JMP", code[0] & 0x0f , code[1]);	  
		   };
		   break;
	case 0x20: {
			 printf("%-10s #$%01X%02X", "CALL", code[0] & 0x0f, code[1]); 
		   };
		   break;
	case 0x30: 
		   {
			   ch8.reg[regNum] = (code[0] & 0x0f);
			   printf("%-10s V%01X, #$%02X", "SE", ch8.reg[regNum], code[1]);
		   };
		   break;
	case 0x40: 
		   {
			   ch8.reg[regNum] = (code[0] & 0x0f);
			   printf("%-10s V%01X, #$%02X", "SNE", ch8.reg[regNum], code[1]);
		   };
		   break;
	case 0x50: 
		   {
			   ch8.reg[regnum] = (code[0] & 0x0f);
			   printf("%-10s V%01x, V%01x", "SE",  );
		   };
		   break;
	case 0x60:
		   {
			   ch8.reg[regNum] = (code[0] & 0x0f);
			   printf("%-10s V%01X, #$%02X", "LD", ch8.reg[regNum], code[1]);
		   };
		   break;
	case 0x70: printf("7 not handled yet") ; break;
	case 0x80: printf("8 not handled yet") ; break;
	case 0x90: printf("9 not handled yet") ; break;
	case 0xa0: printf("a not handled yet") ; break;
	case 0xb0: printf("b not handled yet") ; break;
	case 0xc0: printf("c not handled yet") ; break;
	case 0xd0: printf("d not handled yet") ; break;
	case 0xe0: printf("e not handled yet") ; break;
	case 0xf0: printf("f not handled yet") ; break;

	}
}
