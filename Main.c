#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Program.h"
#include "ProgramLine.h"
#include "Instructions.h"

/*
	JMP, LA is absolute

	ENTRY POINTS ARE ABSOLUTE
	labels are relative!

	BNE, BEQ, BLT, BGT is a relative jump address
	JMP register will be 0 if value passed isn't a register

	externs count as 0

	.asciz, .db, .dh, .dw put after end of program in that order
	.db - bytes
	.dh - words (2bytes)
	.dw - double word (4bytes)
*/

void print_binary(unsigned int number)
{
	if (number >> 1) {
		print_binary(number >> 1);
	}
	putc((number & 1) ? '1' : '0', stdout);
}

void Usage()
{
	printf("asm.exe [file1.as] [file2.as] ... [fileN.as]\n");
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		Usage();
		return 0;
	}

    int i = 0;
	for (i = 1; i < argc; i++)
	{
		const char* CurFilename = argv[i];
		FILE* CurFile = fopen(CurFilename, "r");
		if (CurFile == NULL)
		{
			printf("[ERROR] Couldn't open file '%s' for reading, skipping...\n", CurFilename);
			return -1;
		}

		fseek(CurFile, 0L, SEEK_END);
		long CurFileSize = ftell(CurFile);
		rewind(CurFile);

		char* TextBuffer = (char*)malloc(sizeof(char) * (CurFileSize + 1));
		
		long ReadBytes = fread(TextBuffer, 1, CurFileSize, CurFile);
		fclose(CurFile);
		if (ReadBytes != CurFileSize)
		{
			printf("[ERROR] Size mismatch! File size %d bytes, only read %d.\n", CurFileSize, ReadBytes);
			return -1;
		}
		
		TextBuffer[ReadBytes] = 0;

		struct Program* CurProgram = Program_New();
		
		char* CurrentLine = strtok(TextBuffer, "\n");
		while (CurrentLine != NULL)
		{
			Program_ProcessLine(CurProgram, CurrentLine);
			CurrentLine = strtok(NULL, "\n");
		}

		struct ProgramLine* CurProgLine = CurProgram->LinesHead;
		while (CurProgLine != NULL)
		{
			printf("%s\t%s %s\n", CurProgLine->Label == NULL ? "    " : CurProgLine->Label, InstructionStrs[CurProgLine->Instruction - 1], CurProgLine->Args);
			CurProgLine = CurProgLine->Next;
		}

		Program_ProcessDataLines(CurProgram);

		Program_GenCodes(CurProgram);

		Program_PrintCode(CurProgram, CurFilename);
	}

	return 0;
}