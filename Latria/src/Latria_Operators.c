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
  Latria_Operators.c
  Latria

  Created by Benjamin Friedman on 3/1/15.
*/

#include "Latria_Operators.h"

/* Returns whether or not relational operators are present in the input*/
LATBool hasRelationalOperators(char *i) {
    /* No math operators are allowed */
    char *tmpI;
    
    if(*i=='"') {
        /* Fast forward to NEXT double quote */
        i=strchr(i+1,'"');
        while(*i-1 == '\\') {
            i=strchr(i+1, '"');
        }
        i++;
    } else if(*i == '\'') {
        /* Fast forward to NEXT single quote */
        i=strchr(i+1,'\'');
        while(*i-1 == '\\') {
            i=strchr(i+1, '\'');
        }
        i++;
    }
    
    while(*i) {
        switch(*i) {
            case '<':
                return true; /* < */
            case '>':
                return true; /* > */
            case '!':
                if(*(i+1) == '=')
                    return true; /* != */
                else if(*(i+1)) {
                    if(*(i+2) == '=')
                        return true; /* !== */
                }
                break;
            case '=':
                if(*(i+1) == '=')
                    return true; /* == */
                else if(*(i+1)) {
                    if(*(i+2) == '=')
                        return true; /* === */
                }
                break;
            case '&':
                if(*(i+1) == '&') /* it must be two ampersands*/
                    return true; /* && */
                break;
            case '?':
                return true; /* OR, because why pipes? why not either?or */
            
            case '"':
                /* Fast forward */
                tmpI=strchr(i+1,'"');
                if(tmpI != NULL) {
                    i = tmpI;
                    while(*i-1 == '\\') {
                        i=strchr(i+1, '"');
                    }
                }
                break;
                
            case '\'':
                /* Fast forward */
                tmpI=strchr(i+1,'\'');
                if(tmpI != NULL) {
                    i = tmpI;
                    while(*i-1 == '\\') {
                        i=strchr(i+1, '\'');
                    }
                }
                break;
        }
        i++;
    }
    return false;
}

/* Utilized to convert numeric values to strings */
char lvalWrapper[100];

void performEqualOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    if(getRegisterType(i1) != getRegisterType(i2) || getRegisterType(i1) != getRegisterType(i3)) {
        
        if(getRegisterType(i2) == RegisterBool) {
            /* True/False check the third arg */
            if(getRegisterType(i3) == RegisterString) {
                /* String true/false */
                setNumRegister(i1, (getRegisterString(i3) == NULL || !*getRegisterString(i3)) ? 0 : 1, RegisterBool);
                
            } else if(getRegisterType(i3) == RegisterNum) {
                /* Num true/false */
                setNumRegister(i1, getRegisterNum(i3) == getRegisterNum(i2) ? 1 : 0, RegisterBool);
                
            } else {
                /* Register types did NOT match */
                printf("Type provided to true/false expression is not valid\n");
                exit(2452);
                
            }
            
        } else if(getRegisterType(i3) == RegisterBool) {
            /* True/False check the second arg */
            if(getRegisterType(i2) == RegisterString) {
                /* String true/false */
                setNumRegister(i1, (getRegisterString(i2) == NULL || !*getRegisterString(i2)) ? 0 : 1, RegisterBool);
                
            } else if(getRegisterType(i2) == RegisterNum) {
                /* Num true/false */
                setNumRegister(i1, getRegisterNum(i3) == getRegisterNum(i2) ? 1 : 0, RegisterBool);
                
            } else {
                /* Register types did NOT match */
                printf("Type provided to true/false expression is not valid\n");
                exit(2452);
                
            }
            
        } else {
            /* Register types did NOT match */
            printf("Types provided to == are not matching!\n");
            exit(2452);
        }
        
    } else if(getRegisterType(i2) == RegisterNum) {
        /* Numeric */
        setNumRegister(i1, getRegisterNum(i3) == getRegisterNum(i2) ? 1 : 0, RegisterBool);
        
    } else if(getRegisterType(i2) == RegisterString) {
        /* String compare */
        setNumRegister(i1, strcmp(getRegisterString(i3), getRegisterString(i2)) == 0 ? 1 : 0, RegisterBool);
        
    } else if(getRegisterType(i3) == RegisterNull || getRegisterType(i2) == RegisterNull) {
        if(getRegisterType(i2) == RegisterNull && getRegisterType(i3) == RegisterNull) {
            /* null match */
            setNumRegister(i1, 1, RegisterBool);
        } else {
            /* null mismatch */
            setNumRegister(i1, 0, RegisterBool);
        }
        
    } else {
        /* Unrecognized register type */
        printf("Unrecognized register type provided to == operator\n");
        exit(1944);
        
    }
    
}

void performNotEqualOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    if(getRegisterType(i1) != getRegisterType(i2) || getRegisterType(i1) != getRegisterType(i3)) {
        
        if(getRegisterType(i2) == RegisterNull && getRegisterType(i3) == RegisterNull) {
            /* null match */
            setNumRegister(i1, 0, RegisterBool);
            
        } else if(getRegisterType(i2) == RegisterNull || getRegisterType(i3) == RegisterNull) {
            /* null mismatch */
            setNumRegister(i1, 1, RegisterBool);
            
        } else {
            /* Register types did NOT match */
            printf("Types provided to != are not matching!\n");
            exit(2452);
            
        }
        
    } else if(getRegisterType(i2) == RegisterNum) {
        /* Numeric */
        setNumRegister(i1, getRegisterNum(i3) != getRegisterNum(i2) ? 1 : 0, RegisterBool);
        
    } else if(getRegisterType(i2) == RegisterString) {
        /* String compare */
        setNumRegister(i1, strcmp(getRegisterString(i3), getRegisterString(i2)) != 0 ? 1 : 0, RegisterBool);
        
    } else if(getRegisterType(i3) == RegisterNull && getRegisterType(i2) == RegisterNull) {
        /* null match */
        setNumRegister(i1, 0, RegisterBool);
        
    } else {
        /* Unrecognized register type */
        printf("Unrecognized register type provided to != operator\n");
        exit(1944);
        
    }
}

void performLessThanOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    if(getRegisterType(i1) != getRegisterType(i2) || getRegisterType(i1) != getRegisterType(i3)) {
        /* Register types did NOT match */
        printf("Types provided to < are not matching!\n");
        exit(2452);
        
    } else if(getRegisterType(i2) == RegisterNum) {
        /* Numeric */
        setNumRegister(i1, getRegisterNum(i3) < getRegisterNum(i2) ? 1 : 0, RegisterBool);
        
    } else if(getRegisterType(i2) == RegisterString) {
        /* String compare */
        setNumRegister(i1, (strlen(getRegisterString(i3)) < strlen(getRegisterString(i2))) == 0 ? 1 : 0, RegisterBool);
        
    } else {
        /* Unrecognized register type */
        printf("Unrecognized register type provided to < operator\n");
        exit(1944);
        
    }
    
}

void performLessThanEqualToOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    if(getRegisterType(i1) != getRegisterType(i2) || getRegisterType(i1) != getRegisterType(i3)) {
        /* Register types did NOT match */
        printf("Types provided to < are not matching!\n");
        exit(2452);
        
    } else if(getRegisterType(i2) == RegisterNum) {
        /* Numeric */
        setNumRegister(i1, getRegisterNum(i3) <= getRegisterNum(i2) ? 1 : 0, RegisterBool);
        
    } else if(getRegisterType(i2) == RegisterString) {
        /* String compare */
        setNumRegister(i1, (strlen(getRegisterString(i3)) <= strlen(getRegisterString(i2))) == 0 ? 1 : 0, RegisterBool);
        
    } else {
        /* Unrecognized register type */
        printf("Unrecognized register type provided to < operator\n");
        exit(1944);
        
    }
    
}

void performGreaterThanOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    if(getRegisterType(i1) != getRegisterType(i2) || getRegisterType(i1) != getRegisterType(i3)) {
        /* Register types did NOT match */
        printf("Types provided to < are not matching!\n");
        exit(2452);
        
    } else if(getRegisterType(i2) == RegisterNum) {
        /* Numeric */
        setNumRegister(i1, getRegisterNum(i3) > getRegisterNum(i2) ? 1 : 0, RegisterBool);
        
    } else if(getRegisterType(i2) == RegisterString) {
        /* String compare */
        setNumRegister(i1, (strlen(getRegisterString(i3)) > strlen(getRegisterString(i2))) == 0 ? 1 : 0, RegisterBool);
        
    } else {
        /* Unrecognized register type */
        printf("Unrecognized register type provided to < operator\n");
        exit(1944);
        
    }
    
}

void performGreaterThanEqualToOp(unsigned char i1, unsigned char i2, unsigned char i3) {
    if(getRegisterType(i1) != getRegisterType(i2) || getRegisterType(i1) != getRegisterType(i3)) {
        /* Register types did NOT match */
        printf("Types provided to < are not matching!\n");
        exit(2452);
        
    } else if(getRegisterType(i2) == RegisterNum) {
        /* Numeric */
        setNumRegister(i1, getRegisterNum(i3) >= getRegisterNum(i2) ? 1 : 0, RegisterBool);
        
    } else if(getRegisterType(i2) == RegisterString) {
        /* String compare */
        setNumRegister(i1, (strlen(getRegisterString(i3)) >= strlen(getRegisterString(i2))) == 0 ? 1 : 0, RegisterBool);
        
    } else {
        /* Unrecognized register type */
        printf("Unrecognized register type provided to < operator\n");
        exit(1944);
        
    }
    
}


/* Returns whether or not Logical operators are present in the input*/
/*
LATBool hasLogicalOperators(char *i) {
    return false;
}
 */

/* Returns the result of one of more logical operators in an expression*/
/*
LATBool getLogicalOperatorResult(char *i) {
    return false;
}
 */

/* Returns the result of combined logical and relational operators in an expression (either or/both if both are present)*/
/*
LATBool getCollectiveOperatorResult(char *i) {
    return false;
}
*/
