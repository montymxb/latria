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
 *  latria_regex.h
 *  Created by Benjamin Friedman on 10/27/15.
 */

#ifndef latria_regex_latria_regex_h
#define latria_regex_latria_regex_h

#include "core/latria_core.h"
#include "memory/latria_gc.h"

/* #define LAT_REGEX_DEBUG 1 */

/* General regex runner */
unsigned char regex(char *input, char *regexString);

char *getNextUnescapedClosingParen(char *input);

/* Returns a pointer to a char array holding the capture at given index */
char *getCaptureByIndex(unsigned char captureIndex);

/* Returns the # of captures we made since the last run */
int getCaptureCount();

/* Frees the internal regex cache */
void freeRegexCache();

#endif /* latria_regex_latria_regex_h */
