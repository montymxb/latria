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
 *  latria_math.c
 */

#include "latria_math.h"


/* Simple pow*/
float local_pow(double base, int exponent) {
    
    double result = 1;
    
    while (exponent) {
        
        if (exponent & 1)
            result *= base;
        
        exponent >>= 1;
        base *= base;
    }
    
    return (float)result;
}


/* Substring Implementation*/
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


/* Performs an add operation with registers into another register */
void performAddOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    
    if((getRegisterType(i1) == RegisterString || getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
       (getRegisterType(i2) == RegisterString || getRegisterType(i2) == RegisterNum) &&
       (getRegisterType(i3) == RegisterString || getRegisterType(i3) == RegisterNum))
    {
        
        if((getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
           getRegisterType(i2) == RegisterNum &&
           getRegisterType(i3) == RegisterNum)
        {
            
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
                
                s1 = LATAlloc(s1, len2*sizeof(char), (len1+len2+2)*sizeof(char));
                
                if(len2 == 0) {
                    
                    *s1 = 0;
                }
                strcat(s1, tmp);
                
            } else {
                
                /* Convert number to string and append */
                size_t len1,len2;
                sprintf(numString, "%g", getRegisterNum(i2));
                
                len1 = strlen(nsPointer);
                
                if(s1 != NULL) {
                    
                    len2 = strlen(s1);
                } else {
                    
                    len2 = 0;
                }
                
                s1 = LATAlloc(s1, len2*sizeof(char), (len1+len2+1)*sizeof(char));
                
                if(len2 == 0) {
                    
                    *s1 = 0;
                }
                
                strcat(s1, nsPointer);
                
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


/* Performs a subtraction operation */
void performSubOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    
    if((getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
       getRegisterType(i2) == RegisterNum &&
       getRegisterType(i3) == RegisterNum)
    {
        
        /* All Num */
        setNumRegister(i1, getRegisterNum(i3) - getRegisterNum(i2), RegisterNum);
        
    } else {
        
        printf("One of the provided registers to 'subtract' is of an invalid type\n");
        exit(2452);
    }
}


/* Performs a multiplicative operation */
void performMultiOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    
    if((getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
       getRegisterType(i2) == RegisterNum &&
       getRegisterType(i3) == RegisterNum)
    {
        
        /* All Num */
        setNumRegister(i1, getRegisterNum(i3) * getRegisterNum(i2), RegisterNum);
        
    } else {
        
        printf("One of the provided registers to 'multiply' is of an invalid type\n");
        exit(2452);
    }
}


/* Performs a division operation */
void performDiviOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    
    if((getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
       getRegisterType(i2) == RegisterNum &&
       getRegisterType(i3) == RegisterNum)
    {
        /* All Num */
        
        setNumRegister(i1, getRegisterNum(i3) / getRegisterNum(i2), RegisterNum);
        
    } else {
        
        printf("One of the provided registers to 'divide' is of an invalid type\n");
        exit(2452);
    }
}


/* Performs a modolu operation */
void performModOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    
    if((getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
       getRegisterType(i2) == RegisterNum &&
       getRegisterType(i3) == RegisterNum)
    {
        
        /* All Num */
        setNumRegister(i1, (int)getRegisterNum(i3) % (int)getRegisterNum(i2), RegisterNum);
        
    } else {
        
        printf("One of the provided registers to 'modulus' is of an invalid type\n");
        exit(2452);
    }
}


/* Perform an exponentation operation */
void performExpOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    
    if((getRegisterType(i1) == RegisterNum || getRegisterType(i1) == RegisterNone) &&
       getRegisterType(i2) == RegisterNum &&
       getRegisterType(i3) == RegisterNum)
    {
        
        /* All Num */
        setNumRegister(i1, local_pow(getRegisterNum(i3), (int)getRegisterNum(i2)), RegisterNum);
        
    } else {
        
        printf("One of the provided registers to 'exponent' is of an invalid type\n");
        exit(2452);
    }
}
