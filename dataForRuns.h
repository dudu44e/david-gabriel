#ifndef DATAFORRUNS_H
#define DATAFORRUNS_H
/*R commands, I commands and J commands as an array(instruction commands)*/
static char *RIJ[] =
{"add",
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
"stop"};

static char *directiveCommands[]=
{".db",
".dw",
".dh",
".asciz",
".entry",
".extern"};/*directive commands*/

#endif
