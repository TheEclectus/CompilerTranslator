# CompilerTranslator
An assembly-to-machine code translator I created many years ago.

Compiled using `gcc -Wall -ansi -pedantic -o asm *.c`.

Inputs:
  - One or more .as files, representing GCC-like assembly language sources.
Outputs:
  - A .ent file describing the entrypoints of the program
  - A .ext file describing the externs of the program
  - An .ob file representing the assembly converted into machine code.
