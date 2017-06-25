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
 *  latria_disassembler.h
 *  Created by Benjamin Friedman on 11/4/15.
 */

#ifndef latria_disassembler_latria_disassembler_h
#define latria_disassembler_latria_disassembler_h

#include "core/latria_core.h"
#include "core/latria_opcodes.h"

/*
 *  References to all of latria's op codes
 */

void decompileLatria(char *fn);

/* Functions for reading and writing from the file as we read */
int getCurrentChar();
int getCharByOffsetFromCurrent(int offset);
void moveToNextChar();


#endif /* latria_disassembler_latria_disassembler_h */
