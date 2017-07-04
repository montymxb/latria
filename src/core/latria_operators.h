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
 *  latria_operators.h
 *  Created by Benjamin Friedman on 3/1/15.
 */

#ifndef latria_core_latria_operators_h
#define latria_core_latria_operators_h

#include "latria_core.h"
#include "latria_opcodes.h"
#include "latria_sys.h"

char *setCharTablePointer(int index, char *ptr);

struct CoreObject * getValue(char *name);
struct CoreObject * getArrayValue(char *varName, char *key);

char * convertFloatToString(double value);

/*** RELATIONAL OPS ***/

void performEqualOp(unsigned char i1, unsigned char i2, unsigned char i3);
void performNotEqualOp(unsigned char i1, unsigned char i2, unsigned char i3);
void performLessThanOp(unsigned char i1, unsigned char i2, unsigned char i3);
void performLessThanEqualToOp(unsigned char i1, unsigned char i2, unsigned char i3);
void performGreaterThanOp(unsigned char i1, unsigned char i2, unsigned char i3);
void performGreaterThanEqualToOp(unsigned char i1, unsigned char i2, unsigned char i3);

/*** END RELATIONAL OPS ***/

/*** START REGISTERS ***/

/* Settting Registers */
void setStringRegister(unsigned char registerNum, char *string, RegisterType type);
void setNumRegister(unsigned char registerNum, double num, RegisterType type);
void setFileRegister(unsigned char registerNum, FILE *file, RegisterType type);
void setArrayRegister(unsigned char registerNum, struct CoreObject *array, RegisterType type);
void setNullRegister(unsigned char registerNum);
void setConnectionRegister(unsigned char registerNum, int connId);

/* Get register Value */
double getRegisterNum(unsigned char registerNum);
char *getRegisterString(unsigned char registerNum);
FILE *getRegisterFile(unsigned char registerNum);
struct CoreObject *getRegisterArray(unsigned char registerNum);
int getRegisterConnection(unsigned char registerNum);

/*** END REGISTERS ***/

#endif /* latria_core_latria_operators_h */
