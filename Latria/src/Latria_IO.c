/*
The MIT License (MIT)

Copyright (c) 2016 Benjamin Wilson Friedman

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
 *  Latria_IO.c
 *  Latria
 *
 *  Created by Benjamin Friedman on 11/3/15.
 */

#include "Latria_IO.h"

#define OUTPUT_STACK_SIZE 200


/* Our output stack, holds anything we return from latria */
char *outputVMStack[OUTPUT_STACK_SIZE];

/* Our output stack size, specificies how long each entry is */
short outputVMSize[OUTPUT_STACK_SIZE];

/* Current index of the stack */
short outputVMStackIndex = 0;

void lat_io_writeOutput(unsigned char *output, int size);


/* Writes to the output side of the IO module (called by interface, Latria.c) */
void lat_io_writeOutput(unsigned char *output, int size) {
    
    char *outputPointer;
    int count = 0;
    
    if(outputVMStackIndex > OUTPUT_STACK_SIZE-1) {
        
        printf("\n:>> Latria Stack overflow, too many values accumulated in the output stack in the IO module\n\n");
        exit(111);
    }
    
    /* malloc output pointer */
    outputPointer = malloc((size_t)(size+1)*sizeof(char));
    
    count = 0;
    
    /* write to output pointer */
    while(count < size) {
        
        outputPointer[count] = (char)output[(int)count];
        count++;
    }
    
    /* Write to output stack */
    outputVMSize[outputVMStackIndex] = (short)size;
    outputVMStack[outputVMStackIndex++] = outputPointer;
}


#define OUTPUT_CARRIER_LEN 1024

/* Holds values that are read externally from latria */
unsigned char outputVMCarrier[OUTPUT_CARRIER_LEN];


/* Reads from the output side of the IO module */
unsigned char *lat_io_readOutput() {
    
    int count = 0;
    char *outputStackItem;
    short outputLen;
    
    /* Check for not set or underflow */
    if(outputVMStackIndex <= 0) {
        
        /* Return NULL */
        return NULL;
    }
    
    /* Get size of output len */
    outputLen = outputVMSize[outputVMStackIndex-1];
    
    /* Check if larger than what we can handle */
    if(outputLen > OUTPUT_CARRIER_LEN-3) {
        
        /* Too large to handle */
        printf("\n:>> Output too long%hd\n\n", outputLen);
        exit(1104);
    }
    
    /* Copy our size for to the first 2 bytes (8 bits) */
    outputVMCarrier[0] = (unsigned char)outputLen/255;
    outputVMCarrier[1] = (unsigned char)outputLen;
    
    /* get stack item */
    outputStackItem = outputVMStack[outputVMStackIndex-1];
    
    /* Copy into our carrier */
    while(count < outputLen) {
        
        outputVMCarrier[count+2] = (unsigned char)outputStackItem[count];
        count++;
    }
    
    /* free our pointer & decrement stack index */
    free(outputVMStack[outputVMStackIndex--]);
    
    /* Return our outputCarrier */
    return outputVMCarrier;
    
}
