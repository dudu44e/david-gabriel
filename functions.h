#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define DEFAULT_SIZE 50
#define MAX_NUMBER_8BIT (pow(2,7)-1)
#define MIN_NUMBER_8BIT (-1*(pow(2,7)-1))
#define MAX_NUMBER_16BIT (pow(2,15)-1)
#define MIN_NUMBER_16BIT (-1*(pow(2,15)-1))
#define MAX_NUMBER_32BIT (pow(2,31)-1)
#define MIN_NUMBER_32BIT (-1*(pow(2,31)-1))

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
typedef struct directive{
	int dh:16;
	int dw:32;
	int db:8;
	unsigned int asciz:8;
}directiveC; /*struct for directive commands*/
typedef struct bitsR{
	unsigned int opcode:6;
	unsigned int rs:5;
	unsigned int rt:5;
	unsigned int rd:5;
	unsigned int funct:5;
	unsigned int notUsed:6;
}bitR;/*struct for R commands*/
typedef struct bitsI{
	unsigned int opcode:6;
	unsigned int rs:5;
	unsigned int rt:5;
	unsigned int immed:16;
}bitI;/*struct for I commands*/
typedef struct bitsJ{
	unsigned int opcode:6;
	unsigned int reg:1;
	unsigned int addressValue:25;	
}bitJ;/*struct for J commands*/
typedef struct node *ptr;
typedef struct node{
	directiveC a1;
	bitR b1;
	bitI b2;
	bitJ b3;
	char *labelName;
	char *headline;/*command name*/
	int commandAddress;
	int increaseAdd;/*the size the command takes in memory*/
	int expectedLabel;/*flag that checks if label is expected,this flag is for 2nd run*/
	ptr next;
	
} dataMap;/*struct for data list*/
typedef struct node2 *ptrS;
typedef struct node2{
	char * symbol;
	int value;
	char* attributes;
	ptrS next;
	
} symbolMap;/*struct for symbol list*/

/*
* checks if file type input is legal or not.
* INPUT : file name
* OUTPUT : 1 if file type is legal
*          0 if file type illegal
*/
int fileType(char *);
/*
* analyze file content.
* INPUT : none
* OUTPUT : none
*/
void findCommand();
/*
* analyze string after .asciz command and add letters as node to data linked list.
* INPUT : line from file
* OUTPUT : skipTokens - how many words to skip in order to procced to the next command/label

*/
int readAscii(char *);
/*
* add commands and their data to data list.
* INPUT : node - data node
*	  expectedLabel - flag that represents if there was a label before the command
* OUTPUT : none
*/
void addToList(ptr,int);
/*
* add labels and their attributes to symbol list.
* INPUT : node - given label
* OUTPUT : none
*/
void addToSymbolList(ptrS);
/*
* checks if the label is legal, checks if reserved word.
* INPUT : label - the label given
* OUTPUT : 1 if file type is legal
*          0 if file type illegal
*/
int isLegalLabel(char *);
/*
* adds data that has not been entered in first run to symbol list and data list.
* INPUT : none
* OUTPUT : none
*/
void secondRun();
/*
* creates  .ob,.ent,.ext files from data list and symbol list after the second run.
* INPUT : file name
* OUTPUT : none
*/
void createFiles(char *);
/*
* checks if the given words are equal
* INPUT : first - first word
*	  second - second word
* OUTPUT : 1 if words are not equal
*          0 if words are equal
*/
int equalWords (char *, char *);
/*
* takes a data field convert to binary string and append to it a given string.
* INPUT : part - string to append to
*	  num -	number to convert to binary string
*	  iterations - how many bits to represent the number as binary
* OUTPUT : none
*/
void convertToBinary(char *, int, int);
/*
* takes a number and convert to binary string in two complement form.
* INPUT : part - string to convert to
*	  num -	given number to convert
*	  iterations - how many bits to represent the number as binary
* OUTPUT : none
*/
void convertTwosComplement(char *, int, int);
/*
* free all the memory from symbol list and data list after each file.
* INPUT : none
* OUTPUT : none
*/
void freeMemory();

extern FILE * fp; /*file pointer*/
extern int finalErrorFlag;/*flag that checks if there were errors and if files needs to be created*/
extern int address;/*command address*/
extern int lineCounter;/*counts line in file*/
extern ptr hData;/*header for data map*/
extern ptrS hSymbol;/*header for symbol map*/
extern int DCF;/*data counter*/
extern int ICF;/*instruction command counter*/
extern int updateAddressFlag;/*flag for checking if address need to be updated to label*/

#endif
