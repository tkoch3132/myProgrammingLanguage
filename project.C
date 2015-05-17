#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Main memory capacity*/ 
#define MEM_CAP 100
/*Compiler return status macros.  CMPLR is short for compiler.ERR is short for error*/
#define CMPLR_NO_ERRORS 0
#define CMPLR_ERR_UNKNOWN_CMD 1
#define CMPLR_ERR_WRD_OVERFLOW 2
#define CMPLR_ERR_UNDEFINED_USE 3
#define CMPLR_ERR_NO_HALT_FOUND 4
/*Runtime(execution) return status macros. RT is runtime. ERR again, is short for error.*/
#define RT_NO_ERRORS 0
#define RT_ERR_UNKOWN_CMD 1
#define RT_ERR_WRD_OVERFLOW 2
#define RT_ERR_SEG_FAULT 3
#define RT_ERR_DIV_BY_ZERO 4
#define RT_ERR_UNKOWN_CHARACTER 5

/*primary Function prototypes*/
int compile(int *mem, FILE *sourceFile,int *operand);
int execute(int *mem, int *accumulator,int *instructionCounter,int* instructionRegister,int *operationCode,int *operand);
int isStrValid(int *c);
/*secondary Function prototypes*/
void printTable(const int *mem, int *accumulator,int *instructionCounter,int* instructionRegister,int *operationCode,int *operand);
int getCommandValue(char* key);

int main(int argc, char *argv[])
{
    /*virtual computer Variable declaration & initialization*/
    int memory[MEM_CAP];
    int accumulator, operationCode,instructionCounter, instructionRegister, operand;
    
    if (argc != 2)/*only taking one parameter*/
    {
        printf("Missing filename parameter!");
        return (EXIT_FAILURE);
    }
    
    for(operand = 0; operand <MEM_CAP;operand++)
        memory[operand] = 0;
    accumulator = instructionCounter = instructionRegister = operationCode = operand = 0;
    
    
    stdin = fopen(argv[1],"r");
    if((stdin)== NULL)
    {
        stdin = fopen("/dev/tty","r");
        printf("\nERROR: there were Issues Opening the source code File\n");
        fclose(stdin);
        exit(1);
        
    }
    else
    {
        stdin = fopen(argv[1],"r");
                
        switch(compile(memory, stdin, &operand))
        {
            if(fclose(stdin) == EOF)
            {
                printf("The file:%s Failed to be closed.\n",(argv[1]));
                return (EXIT_FAILURE);
            }
            
            case CMPLR_NO_ERRORS:
                stdin = fopen("/dev/tty","r");
                printf("The program was successfully compiled.(no compile-errors)\n");
                switch(execute(memory, &accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand))
                {
                    case RT_NO_ERRORS:
                        
                        printf("Execution complete.(no runtime-errors)\n");
                        break;
                    case RT_ERR_UNKOWN_CMD:
                        printf("there was a RunTime Error-Unknown Command.\n");
                        break;
                    case RT_ERR_WRD_OVERFLOW:
                        printf("there was a RunTime Error-Word OverFlow.\n");
                        break;
                    case RT_ERR_SEG_FAULT:
                        printf("there was a RunTime Error-Segmentation Fault.\n");
                        break;
                    case RT_ERR_DIV_BY_ZERO:
                        printf("there was a RunTime Error-Division by Zero.\n");
                        break;
                    case RT_ERR_UNKOWN_CHARACTER:
                        printf("there was a RunTime Error-Unknown Character.\n");
                        break;
                }
                break;
            case CMPLR_ERR_UNKNOWN_CMD:
                printf("there was a Compile Error-Unknown Command.\n");
                break;
            case CMPLR_ERR_WRD_OVERFLOW:
                printf("there was a Compile Error-Word Overflow.\n");
                break;
            case CMPLR_ERR_UNDEFINED_USE:
                printf("there was a Compile Error-Undefined Use.\n");
                break;
            case CMPLR_ERR_NO_HALT_FOUND:
                printf("there was a Compile Error-Missing HALT command.\n");
                break;
                
        }
        
    }
    return (EXIT_SUCCESS);
}

/*this function takes a VALID file and parses it into mem, operand helps with looping
 and iterative tasks*/
/*WARNING this function does not close the file*/
int compile(int *mem, FILE *sourceFile,int *operand)
{
    rewind(sourceFile);
    char buffer[34];
    char *bufPointer = buffer;
    
    while(bufPointer != &buffer[33])
    {
        (*bufPointer) = 'a';
        bufPointer++;
    }
    bufPointer = &buffer[17];
    buffer[16] = '\0';
    buffer[33] = '\0';
    
    while(NULL != (fgets(buffer, 16,sourceFile)))
    {
        bufPointer = &buffer[17];
        /*this if statement and everything it encapsulates is the majority of the error checking */
        if(strlen(buffer)<16)/*no line (in proper format) should have strlen >=16*/
        {
            strcpy(bufPointer, buffer);
            if(bufPointer != NULL)
            {
                if((strchr(bufPointer,'\n') == NULL))
                {
                    if(NULL != (fgets(buffer, 16,sourceFile)))
                    {
                        printf("When processing the line:(%s) ", buffer);/*print address*/
                        return CMPLR_ERR_UNDEFINED_USE;
                    }
                    
                }
                
                if(((strchr(bufPointer, ' '))-&buffer[17])> 2*sizeof(char) || 
                        ((strchr(bufPointer, ' '))-&buffer[17]) < 2*sizeof(char))/*checks that the address on the left is 2 chars*/
                {
                    printf("When processing the line:(%s) ", buffer);/*print address*/
                    return CMPLR_ERR_UNDEFINED_USE;
                }
            }
            else
            {
                printf("When processing the line:(%s) ", buffer);/*print address*/
                return CMPLR_ERR_UNDEFINED_USE;
            }
        }
        else
        {
            printf("When processing the line:(%s) ", buffer);/*print address*/
            return CMPLR_ERR_UNDEFINED_USE;
        }
        (*operand) = atoi(strtok(bufPointer,  " "));
        mem[(*operand)] = getCommandValue(strtok(NULL," " ));
        if(mem[(*operand)] >= 10 && mem[(*operand)]<=99)/*operation codes can only be a int from 10 to 99*/
        {
            if(mem[(*operand)] == 22)/*SET*/
            {
                mem[(*operand)] = atoi(strtok(NULL,  " "));
                if((mem[(*operand)]/10000) > 0 || (mem[(*operand)]/10000) < 0)
                {
                    printf("When processing the line:(%s) ", buffer);/*print address*/
                    return CMPLR_ERR_WRD_OVERFLOW;
                }
            }
            else
            {
                mem[(*operand)] *=100;
                bufPointer = strtok(NULL,  " ");
                if((atoi(bufPointer)/100)>=1)/*no operand should be larger than 2 digits*/
                {
                    printf("When processing the line:(%s) ",buffer);/*print address*/
                    return CMPLR_ERR_UNDEFINED_USE;
                }
                mem[(*operand)] += atoi(bufPointer);
                bufPointer = &buffer[17];
                if((mem[(*operand)]/100) == 99)
                {
                    mem[(*operand)] *=10;/*halt has to be 99xx0(temporarily) because they can use SET to make a value 99xx, causing ambiguity*/
                }
            }
            
            if(strtok(NULL," "))/*we read the address, command and int. Checks if there is more than what is allowed on the line*/
            {/*no command takes two parameters so it is a unkown command compile error*/
                printf("When processing the line:(%s) ",buffer);/*print address*/
                return CMPLR_ERR_UNDEFINED_USE;/**/
            }
            
        }
        else if(mem[(*operand)]==CMPLR_ERR_UNKNOWN_CMD)/*getCommandValue told us the command doesnt exisit*/
        {
            printf("When processing the line:(%s) ", buffer);/*print address*/
            return CMPLR_ERR_UNKNOWN_CMD;
        }
        
    }
    /*check that the HALT command was given*/
    (*operand) = 0;
    while((*operand)<MEM_CAP)
    {
        if(((mem[(*operand)])/1000) == 99)/*halt takes up 5 digits(in this function only) thats why we /1000*/
        {
            mem[(*operand)]/= 10;
            while((*operand)<MEM_CAP)/*check for any more halts to convert*/
            {
                (*operand)++;
                if((mem[(*operand)]/1000)== 99)
                    mem[(*operand)]/= 10;
            }  
            ;/*setting halt back to 99xx*/
            (*operand) = 0;/*setting the value back to what was passed*/
            return CMPLR_NO_ERRORS;
        }
        else
            (*operand)++;
    }
    return CMPLR_ERR_NO_HALT_FOUND;
}

/*printing the registers and memory, and then returning can be thought of as a HALT statement:
 * printTable(mem,accumulator,instructionCounter,instructionRegister,opCode,operand);
 * return (RunTime #define int macro);
 * 
 * there is a printf with the type of error and the address of the error after the table is printed
 */
int execute(int *mem, int *accumulator,int *instructionCounter,int* instructionRegister,int *opCode,int *operand)
{
    
    (*accumulator)= (*instructionCounter) = (*instructionRegister) = (*opCode)= (*operand)= 0;
    
    for((*instructionCounter)= 0;(*instructionCounter)<MEM_CAP;(*instructionCounter)++)
    {
        (*instructionRegister) = mem[(*instructionCounter)];
        (*opCode) = ((*instructionRegister)/100);
        (*operand) = ((*instructionRegister)%100);
        
        if(((*instructionRegister)/10000)!= 0)
        {
            printTable(mem, accumulator, instructionCounter, instructionRegister, opCode, operand);
            printf("When processing the address %d ", (*instructionCounter));
            return RT_ERR_WRD_OVERFLOW;
        }
        switch ((*opCode))
        {
            case 0:/*default*/
                continue;
            case 10:/*READ*/
                mem[(*operand)] = -10000;/*4 digits max*/
                
                    printf("Enter a integer to be stored at the address %d: ", (*operand));
                    scanf("%d", &mem[(*operand)]);
                if ((mem[(*operand)] / 10000) != 0) {
                    printTable(mem, accumulator, instructionCounter, instructionRegister, opCode, operand);
                    printf("When processing the address %d ", (*instructionCounter));
                    return RT_ERR_WRD_OVERFLOW;
                }
                break;
            case 11:/*WRIT*/
                if(0 != (mem[(*operand)]/10000))
                {
                    printTable(mem, accumulator, instructionCounter, instructionRegister, opCode, operand);
                    printf("When processing the address %d ", (*instructionCounter));
                    return RT_ERR_WRD_OVERFLOW;
                }
                else
                {
                    printf("%d\n",mem[(*operand)]);
                }
                break;
            case 12:/*PRNT*/
                while((*operand) < MEM_CAP)
                {
                    if (isStrValid(&mem[(*operand)]))
                    {
                        if((mem[(*operand)]/100)!= 0)
                            printf("%c", (char) (mem[(*operand)] / 100));
                        else
                        {
                            (*operand) = -1;/*this indicates there was a null terminating character*/
                            break;
                        }
                        if((mem[(*operand)]%100)!= 0)
                            printf("%c", (char) (mem[(*operand)] % 100));
                        else
                        {
                            (*operand) = -1;/*this indicates there was a null terminating character*/
                            break;
                        }
                    } 
                    else
                    {
                        printTable(mem, accumulator, instructionCounter, instructionRegister, opCode, operand);
                        printf("When processing the address %d ", (*instructionCounter));
                        return RT_ERR_UNKOWN_CHARACTER;
                    }
                    (*operand)++;
                }
                
                
                /*reaching this point means there was not a null terminating character*/
                if((*operand)!= -1)
                {
                    printTable(mem,accumulator,instructionCounter,instructionRegister,opCode,operand);
                    printf("When processing the address %d ",(*instructionCounter));
                    return RT_ERR_SEG_FAULT;
                }
                else
                    break;
            case 20:/*LOAD*/
                
                if( 0 == (mem[(*operand)]/10000))/*check that the new value is <= 4 digits*/
                    (*accumulator) = mem[(*operand)];
                else/*if it is not 4 digits*/
                {
                    printTable(mem,accumulator,instructionCounter,instructionRegister,opCode,operand);
                    printf("When processing the address %d ",(*instructionCounter));
                    return RT_ERR_WRD_OVERFLOW;
                }
                break;
            case 21:/*STOR*/
                mem[(*operand)] = *accumulator;
                break;
            case 30:/*ADD*/
                if(0 == ((mem[(*operand)]+ *accumulator)/10000))
                    (*accumulator) += mem[(*operand)];
                else
                {
                    printTable(mem,accumulator,instructionCounter,instructionRegister,opCode,operand);
                    printf("When processing the address %d ",(*instructionCounter));
                    return RT_ERR_WRD_OVERFLOW;
                }
                break;
            case 31:/*SUB*/
                if(0 == ((mem[(*operand)]- *accumulator)/10000))
                    (*accumulator) -= mem[(*operand)];
                else
                {
                    printTable(mem,accumulator,instructionCounter,instructionRegister,opCode,operand);
                    printf("When processing the address %d ",(*instructionCounter));
                    return RT_ERR_WRD_OVERFLOW;
                }
                break;
            case 32:/*DIV*/
                if(mem[(*operand)] == 0)/*dividing by zero error.*/
                {
                    printTable(mem,accumulator,instructionCounter,instructionRegister,opCode,operand);
                    printf("When processing the address %d ",(*instructionCounter));
                    return RT_ERR_DIV_BY_ZERO;
                }
                
                if(0 == ((mem[(*operand)]- *accumulator)/10000))
                    (*accumulator) /= mem[(*operand)];
                else
                {
                    printTable(mem,accumulator,instructionCounter,instructionRegister,opCode,operand);
                    printf("When processing the address %d ",(*instructionCounter));
                    return RT_ERR_WRD_OVERFLOW;
                }
                break;
            case 33:/*MULT*/
                if(0 == ((mem[(*operand)]* (*accumulator))/10000))
                    (*accumulator) *= mem[(*operand)];
                else
                {
                    printTable(mem,accumulator,instructionCounter,instructionRegister,opCode,operand);
                    printf("When processing the address %d ",(*instructionCounter));
                    return RT_ERR_WRD_OVERFLOW;
                }
                break;
            case 34:/*MOD*/
                if(mem[(*operand)] == 0)/*dividing by zero error.*/
                {
                    printTable(mem,accumulator,instructionCounter,instructionRegister,opCode,operand);
                    printf("When processing the address %d ",(*instructionCounter));
                    return RT_ERR_DIV_BY_ZERO;
                }
                if(0 == ((mem[(*operand)]% *accumulator)/10000))
                    (*accumulator) %= mem[(*operand)];
                else
                {
                    printTable(mem,accumulator,instructionCounter,instructionRegister,opCode,operand);
                    printf("When processing the address %d ",(*instructionCounter));
                    return RT_ERR_WRD_OVERFLOW;
                }
                break;
            case 40:/*BRAN*/
                (*instructionCounter) = ((*operand)-1);/*due to increment*/
                break;
            case 41:/*BRNG*/
                if((*accumulator)<0)
                {
                    (*instructionCounter) = ((*operand)-1);/*due to increment*/
                }
                break;
            case 42:/*BRZR*/
                if((*accumulator)==0)
                {
                    (*instructionCounter) = ((*operand)-1);/*due to increment*/
                }
                break;
            case 99:/*HALT*/
                printTable(mem,accumulator,instructionCounter,instructionRegister,opCode,operand);
                return RT_NO_ERRORS;
                break;
            default:/*unknown command code*/
                printTable(mem,accumulator,instructionCounter,instructionRegister,opCode,operand);
                printf("When processing the address %d ",(*instructionCounter));
                return RT_ERR_UNKOWN_CMD;
                    
        }
    }
    return(RT_NO_ERRORS);
}

/*checks that the mem location in the array can be converted to a string*/
int isStrValid(int *c)
{
    if(c == NULL)
    {
        printf("null Pointer passed to isStrValid.\n");
        return 0;
    }
    
    if((*c)/10000 != 0)
    {
        printf("mem location passed to str invalid.\n");
        return 0;
    }
    
    if(((*c)/100)==0 || ((*c)/100) == 10|| ((((*c)/100)>=65) && ((*c)/100) <=90))
    {
        if(((*c)%100)==0 || ((*c)%100) == 10|| ((((*c)%100)>=65) && ((*c)%100) <=90))
            return 1;
        else
            return 0;
    }
    else
        return 0;
}

void printTable(const int *mem, int *accumulator,int *instructionCounter,int* instructionRegister,int *operationCode,int *operand)
{/*the total line size is 64*/
    printf("%s", "**************************************************************\n"); /*62 stars followed by a \n*/
    
    
    printf("REGISTORS:\n");
    printf("accumulator%13c", ' ');
    printf("%+.4d\n", *accumulator);
    printf("instructionCounter%9c", ' ');
    printf("%-.2d\n", *instructionCounter);
    printf("instructionRegister%5c%+.4d\n", ' ', *instructionRegister);
    printf("operantionCode%13c%-.2d\n", ' ', *operationCode);
    printf("operand%20c%-.2d\n", ' ', *operand);
    printf("MEMORY:\n");
    printf("%7c0%5c1%5c2%5c3%5c4%5c5%5c6%5c7%5c8%5c9 \n",
            ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    for ((*operand) = 0; ((*operand)*10) < MEM_CAP; (*operand)++)
        printf("%2d %+.4d %+.4d %+.4d %+.4d %+.4d %+.4d %+.4d %+.4d %+.4d %+.4d\n", (*operand)*10/*front number*/,
            mem[((*operand)*10) + 0], mem[((*operand)*10) + 1],
            mem[((*operand)*10) + 2], mem[((*operand)*10) + 3],
            mem[((*operand)*10) + 4], mem[((*operand)*10) + 5],
            mem[((*operand)*10) + 6], mem[((*operand)*10) + 7],
            mem[((*operand)*10) + 8], mem[((*operand)*10) + 9]);
    
    
    printf("%s", "**************************************************************\n"); /*62 stars followed by a \n*/
    
    
}

int getCommandValue(char* key)
{
    if(strlen(key)==3)
    {
        if(0 == strcmp(key, "SET"))
            return 22;
        if(0 == strcmp(key, "ADD"))
                return 30;
        if(0 == strcmp(key, "SUB"))
            return 31;
        if(0 == strcmp(key, "DIV"))
            return 32;
        if(0 == strcmp(key, "MOD"))
            return 34;
    }
    else if(strlen(key)==4)
    {
        if(0 == strcmp(key, "READ"))
            return 10;
        if(0 == strcmp(key, "WRIT"))
            return 11;
        if(0 == strcmp(key, "PRNT"))
            return 12;
        if(0 == strcmp(key, "LOAD"))
            return 20;
        if(0 == strcmp(key, "STOR"))
            return 21;
        if(0 == strcmp(key, "MULT"))
            return 33;
        if(0 == strcmp(key, "BRAN"))
            return 40;
        if(0 == strcmp(key, "BRNG"))
            return 41;
        if(0 == strcmp(key, "BRZR"))
            return 42;
        if(0 == strcmp(key, "HALT"))
            return 99;
    }
    return CMPLR_ERR_UNKNOWN_CMD;
}
