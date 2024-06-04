#include "ProgramVariableString.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

struct ProgramVariableString* ProgramVariableString_New(const char* Name)
{
	struct ProgramVariableString* NewVar = (struct ProgramVariableString*)malloc(sizeof(struct ProgramVariableString));
	NewVar->Name = (char*)malloc(sizeof(char) * (strlen(Name) + 1));
	memset(NewVar->Name, 0, sizeof(char) * (strlen(Name) + 1));
	strcpy(NewVar->Name, Name);
	NewVar->Next = NULL;
	return NewVar;
}