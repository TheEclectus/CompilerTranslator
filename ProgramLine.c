#include "ProgramLine.h"

#include <malloc.h>
#include <memory.h>

#include "Instructions.h"
#include "ProgramData.h"

struct ProgramLine* ProgramLine_New()
{
	struct ProgramLine* NewProgramLine = (struct ProgramLine*)malloc(sizeof(struct ProgramLine));
	NewProgramLine->Instruction = 0;
	NewProgramLine->Next = INST_NONE;
	NewProgramLine->Code = 0;
	NewProgramLine->Address = 0;
	NewProgramLine->Args = NULL;
	NewProgramLine->Label = NULL;
	NewProgramLine->Data = NULL;
	return NewProgramLine;
}

void ProgramLine_AddData(struct ProgramLine* Line, struct ProgramData* Data)
{
	if (Line->Data == NULL)
		Line->Data = Data;
	else
	{
		struct ProgramData* DataTail = Line->Data;
		while (DataTail->Next != NULL)
			DataTail = DataTail->Next;

		DataTail->Next = Data;
	}
}