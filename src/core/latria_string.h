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
 *  latria_string.h
 *
 *  Contains various string related utilities
 *
 */

#ifndef latria_core_latria_string_h
#define latria_core_latria_string_h

#include "latria_core.h"
#include "memory/latria_gc.h"

/* string comparison macro */
#define LAT_STRCMP(a, b)  (*(a) != *(b) ? \
(int) ((unsigned char) *(a) - \
(unsigned char) *(b)) : \
strcmp((a), (b)))

/* Strips comments from the pointed characters */
void stripComments(char *input);

/* string duplication */
char *LATstrdup(char *s);

/* string replace */
char *str_replace(char *orig, char *rep, char *with, char removeAll);

/* strips whitespace from the given input */
char * stripWhitespace(char *input);

/* strips line endings from the given input */
void stripLineEndings(char *input);

/* Indicates whether a given sequence is empty or not (contains whitespace chars only or nothing) */
LATBool isSequenceEmpty(char *input);

/* Indicates whether these characters can be converted into a valid number */
LATBool isNumeric(char *val);

/* Indicates whether this string is a recognized conditional operator */
char isConditionalOperator(char *i);

/* Substring implementation */
char * LATsubstring(char *input, int start, int end);

#endif /* latria_core_latria_string_h */
