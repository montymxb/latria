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
 *  latria_char.c
 *
 *  Various character utilities
 *
 */

#include "latria_char.h"

/* Returns whether a character is an operator or not */
LATBool isCharacterOperator(char c) {
    return (c=='+' || c=='-' || c=='*' || c=='/' || c=='^' || c=='%');
}


/* Returns whether this char is an arithmetic op */
char isArithmeticOperator(char i) {
    return (i == '+' || i == '-' || i == '/' || i == '(' || i == ')' || i == '*' || i == '^' || i == '%') ? 1 : 0;
}

/* Returns if what we read is whitespace */
char isWhitespace(char c) {
    
    return (c == ' ' || c == '\t') ? 1 : 0;
}
