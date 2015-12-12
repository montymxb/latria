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
//  Latria_Compiler_Interface.c
//  Latria
//
//  Created by Benjamin Friedman on 11/27/15.
*/

#include "Latria_Compiler_Interface.h"

/*
 *  This is the interface file for the latria compiler.
 *  It interacts with the IO module to convert source code to bytecode
 *
 */

#define INPUT_SIZE 1024

char input[INPUT_SIZE];

/* Takes given latria code & compiles it for increased efficiency (removes whitespace, tabs, comments, etc) */
/* Takes latria code from a provided file to compile */
void compileLatria(char *fn) {
    JumpUpdate *ju = NULL;
    unsigned int lineNum = 0;
    FILE *in, *out;
    size_t len = strlen(fn);
    unsigned char *returned;
    
    /* Create our output file name */
    char *nf = malloc(len+2);
    
    /* Copy the file name we were given */
    strncpy( nf, fn, len);
    
    /* Append 'c' to the end and \0 */
    nf[len] = 'c';
    nf[len+1] = '\0';
    
    /* Open the file to read */
    in = fopen(fn,"r");
    
    /* Check we have an opened file */
    if(in == NULL) {
        
        free(nf);
        
        /* no file */
        printf("\nThe file you provided %s could not be found!\n\n", fn);
        exit(1026);
        
        
    }
    
    out = fopen(nf, "wb");
    
    if(out == NULL) {
        
        free(nf);
        
        /* no file */
        printf("Tmp file file %s could not be opened/created to write to!\n\n", nf);
        exit(1027);
        
        
    }
    
    /* Read a line from our input */
    while(fgets( input, INPUT_SIZE, in)) {
        
        short retSize;
        
        /* Compile this line */
        compileLine(input);
        
        /* See if we read anything */
        if((retSize = getByteCodeCount()) > 0) {
            /* Copy our bytes in */
            returned = readByteCodes();
            
            fwrite(returned, sizeof(unsigned char), retSize, out);
            
        }
        
        lineNum++;
    }
    
    /* Finish up any pending conditional jumps */
    dispatchIfAndElseIf();
    
    while((ju = popJumpUpdate()) != NULL) {
        char jumpCode[LAT_ADDRESS_SIZE+1] = {0};
        fseek( out, ju->bytecodeAddr, SEEK_SET);
        
        /* Extract new jump address into a hex */
        sprintf(jumpCode, LAT_ADDRESS_FORMAT_STRING, ju->jumpAddr);
        
        fwrite(jumpCode, sizeof(unsigned char), LAT_ADDRESS_SIZE, out);
    }
    
    /* Close the rest of our files */
    fclose(in);
    fclose(out);
    
    /* Print out what we compiled */
    ////printf("compiled %s\n",nf);
    
    /* free */
    freeByteCodes();
    freeLexicalAllocations();
    deallocStackStates();
    
    free(nf);
}

unsigned char *byteCodes = NULL;
short maxByteCodeLength = 0;
short currentByteCodeLength = 0;

/* Compiles a given line into latria bytecodes, and stores them locally */
void compileLine(char *line) {
    unsigned char *retVal;
    short size = 0;
    short count = 0;
    lat_io_writeInput(line);
    /* Get what came back */
    retVal = lat_io_compiler_readOutput();
    
    if(retVal == NULL) {
        return;
    }
    
    if(maxByteCodeLength == 0) {
        /* Read the len */
        maxByteCodeLength = (*retVal++ * 256);
        maxByteCodeLength+= *retVal++;
        currentByteCodeLength = maxByteCodeLength;
        size = currentByteCodeLength;
        
        byteCodes = malloc(sizeof(unsigned char *) * (size_t)maxByteCodeLength);
        
    } else {
        /* Read new byte code size and see what we have to do */
        size = (*retVal++ * 256);
        size += *retVal++;
        
        currentByteCodeLength+=size;
        
        if(currentByteCodeLength > maxByteCodeLength) {
            /* realloc */
            maxByteCodeLength = currentByteCodeLength;
            byteCodes = realloc(byteCodes, sizeof(unsigned char *) * (size_t)maxByteCodeLength);
            
        }
    }
    
    /* Copy our bytes in */
    while(count <= size) {
        byteCodes[count+(currentByteCodeLength-size)] = *retVal++;
        count++;
        
    }
}

/* Returns the bytecodes */
unsigned char *readByteCodes() {
    currentByteCodeLength = 0;
    return byteCodes;
}

short getByteCodeCount() {
    return currentByteCodeLength;
}

void freeByteCodes() {
    maxByteCodeLength = 0;
    currentByteCodeLength = 0;
    free(byteCodes),byteCodes=NULL;
}
