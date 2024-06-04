#include "Instructions.h"

#include <memory.h>
#include <string.h>

int InstructionEnumToOperator(int Instr)
{
	if (Instr >= INST_add && Instr <= INST_mvlo)
		return INSTROP_R;
	else if (Instr >= INST_addi && Instr <= INST_sh)
		return INSTROP_I;
	else if (Instr >= INST_jmp && Instr <= INST_stop)
		return INSTROP_J;
	else
		return -1;
}

int InstructionStrToEnum(const char* Str)
{
	int i = 0;
	for (i = 0; i <= INST_stop; i++)
	{
		if (strcmp(Str, InstructionStrs[i]) == 0)
			return i + 1;
	}

	return INST_NONE;
}

/* Instruction enum val - 1 = index*/
const char* InstructionStrs[] = {
	"add",
	"sub",
	"and",
	"or",
	"nor",
	"move",
	"mvhi",
	"mvlo",
	"addi",
	"subi",
	"andi",
	"ori",
	"nori",
	"bne",
	"beq",
	"blt",
	"bgt",
	"lb",
	"sb",
	"lw",
	"sw",
	"lh",
	"sh",
	"jmp",
	"la",
	"call",
	"stop",

	NULL
};