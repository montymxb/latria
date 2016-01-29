/*
The MIT License (MIT)

Copyright (c) 2015 Benjamin Wilson Friedman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 *  Latria_Decompiler.c
 *  Latria
 *
 *  Created by Benjamin Friedman on 11/4/15.
 */

#include "Latria_Decompiler.h"

#define LATRIA_VERSION_NUMBER "0.1.1"

#ifndef LATRIA_EMBEDDED
int main( int argc, char* argv[]) {
    int argI;
    
    /* Iterate over ARGS */
    if(argc > 1) {
        
        /* Potential args were passed, validate and run what we find */
        for(argI = 1; argI < argc; argI++) {
            
            /* Extract this arg */
            char *arg = argv[argI];
            
            if(*arg == '-') {
                
                /* It's a command line arg, parse it */
                if(strlen(arg) > 0) {
                    
                    switch(*(arg+1)) {
                            
                        case 'v':
                            /* Print Version option */
                            printf("\nLatria Disassembler v%s\n\n", LATRIA_VERSION_NUMBER);
                            break;
                            
                        case 'h':
                            /* Print help */
                            printf("\n:: Help ::\n\n");
                            printf("usage:\n");
                            printf("latriad test.lrac (creates a test.lrac.txt file)\n");
                            printf("\n\n");
                            printf("-v    Prints out the version number\n");
                            printf("-h    Prints out the help (this)\n");
                            printf("\n\n");
                            break;
                            
                        default:
                            /* Unrecognized command line arg */
                            printf("\n:>> Unrecognized command line arg passed: %s\n\n",arg);
                            break;
                            
                    }
                } else {
                    
                    /* Empty command line arg passed */
                    printf("\nPassed an empty '-' with no parameter after it\n\n");
                    exit(1);
                    
                }
            } else {
                
                /* Attempt to compile passed in file */
                decompileLatria(arg);
            }
        }
    } else {
        
        /* No args passed, prompt user to run with stuff passed in */
        printf("\n\nusage:\n");
        printf("latriad test.lrac (creates a test.lrac.txt file)\n\n");
    }
    
    return 0;
}
#endif

int instructionCounter;

/* Hande for input file */
FILE *in;

/* Takes given latria code & compiles it for increased efficiency (removes whitespace, tabs, comments, etc)*/
void decompileLatria(char *fn) {
    char hc;
    FILE *out;
    size_t len = strlen(fn);
    
    /* Create our output file name */
    char *nf = malloc(len+5);
    
    /* Copy the file name we were given */
    strncpy( nf, fn, len);
    
    instructionCounter = 0;
    
    /* Append 'c' to the end and \0 */
    nf[len] = '.';
    nf[len+1] = 't';
    nf[len+2] = 'x';
    nf[len+3] = 't';
    nf[len+4] = '\0';
    
    /* Open the file to read */
    in = fopen(fn,"rb");
    
    /* Check we have an opened file */
    if(in == NULL) {
        
        free(nf);
        
        /* no file */
        printf("\nThe file you provided %s could not be found!\n\n", fn);
        exit(1026);
        
    }
    
    /* Open the file to read out */
    out = fopen(nf, "w");
    
    /* Check we have an out file */
    if (out == NULL) {
        
        free(nf);
        
        /* no file */
        printf("The file %s could not be opened/created to write to!\n\n", nf);
        exit(1027);
        
    }
        
    while((hc = (char)getCurrentChar()) != EOF) {
        
        /* Print out the address of this line (one less than max) */
        fprintf(out, "0x%04lx  ", ftell(in)-1);
        
        instructionCounter++;

        /* Sequence Location is beginning or at a read code */
            
        switch(hc) {
            
            case OP_SET:
                fprintf(out, "SET ");
                /* Next byte is a var */
                
                /* Fast forward */
                while((hc = (char)getCurrentChar())) {
                    fprintf(out, "%c", hc);
                }
                
                /* Next should be a register and then done */
                fprintf(out, " 0x%02x\n", (char)getCurrentChar());
                
                break;
                
            case OP_SLEEP:
                fprintf(out, "SLEEP\n");
                break;
                
            case OP_PUSH_STACK:
                fprintf(out, "PUSH STACK\n");
                break;
                
            case OP_POP_STACK:
                fprintf(out, "POP STACK\n");
                break;
                
            case OP_SUBSTR:
                fprintf(out, "SUBSTR\n");
                break;
                
            case OP_MULTI:
                fprintf(out, "MULTI ");
                fprintf(out, "0x%02x, ",(char)getCurrentChar());
                fprintf(out, "0x%02x ",(char)getCurrentChar());
                fprintf(out, "0x%02x\n",(char)getCurrentChar());
                break;
                
            case OP_UNSET:
                fprintf(out, "UNSET ");
                
                /* Fast forward */
                while((hc = (char)getCurrentChar())) {
                    fprintf(out, "%c", hc);
                }
                
                fprintf(out, "\n");
                
                break;
                
            case OP_WRITE:
                fprintf(out, "WRITE\n");
                break;
                
            case OP_JUMP:
                fprintf(out, "JUMP 0x%c",(char)getCurrentChar());
                fprintf(out, "%c",(char)getCurrentChar());
                fprintf(out, "%c",(char)getCurrentChar());
                fprintf(out, "%c\n",(char)getCurrentChar());
                break;
                
            case OP_CALL:
                fprintf(out, "CALL ");
                
                /* Fast forward */
                while((hc = (char)getCurrentChar())) {
                    fprintf(out, "%c", hc);
                }
                
                fprintf(out, "\n");
                
                break;
                
            case OP_CLOSE:
                fprintf(out, "CLOSE\n");
                break;
                
            case OP_DIVI:
                fprintf(out, "DIVI ");
                fprintf(out, "0x%02x, ",(char)getCurrentChar());
                fprintf(out, "0x%02x ",(char)getCurrentChar());
                fprintf(out, "0x%02x\n",(char)getCurrentChar());
                break;
                
            case OP_EXEC:
                fprintf(out, "EXEC\n");
                while((hc = (char)getCurrentChar()) != OP_EXEC) {
                    if(hc == '\0') {
                        fprintf(out, "\n");
                    } else {
                        fprintf(out,"%c", hc);
                    }
                }
                fprintf(out, "EXEC\n");
                break;
                
            case OP_EXPONENT:
                fprintf(out, "EXPONENT ");
                fprintf(out, "0x%02x, ",(char)getCurrentChar());
                fprintf(out, "0x%02x ",(char)getCurrentChar());
                fprintf(out, "0x%02x\n",(char)getCurrentChar());
                break;
                
            case OP_FIND:
                fprintf(out, "FIND\n");
                break;
                
            case OP_FUNC:
                fprintf(out, "FUNC ");
                /* Fast forward */
                while((hc = (char)getCurrentChar())) {
                    fprintf(out, "%c", hc);
                }
                fprintf(out, "\n");
                break;
                
            case OP_COND_JUMP:
                fprintf(out, "COND_JUMP 0x%02x %d, 0x%c%c%c%c\n",(char)getCurrentChar(),(char)getCurrentChar(),(char)getCurrentChar(),(char)getCurrentChar(),(char)getCurrentChar(),(char)getCurrentChar());
                break;
                
            case OP_INPUT:
                fprintf(out, "INPUT\n");
                break;
                
            case OP_EQUAL:
                fprintf(out, "EQUAL ");
                fprintf(out, "0x%02x, ",(char)getCurrentChar());
                fprintf(out, "0x%02x ",(char)getCurrentChar());
                fprintf(out, "0x%02x\n",(char)getCurrentChar());
                break;
                
            case OP_AND:
                fprintf(out, "AND ");
                printf("And << NOT YET IMPLEMENTED >>\n");
                exit(1);
                break;
                
            case OP_SUB:
                fprintf(out, "SUB ");
                fprintf(out, "0x%02x, ",(char)getCurrentChar());
                fprintf(out, "0x%02x ",(char)getCurrentChar());
                fprintf(out, "0x%02x\n",(char)getCurrentChar());
                break;
                
            case OP_MODULUS:
                fprintf(out, "MODULUS ");
                fprintf(out, "0x%02x, ",(char)getCurrentChar());
                fprintf(out, "0x%02x ",(char)getCurrentChar());
                fprintf(out, "0x%02x\n",(char)getCurrentChar());
                break;
                
            case OP_OPEN:
                fprintf(out, "OPEN\n");
                break;
                
            case OP_PARENC:
                fprintf(out, "PAREN-CLOSE ");
                printf("Paren-Close << NOT YET IMPLEMENTED >>\n");
                exit(1);
                break;
                
            case OP_PARENO:
                fprintf(out, "PAREN-OPEN ");
                printf("Paren-Open << NOT YET IMPLEMENTED >>\n");
                exit(1);
                break;
                
            case OP_ADD:
                fprintf(out, "ADD ");
                fprintf(out, "0x%02x, ",(char)getCurrentChar());
                fprintf(out, "0x%02x ",(char)getCurrentChar());
                fprintf(out, "0x%02x\n",(char)getCurrentChar());
                break;
                
            case OP_POP:
                fprintf(out, "POP ");
                printf("Pop << NOT YET IMPLEMENTED (on its own) >>\n");
                exit(1);
                break;
                
            case OP_PLATFORM:
                fprintf(out, "PLATFORM-NAME\n");
                break;
                
            case OP_TIME:
                fprintf(out, "TIME\n");
                break;
                
            case OP_PRINT:
                fprintf(out, "PRINT\n");
                break;
                
            case OP_PRINT_MEM:
                fprintf(out, "PRINT-MEM\n");
                break;
                
            case OP_PURGE:
                fprintf(out, "PURGE\n");
                break;
                
            case OP_PUSH:
                fprintf(out, "PUSH 0x%02x\n", (char)getCurrentChar());
                break;
                
            case OP_RAND:
                fprintf(out, "RANDOM\n");
                break;
                
            case OP_READ:
                fprintf(out, "READ\n");
                break;
                
            case OP_REG_MATCH:
                fprintf(out, "REGEX-MATCH\n");
                break;
                
            case OP_GET_CAP:
                fprintf(out, "GET-CAPTURE\n");
                break;
                
            case OP_REMOVE:
                fprintf(out, "REMOVE\n");
                break;
                
            case OP_REPLACE:
                fprintf(out, "REPLACE\n");
                break;
                
            case OP_RETURN:
                fprintf(out, "RETURN 0x%02x\n", (char)getCurrentChar());
                break;
                
            case OP_SEED_RAND:
                fprintf(out, "SEED-RAND\n");
                break;
                
            case OP_TRUE:
                fprintf(out, "TRUE ");
                printf("True << NOT YET IMPLEMENTED >>\n");
                exit(1);
                break;
                
            case OP_FALSE:
                fprintf(out, "FALSE ");
                printf("False << NOT YET IMPLEMENTED >>\n");
                exit(1);
                break;
                
            case OP_OR:
                fprintf(out, "OR ");
                printf("OR << NOT YET IMPLEMENTED >>\n");
                exit(1);
                break;
                
            case OP_LESST:
                fprintf(out, "LESS-THAN ");
                fprintf(out, "0x%02x, ",(char)getCurrentChar());
                fprintf(out, "0x%02x ",(char)getCurrentChar());
                fprintf(out, "0x%02x\n",(char)getCurrentChar());
                break;
                
            case OP_GREATRT:
                fprintf(out, "GREATER-THAN ");
                fprintf(out, "0x%02x, ",(char)getCurrentChar());
                fprintf(out, "0x%02x ",(char)getCurrentChar());
                fprintf(out, "0x%02x\n",(char)getCurrentChar());
                break;
                
            case OP_LESST_EQUAL:
                fprintf(out, "LESS-THAN-EQUAL ");
                fprintf(out, "0x%02x, ",(char)getCurrentChar());
                fprintf(out, "0x%02x ",(char)getCurrentChar());
                fprintf(out, "0x%02x\n",(char)getCurrentChar());
                break;
                
            case OP_GTRTHN_EQL:
                fprintf(out, "GREATER-THAN-EQUAL ");
                fprintf(out, "0x%02x, ",(char)getCurrentChar());
                fprintf(out, "0x%02x ",(char)getCurrentChar());
                fprintf(out, "0x%02x\n",(char)getCurrentChar());
                break;
                
            case OP_NOT_EQUAL:
                fprintf(out, "NOT-EQUAL ");
                fprintf(out, "0x%02x, ",(char)getCurrentChar());
                fprintf(out, "0x%02x ",(char)getCurrentChar());
                fprintf(out, "0x%02x\n",(char)getCurrentChar());
                break;
                
            case OP_BITAND:
                fprintf(out, "BIT-AND\n");
                break;
                
            case OP_BITNOT:
                fprintf(out, "BIT-NOT\n");
                break;
                
            case OP_BITOR:
                fprintf(out, "BIT-OR\n");
                break;
                
            case OP_BITXOR:
                fprintf(out, "BIT-XOR\n");
                break;
                
            case OP_ARRAY_SET:
                fprintf(out, "ARRAY-SET ");
                /* Next byte is a var */
                
                /* Fast forward */
                while((hc = (char)getCurrentChar())) {
                    fprintf(out, "%c", hc);
                }
                
                fprintf(out, " ");
                
                if((hc = (char)getCurrentChar()) > 2) {
                    /* normal text value */
                    fprintf(out, "%c", hc);
                    
                    /* Fast forward */
                    while((hc = (char)getCurrentChar())) {
                        fprintf(out, "%c", hc);
                    }
                    
                    fprintf(out, " ");
                    
                } else {
                    /* register address */
                    fprintf(out, "0x%02x", hc);
                    
                }
                
                /* Next should be a register and then done */
                fprintf(out, " 0x%02x\n", (char)getCurrentChar());
                break;
                
            case OP_FUNC_END:
                fprintf(out, "FUNC-END\n");
                break;
                
            case OP_VAR:
                fprintf(out, "VAR ");
                printf("Var << NOT YET IMPLEMENTED >>\n");
                exit(1);
                break;
                
            case OP_STRING:
                fprintf(out, "STRING ");
                printf("String << NOT YET IMPLEMENTED >>\n");
                exit(1);
                break;
                
            case OP_NUM:
                fprintf(out, "NUM ");
                printf("Num << NOT YET IMPLEMENTED >>\n");
                exit(1);
                break;
                
            case OP_BOOL:
                fprintf(out, "BOOL ");
                printf("Bool << NOT YET IMPLEMENTED >>\n");
                exit(1);
                break;
                
            case OP_LOAD:
                fprintf(out, "LOAD\n");
                break;
                
            case OP_CONTINUE:
                fprintf(out, "CONTINUE\n");
                break;
                
            case OP_BREAK:
                fprintf(out, "BREAK\n");
                break;
                
            case OP_SQRT:
                fprintf(out, "SQRT\n");
                break;
                
            case OP_STRT_SERV:
                fprintf(out, "START-SERVER\n");
                break;
                
            case OP_SEND_DATA:
                fprintf(out, "SEND-DATA\n");
                break;
                
            case OP_READ_DATA:
                fprintf(out, "READ-DATA\n");
                break;
                
            case OP_CLOSE_CONN:
                fprintf(out, "CLOSE-CONNECTION\n");
                break;
                
            case OP_CONNECT:
                fprintf(out, "CONNECT\n");
                break;
                
            case OP_GC_RATE:
                fprintf(out, "ALTER_GC_RATE\n");
                break;
                
            case OP_RUN:
                fprintf(out, "RUN\n");
                break;
                
            case OP_RGI:
                /* 1 byte register */
                fprintf(out, "REGISTER ");
                /* 1 byte address */
                fprintf(out,"0x%02x ", (char)getCurrentChar());
                /* 1 byte type */
                
                hc = (char)getCurrentChar();
                
                if(hc == OP_VAR) {
                    /* var */
                    fprintf(out, "VAR ");
                    
                } else if(hc == OP_NUM) {
                    /* num */
                    fprintf(out, "NUM ");
                    
                } else if(hc == OP_STRING) {
                    /* string */
                    fprintf(out, "STRING ");
                    
                } else if(hc == OP_TRUE) {
                    /* true */
                    fprintf(out, "TRUE\n");
                    break;
                    
                } else if(hc == OP_FALSE) {
                    /* false */
                    fprintf(out, "FALSE\n");
                    break;
                    
                } else if(hc == OP_POP) {
                    /* Sets register to first value we just popped off the stack! */
                    fprintf(out, "POP\n");
                    break;
                    
                } else if(hc == OP_ARRAY_VAR) {
                    /* varkey (indicates we are setting a variable key) */
                    fprintf(out, "ARRAY-VAR ");
                    
                    /* Print out var */
                    while((hc = (char)getCurrentChar())) {
                        fprintf(out, "%c", hc);
                    }
                    
                    fprintf(out, " ");
                    
                    /* Print out key */
                    /* Fast forward */
                    while((hc = (char)getCurrentChar())) {
                        fprintf(out, "%c", hc);
                    }
                    
                    /* Break line and break out */
                    fprintf(out, "\n");
                    break;
                    
                } else if(hc == OP_RGI) {
                    /* register to register */
                    fprintf(out, "REGISTER ");
                    
                } else {
                    printf("Unrecognized type in register set command\n");
                    exit(9410);
                    
                }
                
                if(hc != OP_RGI) {
                    /* multi byte value */
                    /* Fast forward */
                    while((hc = (char)getCurrentChar())) {
                        fprintf(out, "%c", hc);
                    }
                    
                } else {
                    /* print out 2nd register address */
                    fprintf(out,"0x%02x", (char)getCurrentChar());
                    
                }
                
                fprintf(out,"\n");
                break;
            
            default:
                /* Read something that was not a registered op code! */
                printf("Instruction # %d\n", instructionCounter);
                printf("0x%02x\n",getCharByOffsetFromCurrent(-2));
                printf("0x%02x\n",(char)getCurrentChar());
                printf("Unrecognized OP Code: 0x%02x\n", (char)getCurrentChar());
                printf("0x%02x\n",(char)getCurrentChar());
                printf("0x%02x\n",(char)getCurrentChar());
                exit(1490);
                break;
                
        }
        
    }
    
    /* Close both files and be done! */
    fclose(in);
    fclose(out);
    
    /* Print out what we compiled */
    printf("disassembled %s\n",nf);
    
    free(nf);
}

/* Returns the current character */
int getCurrentChar() {
    return fgetc(in);
}

/* Returns a character relative to the current character */
int getCharByOffsetFromCurrent(int offset) {
    if(fseek(in, offset, SEEK_CUR) == 0) {
        return (char)getCurrentChar();
        
    } else {
        printf("Invalid seek!\n");
        exit(4091);
        return EOF;
        
    }
}

/* Moves to the next character */
void moveToNextChar() {
    fgetc(in);
}
