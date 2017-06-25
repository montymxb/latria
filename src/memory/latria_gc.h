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
 *  latria_gc.h
 *  Created by Benjamin Friedman on 3/1/15.
 */

#ifndef latria_memory_latria_gc_h
#define latria_memory_latria_gc_h

#include "core/latria_core.h"
#include "core/latria_vars.h"

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

RegisterType getRegisterType(unsigned char registerNum);

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

void *LATAlloc(void *ptr, size_t so, size_t sn);
void LATDealloc(void *ptr);
void *lmalloc(size_t sn);
void *lrealloc(void *ptr, size_t sn);
void lfree(void *ptr);

/*** END STACK REGISTERS (ARGS) ****/

/* Frees any strings on registers of string type */
void __freeAllRegisterValues();

#endif /* latria_memory_latria_gc_h */
