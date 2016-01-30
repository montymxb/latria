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
 * Latria_Operators.c
 * Latria
 *
 * Created by Benjamin Friedman on 3/1/15.
 */

#include "Latria_Operators.h"


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
