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
 *  Latria_Compiler_Interface.c
 *  Latria
 *
 *  Created by Benjamin Friedman on 11/27/15.
 */

#include "Latria_Compiler_Interface.h"

/*
 *  This is the interface file for the latria compiler.
 *  It interacts with the IO module to convert source code to bytecode
 *
 */

#define INPUT_SIZE 1024

char input[INPUT_SIZE];


/* Takes given latria code & compiles it for increased efficiency (removes whitespace, tabs, comments, etc)
 * Takes latria code from a provided file to compile
 */
void compileLatria(char *fn) {
    
    JumpUpdate *ju = NULL;
    FILE *in, *out;
    size_t len = strlen(fn);
    
    /* Create our output file name */
    char *nf = malloc(len+2);
    
    /* Copy the file name we were given */
    strncpy( nf, fn, len);
    
    /* Append 'c' to the end and \0 (compiled files have the extension .lrac) */
    nf[len] = 'c';
    nf[len+1] = '\0';
    
    /* Open the file to read */
    in = fopen(fn,"r");
    
    /* Check we have an opened file */
    if(in == NULL) {
        
        /* We did NOT open a file, fail and exit */
        
        /* free our output file name */
        free(nf);
        
        /* no file */
        printf("\nThe file you provided %s could not be found!\n\n", fn);
        exit(1026);
    }
    
    /* Input file is good, open our new (compiled) file */
    out = fopen(nf, "wb");
    
    /* Validate we actually opened a file */
    if(out == NULL) {
        
        /* output file was not opened, cleanup and exit */
        free(nf);
        printf("Tmp file file %s could not be opened/created to write to!\n\n", nf);
        exit(1027);
    }
    
    /* A files are good, start reading input and compiling */
    
    /* Read a line from our input */
    while(fgets( input, INPUT_SIZE, in)) {
        
        short retSize;
        
        /* Compile this line of latria */
        compileLine(input);
        
        /* Check to see if this line created any bytecode to read */
        if((retSize = getByteCodeCount()) > 0) {
            
            /* Bytecode available, write it into our output file */
            fwrite(readByteCodes(), sizeof(unsigned char), (size_t)retSize, out);
            
        }
    }
    
    /* Dispatch up any pending conditional jumps */
    dispatchIfAndElseIf();
    
    /* Loop over any jump updates we need to process */
    while((ju = popJumpUpdate()) != NULL) {
        
        /* Create an array to hold our new jump instruction address */
        char jumpCode[LAT_ADDRESS_SIZE+1] = {0};
        
        /* Seek to the indicated address to update */
        fseek( out, ju->bytecodeAddr, SEEK_SET);
        
        /* Convert our new jump address into a hexcode */
        sprintf(jumpCode, LAT_ADDRESS_FORMAT_STRING, ju->jumpAddr);
        
        /* Update the address we are currently at with our new hex one */
        fwrite(jumpCode, sizeof(unsigned char), LAT_ADDRESS_SIZE, out);
        
    }
    
    /* Close our input and output files */
    fclose(in);
    fclose(out);
    
    /* free underlying byte code allocations */
    freeByteCodes();
    
    /* free underlying lexical allocations */
    freeLexicalAllocations();
    
    /* deallocate the stack */
    deallocStackStates();
    
    /* free our compiled file name */
    free(nf);
}


/* Holds a reference to bytecodes */
unsigned char *byteCodes = NULL;

/* maximum length of bytecodes we can store in the above pointer currently */
short maxByteCodeLength = 0;

/* current length of bytecodes */
short currentByteCodeLength = 0;


/* Compiles a given line into latria bytecodes, and stores it */
void compileLine(char *line) {
    
    unsigned char *retVal;
    short size = 0;
    short count = 0;
    
    /* send this line into our IO module to be compiled */
    lat_io_writeInput(line);
    
    /* Read the results from our output module */
    retVal = lat_io_compiler_readOutput();
    
    /* Check if we got any bytecodes back */
    if(retVal == NULL) {
        
        /* No bytecodes read, return */
        return;
    }
    
    /* Check if we need to allocate to fit these bytecodes */
    if(maxByteCodeLength == 0) {
        
        /* The first 2 bytes of our returned bytecodes are a hex representation of the length of data to read, extract them */
        maxByteCodeLength = (short)(*retVal++ * 256);
        maxByteCodeLength = (short)(maxByteCodeLength + *retVal++);
        
        /* Indicate our current length is our max (what we read) */
        currentByteCodeLength = maxByteCodeLength;
        
        /* Mark the size of the bytes we just read */
        size = currentByteCodeLength;
        
        /* allocate room for our bytecodes */
        byteCodes = malloc(sizeof(unsigned char *) * (size_t)maxByteCodeLength);
        
    } else {
        
        /* We have already allocated room, extract the first 2 bytes to get the hex representation of the bytecode length */
        size = (short)(*retVal++ * 256);
        size = (short)(size +*retVal++);
        
        /* Indicate our current length is increase by the size of the codes we just read */
        currentByteCodeLength = (short)(currentByteCodeLength + size);
        
        /* Check if we need to realloc up */
        if(currentByteCodeLength > maxByteCodeLength) {
            
            /* We need more room for these bytes, up our max to the current count and realloc */
            maxByteCodeLength = currentByteCodeLength;
            byteCodes = realloc(byteCodes, sizeof(unsigned char *) * (size_t)maxByteCodeLength);
            
        }
    }
    
    /* Copy our bytes into our byte pointer from our current offest */
    while(count <= size) {
        
        byteCodes[count+(currentByteCodeLength-size)] = *retVal++;
        count++;
    }
}


/* Returns currently stored bytecodes, and resets the current length to 0 */
unsigned char *readByteCodes() {
    
    currentByteCodeLength = 0;
    return byteCodes;
}


/* Returns the current length of stored bytecodes */
short getByteCodeCount() {
    
    return currentByteCodeLength;
}


/* frees stored bytecodes and resets related variables */
void freeByteCodes() {
    
    maxByteCodeLength = 0;
    currentByteCodeLength = 0;
    free(byteCodes),byteCodes=NULL;
}
