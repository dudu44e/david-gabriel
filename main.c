/*
 * Maman14 - course 20465 - b2021
 * AUTHORS: David (Elgrabli) Gabriel (305529554) and Lior Guetta (318985520).
 * DATE: 15/08/2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

FILE * fp;
int finalErrorFlag;
int address=100;
int lineCounter =0;/*counter for the lines in each file*/
int DCF=0;/*directive commands counter*/
int ICF=0;/*instruction commands counter*/
ptr hData=NULL;/*header for data map*/
ptrS hSymbol = NULL;/*header for symbol map*/
int updateAddressFlag=0;/*flag that indicates if we need to update the address in the second run for data nodes*/
int main(int argc, char *argv[]) {
	int i;
	char filename[40];
	for(i=1;i<argc;i++){
		fp = fopen(argv[i],"r");
		if(fp!=NULL){
			if(fileType(argv[i]) ==1){ /*checks if type file is legal*/
					printf("\ntype of file illegal\n");
					continue;
				}
					findCommand();/*read file and create data and symbol list*/
					if(finalErrorFlag!=1){
						secondRun();/*complete the data information in list*/
						memcpy(filename,argv[i],strlen(argv[i])-3);
						filename[strlen(argv[i])-3]='\0';
						createFiles(filename);
						lineCounter=0;
					}
					else{
						finalErrorFlag=0;
						lineCounter=0;
					}
				freeMemory();/*free memory for next file it there is any*/
			}
		else
			printf("\nerror, couldn't open file\n");
	
	}

	return 0;

}
