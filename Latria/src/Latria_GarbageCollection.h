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
*  Latria_GarbageCollection.h
*  Latria
*
*  Created by Benjamin Friedman on 3/1/15.
*/

#ifndef __Latria_GarbageCollection__
#define __Latria_GarbageCollection__

#include "Latria_ControlFlow.h"

void forceMemoryFree();
void forceALLMemoryFree();
void adjustGCRate(float rate);
void deconstructLATVM();
void createLatriaVM();
char *setCharTablePointerByLEN(int index, unsigned long len);

void setBlockCommentState(unsigned char ns);
unsigned char getBlockCommentState();

void setSysResult(char *rez);
char *getSysResult();

void printMem();

void setPrintCacheMode(unsigned char c);
unsigned char getPrintCacheMode();

/* Forwardly declared form Latria_Function.h */
void LATF_printMem();

/*** START STACK REGISTERS (ARGS) ****/
void pushRegister(unsigned char registerNum);

void __pushStackRegister();
RegisterType getStackRegisterType();

double popStackRegisterNum();
char *popStackRegisterString();
struct CoreObject *popStackRegisterArray();
FILE *popStackRegisterFile();
void popStackRegisterNull();
int popStackRegisterConnection();

/*** END STACK REGISTERS (ARGS) ****/

/* Frees any strings on registers of string type */
void __freeAllRegisterValues();

/* Forwardly declared from Latria_Vars.h */
struct CoreObject *copyArrayCoreObject(struct CoreObject *co);



/* Forwardly declared from Latria_Var.h */
void Core_printMem();
int Core__printMem(struct CoreObject *node, int mem);

#endif /* defined(__Latria__Latria_GarbageCollection__) */
