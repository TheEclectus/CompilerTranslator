#pragma once

struct ProgramVariableString
{
	struct ProgramVariableString* Next;

	char* Name;
};

struct ProgramVariableString* ProgramVariableString_New(const char* Name);