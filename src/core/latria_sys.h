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
 *  latria_sys.h
 *  Created by Benjamin Friedman on 1/22/15.
 */

#ifndef latria_core_latria_sys_h
#define latria_core_latria_sys_h

#include "latria_core.h"
#include "latria_operators.h"
#include "latria_math.h"
#include "latria_vars.h"

/* System Status Enum */
typedef enum {
    FAILED,
    SUCCEEDED,
    HAS_RESULT,
    ASSIGNMENT_SAFE
} Latria_Sys_Status;

void Flush_Batched_Write();

#ifdef INCLUDECOMPILER
char * Read_Batched_Write();
#endif

/* Declare only if tests are included */
#ifdef LAT_TESTS
void Flush_Batched_Write_NO_PRINT();
#endif

/* Gets the current system status */
Latria_Sys_Status getSysStatus();

/* Sets the current system status */
void setSystemStatus(Latria_Sys_Status status);

/* System Functions */
void Sys_Print(char *input);
void Sys_Purge();
void Sys_GC(float input);
void Sys_PrintNum(double num);
void Sys_Platform();
unsigned long Sys_Time();

/* System Functions */
void Sys_Find(char *arg1, char *arg2);
void Sys_PrintMem();
void Sys_Replace(char *arg1, char *arg2, char *arg3, char arg4);
void Sys_Substr(char *arg1, int arg2, int arg3);

void Sys_Match(char *matchString, char *regexString);

/* Random */
int  Sys_Random(int input);
void Sys_RandomSeed(unsigned int input);

/* Reads user input input for a line */
void Sys_Input();

/* file io */
FILE *Sys_Open(char *fileName, char *mode);
void Sys_Read(FILE *file);
void Sys_Close(FILE *file);
void Sys_Remove(char *fileName);

void Sys_Sleep(unsigned int sleepVal);

/* Bitwise ops */
int Sys_bit_not(int i1);
int Sys_bit_and(int i1, int i2);
int Sys_bit_or(int i1, int i2);
int Sys_bit_xor(int i1, int i2);

double Sys_SquareRoot(double i);

char didReadSafeNull();
void setSafeNull(char c);

#endif /* latria_core_latria_sys_h */
