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
//  Latria_C_Libs.h
//  Latria
//
//  Created by Benjamin Friedman on 11/3/15.
*/

#ifndef Latria_C_Libs_h
#define Latria_C_Libs_h

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#define LAT_ADDRESS_SIZE 4
#define LAT_ADDRESS_FORMAT_STRING "%04x"

#define JUMP_UPDATE_INCREMENT 5

#if defined(INCLUDECOMPILER)
#include "Latria_Libs.h"
#else
#include "Latria_C_Dup_Libs.h"
#endif

LATBool isSequenceEmpty(char *input);
char isWhitespace(char c);
void stripLineEndings(char *input);

/* Expression Checking Expression */
char isArithmeticOperator(char i);
char isConditionalOperator(char *i);

#endif /* Latria_C_Libs_h */
