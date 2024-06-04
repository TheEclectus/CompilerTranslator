#include "Program.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>

#include "Instructions.h"
#include "ProgramVariable.h"
#include "ProgramLine.h"
#include "ProgramVariableString.h"
#include "ProgramData.h"
#include "Utils.h"

struct Program* Program_New()
{
	struct Program* NewProgram = (struct Program*)malloc(sizeof(struct Program));
	NewProgram->LinesHead = NULL;
	NewProgram->DataLinesHead = NULL;
	NewProgram->EntryHead = NULL;
	NewProgram->ExternHead = NULL;
	NewProgram->ExternUsageHead = NULL;

	return NewProgram;
}

void Program_Delete(struct Program* Mem)
{
	free(Mem);
}

void Program_AddLine(struct Program* Prog, struct ProgramLine* Line)
{
	if (Prog->LinesHead == NULL)
	{
		Prog->LinesHead = Line;
		Line->Address = 100;
	}
	else
	{
		struct ProgramLine* CurLine = Prog->LinesHead;
		while (CurLine->Next != NULL)
		{
			CurLine = CurLine->Next;
		}
		CurLine->Next = Line;
		Line->Address = CurLine->Address + 4;
	}
}

void Program_AddDataLine(struct Program* Prog, struct ProgramLine* Line)
{
	if (Prog->DataLinesHead == NULL)
	{
		Prog->DataLinesHead = Line;
	}
	else
	{
		struct ProgramLine* CurLine = Prog->DataLinesHead;
		while (CurLine->Instruction <= Line->Instruction && CurLine->Next != NULL)
		{
			CurLine = CurLine->Next;
		}

		if (CurLine->Next == NULL)
		{
			CurLine->Next = Line;
		}
		else
		{
			struct ProgramLine* OldNext = CurLine->Next;

			CurLine->Next = Line;
			Line->Next = OldNext;
		}
	}
}

void Program_ProcessLine(struct Program* Prog, const char* Line)
{
	struct ProgramLine* NewLine;
	int WordIndex;
	char* Word;
	long WordLen;
	const char* EntryName;
	const char* EntryNameEnd;
	int EntryNameLen;
	char NameBuf[64];

	const char* ExternName;
	const char* ExternNameEnd;

	int ExternNameLen;

	long LineLen = strlen(Line);
	if (LineLen == 0)
		return;

	Line = SkipWhitespace(Line);
	if (Line == NULL)
		return;

	if (Line[0] == ';')
	{
		return;
	}

	NewLine = ProgramLine_New();

	WordIndex = 0;
	while (Line != NULL)
	{
		Word = NextWord(&Line, " \t");
		if (Word == NULL)
			return;

		WordLen = strlen(Word);
		if (Word[WordLen - 1] == ':')
		{
			if (WordIndex > 0)
			{
				printf("[ERROR] Malformed input: label found out of place.\n");
				exit(-1);
			}
			else
			{
				NewLine->Label = (char*)malloc(sizeof(char) * WordLen);
				memset(NewLine->Label, 0, sizeof(char) * WordLen);
				memcpy(NewLine->Label, Word, WordLen - 1);
			}
		}
		else if (Word[0] == '.')
		{
			if (WordIndex >= 2)
			{
				printf("[ERROR] Malformed input: data declaration found as argument.\n");
				exit(-1);
			}
			else
			{
				if (strcmp(Word, ".entry") == 0)
				{
					free(NewLine);

					EntryName = SkipWhitespace(Line);
					EntryNameEnd = strpbrk(EntryName, " \t");
					EntryNameLen = strlen(EntryName);
					if (EntryNameEnd == NULL)
					{
						EntryNameEnd = EntryName + EntryNameLen;
					}

					memset(NameBuf, 0, sizeof(NameBuf));
					memcpy(NameBuf, EntryName, EntryNameEnd - EntryName);

					Program_AddEntry(Prog, NameBuf);

					break;
				}
				else if (strcmp(Word, ".extern") == 0)
				{
					free(NewLine);

					ExternName = SkipWhitespace(Line);
					ExternNameEnd = strpbrk(ExternName, " \t");
					ExternNameLen = strlen(ExternName);
					if (ExternNameEnd == NULL)
					{
						ExternNameEnd = ExternName + ExternNameLen;
					}

					memset(NameBuf, 0, sizeof(NameBuf));
					memcpy(NameBuf, ExternName, ExternNameEnd - ExternName);

					Program_AddExtern(Prog, NameBuf);

					break;
				}

				if (strcmp(Word, ".asciz") == 0)
				{
					NewLine->Instruction = INST_asciz;
				}
				else if (strcmp(Word, ".db") == 0)
				{
					NewLine->Instruction = INST_db;
				}
				else if (strcmp(Word, ".dh") == 0)
				{
					NewLine->Instruction = INST_dh;
				}
				else if (strcmp(Word, ".dw") == 0)
				{
					NewLine->Instruction = INST_dw;
				}
			}
		}
		else
		{
			int InstructionEnum = InstructionStrToEnum(Word);
			if (InstructionEnum == INST_NONE)
			{
				printf("[ERROR] Malformed input: invalid instruction '%s'.\n", Word);
				exit(-1);
			}
			else
			{
				NewLine->Instruction = InstructionEnum;
			}
		}

		free(Word);
		WordIndex++;

		if (NewLine->Instruction != INST_NONE)
		{
			if (Line == NULL)
				NewLine->Args = "";
			else
			{
				const char* ArgLine = Line + 1;
				long RemainingChars = strlen(ArgLine);
				NewLine->Args = (char*)malloc(sizeof(char) * RemainingChars);
				memset(NewLine->Args, 0, sizeof(char) * RemainingChars);
				strcpy(NewLine->Args, ArgLine);
			}


			if (NewLine->Instruction >= INST_asciz)
				Program_AddDataLine(Prog, NewLine);
			else
				Program_AddLine(Prog, NewLine);

			break;
		}
	}
}

void Program_ProcessDataLines(struct Program* Prog)
{
	struct ProgramLine* ProgramTail;
	int Address;
	struct ProgramLine* CurLine;

	const char* FirstQuote;
	const char* LastQuote;

	struct ProgramData* NewData;

	char CurChar;
	const char* Ch;

	long WordLen;
	const char* WordEnd;

	if (Prog->DataLinesHead == NULL)
		return;

	ProgramTail = Prog->LinesHead;
	while (ProgramTail->Next != NULL)
		ProgramTail = ProgramTail->Next;

	Prog->DataLinesHead->Address = ProgramTail->Address + 4;

	Address = Prog->DataLinesHead->Address;

	CurLine = Prog->DataLinesHead;
	while (CurLine != NULL)
	{
		CurLine->Address = Address;

		if (CurLine->Instruction == INST_asciz)
		{
			FirstQuote = strchr(CurLine->Args, '"');
			LastQuote = strrchr(CurLine->Args, '"');

			if (FirstQuote == NULL || LastQuote == NULL)
			{
				printf("[ERROR] Malformed input: improperly-formed string literal.\n");
				exit(-1);
			}

			Ch = NULL;
			for (Ch = FirstQuote + 1; Ch < LastQuote; Ch++)
			{
				CurChar = *Ch;
				printf("%c\n", CurChar);
				NewData = ProgramData_New(1, CurChar);
				ProgramLine_AddData(CurLine, NewData);

				Address++;
			}
		}
		else
		{
			char CurWord[64];
			int ConversionError;
			int ArgVal;
			int DataSize;

			struct ProgramData* NewData;

			const char* ArgLine = CurLine->Args;
			while (ArgLine != NULL)
			{
				const char* Word = NextWord(&ArgLine, ",");
				if (Word == NULL)
				{
					if (ArgLine[0] == ',')
					{
						Word = ArgLine + 1;
					}
					else
					{
						printf("[ERROR] Malformed input: expected argument.\n");
						exit(-1);
					}
				}

				Word = SkipWhitespace(Word);
				if (Word == NULL)
				{
					printf("[ERROR] Malformed input: empty argument after comma.\n");
					exit(-1);
				}

				WordLen = strlen(Word);
				WordEnd = strpbrk(Word, " \t");
				if (WordEnd == NULL)
				{
					WordEnd = Word + WordLen;
				}

				if (WordLen >= 64)
				{
					printf("[ERROR] Input overflow: maximum argument length of 63 characters (found %d).\n", (int)WordLen);
					exit(-1);
				}

				memset(&CurWord, 0, sizeof(CurWord));
				memcpy(CurWord, Word, WordEnd - Word);

				ConversionError = 0;
				ArgVal = StrToInt(CurWord, &ConversionError);
				if (ConversionError)
				{
					printf("[ERROR] Malformed input: expected numeric literal.\n");
					exit(-1);
				}

				DataSize = 0;
				switch (CurLine->Instruction)
				{
				case INST_db:
					DataSize = 1;
					break;
				case INST_dh:
					DataSize = 2;
					break;
				case INST_dw:
					DataSize = 4;
					break;
				default:
					printf("[ERROR] Malformed input: unknown data instruction %d.\n", CurLine->Instruction);
					exit(-1);
					break;
				}

				NewData = ProgramData_New(DataSize, ArgVal);
				ProgramLine_AddData(CurLine, NewData);

				printf("%d\n", ArgVal);

				Address += DataSize;
			}

		}

		CurLine = CurLine->Next;
	}
}

int Program_FindLabel(struct Program* Prog, const char* Label)
{
	struct ProgramLine* CurLine = Prog->LinesHead;
	while (CurLine != NULL)
	{
		if (CurLine->Label != NULL)
		{
			if (strlen(CurLine->Label) > 0 && strcmp(CurLine->Label, Label) == 0)
				return CurLine->Address;
		}

		CurLine = CurLine->Next;
	}

	CurLine = Prog->DataLinesHead;
	while (CurLine != NULL)
	{
		if (CurLine->Label != NULL)
		{
			if (strlen(CurLine->Label) > 0 && strcmp(CurLine->Label, Label) == 0)
				return CurLine->Address;
		}

		CurLine = CurLine->Next;
	}

	return -1;
}

int Program_IsLabelEntry(struct Program* Prog, const char* Label)
{
	struct ProgramVariableString* CurEntry = Prog->EntryHead;
	while (CurEntry != NULL)
	{
		if (strcmp(Label, CurEntry->Name) == 0)
			return 1;

		CurEntry = CurEntry->Next;
	}

	return 0;
}

int Program_IsLabelExtern(struct Program* Prog, const char* Label)
{
	struct ProgramVariableString* CurExtern = Prog->ExternHead;
	while (CurExtern != NULL)
	{
		if (strcmp(Label, CurExtern->Name) == 0)
			return 1;

		CurExtern = CurExtern->Next;
	}

	return 0;
}

void Program_AddEntry(struct Program* Prog, const char* Label)
{
	if (Prog->EntryHead == NULL)
	{
		Prog->EntryHead = ProgramVariableString_New(Label);
	}
	else
	{
		struct ProgramVariableString* CurEntry = Prog->EntryHead;
		while (CurEntry->Next != NULL)
		{
			CurEntry = CurEntry->Next;
		}
		CurEntry->Next = ProgramVariableString_New(Label);
	}

	printf("[DEBUG] Added entry '%s'\n", Label);
}

void Program_AddExtern(struct Program* Prog, const char* Label)
{
	if (Prog->ExternHead == NULL)
	{
		Prog->ExternHead = ProgramVariableString_New(Label);
	}
	else
	{
		struct ProgramVariableString* CurExtern = Prog->ExternHead;
		while (CurExtern->Next != NULL)
		{
			CurExtern = CurExtern->Next;
		}
		CurExtern->Next = ProgramVariableString_New(Label);
	}

	printf("[DEBUG] Added extern '%s'\n", Label);
}

void Program_AddExternUsage(struct Program* Prog, const char* Label, int Val)
{
	struct ProgramVariable* NewVar = (struct ProgramVariable*)malloc(sizeof(struct ProgramVariable));
	NewVar->Name = (char*)malloc(strlen(Label) + 1);
	memset(NewVar->Name, 0, strlen(Label) + 1);
	strcpy(NewVar->Name, Label);
	NewVar->Value = Val;
	NewVar->Next = NULL;

	if (Prog->ExternUsageHead == NULL)
	{
		Prog->ExternUsageHead = NewVar;
	}
	else
	{
		struct ProgramVariable* CurExtern = Prog->ExternUsageHead;
		while (CurExtern->Next != NULL)
		{
			CurExtern = CurExtern->Next;
		}
		CurExtern->Next = NewVar;
	}

	printf("[DEBUG] Added extern usage '%s' on line %d\n", Label, Val);
}

int Program_GetValue(struct Program* Prog, const char* ValueStr)
{
	int Err = 0;
	int Val = 0;
	printf("Getting value for label '%s'\n", ValueStr);
	if (ValueStr[0] == '$')
	{
		Val = StrToInt(ValueStr + 1, &Err);
		if (Err)
		{
			printf("[ERROR] Malformed input: '%s' is not a valid register.\n", ValueStr);
			exit(-1);
		}
		return Val;
	}
	else
	{
		Val = StrToInt(ValueStr, &Err);
		if (Err)
		{
			if (Program_IsLabelExtern(Prog, ValueStr))
				return 0;
			else
			{
				Val = Program_FindLabel(Prog, ValueStr);
				if (Val == -1)
				{
					printf("[ERROR] Malformed input: '%s' is not a valid label.\n", ValueStr);
					exit(-1);
				}
				return Val;
			}
		}
		return Val;
	}
}

void Program_GenCodes(struct Program* Prog)
{
	struct ProgramLine* CurLine = Prog->LinesHead;
	while (CurLine != NULL)
	{
		int Opcode = 0;
		const int Inst = CurLine->Instruction;
		int ArgNum = 0;
		const char* ArgLine = CurLine->Args;

		int Op = InstructionEnumToOperator(CurLine->Instruction);
		if (Op == -1)
		{
			printf("[ERROR] Generation error: invalid instruction %d\n", CurLine->Instruction);
			exit(-1);
		}

		if (Op == INSTROP_R)
		{
			if (Inst >= INST_move && Inst <= INST_mvlo)
			{
				Opcode = (1 << 26) + ((Inst - (INST_move - 1)) << 6);
			}
			else
			{
				Opcode = (Inst - (INST_add - 1)) << 6;
			}
		}
		else if (Inst >= INST_add && Inst <= INST_sh)
		{
			int OpVal = (Inst - INST_add) + 10;
			Opcode = OpVal << 26;
		}
		else if (Inst >= INST_jmp && Inst <= INST_call)
		{
			int OpVal = (Inst - INST_jmp) + 30;
			Opcode = OpVal << 26;
		}
		else if (Inst == INST_stop)
		{
			Opcode = 63 << 26;
		}

		while (ArgLine != NULL)
		{
			const char* Word = NextWord(&ArgLine, ",");
			long WordLen;
			char CurWord[64];

			if (Word == NULL)
			{
				if (ArgLine[0] == ',')
				{
					Word = ArgLine + 1;
				}
				else
				{
					break;
				}
			}

			Word = SkipWhitespace(Word);
			if (Word == NULL)
			{
				break;
			}

			WordLen = strlen(Word);
			if (WordLen > 0)
			{
				int ArgVal;
				int IsArgLabel;
				int IsArgEntry;
				int IsArgExtern;

				const char* WordEnd = strpbrk(Word, " \t");
				if (WordEnd == NULL)
				{
					WordEnd = Word + WordLen;
				}

				if (WordLen >= 64)
				{
					printf("[ERROR] Input overflow: maximum argument length of 63 characters (found %d).\n", WordLen);
					exit(-1);
				}

				memset(&CurWord, 0, sizeof(CurWord));
				memcpy(CurWord, Word, WordEnd - Word);

				ArgVal = Program_GetValue(Prog, CurWord);
				IsArgLabel = Program_FindLabel(Prog, CurWord);
				if (IsArgLabel == -1)
					IsArgLabel = 0;
				else
					IsArgLabel = 1;
				IsArgEntry = Program_IsLabelEntry(Prog, CurWord);

				IsArgExtern = Program_IsLabelExtern(Prog, CurWord);
				if (IsArgExtern)
				{
					Program_AddExternUsage(Prog, CurWord, CurLine->Address);
				}

				if (IsArgLabel == 1 && IsArgEntry == 0)
				{
					ArgVal = ArgVal - CurLine->Address;
				}

				switch (Op)
				{
				case INSTROP_R:
					if (ArgNum == 0)
						Opcode |= ((ArgVal & 0x1F) << 21);
					else if (ArgNum == 1)
						Opcode |= ((ArgVal & 0x1F) << 16);
					else if (ArgNum == 2)
						Opcode |= ((ArgVal & 0x1F) << 11);
					break;
				case INSTROP_I:
					if (ArgNum == 0)
						Opcode |= ((ArgVal & 0x1F) << 21);
					else if (ArgNum == 1)
						Opcode |= ((ArgVal & 0x1F) << 16);
					else if (ArgNum == 2)
						Opcode |= (ArgVal & 0xFFFF);
					break;
				case INSTROP_J:
					if (ArgNum == 0)
					{
						int IsRegister = CurWord[0] == '$' ? 1 : 0;
						Opcode |= ((ArgVal & IsRegister) << 25);

						Opcode |= ((ArgVal & 0x1FFFFFF));
					}

					break;
				}

				ArgNum++;
			}
		}

		CurLine->Code = Opcode;

		/*print_binary(Opcode);
		printf("\n");*/

		CurLine = CurLine->Next;
	}
}

void Program_PrintCode(struct Program* Prog, const char* InFilename)
{
	char BaseFilename[256];
	memset(BaseFilename, 0, sizeof(BaseFilename));
	char* Period = strrchr(InFilename, '.');
	if (Period == NULL)
		strcpy(BaseFilename, InFilename);
	else
		memcpy(BaseFilename, InFilename, Period - InFilename);

	char ObFilename[256];
	memset(ObFilename, 0, sizeof(ObFilename));
	strcpy(ObFilename, BaseFilename);
	strcat(ObFilename, ".ob");

	char EntFilename[256];
	memset(EntFilename, 0, sizeof(EntFilename));
	strcpy(EntFilename, BaseFilename);
	strcat(EntFilename, ".ent");

	char ExtFilename[256];
	memset(ExtFilename, 0, sizeof(ExtFilename));
	strcpy(ExtFilename, BaseFilename);
	strcat(ExtFilename, ".ext");

	FILE* ObFile = fopen(ObFilename, "w");
	if (ObFile == NULL)
	{
		printf("[ERROR] Couldn't open file '%s' for writing.\n", ObFilename);
		exit(-1);
	}

	{
		{
			struct ProgramLine* CurLine = Prog->LinesHead;
			while (CurLine != NULL)
			{
				unsigned char* Bytes = (unsigned char*)&CurLine->Code;
				fprintf(ObFile, "%04d %02X %02X %02X %02X\n", CurLine->Address, Bytes[0], Bytes[1], Bytes[2], Bytes[3]);

				CurLine = CurLine->Next;
			}
		}

		{
			struct ProgramLine* CurDataLine = Prog->DataLinesHead;

			int AddressStart = CurDataLine->Address;
			int AddressOffset = 0;

			while (CurDataLine != NULL)
			{
				struct ProgramData* CurData = CurDataLine->Data;
				while (CurData != NULL)
				{
					unsigned char* Bytes = (unsigned char*)&CurData->Value;
					int CurDataByte = 0;
					for (CurDataByte = 0; CurDataByte < CurData->Len; CurDataByte++)
					{
						if (AddressOffset % 4 == 0)
						{
							fprintf(ObFile, "%04d ", AddressStart + AddressOffset);
						}
						fprintf(ObFile, "%02X ", Bytes[CurDataByte]);
						AddressOffset++;
						if (AddressOffset % 4 == 0)
							fprintf(ObFile, "\n");
					}

					CurData = CurData->Next;
				}


				CurDataLine = CurDataLine->Next;
			}
		}
	}
	fclose(ObFile);

	FILE* EntFile = fopen(EntFilename, "w");
	if (EntFile == NULL)
	{
		printf("[ERROR] Couldn't open file '%s' for writing.\n", EntFilename);
		exit(-1);
	}

	{
		struct ProgramVariableString* CurEntry = Prog->EntryHead;
		while (CurEntry != NULL)
		{
			fprintf(EntFile, "%s %04d\n", CurEntry->Name, Program_GetValue(Prog, CurEntry->Name));

			CurEntry = CurEntry->Next;
		}
	}
	fclose(EntFile);

	FILE* ExtFile = fopen(ExtFilename, "w");
	if (ExtFile == NULL)
	{
		printf("[ERROR] Couldn't open file '%s' for writing.\n", ExtFilename);
		exit(-1);
	}

	{
		struct ProgramVariable* CurExtern = Prog->ExternUsageHead;
		while (CurExtern != NULL)
		{
			fprintf(ExtFile, "%s %04d\n", CurExtern->Name, CurExtern->Value);

			CurExtern = CurExtern->Next;
		}
	}
	fclose(ExtFile);
}