#include "functions.h"
#include "dataForRuns.h"

/*checks if two digit or 1 digit for hexa base and 0 in start*/
int howManyDigits(char * block){
	int y =atoi(block);
	if (y<=16){
		return 1;
	}
	return 0;
	
}
void createFiles(char * fileName){
	FILE *entFile;/*pointer for entry file*/
	FILE *extFile;/*pointer for external file*/
	FILE *obFile;/*pointer for object file*/
	int printLineAddress=100;
	char * entryFileEnding=".ent";
	char * externFileEnding=".ext";
	char * binaryFileEnding=".ob";
	char  entryFileName[40];
	char  externFileName[40];
	char  binaryFileName[40];
	strcpy(binaryFileName,fileName);/*saves the binary file name*/
	strcat(binaryFileName,binaryFileEnding);/*add the ending to binary file name*/
	obFile=fopen(binaryFileName,"w");/*create file*/
	ptrS symbolTemp;/*pointer for symbol list*/
	symbolTemp =hSymbol;
	ptr dataTemp;/*pointer for data list*/
	dataTemp= hData;
	while(symbolTemp!=NULL){/*checks if there is extern and if so creates it*/
		if(equalWords(symbolTemp->attributes,"extern")==0){
			strcpy(externFileName,fileName);
			strcat(externFileName,externFileEnding);
			extFile=fopen(externFileName,"w");
			break;
		}
		symbolTemp = symbolTemp->next;
	}
	symbolTemp = hSymbol;/*reset symbol temp*/
	while(symbolTemp!=NULL){/*checks if there is entry and if so creates it*/
		if(equalWords(symbolTemp->attributes,"entry")==0){
			strcpy(entryFileName,fileName);
			strcat(entryFileName,entryFileEnding);
			entFile=fopen(entryFileName,"w");
			break;
		}
		symbolTemp = symbolTemp->next;
	}
	symbolTemp = hSymbol;/*reset symbol temp*/
	ptrS symbolTemp2;
	while(symbolTemp!=NULL&&entFile!=NULL){ /*create entry file*/
		if(equalWords(symbolTemp->attributes,"entry")==0){
			symbolTemp2 = hSymbol;
			while(symbolTemp2!=NULL){
				if(equalWords(symbolTemp2->symbol,symbolTemp->symbol)==0&&equalWords(symbolTemp2->attributes,"code")==0){
					fprintf(entFile,"%s %04d \n",symbolTemp->symbol,symbolTemp2->value);
				}
				symbolTemp2=symbolTemp2->next;
			}
		}
		if(symbolTemp->next==NULL)
			fclose(entFile);
		symbolTemp = symbolTemp->next;
	}
	symbolTemp=hSymbol;/*reset symbol temp*/
	fprintf(obFile,"     %d %d",ICF,DCF);/*print first row icf dcf*/
	while(dataTemp!=NULL){/*part of function for R*/
		int i;
		int flag=-1;/*flag to RIJ*/
		int locationInTemp=24;/*last token location*/
		char temp[33]="";/*command */
		char block[9];/*block of 8 digits from command*/
		for(i=0;i<27;i++){/*checks if command is RIJ*/
		 	if(equalWords(dataTemp->headline,RIJ[i])==0){
				if(i<8){
					flag=1;
				}
				else if(i>7&&i<23){
					flag=2;
				}
				else{
					flag=3;
				}
			}
		}
		if(flag==1){/*R commands*/
		/*creates a binary string from all relevant fields*/
		convertToBinary(temp,dataTemp->b1.opcode,6);
		convertToBinary(temp,dataTemp->b1.rs,5);
		convertToBinary(temp,dataTemp->b1.rt,5);
		convertToBinary(temp,dataTemp->b1.rd,5);
		convertToBinary(temp,dataTemp->b1.funct,5);
		convertToBinary(temp,dataTemp->b1.notUsed,6);
		fprintf(obFile,"\n%04d ",printLineAddress);
		printLineAddress+=4;
		}
		else if(flag==2){/*I commands*/
		/*creates a binary string from all relevant fields*/
		convertToBinary(temp,dataTemp->b2.opcode,6);
		convertToBinary(temp,dataTemp->b2.rs,5);
		convertToBinary(temp,dataTemp->b2.rt,5);
		convertToBinary(temp,dataTemp->b2.immed,16);
		fprintf(obFile,"\n%04d ",printLineAddress);
		printLineAddress+=4;
		}
		
		else if (flag==3){/*J commands*/
		/*creates a binary string from all relevant fields*/
		convertToBinary(temp,dataTemp->b3.opcode,6);
		convertToBinary(temp,dataTemp->b3.reg,1);
		convertToBinary(temp,dataTemp->b3.addressValue,25);
		fprintf(obFile,"\n%04d ",printLineAddress);
		printLineAddress+=4;
		}
		else{
			dataTemp=dataTemp->next;
			continue;
		}
		for(i=1;i<5;i++){/*divide binary string of each command to 8bits blocks and print it to file in hexadecimal form*/
			if(locationInTemp==-1)
				locationInTemp=0;
			int intRepOfBlock;
			strncpy(block,&temp[locationInTemp],8);/*insert to block 8 digits*/
			block[8]='\0';/*end block*/
			intRepOfBlock=(int)strtol(block,NULL,2);
			fprintf(obFile,"%02X ",intRepOfBlock);/*print in file*/
			locationInTemp=locationInTemp -8;/*progress the scanning*/
			
		}
		dataTemp=dataTemp->next;
	}
	dataTemp=hData;/*reset dataTemp*/
	if(printLineAddress%4==0){/*start a new line in ob file if current line is already full with 4 bytes*/
		fprintf(obFile,"\n%04d ",printLineAddress);
	}
	while(dataTemp!=NULL){
		int j;
		int directiveFlag=-1;                                       	
		for(j=0;j<6;j++){/*checks if command is legal*/
			if(equalWords(dataTemp->headline,directiveCommands[j])==0){
				if(j==0||j==3){/*print line address +1  asciz db*/
					directiveFlag =1;
				}
				if(j==1){/*+4 dw*/
					directiveFlag =3;
				}
				if(j==2){/*+2 dh*/
					directiveFlag =2;
				}
			}
		}
		if(directiveFlag==1){/*for asciz*/
			if(equalWords(".asciz",dataTemp->headline)==0){
					fprintf(obFile,"%02X ",dataTemp->a1.asciz);
			}
			else{/*for db*/
				char dbBinary[9]="";
				if(dataTemp->a1.db<0){
				convertToBinary(dbBinary,abs(dataTemp->a1.db),8);
				convertTwosComplement(dbBinary,dataTemp->a1.db,8);
				}
				else{
					convertToBinary(dbBinary,dataTemp->a1.db,8);
				}
				fprintf(obFile,"%02X ",strtol(dbBinary,NULL,2));
			}
			printLineAddress+=1;
			if(printLineAddress%4==0){/*check if we need to start new line in ob file*/
				fprintf(obFile,"\n%04d ",printLineAddress);
			}
		}
		
		if(directiveFlag==2){/*for dh*/
			/*char part[17];*/
			char dhBinary[17]="";
			int intRepOfBlock;
			char block[9];
	
			if(dataTemp->a1.dh<0){
				convertToBinary(dhBinary,abs(dataTemp->a1.dh),16);
				convertTwosComplement(dhBinary,dataTemp->a1.dh,16);
			}
			else{
				convertToBinary(dhBinary,dataTemp->a1.dh,16);
			}
			strncpy(block,&dhBinary[8],8);/*insert to block 8 digits*/
			block[8]='\0';/*end block*/
			intRepOfBlock=(int)strtol(block,NULL,2);
			
			
			fprintf(obFile,"%02X ",intRepOfBlock);
			printLineAddress++;
			
			if(printLineAddress%4==0){/*check if we need to start new line in ob file*/
				fprintf(obFile,"\n%04d ",printLineAddress);
			}
			
			
			memcpy(block,dhBinary,8);/*take the first 8 letters from dhBinary and save them in dhBinary8bits*/
			
			intRepOfBlock=(int)strtol(block,NULL,2);
			
			fprintf(obFile,"%02X ",intRepOfBlock);
			printLineAddress++;
			if(printLineAddress%4==0){/*check if we need to start new line in ob file*/
				fprintf(obFile,"\n%04d ",printLineAddress);
			}
		}
		if(directiveFlag==3){/*dw*/
			int i;
			char dwBinary[33]="";
			char block[9];
			int locationInDw=24;
			if(dataTemp->a1.dw<0){
				convertToBinary(dwBinary,abs(dataTemp->a1.dw),32);
				convertTwosComplement(dwBinary,dataTemp->a1.dw,32);
			}
			else{
				convertToBinary(dwBinary,dataTemp->a1.dw,32);
			}
			for(i=1;i<5;i++){/*divide binary string of each command to 8bits blocks and print it to file in hexadecimal form*/
				if(locationInDw==-1)
					locationInDw=0;
				int intRepOfBlock;
				strncpy(block,&dwBinary[locationInDw],8);/*insert to block 8 digits*/
				block[8]='\0';/*end block*/
				intRepOfBlock=(int)strtol(block,NULL,2);
				fprintf(obFile,"%02X ",intRepOfBlock);/*print in file*/
				locationInDw=locationInDw -8;/*progress the scanning*/
				printLineAddress++;
				if(printLineAddress%4==0){
					fprintf(obFile,"\n%04d ",printLineAddress);
				}
			}
		}
		dataTemp=dataTemp->next;
	}
	dataTemp=hData;/*reset dataTemp*/
	symbolTemp = hSymbol;/*reset symbolTemp*/
		while(symbolTemp!=NULL&&extFile!=NULL){/*write data to extern file*/
			if(equalWords(symbolTemp->attributes,"extern")!=0){
				symbolTemp=symbolTemp->next;
				continue;
			}
			else{
				dataTemp=hData;
				while(dataTemp!=NULL){
					if(dataTemp->labelName!=NULL&&equalWords(dataTemp->labelName,symbolTemp->symbol)==0){
						fprintf(extFile,"%s %d\n",symbolTemp->symbol,dataTemp->commandAddress);
					}
					dataTemp=dataTemp->next;
				}
			}
		
			symbolTemp=symbolTemp->next;
		}
		fclose(extFile);/*close the file*/
		symbolTemp = hSymbol;/*reset symbolTemp*/
		dataTemp= hData;/*reset dataTemp*/
	}	

