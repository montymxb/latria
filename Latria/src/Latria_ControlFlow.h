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
 *  Latria_ControlFlow.h
 *  Latria
 *
 *  Created by Benjamin Friedman on 3/2/15.
 */

#ifndef __Latria_ControlFlow__
#define __Latria_ControlFlow__

#include "Latria_Operators.h"

struct ControlFlowBlock {
    
    int type; /* the type of block this is*/
    /*
     if=1
     for=2
     elseif=3
     else=4
     while=5
     */
    
    char *initializer; /* initialize of for*/
    char *limit; /* limit of for/while*/
    char *step; /* step of for loop*/
    
    char *content; /* content within this block*/
    char *eol; /* end of line, for easy concatenation to content*/
    int brackCount;
    
    int state; /* tells us the current state of this block, and whether to do anything with it at all*/
    LATBool evaluatedAs;
    /*
     0, set up as false
     1, set up as true
     2, finished and should be popped, however this tells us that a subseqent elseif/else if valid. If we see an else/elseif without a block at state 2 then we should absolutely throw an error about it.
     */
    
    struct ControlFlowBlock *nxtBlock;
};

/* exterior declarations*/
struct ControlFlowBlock * getControlFlowBlock();
void setControlFlowBlock(struct ControlFlowBlock *cfb);
void appendContentToControlFlowBlock();
void freeContentsOfControlFlowBlock(struct ControlFlowBlock *cfb);

LATBool isControlBlockActive();

#endif /* defined(__Latria_ControlFlow__) */
