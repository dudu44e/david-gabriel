#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "functions.h"
#include "dataForRuns.h"

#define LINE_SIZE 1024
#define LABEL_SIZE 100

char* buffer=" \t\n\v\f\r";/*seperator between words*/
void findCommand(){
	char *token;/*word in line*/
	char line[LINE_SIZE];/*saves each line from files*/
	char copyLine[LINE_SIZE];/*copy of each line save*/
	int flag=-1;/*flag for commands: if flag between 0 to 7 is R, 8 to 20 is I, 21 to 24 is J */
	int directiveFlag=-1;/*indicates which directive flag we are in, -1 represent no command*/
	int errorflag=0;/*checks if there are errors*/
	int argCounter =0;/*how many arguments in commands*/
	int argumentNumber;/*represent each argument value*/
	char label[LABEL_SIZE];/*saves the label*/
	while(fgets(line,LINE_SIZE,fp)!=NULL){/*reads lines*/
		strcpy(copyLine,line);
		lineCounter++;
		token = strtok(line,buffer);/*word in line*/
		if(token==NULL){/*if line is empty continue to next line*/
			continue;
		}
		if(token[0]==';'){/*checks if comment line*/
			continue;
		}
	while(token!=NULL){/*iterates through the words*/
		while(token!=NULL){
			if(token[0]==':'){/*if starts with ":" its illegal*/
				printf("\nLine %d :illegal start of command\n",lineCounter);
				finalErrorFlag=1;
				token=strtok(NULL,buffer);
				continue;
			}
			if(token[strlen(token)-1]==':'&&strlen(token)>1){/*checks if label*/
				if(isLegalLabel(token)==1&&isalpha(token[0])){
					ptrS temp;
					updateAddressFlag=1;
					strncpy(label, token, strlen(token)-1);/*copy to label the label we found*/
					temp= (ptrS)malloc(sizeof(symbolMap)); /*dymamic allocation*/
					temp->attributes = (char*)malloc(sizeof(10));
					temp->symbol= (char*)malloc(sizeof(DEFAULT_SIZE));
					label[strlen(token)-1]='\0';/*end the label*/
					temp->attributes = "code";
					strcpy(temp->symbol,label);
					addToSymbolList(temp);/*add the label to the symbol list*/
					free(temp);
					memset(label,0,strlen(label));
				}
				else{
					finalErrorFlag=1;
					printf("\nLine %d : label must start with a letter or reserved word can't be used as label \n",lineCounter);
				}
				token=strtok(NULL,buffer);/*move to the next word*/
				continue;
			}
			else{
				int i,j;
				for(i=0;i<27;i++){/*checks if command is legal*/
					if(equalWords(token,RIJ[i])==0){
						flag = i;/*determines the flag*/
						}
				}
				for(j=0;j<6;j++){/*checks if command is legal*/
					if(equalWords(token,directiveCommands[j])==0){
						directiveFlag = j;/*determines the directiveFlag*/
						}
				}
				if(flag!=-1||directiveFlag!=-1){
					token = strtok(NULL,buffer);/*move to next word in line*/
					break;
				}
			}
			if(flag==-1&&directiveFlag==-1){/*not a label or command*/
				finalErrorFlag=1;
				printf("\n Line %d: illegal input, not a command or a label\n",lineCounter);
			}
			token = strtok(NULL,buffer);/*move to next word in line*/
			break;
		}
		if(directiveFlag>-1&&directiveFlag<3){/*.db,.dw,.dh*/
			int i,arg,commas;
			commas=0;
			argCounter=0;
			while(token!=NULL){/*loop that can iterate through words and save argument*/
				if(token[0]!='+'&&token[0]!='-'&&token[0]!=','&&!isdigit(token[0])){
					if(argCounter==0){
						finalErrorFlag=1;
						printf("\n Line %d : no arguments has been recieved \n",lineCounter);
					}
					argCounter=0;
					break;
				}
				for(i=0;i<strlen(token)&&token!=NULL;){/*iterates through letters in the word*/
				ptr temp2= (ptr)malloc(sizeof(dataMap));
				temp2->headline = (char *)malloc(sizeof(DEFAULT_SIZE));
					if(token[i]==','){/*count commas*/
						commas++;
						i++;
						continue;
					}
					else{
						if(!isdigit(token[i])&&token[i]!='+'&&token[i]!='-'){
							finalErrorFlag=1;
							if(i==0){
								printf("\n Line %d: illegal start of phrase after command\n",lineCounter);
							}
							break;
						}/*end of checking if start of phrase is legal or illegal argument*/
						else{
							argCounter++;
							arg=atoi(&token[i]);
							while(isdigit(token[i])||token[i]=='+'||token[i]=='-'){
								i++;
							}/*end of loop that skip the current number*/
							if(equalWords(".db",directiveCommands[directiveFlag])==0){/*checks if .db and update the fields*/
								if(updateAddressFlag==0){/*mark node if address need to be added*/
									temp2->expectedLabel=0;
								}
								else
									temp2->expectedLabel=1;
								if(arg>MAX_NUMBER_8BIT||arg<MIN_NUMBER_8BIT){/*checks if in range*/
									finalErrorFlag=1;
									printf("\n Line %d : number out of range\n",lineCounter);
								}
								strcpy(temp2->headline,directiveCommands[directiveFlag]);
								temp2->a1.db = arg;
								temp2->increaseAdd=1;
								DCF+=1;
							}
							if(equalWords(".dh",directiveCommands[directiveFlag])==0){/*checks if .dh and update the fields*/
								if(updateAddressFlag==0){/*mark node if address need to be added*/
									temp2->expectedLabel=0;
								}
								else
									temp2->expectedLabel=1;
								if(arg>MAX_NUMBER_16BIT||arg<MIN_NUMBER_8BIT){/*checks if in range*/
									finalErrorFlag=1;
									printf("\n Line %d : number out of range\n",lineCounter);
								}
								DCF+= 2;
								strcpy(temp2->headline,directiveCommands[directiveFlag]);
								temp2->a1.dh = arg;
								temp2->increaseAdd=2;
							}
							if(equalWords(".dw",directiveCommands[directiveFlag])==0){/*checks if .dw and update the fields*/
								if(updateAddressFlag==0){/*mark node if address need to be added*/
									temp2->expectedLabel=0;
								}
								else
									temp2->expectedLabel=1;
								if(arg>MAX_NUMBER_32BIT||arg<MIN_NUMBER_32BIT){/*checks if in range*/
									finalErrorFlag=1;
									printf("\n Line %d : number out of range\n",lineCounter);
								}
								DCF+= 4;
								strcpy(temp2->headline,directiveCommands[directiveFlag]);
								temp2->a1.dw = arg;
								temp2->increaseAdd=4;
								}
							if((argCounter-commas)!=1){/*checks if there are too many commas*/
								finalErrorFlag=1;
								printf("\n Line %d: too many commas\n",lineCounter);
							}
							if(finalErrorFlag!=1){/*if there are no errors add node to list*/
								temp2->commandAddress=0;
								updateAddressFlag=0;
								temp2->labelName =NULL;
								addToList(temp2,0);
							}
							free(temp2);		
						}
				  }
				}/*end of for loop*/
				token=strtok(NULL,buffer);/*continue to next word*/
			}/*end of while*/
		}
		if(directiveFlag==3){/*checks if .asciz and iterates through line where asciz appeared*/
			int skipTokens;
			int i;
			skipTokens=readAscii(copyLine);/*method to save each letter in data list*/
			for(i=0;i<skipTokens+1;i++){
				token=strtok(NULL,buffer);
			}
		}
		if(directiveFlag == 4||directiveFlag==5){/*checks if.entry or .extern and if it legal*/
			int i;
			for(i=0;i<strlen(token)-1;i++){
				if(isdigit(token[0])){
					printf("\nLine %d :illegal start for label\n",lineCounter);
					finalErrorFlag=1;
					break;
				}
				if(!isalnum(token[i])){
					printf("\nLine %d :illegal label\n",lineCounter);
					finalErrorFlag=1;
					break;
					}
			}
			if(directiveFlag==4&&finalErrorFlag!=1){/*add entry to symbol list*/
				ptrS node = (ptrS)malloc(sizeof(symbolMap));
				node->symbol= (char*)malloc(sizeof(DEFAULT_SIZE));
				strcpy(node->symbol,token);
				node->attributes ="entry";
				addToSymbolList(node);
			}
			if(directiveFlag==5&&finalErrorFlag!=1){/*add extern to symbol list*/
				ptrS node = (ptrS)malloc(sizeof(symbolMap));
				node->symbol= (char*)malloc(sizeof(DEFAULT_SIZE));
				strcpy(node->symbol,token);
				node->value = 0;
				node->attributes ="extern";
				addToSymbolList(node);
			}
			
		
		}
		if(flag>-1&&flag<5){/*if first 5 commands in R*/
			ptr temp=(ptr)malloc(sizeof(dataMap));
			temp->headline = (char *) malloc(sizeof(DEFAULT_SIZE));
			argCounter=0;
			if(flag==0){
				temp->b1.opcode =0;
				temp->b1.funct =1;
				strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==1){
				temp->b1.opcode =0;
				temp->b1.funct =2;
				strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==2){
				temp->b1.opcode =0;
				temp->b1.funct =3;
				strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==3){
				temp->b1.opcode =0;
				temp->b1.funct =4;
				strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==4){
				temp->b1.opcode =0;
				temp->b1.funct =5;
				strcpy(temp->headline,RIJ[flag]);
			}
			
			while(token!=NULL&&argCounter!=3){/*loop through words and save the 3 arguments for each command in the first 5 commands in R*/
				int commas=0;
				int rs,rt,rd;
				while(argCounter!=3&&token!=NULL){
					int i;
					for(i=0;i<strlen(token);i++){
						if(token[i]==','){
							commas = commas +1;
							continue;
						}
						else{/*checks if register is legal and saves the data to the field*/
							if(token[i]=='$'){
								i++;
								if(isdigit(token[i])&&isdigit(token[i+1])){
									if(argCounter==0){
										rs= atoi(&token[i]);
										if(rs<0||rs>31){
											finalErrorFlag=1;
											printf("\n Line %d : illegal argument, number not in range \n",lineCounter);
										}
									}
									else if(argCounter ==1){
										rt = atoi(&token[i]);
										if(rt<0||rt>31){
											finalErrorFlag=1;
											printf("\n Line %d : illegal argument, number not in range \n",lineCounter);
										}
									}
									else{
									 rd =atoi(&token[i]);
									 if(rd<0||rd>31){
											finalErrorFlag=1;
											printf("\n Line %d : illegal argument, number not in range \n",lineCounter);
										}
									}
									i++;
								}
								else if(isdigit(token[i])){
									if(argCounter==0){
										rs= atoi(&token[i]);
									}
									else if(argCounter ==1){
										rt = atoi(&token[i]);
									}
									else{
									 	rd =atoi(&token[i]);
									}
								}
								else{
									finalErrorFlag=1;
									printf("\n Line %d : illegal argument \n",lineCounter);
								}
								argCounter++;
								if(argCounter==3){
									break;
								}
							}
							else if(isdigit(token[i])){
								continue;
							}
							else{
								finalErrorFlag=1;
								printf("\n Line %d : illegal argument \n",lineCounter);
								break;
							}
						}
					}
					token = strtok(NULL,buffer);
				}
				if(finalErrorFlag==0 && argCounter==3&& commas==2){/*checks if everything is legal in syntax and if so update the node fields and add it to data list*/
					temp->b1.rs=rs;
					temp->b1.rt=rt;
					temp->b1.rd= rd;
					argCounter=0;
					temp->commandAddress=address;
					temp->expectedLabel=0;
					addToList(temp,updateAddressFlag);
					address+=4;
					updateAddressFlag=0;
					free(temp);
					ICF = ICF+4;
				}
				else{
					argCounter=0;
					printf("\nLine %d :not enough arguments for this command or illegal argument\n",lineCounter);
					free(temp);
				}
				token = strtok(NULL,buffer);
			}
		}/*end of checking if first 5 commands*/
		if(flag>4&&flag<8){/*if other commands in R*/
			ptr temp= (ptr)malloc(sizeof(dataMap));
			temp->headline = (char *) malloc(sizeof(DEFAULT_SIZE));
			argCounter=0;
			if(flag==5){
			temp->b1.opcode =1;
			temp->b1.funct =1;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==6){
			temp->b1.opcode =1;
			temp->b1.funct =2;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==7){
			temp->b1.opcode =1;
			temp->b1.funct =3;
			strcpy(temp->headline,RIJ[flag]);
			}
			while(token!=NULL&&argCounter!=2){/**/
				int commas=0;
				int rs,rd;
				while(argCounter!=2&&token!=NULL){/*loop through words and save the 2 arguments for each command in the other commands in R*/
					int i;
					for(i=0;i<strlen(token);i++){
						if(token[i]==','){
							commas++;
							continue;
						}/*count commas*/
						else{/*checks if syntax of register is legal and update relevant field*/
							if(token[i]=='$'){
								i++;
								if(isdigit(token[i])&&isdigit(token[i+1])){
									if(argCounter==0){
										rs= atoi(&token[i]);
										if(rs<0||rs>31){
											finalErrorFlag=1;
											printf("\n Line %d : illegal argument, number not in range \n",lineCounter);
										}
									}
									else{
										rd =atoi(&token[i]);
										if(rd<0||rd>31){
											finalErrorFlag=1;
											printf("\n Line %d : illegal argument, number not in range \n",lineCounter);
										}
									}
									i++;
								}/*end of checking if 2 digit register*/
								else if(isdigit(token[i])){
									if(argCounter==0){
										rs= atoi(&token[i]);
										if(rs<0||rs>31){
											finalErrorFlag=1;
											printf("\n Line %d : illegal argument, number not in range \n",lineCounter);
										}
									}
									else{
									 	rd =atoi(&token[i]);
									 	if(rd<0||rd>31){
											finalErrorFlag=1;
											printf("\n Line %d : illegal argument, number not in range \n",lineCounter);
										}
									}
								}/*end of checking if 1 digit register*/
								else{
									finalErrorFlag=1;
									printf("\n Line %d : illegal argument \n",lineCounter);
								}
								argCounter++;
								if(argCounter==2){
									break;
								}
							}/*if that saves register*/
							else if(isdigit(token[i])){
								printf("\n Line %d : illegal argument, number not in range \n",lineCounter);
								finalErrorFlag=1;
								continue;
							}/*checks if there are more than 2 digits in a register*/
							else{
								finalErrorFlag=1;
								printf("\n Line %d : illegal argument \n",lineCounter);
								break;
							}
						}/*end of else that saves registers*/
					}/*end of for loop*/
					token = strtok(NULL,buffer);
				}
				if(finalErrorFlag==0 && argCounter==2&& commas==1){/*checks if synatx is legal and update the fields and add the node to data list*/
					temp->b1.rs=rs;
					temp->b1.rt=0;
					temp->b1.rd= rd;
					argCounter=0;
					temp->commandAddress=address;
					temp->expectedLabel=0;
					addToList(temp,updateAddressFlag);
					address+=4;
					updateAddressFlag=0;
					free(temp);
					ICF = ICF+4;
				}
				else{
					argCounter=0;
					printf("\nLine %d :not enough arguments for this command or illegal argument\n",lineCounter);
					free(temp);
				}
				token = strtok(NULL,buffer);

			}
		}/*end of other commands in R*/
		if((flag>7&&flag<13)||(flag>16&&flag<23)){/*if first commands or last commands in I*/
			ptr temp=(ptr)malloc(sizeof(dataMap));
			temp->headline =(char *)malloc(sizeof(DEFAULT_SIZE));
			argCounter=0;
			if(flag==8){
			temp->b2.opcode =10;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==9){
			temp->b2.opcode =11;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==10){
			temp->b2.opcode =12;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==11){
			temp->b2.opcode =13;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==12){
			temp->b2.opcode =14;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==17){
			temp->b2.opcode =19;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==18){
			temp->b2.opcode =20;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==19){
			temp->b2.opcode =21;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==20){
			temp->b2.opcode =22;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==21){
			temp->b2.opcode =23;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==22){
			temp->b2.opcode =24;
			strcpy(temp->headline,RIJ[flag]);
			}
		
			while(token!=NULL&&argCounter!=3){/*loop through words and save the 3 arguments for each command in the first and last commands in I*/
				int commas=0;
				int rs,immed,rt,immedSign;
				while(argCounter!=3&&token!=NULL){/*loop through words that check arguments and update the fields*/
					int i;
					for(i=0;i<strlen(token);){
						if(token[i]==','){/*count commas*/
							commas++;
							i++;
							continue;
						}
						else{
						if(argCounter==0||argCounter==2){/*checks registers in first and last argument*/
							if(token[i]=='$'){
								i++;
								if(isdigit(token[i])&&isdigit(token[i+1])){
									if(argCounter==0){
										rs= atoi(&token[i]);
										if(rs<0||rs>31){
											finalErrorFlag=1;
											printf("\n Line %d : illegal argument, number not in range \n",lineCounter);
										}
									}
									else{
										rt =atoi(&token[i]);
										if(rt<0||rt>31){
											finalErrorFlag=1;
											printf("\n Line %d : illegal argument, number not in range \n",lineCounter);
										}
									}
									i+=2;
								}/*end of if register has 2 digits*/
								else if(isdigit(token[i])){
									if(argCounter==0){
										rs= atoi(&token[i]);
										if(rs<0||rs>31){
											finalErrorFlag=1;
											printf("\n Line %d : illegal argument, number not in range \n",lineCounter);
										}
									}
									else{
									 	rt =atoi(&token[i]);
									 	if(rt<0||rt>31){
											finalErrorFlag=1;
											printf("\n Line %d : illegal argument, number not in range \n",lineCounter);
										}
									}
									i++;
								}/*end of if register has 1 digit*/
								else{
									finalErrorFlag=1;
									printf("\n Line %d : illegal argument \n",lineCounter);
								}/*end of register is illegal*/
								argCounter++;
								if(argCounter==2){
									argCounter++;
									break;
								}
							}/*end of if that checks if it is a register and save it*/
							else if(isdigit(token[i])){
								printf("\n Line %d : illegal argument, number not in range \n",lineCounter);
								finalErrorFlag=1;
								continue;
							}
							else{
								finalErrorFlag=1;
								printf("\n Line %d : illegal argument \n",lineCounter);
								break;
							}
						}/*end of if that checks if register*/
						else{
							if(argCounter>1){
								break;
							}
							if(token[i]==','){/*saving second argument to immed field*/
								i++;
								commas++;
								}
								argCounter++;
								immedSign=1;
								if(token[i]=='-'){/*checks sign of argument*/
									immedSign=-1;
									i++;/*move to next letter*/
								}
								if(token[i]=='+'){/*checks sign of argument*/
									i++;/*move to next letter*/
								}
								if(!isdigit(token[i])){
									finalErrorFlag=1;
									printf("\nLine %d :second argument must be a number\n",lineCounter);
								}
								immed =atoi(&token[i])*immedSign;
								while(isdigit(token[i])){
								i++;
								}
							}/*end of else that saves the immed parameter*/
						}/*end of else*/
					}/*end of for loop*/
					
					token = strtok(NULL,buffer);
				}/*end of*/
				if(finalErrorFlag==0 && argCounter==3&& commas==2){/*checks if syntax is legal and update the fields and add the node to data list*/
					temp->b2.rs=rs;
					temp->b2.rt=rt;
					temp->b2.immed= immed;
					argCounter=0;
					temp->commandAddress=address;
					temp->expectedLabel=0;
					addToList(temp,updateAddressFlag);
					address+=4;
					updateAddressFlag=0;
					free(temp);
					ICF = ICF+4;
				}
				else{
					argCounter=0;
					printf("\nLine %d :not enough arguments for this command or illegal argument\n",lineCounter);
					free(temp);
				}
				token = strtok(NULL,buffer);

			}
		
		}/*end of first commands or last commands in I*/
		if(flag>12&&flag<17){/*if other commands in I*/
			int rs,rt,commas,i;
			char label[DEFAULT_SIZE];
			ptr temp=(ptr)malloc(sizeof(dataMap));
			temp->headline =(char *) malloc(sizeof(DEFAULT_SIZE));
			commas=0;
			argCounter=0;
			if(flag==13){
			temp->b2.opcode =15;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==14){
			temp->b2.opcode =16;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==15){
			temp->b2.opcode =17;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==16){
			temp->b2.opcode =18;
			strcpy(temp->headline,RIJ[flag]);
			}
			while(token!=NULL&&argCounter!=3){/*loop through words and save the 3 arguments for each command in the other commands in I*/
				if(commas>2){/*checks if there are too many commas*/
					finalErrorFlag=1;
					printf("\n Line %d: too many commas\n",lineCounter);
					break;
				}
				if(token[0]!='$'&&token[0]!=','&&!isalpha(token[0])){/*checks if illegal start of argument*/
					finalErrorFlag=1;
					printf("\n Line %d: illegal start of phrase, must be register or label\n",lineCounter);
					break;
				}
				for(i=0;i<strlen(token);){
					if(token[i]==','){
						while(token[i]==','){/*skip and count commas*/
							commas++;
							i++;
						}
						continue;
					}
					if(token[i]=='$'){/*save registers*/
						i++;
						if(!isdigit(token[i])){/*checks if register starts with number*/
							finalErrorFlag=1;
							printf("\n Line %d: register must be a number \n",lineCounter);
							break;
						}
						else{
							if(argCounter==0){/*saves the first register - rs*/
								rs = atoi(&token[i]);
								argCounter++;
								while(isdigit(token[i])){i++;}/*skips the digits in token*/
					
							}
							else if(argCounter==1){/*saves the second register - rt*/
								rt = atoi(&token[i]);
								argCounter++;
								while(isdigit(token[i])){i++;}/*skips the digits in token*/
							}
							else{/*throws error if 3rd argument is not label*/
								finalErrorFlag=1;
								printf("\n Line %d: expected a label and not a register\n",lineCounter);
								break;
							}
						}
					}/*and of if that saves registers*/
					else if(isalpha(token[i])){/*checks if label is legal and save it if so*/
						if(argCounter!=2){/*throw error if label is not the 3rd argument*/
							finalErrorFlag =1;
							printf("\n Line %d: expected a register\n",lineCounter);
							break;
						}
						else{
							strcpy(label,&token[i]);/*saves the label*/
							while(i<strlen(token)){/*checks if label is legal*/
								if(!isalpha(token[i])&&!isdigit(token[i])){
									finalErrorFlag=1;
									printf("\n Line %d: label must contain combinations of digits and letters \n",lineCounter);
									break;
								}
								else{
									i++;
								}
							}
							argCounter=3;
							break;
						}
					}/*end of if that checks label*/
					else{
						finalErrorFlag=1;
						printf("\n Line %d: illegal start of argument \n",lineCounter);
						break;
					}
				}
				token = strtok(NULL,buffer);
			}/*end of while*/
			if(finalErrorFlag==0&&argCounter==3&&commas==2){/*checks if syntax is legal and update the fields and add the node to data list*/
				if(rs>31||rs<0||rt>31||rt<0){
				 	printf("\n Line %d: register does not exist \n",lineCounter);
				 	finalErrorFlag=1;
				}
				else if(isLegalLabel(label)==0){
					printf("\n Line %d: label can not be reserved word\n",lineCounter);
					finalErrorFlag=1;
				}
	
				else{
					temp->b2.rs =rs;
					temp->b2.rt =rt;
					temp->labelName = (char*) malloc(sizeof(DEFAULT_SIZE));
					strcpy(temp->labelName, label);
					temp->commandAddress=address;
					temp->expectedLabel=0;
					addToList(temp,updateAddressFlag);
					address+=4;
					updateAddressFlag=0;
					free(temp);
					ICF=ICF+4;
				}
			}/*end of while*/
			else{
				printf("\n Line %d: too few commas or arguments or illegal argument \n",lineCounter);
			}
		}/*end of other commands in I*/
			if(flag>22&&flag<27){  /*command in J*/
			ptr temp=(ptr)malloc(sizeof(dataMap));
			temp->labelName = (char*)malloc(sizeof(DEFAULT_SIZE));
			temp->headline = (char*)malloc(sizeof(DEFAULT_SIZE));
			argCounter=0;
			if(flag==23){
			temp->b3.opcode =30;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==24){
			temp->b3.opcode =31;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==25){
			temp->b3.opcode =32;
			strcpy(temp->headline,RIJ[flag]);
			}
			if(flag==26){
			temp->b3.opcode =63;
			temp->b3.reg =0;
			temp->b3.addressValue =0;
			strcpy(temp->headline,RIJ[flag]);
			ICF+=4;
			temp->commandAddress=address;
			temp->expectedLabel=0;
			addToList(temp,updateAddressFlag);
			address+=4;
			updateAddressFlag=0;
			argCounter=0;
			flag =-1;
			directiveFlag=-1;
			errorflag=0;
			argumentNumber=-1;
			continue;
			
			}	
			else if(token[0]=='$'){  /*checks if register*/
				if(!isdigit(token[1])){
					finalErrorFlag=1;
					printf("\nLine %d :illegal argument,register must be a valid number\n",lineCounter);
				}
				else{
					int i=1;
					argumentNumber = atoi(&token[1]);
					while(isdigit(token[i])){i++;}
					if(token[i]!='\0'&&token[i]!='\n'){
						finalErrorFlag=1;
						printf("\nLine %d : inccorect syntax,register number must stand alone\n",lineCounter);
					}
					temp->b3.reg = 1;
					temp->b3.addressValue = argumentNumber;
				}/*end of else that check register*/
			}
			else{ /*checks if label*/
				if(isalpha(token[0])){
					int i;
					for(i=1;i<strlen(token);i++){
						if(!isalpha(token[i])&&!isdigit(token[i])){
						printf("\nLine %d :illegal label\n",lineCounter);
						finalErrorFlag=1;
						break;
						}			
					}
					temp->b3.reg = 0;
					if(isLegalLabel(token)==1){
						strcpy((temp->labelName),token);
					}
					else{
						finalErrorFlag=1;
						printf("\n Line %d: label can not be a reserved word \n",lineCounter);
					}
				}
				else{
					printf("\nLine %d :illegal label\n",lineCounter);
					finalErrorFlag=1;
				}
			}
				if(finalErrorFlag!=1&&flag!=26){/*checks if syntax is legal and update the fields and add the node to data list*/
					temp->commandAddress=address;
					temp->expectedLabel=0;
					addToList(temp,updateAddressFlag);
					address+=4;
					updateAddressFlag=0;
					ICF=ICF+4;
				}
				free(temp);
			}/*checks if commands in J*/	
		if(errorflag==1){/*checks if there was an error and reset all flags and continue to the next command*/
			finalErrorFlag=1;
			errorflag=0;
			argumentNumber=-1;
			argCounter=0;
			continue;
		}
		argCounter=0;
		flag =-1;
		directiveFlag=-1;
		errorflag=0;
		token = strtok(NULL,buffer);/*saves the next word*/

	}
}/*end of get line*/
}/*end of findCommand*/
