#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>

//Global variable
int program_length=0X0;

bool pass1();
void pass2();

void main(){

    //calling pass1 function to generate intermediate file and symbol table
    if(pass1() == false){
        printf("\n Pass 1 is not successuful.");
        return;
    }else{
        printf("\n Pass 1 successuful.");
    }

    //calling function pass2 to generate object code
    pass2();

    return;
}


bool pass1(){

    //file pointers to read and write into files
    FILE *fAssembly, *fOperation, *fSymbol, *fIntermediate;
    int length;

	
	fAssembly = fopen("assembly_code.txt","r");
	if(fAssembly == NULL){
        printf("Assembly code file missing");
        return false;
    }
	
    fOperation = fopen("operation_code_table.txt","r");
	if(fOperation == NULL){
        printf("Operation code table file missing");
        return false;
    }
	
    fSymbol = fopen("symbol_table.txt","w+");
	fIntermediate = fopen("intermediate_file.txt","w");

    //character arrays to store line of assembly file and corresponding
    //label, mnemonic, operand
    char buffer[128],label[14],mnemonic[14],operand[14], symbol[14];

    //start variable to indicate starting point of program and locctr is location counter
    //program length denotes length of program
    int start=0X0, locctr=0X0, flag=0, count=0X0, scanned_variable_count, addr=0X1;      

    fgets(buffer, 128, fAssembly);
	sscanf(buffer,"%s %s %s",label,mnemonic,operand);
 
    if(strcmp(mnemonic,"START")==0)
	{
		locctr = atoi(operand);	//save operand as starting address
        
		while(locctr > 0)
		{
			start = start + (locctr%10)*pow(16,count);
			locctr = locctr/10;
			count++;
		}
		locctr = start;
        fprintf(fIntermediate,"%X\t%s\t%s\t%s\n",start,label,mnemonic,operand);
	}
	else
	{
		locctr = 0X0;
	}

    //analysing each line from assembly and then 
    //writing it to intermediate file with its location counter
    while(!feof(fAssembly))
	{
		fgets(buffer,64,fAssembly);
		scanned_variable_count = sscanf(buffer,"%s%s%s",label,mnemonic,operand);

        //check whether the scanned line is comment or not
		if(label[0] == '.'){
            continue;

        }else{

			if(scanned_variable_count == 1) //this indicates instruction without operand
			{
				strcpy(mnemonic,label);
				fprintf(fIntermediate,"%04X\t\t%s\n",locctr,mnemonic);
			}
			else if(scanned_variable_count == 2) //this indicates instruction with operand
			{
				strcpy(operand,mnemonic);
				strcpy(mnemonic,label);
				fprintf(fIntermediate,"%X\t\t%s\t%s\n",locctr,mnemonic,operand);
			}
			else if(scanned_variable_count == 3) //this indicate label is present
			{
				rewind(fSymbol);		
				while(!feof(fSymbol))
				{
					flag = 0;
					fscanf(fSymbol,"%s%X",symbol,&addr);
					if(strcmp(label,symbol)==0)
					{
						flag = 1;	//duplicate label found
						printf("\nDuplicate label found: %s",label);
						return false;
					}
				}					
				
				if(flag == 0)	//no duplicate symbol found
				{
					fprintf(fSymbol,"%s\t%X\n",label,locctr);
					fprintf(fIntermediate,"%X\t%s\t%s\t%s\n",locctr,label,mnemonic,operand);
				}
			}
			
            char temp_opcode[2], temp_mnemonic[14];
			rewind(fOperation);
			while(!feof(fOperation))	//search optab for OPCODE
			{
				fscanf(fOperation,"%s %s", temp_mnemonic, &temp_opcode);
				if(strcmp(mnemonic,temp_mnemonic)==0)
				{
					locctr += 3;
					flag = 0;
					break;
				}
				if(strcmp(mnemonic,"WORD")==0 || strcmp(mnemonic,"word")==0)
				{	
					locctr += 3;
					flag = 0;
					break;
				}
				if((strcmp(mnemonic,"RESW")==0) || (strcmp(mnemonic,"resw")==0))
				{	
					locctr += 3*atoi(operand);
					flag = 0;
					break;
				}
				if(strcmp(mnemonic,"RESB")==0 || strcmp(mnemonic,"resb")==0)
				{	
					locctr += atoi(operand);
					flag = 0;
					break;
				}
				if(strcmp(mnemonic,"BYTE")==0 || strcmp(mnemonic,"byte")==0)
				{
					int j = strlen(operand);
					if(operand[0] !='C' && operand[0] != 'X')
					{	
						locctr += 1;
						flag = 0;
						break;
					}
					if(operand[0] =='C')
					{
						locctr += j-3;	//-3 is done to account for C' '
						flag = 0;
						break;
					}
					if(operand[0] =='X')
					{	
						if((j-3)%2 != 0)
							locctr += (j-3)/2 + 1;
						else
							locctr += (j-3)/2 ;
						flag = 0;
						break;
					}
					else
					{
						flag = 1;
					}
				}
				if(flag == 1)
				{
					printf("\n%s not present in operation code table file.",mnemonic);
					return false;
				}
			}
		}
		if(strcmp(mnemonic,"END")==0)
		{
			break;
		}
	}
	printf("\nSymbol table successfully generated.\n");
    printf("\nIntermediate file successfully generated.\n");
	
	
	program_length = locctr - start;
	

    fclose(fAssembly);
    fclose(fOperation);
	fclose(fSymbol);
	fclose(fIntermediate);
	
    return true;
}



void pass2(){

    FILE *fObject, *fOperation, *fSymbol, *fIntermediate;

    fOperation = fopen("operation_code_table.txt","r");
	if(fOperation == NULL){
        printf("Operation code table file missing");
        return false;
    }
	
    fSymbol = fopen("symbol_table.txt","r");
    if(fSymbol == NULL){
        printf("Symbol table file missing");
        return false;
    }
	fIntermediate = fopen("intermediate_file.txt","r");
    if(fIntermediate == NULL){
        printf("Intermediate file missing");
        return false;
    }

    fObject = fopen("object_code.txt","w");

    



    fclose(fObject);
    fclose(fOperation);
	fclose(fSymbol);
	fclose(fIntermediate);

    return;
}