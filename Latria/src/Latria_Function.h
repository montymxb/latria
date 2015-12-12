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
 *  Latria (Atria Lang)
 */

#ifndef __Latria_Function__
#define __Latria_Function__

#include "Latria_ReferenceStack.h"

/* Push & Pop for moving around in our instructions when exiting a function call */
void pushFunctionRevertState(long int index);
long int popFunctionRevertState();

/*Creates a block with given key*/
struct LATFunction* createLATFunction(char * key);

/*Appends given char * to a block*/
void appendCharsToLATFunction(struct LATFunction *block, char *input);

/*Destroys a given block*/
void destroyLATFunction();

struct LATFunction * LATF_findObject(char *key);
struct LATFunction * LATF_findObjectWithStack(char *key, struct LATReference_Stack *stack);
struct LATFunction * LATF_findObjectWithKey(char *key, struct LATFunction **node);
struct LATFunction * LATF_generateReplacement(struct LATFunction *node);

struct LATFunction * LATF_freeObjectWithKey(char *key, struct LATFunction *node);
void LATF_freeObjects();

int LATF__printMem(struct LATFunction *node, int mem);

void LATF_setValue(char *name, char *val);
void LATF_setValueForKey(char *key, char *value, struct LATFunction **node);

void LATF_addParam(char *paramName, char *functionName);
void _LATF_addParam(char *paramName, struct LATFParameter **parentParam);

/* struct LATFParameter * LATF_getParam(char *functionName);*/
/* struct LATFParameter * _LATF_getParam(struct LATFParameter *parentParam);*/

void LATF_freeParams(char *functionName);
void _LATF_freeParams(struct LATFParameter *parentParam);

#endif /* defined(__Latria_Function__) */
