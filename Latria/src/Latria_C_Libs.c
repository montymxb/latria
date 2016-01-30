/*
The MIT License (MIT)

Copyright (c) 2016 Benjamin Wilson Friedman

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
 * Latria_C_Libs.c
 *  Latria
 *
 *  Created by Benjamin Friedman on 11/3/15.
 */

#include "Latria_C_Libs.h"

/* Returns whether a sequence of chars is empty or not */
LATBool isSequenceEmpty(char *input) {
    
    char *tmp = stripWhitespace(input);
    
    while(*tmp) {
        
        if(*tmp != '\n' && *tmp != '\r' && *tmp != '\t') {
            
            return false;
        }
        tmp++;
    }
    return true;
}


/* Strips ALL \n's and \r's away from the end of our input */
void stripLineEndings(char *input) {
    
    size_t len = strlen(input);
    input+=len-1;
    
    while(*input == '\n' || *input == '\r') {
        
        *input-- = '\0';
    }
    
}


/* Returns whether this char is an arithmetic op */
char isArithmeticOperator(char i) {
    
    return (i == '+' || i == '-' || i == '/' || i == '(' || i == ')' || i == '*' || i == '^' || i == '%') ? 1 : 0;
}

/* Returns whether t
 he passed char * is a valid conditional op */
char isConditionalOperator(char *i) {
    
    if(!*i) {
        
        return 0;
    }
    
    if(*i == '=') {
        
        if(*(i+1) && *(i+1) == '=') {
            
            /* == */
            return 1;
            
        }
        
    } else if(*i == '<') {
        
        /* <? */
        return 1;
        
    } else if(*i == '>') {
        
        /* >? */
        return 1;
        
    } else if(*i == '!' && *(i+1) && *(i+1) == '=') {
        
        /* != */
        return 1;
        
    } else if(*i == '&' && *(i+1) && *(i+1) == '&') {
        
        /* && */
        return 1;
        
    } else if(*i == '|' && *(i+1) && *(i+1) == '|') {
        
        /* || */
        return 1;
        
    }
    
    return 0;
}


/* Returns if what we read is whitespace */
char isWhitespace(char c) {
    
    return (c == ' ' || c == '\t') ? 1 : 0;
}
