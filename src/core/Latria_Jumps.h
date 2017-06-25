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
 *  latria_jumps.h
 *  Created by Benjamin Friedman on 11/8/15.
 */

#ifndef latria_core_latria_jumps_h
#define latria_core_latria_jumps_h

#include "latria_core.h"

#define LAT_LABEL_STACK_INCREMENT 10

/* Function 'label' definition */
typedef struct {
    
    /* Instruction index of this label */
    long int instructionIndex;
    
    /* Func handle name */
    char *funcName;
    
}LATFuncLabel;

void pushFuncLabel(long int instructionIndex, char *fname);
long int getPositionFromFuncLabel(char *fname);

void freeAllLabels();

#endif /* latria_core_latria_jumps_h */
