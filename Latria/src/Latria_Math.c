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
 *  Latria (Atria Lang)
 */

#include "Latria_Math.h"

unsigned char didDetectStringOperands;

/* Checks to see if the next 'compressable' parenthese block is not a function, and thus can be compressed*/
LATBool currentBlockIsCompressable(char *i) {
    char *iT = i;
    int c=-1,ce=-1,counter=0;
    while(*iT) {
        if(*iT == '(')
            c=counter;
        else if(*iT == ')') {
            ce=counter;
            break;
        }
        iT++;
        counter++;
    }
    
    if(c >= 0 && ce >= 0) {
        char lastChar;
        char *s = LATstrdup(i);
        char *si = s;
        LATBool isCompressable = false;
        s+=c;
        lastChar=*s;
        *s='\0';
        /* if we have something let's show for it */
        if(--c > 1) {
            while(*--s) {
                if(--c < 1)
                    break;
                if(isCharacterOperator(*s)) {
                    s++;
                    break;
                }
            }
            if(LATF_findObject(s) == NULL)
                isCompressable = true;
            LATDealloc(si), si = NULL;
        } else {
            /* nothing to do,free what we allocated and go on */
            *s=lastChar;
            LATDealloc(si), si = NULL;
            return true;
        }
        return isCompressable;
    }
    return false;
}

LATBool isCharacterOperator(char c) {
    return (c=='+' || c=='-' || c=='*' || c=='/' || c=='^' || c=='%');
}

/*
 * centerSpot - location of current operator we are working around (+,-,* etc)
 * leader     - Left value we are working with for length
 * ender      - Right value we are working with for length
 * input      - Input we are replacing this expression in
 * value      - value we are using to replace indicated section
 */
char * replaceSectionMatchingExpressionWithValue(char *centerSpot, char *leader, char *ender, char *input, char *value) {
    
    unsigned long leaderLen=0, enderLen=0, valueLen=0;
    size_t eLen;
    LATBool isValid = true;
    char *llc = leader, *eec = ender, *vvc = value;
    char *tmp, *replacementString;
    while(isValid) {
        isValid = false;
        if(*llc) {
            leaderLen++;
            llc++;
            isValid = true;
        }
        if(*eec) {
            enderLen++;
            eec++;
            isValid = true;
        }
        if(*vvc) {
            valueLen++;
            vvc++;
            isValid = true;
        }
    }
    
    /*  warning this optimization may not be that great...*/
    /* used to count by strlen for EACH ::strlen(leader)+strlen(ender)+strlen(value) */
    replacementString = setCharTablePointerByLEN( 13, (leaderLen+enderLen+valueLen+1) * sizeof(char));
    replacementString[0]='\0';
    tmp = replacementString;
    tmp = LATstrcat(tmp, leader);
    tmp = LATstrcat(tmp, centerSpot);
    
    eLen = strlen(ender);
    strncpy(tmp, ender, eLen);
    tmp[eLen] = '\0';
    
    return str_replace(input, replacementString, value, 0);
}

float performOperationBetweenInputs(float lval, float rval, char opSymbol) {
    switch (opSymbol) {
        case '^':
            return local_pow((double)lval,(int)rval);
        case '%':
            return (int)lval%(int)rval;
        case '*':
            return lval*rval;
        case '/':
            return lval/rval;
        case '+':
            return lval+rval;
        case '-':
            return lval-rval;
        default:
            return 0;
    }
}

/* Simple pow*/
float local_pow(double base, int exp) {
    int result = 1;
    while (exp) {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }
    return result;
}

/* C implementation of substring*/
char * LATsubstring(char *input, int start, int end) {
    unsigned int y=0;
    int iLen = (int)strlen(input), x;
    char *rez;
    
    if(end < 0) {
        end = iLen;
    }
    
    rez = LATAlloc(NULL, 0, (size_t)(end-start+1)*sizeof(char));
    
    /* Fix from valgrind, NOT safe to move above the alloc above */
    if(end > iLen) {
        end = iLen;
    }
    
    for(x = start; x < end; x++) {
        rez[y++] = input[x];
    }
    rez[y]='\0';
    return rez;
}

/* Determines whether or not the plus sign is the only operator in the given input*/
LATBool plusIsOnlyOperatorBetween(char *i) {
    unsigned int cc = 0;
    if(*i == '"') {
        cc++;
        i++;
    }
    while(*i) {
        if(*i == '"' && *(i-1) != '\\')
            cc++;
        else if(cc%2==0) {
            switch (*i) {
                case '-':
                    return false;
                case '*':
                    return false;
                case '/':
                    return false;
                case '^':
                    return false;
                case '%':
                    return false;
            }
        }
        i++;
    }
    return true;
}

LATBool didFailWithStringOperands() {
    return didDetectStringOperands;
}

/* Checks to see if ANY math operators are present, NOT quoted */
LATBool noMathOperatorsPresent(char *input) {
    
    char *s=input;
    unsigned char passedFirst = 0;
    
    /* While loop over our input */
    while(*s) {
        /* If tgt string found */
        if(*s == '+' || *s == '-' || *s == '*' || *s == '/' || *s == '^' || *s == '%') {
            /* Found an unquoted mathematical expression, return true */
            return false;
            
        } else if(*s == '"') {
            /* Double Quote */
            if((passedFirst == 1 && *(s-1) != '\\') || passedFirst == 0) {
                /* Not escaped or first pass */
                while(true) {
                    /* Loop until we escape double quotes */
                    char *yc=strchr(s+1, '"');
                    if(yc) {
                        /* We found double quotes */
                        if(*(yc-1) != '\\') {
                            /* Valid closing Double Quote, set out pointer to it and move along */
                            s = yc;
                            break;
                        }
                    } else {
                        /* No closing single quote found, it is possible this is embedded, ignore it */
                        break;
                    }
                }
            }
            
        } else if(*s == '\'') {
            /* Single Quote */
            if((passedFirst == 1 && *(s-1) != '\\') || passedFirst == 0) {
                /* Not escaped or first pass */
                while(true) {
                    /* Loop until we escape single quotes */
                    char *yc=strchr(s+1, '\'');
                    if(yc) {
                        /* We found single quotes */
                        if(*(yc-1) != '\\') {
                            /* Valid closing Single Quote, set out pointer to it and move along */
                            s = yc;
                            break;
                        }
                    } else {
                        /* No closing single quote found, it is possible this is embedded, ignore it */
                        break;
                    }
                }
            }
        }
        passedFirst = 1;
        s++;
    }
    
    return true;
}

void performAddOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    if((getRegisterType(i1) == RegisterString || getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
       (getRegisterType(i2) == RegisterString || getRegisterType(i2) == RegisterNum) &&
       (getRegisterType(i3) == RegisterString || getRegisterType(i3) == RegisterNum)) {
        
        if((getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
           getRegisterType(i2) == RegisterNum &&
           getRegisterType(i3) == RegisterNum) {
            /* All Num */
            setNumRegister(i1, getRegisterNum(i3) + getRegisterNum(i2), RegisterNum);
            
        } else {
            /* One or more strings, add as string */
            char numString[20];
            char *nsPointer = numString;
            char *s1 = NULL;
            if(getRegisterType(i3) == RegisterString) {
                /* Set string */
                char *tmp = getRegisterString(i3);
                if(*tmp) {
                    /* We have something to copy over */
                    s1 = LATstrdup(tmp);
                }
                
            } else {
                /* Convert number to string and set */
                sprintf(numString, "%g", getRegisterNum(i3));
                s1 = LATstrdup(nsPointer);
                
            }
            
            if(getRegisterType(i2) == RegisterString) {
                /* Append to existing value */
                size_t len1,len2;
                char *tmp = getRegisterString(i2);
                len1 = strlen(tmp);
                if(s1 != NULL) {
                    len2 = strlen(s1);
                } else {
                    len2 = 0;
                }
                
                s1 = LATAlloc(s1, len2*sizeof(char), (len1+len2+1)*sizeof(char));
                if(len2 == 0) {
                    *s1 = 0;
                }
                LATstrcat(s1, tmp);
                
            } else {
                /* Convert number to string and append */
                size_t len1,len2;
                sprintf(numString, "%g", getRegisterNum(i2));
                
                len1 = strlen(nsPointer);
                len2 = strlen(s1);
                
                s1 = LATAlloc(s1, len2*sizeof(char), (len1+len2+1)*sizeof(char));
                if(len2 == 0) {
                    *s1 = 0;
                }
                LATstrcat(s1, nsPointer);
                
            }
            
            /* Assign result to the first register */
            setStringRegister(i1, s1, RegisterString);
            
            /* free s1 */
            LATDealloc(s1);
        }
        
    } else {
        /* One is invalid */
        printf("One of the provided registers to 'add' is of an invalid type\n");
        exit(2452);
        
    }
}

void performSubOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    if((getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
       getRegisterType(i2) == RegisterNum &&
       getRegisterType(i3) == RegisterNum) {
        /* All Num */
        setNumRegister(i1, getRegisterNum(i3) - getRegisterNum(i2), RegisterNum);
        
    } else {
        printf("One of the provided registers to 'subtract' is of an invalid type\n");
        exit(2452);
    }
}

void performMultiOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    if((getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
       getRegisterType(i2) == RegisterNum &&
       getRegisterType(i3) == RegisterNum) {
        /* All Num */
        setNumRegister(i1, getRegisterNum(i3) * getRegisterNum(i2), RegisterNum);
        
    } else {
        printf("One of the provided registers to 'multiply' is of an invalid type\n");
        exit(2452);
    }
}

void performDiviOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    if((getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
       getRegisterType(i2) == RegisterNum &&
       getRegisterType(i3) == RegisterNum) {
        /* All Num */
        setNumRegister(i1, getRegisterNum(i3) / getRegisterNum(i2), RegisterNum);
        
    } else {
        printf("One of the provided registers to 'divide' is of an invalid type\n");
        exit(2452);
    }
}

void performModOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    if((getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
       getRegisterType(i2) == RegisterNum &&
       getRegisterType(i3) == RegisterNum) {
        /* All Num */
        setNumRegister(i1, (int)getRegisterNum(i3) % (int)getRegisterNum(i2), RegisterNum);
        
    } else {
        printf("One of the provided registers to 'modulus' is of an invalid type\n");
        exit(2452);
    }
}

void performExpOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    if((getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
       getRegisterType(i2) == RegisterNum &&
       getRegisterType(i3) == RegisterNum) {
        /* All Num */
        setNumRegister(i1, local_pow(getRegisterNum(i3), (int)getRegisterNum(i2)), RegisterNum);
        
    } else {
        printf("One of the provided registers to 'exponent' is of an invalid type\n");
        exit(2452);
    }
}

