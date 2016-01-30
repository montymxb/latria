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
 *  Latria_C_Dup_Libs.c
 *  Latria
 *
 *  Created by Benjamin Friedman on 11/27/15.
 */

#include "Latria_C_Dup_Libs.h"

#ifndef INCLUDECOMPILER

/* Used to ignore sections of code that are encapsulated in block comments */
unsigned char blockCommentState = 0;


/* Strips comments from the input if they are present */
void stripComments(char *input) {
    
    if (strstr(input, "//") != 0 || getBlockCommentState() || strstr(input, "/*")) {
        
        char *origI = input;
        LATBool isInSQuotes = false, isInDQuotes = false;
        LATBool blockedOutThisLine = false;
        
        while(*input) {
            
            switch(*input) {
                    
                case '\'':
                    
                    if(!isInDQuotes)
                        isInSQuotes = !isInSQuotes;
                    break;
                    
                    
                case '"':
                    
                    if(!isInSQuotes)
                        isInDQuotes = !isInDQuotes;
                    break;
                    
                    
                case '/':
                    
                    if(!isInSQuotes && !isInDQuotes) {
                        
                        if(*(input+1) == '/') {
                            
                            /* Line  comment */
                            *input = '\0';
                            
                        } else if(*(input+1) == '*') {
                            
                            /* Block comment */
                            blockedOutThisLine = true;
                            *input = '\0';
                            setBlockCommentState(1);
                        }
                    }
                    break;
                    
                    
                case '*':
                    
                    if(!isInSQuotes && !isInDQuotes) {
                        
                        if(getBlockCommentState()) {
                            
                            if(*(input+1) == '/') {
                                
                                setBlockCommentState(0);
                                input+=2;
                                
                                if(*input) {
                                    
                                    if(blockedOutThisLine) {
                                        
                                        char *oit = origI+strlen(origI);
                                        
                                        while(*input) {
                                            
                                            *oit++ = *input;
                                            input++;
                                        }
                                        
                                        /* cap the end */
                                        *oit='\0';
                                        
                                    } else {
                                        
                                        char *oit = origI;
                                        
                                        while(*input) {
                                            
                                            *oit++ = *input;
                                            input++;
                                        }
                                        
                                        /* cap the end */
                                        *oit='\0';
                                    }
                                    
                                    /* reset to the beginning, we may have more we need to go over */
                                    input = origI;
                                    stripComments(input);
                                    return;
                                    
                                } else {
                                    
                                    *origI = '\0';
                                }
                            }
                        }
                    }
            }
            
            input++;
        }
        
        
        /* if we did NOT block out in this line, and we ARE blocked out, this line is dead to us, mark it as so */
        if(!blockedOutThisLine && getBlockCommentState()) {
            
            *origI='\0';
        }
    }
}


/* Returns whether a character is an operator or not */
LATBool isCharacterOperator(char c) {
    
    return (c=='+' || c=='-' || c=='*' || c=='/' || c=='^' || c=='%');
}


/* Strips whitespace from the input (avoiding that which is in quotes) */
char * stripWhitespace(char *input) {
    
    char *i = input, *j = input;
    
    if(*j == '"') {
        
        /* Fast forward to NEXT double quote, if applicable */
        char *nDQ = strchr(j+1, '"');
        
        if(nDQ != NULL) {
            
            /* Validate */
            if(*(nDQ-1) != '\\') {
                
                /* Fast forward */
                int z = 0;
                int inLen = (int)(nDQ-j)+1;
                
                for(;z<inLen;z++) {
                    
                    *i++ = *j++;
                }
            }
        }
    } else if(*j == '\'') {
        
        /* Fast forward to NEXT single quote, if applicable */
        char *nDQ = strchr(j+1, '\'');
        
        if(nDQ != NULL) {
            
            /* Validate */
            if(*(nDQ-1) != '\\') {
                
                /* Fast forward */
                int z = 0;
                int inLen = (int)(nDQ-j)+1;
                
                for(;z<inLen;z++) {
                    
                    *i++ = *j++;
                }
            }
        }
    }
    
    /* Loop over 'j' */
    while(*j) {
        
        if(*j == '"') {
            
            /* Fast forward to NEXT double quote, if applicable */
            char *nDQ = strchr(j+1, '"');
            
            if(nDQ != NULL) {
                
                /* Validate */
                if(*(nDQ-1) != '\\') {
                    
                    /* Fast forward */
                    int z = 0;
                    int inLen = (int)(nDQ-j);
                    
                    for(;z<inLen;z++) {
                        
                        *i++ = *j++;
                    }
                }
            }
        } else if(*j == '\'') {
            
            /* Fast forward to NEXT single quote, if applicable */
            char *nDQ = strchr(j+1, '\'');
            
            if(nDQ != NULL) {
                
                /* Validate */
                if(*(nDQ-1) != '\\') {
                    
                    /* Fast forward */
                    int z = 0;
                    int inLen = (int)(nDQ-j);
                    
                    for(;z<inLen;z++) {
                        
                        *i++ = *j++;
                    }
                }
            }
        }
        
        *i = *j++;
        
        if(!isspace(*i)) {
            
            /* This character is NOT a space, so don't move along as expected, otherwise this will get overriden in the next pass */
            i++;
        }
        
    }
    
    /* Cap the end, as it will just be shorter if whitespace was present */
    *i = '\0';
    return input;
}


/* Checks to see if a string is compatible for math operations*/
LATBool isNumeric (char * s) {
    
    if(s == NULL || !*s) {
        
        /* null or empty */
        return false;
        
    }
    
    if(*s == '-' || *s == '+') {
        
        /* advance 1 if negative/positive start */
        s++;
        
    }
    
    while(*s) {
        
        if(!isdigit(*s) && *s != '.' && !isspace(*s)) {
            
            /* space or non-digit */
            return false;
            
        }
        
        /* bump */
        s++;
    }
    
    return true;
}


/* sets the current block comment state for this VM */
void setBlockCommentState(unsigned char ns) {
    
    blockCommentState = ns;
}

/* Returns the current block comment state for this VM */
unsigned char getBlockCommentState() {
    
    return blockCommentState;
}
#endif
