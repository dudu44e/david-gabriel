#include "functions.h"
#include "dataForRuns.h"

void secondRun(){
	ptrS symbolPtr = hSymbol;
	ptr dataPtr = hData;
	while(symbolPtr!=NULL){/*update every node in data list that attached to symbol which is extern attribute in the original file to 0 in address value*/
		if(equalWords(symbolPtr->attributes,"extern")==0){
			dataPtr= hData;
			while(dataPtr!=NULL){
				if(dataPtr->labelName!=NULL&&equalWords(symbolPtr->symbol,dataPtr->labelName)==0){
					dataPtr->b3.addressValue=0;
				}
				dataPtr = dataPtr->next;
			}
		}
		symbolPtr=symbolPtr->next;
	}
	symbolPtr= hSymbol;
	dataPtr = hData;
	while(symbolPtr!=NULL){/*update every node in data list that expects label*/
		if(equalWords(symbolPtr->attributes,"extern")==0||equalWords(symbolPtr->attributes,"entry")==0){
			symbolPtr->value=0;
			symbolPtr= symbolPtr->next;
			continue;
		}
		if(symbolPtr->value>=100){
			symbolPtr=symbolPtr->next;
			continue;
		}
		while(dataPtr!=NULL){
			if((dataPtr->commandAddress)>=100){
				dataPtr=dataPtr->next;
				continue;
			}
			if(dataPtr->commandAddress<100&&dataPtr->expectedLabel==0){
				dataPtr->commandAddress=address;
				address+=dataPtr->increaseAdd;
				dataPtr=dataPtr->next;
				continue;
			}
			if(dataPtr->commandAddress<100&&dataPtr->expectedLabel==1){
				symbolPtr->value = address;
				dataPtr->commandAddress=address;
				address+=dataPtr->increaseAdd;
				while(equalWords(dataPtr->headline,dataPtr->next->headline)==0){
					dataPtr->next->commandAddress=address;
					address+=dataPtr->increaseAdd;
					dataPtr=dataPtr->next;
				}
			}
			dataPtr=hData;
			break;
		}
		symbolPtr=symbolPtr->next;
	}
	symbolPtr = hSymbol;
	dataPtr = hData;
	while(dataPtr!=NULL){
		if(dataPtr->commandAddress<100&&dataPtr->expectedLabel==0){
			dataPtr->commandAddress=address;
			address+=dataPtr->increaseAdd;
		}
		dataPtr= dataPtr->next;
	}
	
	while(symbolPtr!=NULL){/*update the address of each node in data list that belongs to J commands or I commands that have label*/
		if(equalWords(symbolPtr->attributes,"extern")==0||equalWords(symbolPtr->attributes,"entry")==0){
			symbolPtr = symbolPtr->next;
			continue;
		}
		else{
			while(dataPtr!=NULL){
				if(dataPtr->labelName ==NULL){
					dataPtr = dataPtr->next;
					continue;	
				}
				else if(equalWords(symbolPtr->symbol,dataPtr->labelName)==1){
					dataPtr = dataPtr->next;
					continue;
				}
				else{
					if(equalWords(dataPtr->headline,"jmp")==0||equalWords(dataPtr->headline,"la")==0||
					equalWords(dataPtr->headline,"call")==0){
						dataPtr->b3.addressValue = symbolPtr->value;
					}
					else if(equalWords(dataPtr->headline,"beq")==0||equalWords(dataPtr->headline,"bne")==0||
					equalWords(dataPtr->headline,"blt")==0||equalWords(dataPtr->headline,"bgt")==0){
						dataPtr->b2.immed =(symbolPtr->value)-(dataPtr->commandAddress);
					}
						dataPtr = dataPtr->next;
				}
				dataPtr = dataPtr->next;
			}
		}
		symbolPtr = symbolPtr->next;
		dataPtr=hData;
	}
}
