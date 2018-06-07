#include "chip8.h"

int main(int argc, char** argv)
{
	chip8 ch8Att;
	int fileSize;
	FILE* chip8_Rom = fopen(argv[1], "rb");
	if (chip8_Rom == NULL)
	{
		puts("Error: File %s could not be opened", argv[1]);
	}

	fseek(chip8_Rom, 0, SEEK_END);
	fileSize = ftell(chip8_Rom);
	fseek(chip8_Rom, 0, SEEK_SET);

	uint8_t* buffer = (char*)malloc(fileSize);
	fread(buffer, fileSize, 1, chip8_Rom);

	ch8Att.pc = 0x00;
	while (ch8Att.pc < fileSize)
	{
		ch8Dissassemble(buffer, ch8Att.pc);
		ch8Att.pc += 2;
		puts("");
	}
	
	fclose(chip8_Rom);
	free(buffer);

	system("PAUSE");
	return 0;
}