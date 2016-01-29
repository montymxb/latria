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
 *  Latria_ReferenceStack.h
 *  Latria
 *
 *  Created by Benjamin Friedman on 2/11/15.
 */

#ifndef __Latria_ReferenceStack__
#define __Latria_ReferenceStack__

#include "Latria_InLang.h"

struct CoreObject *stack_getCoreObject();
struct LATFunction *stack_getLATFunction();

void stack_setCoreObject(struct CoreObject *_cop);
void stack_setLATFunction(struct LATFunction *_latf);

struct CoreObject *_stack_getCoreObject(struct LATReference_Stack *stack);
struct LATFunction *_stack_getLATFunction(struct LATReference_Stack *stack);

struct LATReference_Stack * getPrimaryStack();
struct LATReference_Stack * getNextStack(struct LATReference_Stack *stack);

void setTailCallName(char *tcn);
char *getTailCallName();
void clearTailCallName();

/* For regular return values */
void stack_setReturnValue(char *i);
void OVERRIDE_stack_setReturnValue(char *i);
char *stack_getReturnValue();

/* For returned objects (arrays) */
void stack_setReturnObject(struct CoreObject *co);
struct CoreObject *stack_getReturnObject();

LATBool getFunctionBlockActive();
char * getFunctionBlockName();
void setFunctionBlockName(char *_in);
void setBracketCount(unsigned int i);
unsigned int getBracketCount();

void stack_pushStack();
void stack_popStack();

void stack_freeHeapAndStack();

/* Forwardly declared*/
void freeAllObjects( struct CoreObject *node);
/* Forwardly declared*/
void LATF_freeAllObjects( struct LATFunction *node);

#endif /* defined(__Latria_ReferenceStack__) */
