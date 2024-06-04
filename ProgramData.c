#pragma once

#include "ProgramData.h"

#include <malloc.h>
#include <memory.h>

struct ProgramData* ProgramData_New(int Len, int Val)
{
	struct ProgramData* NewProgramData = (struct ProgramData*)malloc(sizeof(struct ProgramData));
	NewProgramData->Next = NULL;
	NewProgramData->Len = Len;
	NewProgramData->Value = Val;
	return NewProgramData;
}