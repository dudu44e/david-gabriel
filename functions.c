
#include "functions.h"
/*reserved words because they are commands*/
char * reservedWords[] ={"add","sub","and","or","nor","move","mvhi","mvlo","addi","subi","andi","ori","bne","beq","blt","bgt","lb","sb","lw","sw","lh","sh","jmp","la","call","stop","dh","dw","db","asciz","entry","extern"};

int equalWords (char * first, char * second){
	if(strcmp(second,"stop")==0||strcmp(first,"stop")==0){
	}
	if(strlen(first)!=strlen(second)){
		return 1;
	}
	else{
		if(strcmp(first,second)==0){
			return 0;
		}
		else{
			return 1;
		}
	}
}

int fileType(char * str){
	int length = strlen(str);
	if(length<3)
		return 0;
	if(str[length-3]!='.'||str[length-2]!='a'||str[length-1]!='s'){
		return 1;
	}
	return 0;
}
int readAscii(char * line){
	int skipTokens=0;
	int i;
	int bracketCounter=0;
	ptr temp= (ptr)malloc(sizeof(dataMap));
		for(i=0;line[i]!=NULL&&i<strlen(line);i++){
			if(line[i]=='"'){
				bracketCounter++;
				i++;
				break;
			}
		}
		while(line[i]!=NULL&&line[i]!='"'){
			if(line[i]==' '){
				skipTokens++;
				while(line[i]==' '){
					temp->a1.asciz=line[i];
					temp->headline=".asciz";
					temp->increaseAdd=1;
					if(updateAddressFlag==0){
						temp->expectedLabel=0;
					}
					else
						temp->expectedLabel=1;
					addToList(temp,0);
					i++;
					DCF++;				
				}
			}
			else{
				temp->a1.asciz=line[i];
				temp->headline=".asciz";
				temp->increaseAdd=1;
				if(updateAddressFlag==0){
					temp->expectedLabel=0;
				}
				else
					temp->expectedLabel=1;
				addToList(temp,0);
				DCF++;
				i++;
			}
		}
		if(line[i]!=NULL&&line[i]=='"')
			bracketCounter++;
	if(bracketCounter%2!=0||bracketCounter==0){
		finalErrorFlag=1;
		printf("\nLine %d :no closure for brackets or no arguments has been recieved\n",lineCounter);
	}
	temp->a1.asciz='\0';
	temp->headline=".asciz";
	temp->increaseAdd=1;
		if(updateAddressFlag==0){
			temp->expectedLabel=0;
		}
		else
			temp->expectedLabel=1;
	addToList(temp,0);
	DCF++;
	free(temp);
	return skipTokens;
}
void addToList(ptr node,int flag){
	ptr temp = (ptr) malloc (sizeof(dataMap));/*dynamic allocation*/
	ptrS symbolPtr = hSymbol;
	ptr p1;
	p1=hData;
	node->next = NULL;
	if(flag==1&&hSymbol!=NULL){
		while(symbolPtr->next!=NULL){
			symbolPtr= symbolPtr->next;
		}
		if(!equalWords(symbolPtr->attributes,"extern")==0&&!equalWords(symbolPtr->attributes,"entry")==0){
			symbolPtr->value = address;
		}
	}
	memcpy(temp,node,sizeof(dataMap));
	if(hData==NULL){
		hData= temp;
	}
	else{
		while((p1->next)!= NULL){
			p1=p1->next;
		}
		p1->next=temp;
	}
	return;
}

void addToSymbolList(ptrS node){
	ptrS p1;
	ptrS temp = (ptrS) malloc (sizeof(symbolMap));
	node->next = NULL;
	memcpy(temp,node,sizeof(symbolMap));
	if(hSymbol==NULL){
		hSymbol= temp;
	}
	else{
		p1=hSymbol;
		while(p1->next!=NULL){
			if(equalWords(p1->symbol,temp->symbol)==0&&equalWords(p1->attributes,temp->attributes)==0){
				printf("\n Line %d: this label already exist\n",lineCounter);
				finalErrorFlag=1;
				return;
			}
			else
				p1=p1->next;
		}
	p1->next=temp;
	}
}
int isLegalLabel(char *label){	
	int i;
	for(i=0;i<32;i++){
		if(equalWords(label,reservedWords[i])==0){
			return 0;
		}
	}
	return 1;	
}
void convertToBinary(char * part, int num,int iterations){
	int i,k;
	for(i=iterations-1;i>=0;i--){
		k=abs(num)>>i;
		if(k & 1){
			strcat(part,"1");
		}
		else{
				strcat(part,"0");
		}
		
	}
	if(num<0){
		i=0;
		while(part[i]=='0'){
			part[i]='1';
			i++;
		}
	}

}
void convertTwosComplement(char * part, int num,int iterations){
	int i;
	i=iterations-1;
	if(num>=0){
		return;
	}
	while(part[i]!='1'){
		i--;
	}
	i--;
	while(i>=0){
		if(part[i]=='1'){
			part[i]='0';
		}
		else
			part[i]='1';
		i--;
	}
}
void freeMemory(){
	ptrS ptrS2 = hSymbol;
	ptrS ptrS1 = ptrS2;
	ptr ptr2 = hData;
	ptr ptr1 = ptr2;
	while(ptrS2!=NULL){
		ptrS2=ptrS2->next;
		free(ptrS1);
		ptrS1=ptrS2;
	}
	while(ptr2!=NULL){
		ptr2=ptr2->next;
		free(ptr1);
		ptr1=ptr2;
	}
	hData=NULL;
	hSymbol=NULL;

}

