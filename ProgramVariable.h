#pragma once

struct ProgramVariable
{
	struct ProgramVariable* Next;

	char* Name;
	int Value;
};