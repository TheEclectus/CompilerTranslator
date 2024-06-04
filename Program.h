#pragma once

struct ProgramLine;
struct ProgramVariableString;
struct ProgramVariable;

/*
	Maybe ProgramLine should be {Label, instruction, args, code}? Then they could be shuffled around to
	match with how the final output would be before

	Program sequence:
	First pass:
		- Every text line is iterated through, empty lines being ignored
		- .entrys, .externs, .db/h/w and labels are parsed, labels on data
			- .entrys and labels are distinct: The EntryHead list exists to r
		- instructions converted to enumeration
		- textual arguments (everything after whitespace to right of instruction) copied + saved
		- labels on data (.asciz, .db/h/w) updated as variables are assigned final offsets
	Second pass:
		- Lines arguments split by comma, whitespace removal, etc.
			- Registers (e.g. $4) replaced with their value ($4 = 4 = 0100)
			- Raw values are replaced with their value
			- .externs are replaced with 0
			- .entrys are replaced with their line value
			- labels are replaced with their line value
		- If appropriate number of arguments found, convert to final machine code and store
*/
struct Program
{
	struct ProgramLine* LinesHead;
	/*
		Addresses will be calculated after all lines are read
		Lines will be ordered into sections as they are added, in the order they are listed:
			.asciz
			.db
			.dh
			.dw
	*/
	struct ProgramLine* DataLinesHead;
	struct ProgramVariableString* EntryHead;

	struct ProgramVariableString* ExternHead;

	struct ProgramVariable* ExternUsageHead;
};

struct Program* Program_New();
void Program_Delete(struct Program* Mem);
void Program_AddLine(struct Program* Prog, struct ProgramLine* Line);
/* Doesn't set address, and sorts/places*/
void Program_AddDataLine(struct Program* Prog, struct ProgramLine* Line);
void Program_ProcessLine(struct Program* Prog, const char* Line);
/* End of 1st pass, assigns addresses to data lines and expands data into ProgramLine->Data list*/
void Program_ProcessDataLines(struct Program* Prog);
/* Returns label address, -1 on failure to find*/
int Program_FindLabel(struct Program* Prog, const char* Label);
int Program_IsLabelEntry(struct Program* Prog, const char* Label);
int Program_IsLabelExtern(struct Program* Prog, const char* Label);
void Program_AddEntry(struct Program* Prog, const char* Label);
void Program_AddExtern(struct Program* Prog, const char* Label);
void Program_AddExternUsage(struct Program* Prog, const char* Label, int Val);
int Program_GetValue(struct Program* Prog, const char* ValueStr);
void Program_GenCodes(struct Program* Prog);
void Program_PrintCode(struct Program* Prog, const char* InFilename);