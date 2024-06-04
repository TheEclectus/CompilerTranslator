#pragma once

struct ProgramData
{
	struct ProgramData* Next;

	int Len;
	int Value;
};

struct ProgramData* ProgramData_New(int Len, int Val);