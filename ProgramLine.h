#pragma once

struct ProgramData;

struct ProgramLine
{
	struct ProgramLine* Next;

	int Instruction;
	char* Args;
	char* Label;
	int Address;
	struct ProgramData* Data;

	int Code;
};

struct ProgramLine* ProgramLine_New();
void ProgramLine_AddData(struct ProgramLine* Line, struct ProgramData* Data);