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
 *  Latria (Atria Lang)
 */

#include "Latria_Function.h"

/*
 * latria function 'class'
 *
 */

long int *revertStates  = NULL;
int revertStatesMax     = 0;
int revertStatesIndex   = 0;


/* Adds a function revert state */
void pushFunctionRevertState(long int index) {
    
    if(revertStatesIndex > revertStatesMax-1) {
        
        if(revertStates != NULL) {
            
            /* realloc */
            revertStatesMax+=10;
            revertStates = LATAlloc(revertStates, (size_t)(revertStatesMax-10) * sizeof(long int), (size_t)revertStatesMax * sizeof(long int));
            
        } else {
            
            /* malloc */
            revertStatesMax+=10;
            revertStates = LATAlloc(NULL, 0, (size_t)revertStatesMax * sizeof(long int));
            
        }
    }
    
    /* Set this revert state */
    revertStates[revertStatesIndex] = index;
    /* bump */
    revertStatesIndex++;
    
}


/* Returns the last function revert state */
long int popFunctionRevertState() {
    
    return revertStates[--revertStatesIndex];
}


/* Starts mem freeing */
void LATF_freeObjects() {
    
    LATDealloc(revertStates), revertStates = NULL;
}
