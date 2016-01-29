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
 *  Latria_Jumps.c
 *  Latria
 *
 *  Created by Benjamin Friedman on 11/8/15.
 */

#include "Latria_Jumps.h"

LATFuncLabel *funcLabelStack = NULL;
int funcLabelIndex = 0;
int funcLabelMax = 0;

/* Pushes a new func label on the stack with a new given name and index */
void pushFuncLabel(long int instructionIndex, char *fname) {
    
    /* Check for existing defintion, if one is found update the instructionIndex and exit out */
    int x = 0;
    while(x < funcLabelIndex) {
        if(strcmp(funcLabelStack[x].funcName, fname) == 0) {
            /* Update this definition */
            funcLabelStack[x].instructionIndex = instructionIndex;
            return;
            
        }
        x++;
        
    }
    
    if(funcLabelIndex >= funcLabelMax-1) {
        if(funcLabelStack != NULL) {
            /* realloc */
            funcLabelMax+=10;
            funcLabelStack = LATAlloc(funcLabelStack, (size_t)(funcLabelMax-10) * sizeof(LATFuncLabel), (size_t)funcLabelMax * sizeof(LATFuncLabel));
            
        } else {
            /* malloc */
            funcLabelMax+=10;
            funcLabelStack = LATAlloc(NULL, 0, (size_t)funcLabelMax * sizeof(LATFuncLabel));
            
        }
    }
    
    /* Push the label */
    funcLabelStack[funcLabelIndex].instructionIndex = instructionIndex;
    funcLabelStack[funcLabelIndex].funcName = LATstrdup(fname);
    
    funcLabelIndex++;
}

/* Returns the file address of a function from a given fname passed */
long int getPositionFromFuncLabel(char *fname) {
    
    int x = 0;
    
    while(x < funcLabelIndex) {
        
        if(*fname == *funcLabelStack[x].funcName && strcmp(fname, funcLabelStack[x].funcName) == 0) {
            /* Match! */
            return funcLabelStack[x].instructionIndex;
            
        }
        
        x++;
    }
    
    /* None found! */
    printf("Unable to find a function label by the name of '%s'\n", fname);
    exit(59013);
    return 0;
}

/* Frees all label related data */
void freeAllLabels() {
    int curLabelIndex = 0;
    
    /* Free all function items */
    while(curLabelIndex < funcLabelIndex) {
        LATDealloc(funcLabelStack[curLabelIndex].funcName);
        curLabelIndex++;
    }
    
    LATDealloc(funcLabelStack),funcLabelStack = NULL;
}
