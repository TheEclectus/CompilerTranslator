#pragma once

enum Instruction
{
	INST_NONE,

	/* R	[ 6 opcode | 5 reg1 | 5 reg2 | 5 reg3 | 5 funct | 6 NULL ]*/
	INST_add,
	INST_sub,
	INST_and,
	INST_or,
	INST_nor,
	INST_move,
	INST_mvhi,
	INST_mvlo,

	/* I	[ 6 opcode | 5 reg1 | 5 reg2 | 16 reg3 ]*/
	INST_addi,
	INST_subi,
	INST_andi,
	INST_ori,
	INST_nori,
	INST_bne,
	INST_beq,
	INST_blt,
	INST_bgt,
	INST_lb,
	INST_sb,
	INST_lw,
	INST_sw,
	INST_lh,
	INST_sh,

	/* J	[ 6 opcode | 1 reg | 25 address ]*/
	INST_jmp,
	INST_la,
	INST_call,
	INST_stop,		/* stop is special, no argument, address is filled with 0s*/

	/* Data*/
	INST_asciz,
	INST_db,
	INST_dh,
	INST_dw
};

enum InstructionOperators
{
	INSTROP_R,
	INSTROP_I,
	INSTROP_J
};

int InstructionEnumToOperator(int Instr);
int InstructionStrToEnum(const char* Str);

extern const char* InstructionStrs[];