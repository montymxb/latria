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
 *  Latria_ControlFlow.c
 *  Latria
 *
 *  Created by Benjamin Friedman on 3/2/15.
 */

#include "Latria_ControlFlow.h"

/* Pre-defined control flow statement headers*/
#define LAT_IF      "if"
#define LAT_ELSE    "else"
#define LAT_ELSEIF  "elseif"

#define LAT_FOR     "for"
#define LAT_WHILE   "while"

/* Returns whether or not there is a currently ACTIVE control block */
LATBool isControlBlockActive() {
    return getControlFlowBlock()?true:false;
}

void appendContentToControlFlowBlock(struct ControlFlowBlock *cfb, char *input) {
    int i2c;
    int i2 = 0;
    char *input2;
    /* LATAlloc our input +1 for a \n*/
    if(!input) return; /* for internal function stacking*/
    
    i2c = (int)strlen(input);
    
    if(i2c == 0)
        return;
    
    input2 = LATAlloc(NULL, 0, sizeof(char)*(size_t)(i2c+2)); /* HISTORY issue with the +2 just before this, used to be +1, not quite long enough for the \0 being addded about 6 lines down, massive memory issues there*/
    
    while(i2 < i2c) {
        input2[i2] = input[i2];
        i2++;
    }
    
    input2[i2c] = '\n';
    input2[i2c+1] = '\0';
    if(cfb->content == 0) {
        char *ppt;
        /* memory not allocated, let's put it out there initially*/
        cfb->content = input2;
        ppt = cfb->content;
        ppt+=strlen(ppt);
        cfb->eol = ppt;
    } else {
        int fcLen,newLen;
        /* memory already exits,get existing size, LATAlloc to increase it up to the new desired bounds, and append our item using the Latria_Lib append function*/
        
        /* make i2c match our new size */
        i2c+=2;
        fcLen = (int)strlen(cfb->content);
        
        newLen = (int)(i2c + fcLen + 1);
        cfb->content = LATAlloc(cfb->content, (size_t)fcLen * sizeof(char), (size_t)newLen * sizeof(char));
        cfb->eol = LATstrcat(cfb->content,input2);
        LATDealloc(input2), input2 = NULL;
    }
}

void freeContentsOfControlFlowBlock(struct ControlFlowBlock *cfb) {
    if(cfb->content)
        LATDealloc(cfb->content);
    if(cfb->initializer)
        LATDealloc(cfb->initializer);
    if(cfb->limit)
        LATDealloc(cfb->limit);
    if(cfb->step)
        LATDealloc(cfb->step);
}
