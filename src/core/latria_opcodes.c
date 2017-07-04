/*
 The MIT License (MIT)
 
 Copyright (c) 2017 Benjamin Wilson Friedman
 
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
 * latria_opcodes.c
 * Created by Benjamin Friedman on 06/24/17.
 */

#include "latria_opcodes.h"


#define INSTRUCTION_STEP 100

/* Normal instruction components */
unsigned char *instructions = NULL;
int instructionSize = 0;
int instructionCount = 0;

/* Batched instruction components */
unsigned char *batchedInstructions = NULL;
int batchedInstructionSize = 0;
int batchedInstructionCount = 0;

long int programByteCount = 0;


#if defined(INCLUDECOMPILER) || defined(COMPILER_ONLY)

/* write mode NORMAL by default */
CompilerWriteMode writeMode = WRITE_MODE_NORMAL;

void startInstructions() {
    
    instructionSize = INSTRUCTION_STEP;
    instructions = malloc((size_t)instructionSize*sizeof(char));
    *instructions = '\0';
    programByteCount+=instructionCount;
    instructionCount = 0;
    
}

/* Writes out an OP code */
void writeOutCode(char code) {
    
    if(writeMode == WRITE_MODE_NORMAL) {
        
        /* Normal Mode */
        
        /* Check if too large */
        if(instructionCount+COMMAND_SEQUENCE_SIZE >= instructionSize-1) {
            
            /* realloc up */
            instructionSize+=INSTRUCTION_STEP;
            instructions = realloc(instructions, (size_t)instructionSize);
            
        }
        
        /* Copy in this instruction */
        instructions[instructionCount++] = (unsigned char)code;
        
        /* Close */
        instructions[instructionCount] = '\0';
        
    } else {
        
        /* Batched Mode */
        
        /* Check if too large */
        if(batchedInstructionCount+COMMAND_SEQUENCE_SIZE >= batchedInstructionSize-1) {
            
            /* realloc up */
            batchedInstructionSize+=INSTRUCTION_STEP;
            
            if(batchedInstructions != NULL) {
                
                /* realloc */
                batchedInstructions = realloc(batchedInstructions, (size_t)batchedInstructionSize*sizeof(char));
                
            } else {
                
                /* malloc */
                batchedInstructions = malloc((size_t)batchedInstructionSize*sizeof(char));
                
            }
            
        } else if(batchedInstructions == NULL) {
            
            batchedInstructionSize+=INSTRUCTION_STEP;
            /* malloc */
            batchedInstructions = malloc((size_t)batchedInstructionSize*sizeof(char));
            
        }
        
        /* Copy in this instruction */
        batchedInstructions[batchedInstructionCount++] = (unsigned char)code;
        
        /* Close */
        batchedInstructions[batchedInstructionCount] = '\0';
        
    }
    
}


/* Writes an arbitrary character */
void writeOutCharacter(char code) {
    
    if(writeMode == WRITE_MODE_NORMAL) {
        
        /* Normal Mode */
        
        /* Check if too large */
        if(instructionCount+2 >= instructionSize-1) {
            
            /* realloc up */
            instructionSize+=INSTRUCTION_STEP;
            instructions = realloc(instructions, (size_t)instructionSize);
            
        }
        
        /* Copy in this instruction and bump */
        instructions[instructionCount++] = (unsigned char)code;
        /* Close */
        instructions[instructionCount] = '\0';
        
    } else {
        
        /* Batched Mode */
        
        /* Check if too large */
        if(batchedInstructionCount+2 >= batchedInstructionSize-1) {
            
            /* realloc up */
            batchedInstructionSize+=INSTRUCTION_STEP;
            
            if(batchedInstructions != NULL) {
                
                /* realloc */
                batchedInstructions = realloc(batchedInstructions, (size_t)batchedInstructionSize*sizeof(char));
                
            } else {
                
                /* malloc */
                batchedInstructions = malloc((size_t)batchedInstructionSize*sizeof(char));
                
            }
            
        } else if(batchedInstructions == NULL) {
            
            batchedInstructionSize+=INSTRUCTION_STEP;
            /* malloc */
            batchedInstructions = malloc((size_t)batchedInstructionSize*sizeof(char));
            
        }
        
        /* Copy in this instruction and bump */
        batchedInstructions[batchedInstructionCount++] = (unsigned char)code;
        /* Close */
        batchedInstructions[batchedInstructionCount] = '\0';
    }
    
}


/* Writes out an array of characters */
void writeOutCharacters(char *string) {
    
    size_t len = strlen(string);
    int count = 0;
    
    if(writeMode == WRITE_MODE_NORMAL) {
        
        /* Normal Mode */
        
        /* Check if too large */
        if(instructionCount+(int)len+1 >= instructionSize-1) {
            
            /* realloc up */
            instructionSize+=(int)(INSTRUCTION_STEP > len ? INSTRUCTION_STEP : len + INSTRUCTION_STEP);
            instructions = realloc(instructions, (size_t)instructionSize);
            
        }
        
        /* Copy in this instruction and bump */
        while(count < (int)len) {
            
            instructions[instructionCount++] = (unsigned char)string[count];
            count++;
            
        }
        
        /* Close */
        instructions[instructionCount++] = 0;
        
    } else {
        
        /* Batched Mode */
        
        /* Check if too large */
        if(batchedInstructionCount+(int)len+1 >= batchedInstructionSize-1) {
            
            /* realloc up */
            batchedInstructionSize+=(int)(INSTRUCTION_STEP > len ? INSTRUCTION_STEP : len + INSTRUCTION_STEP);
            
            if(batchedInstructions != NULL) {
                
                /* realloc */
                batchedInstructions = realloc(batchedInstructions, (size_t)batchedInstructionSize*sizeof(char));
                
            } else {
                
                /* malloc */
                batchedInstructions = malloc((size_t)batchedInstructionSize*sizeof(char));
                
            }
            
        } else if(batchedInstructions == NULL) {
            
            batchedInstructionSize+=(int)(INSTRUCTION_STEP > len ? INSTRUCTION_STEP : len + INSTRUCTION_STEP);
            /* malloc */
            batchedInstructions = malloc((size_t)batchedInstructionSize*sizeof(char));
            
        }
        
        /* Copy in this instruction and bump */
        while(count < (int)len) {
            
            batchedInstructions[batchedInstructionCount++] = (unsigned char)string[count];
            count++;
            
        }
        
        /* Close */
        batchedInstructions[batchedInstructionCount++] = '\0';
    }
}


/* Used to write out precompiled RAW instructions */
void writeRAW(unsigned char *string, int size) {
    
    int x = 0;
    
    /* Check if too large */
    if(instructionCount+size >= instructionSize-1) {
        
        /* realloc up to whichever is largest */
        instructionSize+=(INSTRUCTION_STEP < size ? size : INSTRUCTION_STEP+size+1);
        instructions = realloc(instructions, (size_t)instructionSize);
        
    }
    
    /* Copy in this instruction and bump */
    while(x < size) {
        
        instructions[instructionCount++] = string[x];
        x++;
        
    }
    
    instructions[instructionCount] = '\0';
}

/* Dispatches all queued instructions */
void dispatchInstructions() {
    
    if(instructions != NULL && instructionCount > 0) {
        
        lat_io_compiler_writeOutput(instructions, instructionCount);
    }
    
    if(instructions != NULL) {
        
        free(instructions), instructions = NULL;
    }
}


/* Changes the write mode */
void setWriteMode(CompilerWriteMode mode) {
    
    if(mode == WRITE_MODE_BATCHED) {
        
        /* reset */
        batchedInstructionCount = 0;
    }
    
    writeMode = mode;
}


/* Returns batched bytecodes */
unsigned char *readBatchedData() {
    
    return batchedInstructions;
}


/* Returns the size of batched bytecode instructions */
int readBatchedDataSize() {
    
    return batchedInstructionCount;
}


/* free and reset */
void unsetBatchedData() {
    
    free(batchedInstructions), batchedInstructions = NULL;
    batchedInstructionCount = 0;
}


/* null and reset */
void nullifyBatchedData() {
    
    batchedInstructions = NULL;
    batchedInstructionCount = 0;
}


/* Returns the number of bytes the program being written out is composed of currently */
long int getProgramByteCount() {
    
    return programByteCount+instructionCount;
}


/* Writes out an LAT_ADDRESS_SIZE sized byte address (probably 8 bytes) from an unsigned int */
void writeOutByteAddress(unsigned int address) {
    
    char hex[LAT_ADDRESS_SIZE+1];
    int size = LAT_ADDRESS_SIZE;
    int x = 0;
    
    /* Check if too large */
    if(instructionCount+size >= instructionSize-1) {
        /* realloc up to whichever is largest */
        instructionSize+=(INSTRUCTION_STEP < size ? size : INSTRUCTION_STEP+size+1);
        instructions = realloc(instructions, (size_t)instructionSize);
        
    }
    
    /* Generate LAT_ADDRESS_FORMAT_STRING byte hex from unsigned int */
    sprintf(hex, LAT_ADDRESS_FORMAT_STRING, address);
    
    /* Copy in this instruction and bump */
    while(x < size) {
        
        instructions[instructionCount++] = (unsigned char)hex[x];
        x++;
        
    }
    
    instructions[instructionCount] = '\0';
}

#endif
