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
 *  Latria (Atria Lang)
 */

#include "Latria_Lexical.h"


/**** XLANG COMPONENTS ****/

#define INJECT_S    "IN{"
/* length of start sequence */
#define ISL         3
#define INJECT_E    "}"
/* length of closing sequence*/
#define IEL         1


#define OPENING_MARK    "##"
/* length of opening mark */
#define OML             2
#define CLOSING_MARK    "##>"
/* length of closing mark */
#define CML             3


/* Maximum Length of Characters we can store from a pipe */
#define STARTING_MAXIMUM_STORAGE_LENGTH 100


 #pragma message("BACKBURNER For Tail call recursion we need to meet a few conditions. 1. We must be in a function (i.e we have a trace we can jump 'back' to that is not null). 2. We must have just read a 'call' and it's following name. 3. If the 2 following characters we are reading are a POP_STACK and a FUNC_END command. If these criteria are met we should pop our current stack and make the jump to our function. Do not push up another returning mark to get back from our function, the existing one will work just fine")


/* TODO most of this should be moved inside the Latria VM, otherwise it may get meddled up when we have multiple instances of Latria running, another note we should check for this kinda stuff EVERYWHERE, just to be sure that it's all nicely packed in the VM*/


/* Instruction Counter for debugging purposes */
int instructionCounter = 0;

/* Holder for hex values being passed to and from */
unsigned char hexHolder[2];
unsigned char *hexPointer;

int address = -1;


#define ISTATE_NONE         0x00
#define ISTATE_XLANG        0x01
#define ISTATE_FUNC         0x02
#define ISTATE_CONTINUE     0x03
#define ISTATE_BREAK        0x04


/* 20 interpreter states to start */
unsigned int interpreterState[20] = {ISTATE_NONE};
int interpreterIndex = 0;


/* Function counter, keeps track of functions within functions we are skipping through */
int funcCounter = 0;


/* Runs a given instruction set of a given size until complete. This is the vm core loop */
void runInstructions() {
    
    int hc;
    
    while((hc = getCurrentChar()) != EOF) {
        
        unsigned char gotoType;
        char xlangBacker[1024];
        
        char *xlangPointer = xlangBacker;
        char vbacker[128],kbacker[128];
        unsigned short xlangMarker = 0;
        FILE *tmpFile;
        char *variable=vbacker,*key=kbacker;
        double argNum1,argNum2;
        char *arg3;
        RegisterType regType;
        unsigned char d0,d1;
        unsigned int registerNum;
        int incrementCounter;
        long int priorFileIndex;
        char offset[5] = {0};
        unsigned char h1,h2,h3;
        
        instructionCounter++;
        
        if(interpreterState[interpreterIndex] == ISTATE_XLANG) {
            
            /* Continue to exec and reset the last char read back into the stream */
            hc = OP_EXEC;
            getCharByOffsetFromCurrent(-2);
            
        } else if(interpreterState[interpreterIndex] == ISTATE_FUNC) {
            
            /* Continue to read to end of function definition */
            hc = OP_FUNC;
            getCharByOffsetFromCurrent(-2);
            
        } else if(interpreterState[interpreterIndex] == ISTATE_CONTINUE) {
            
            /* Continue */
            hc = OP_CONTINUE;
            getCharByOffsetFromCurrent(-2);
            
        } else if(interpreterState[interpreterIndex] == ISTATE_BREAK) {
            
            /* Continue */
            hc = OP_BREAK;
            getCharByOffsetFromCurrent(-2);
            
        }
        
        /* Sequence Location is beginning or at a read code */
        
        switch(hc) {
                
            case OP_RGI:
                /* Sets a Register */
                
                /* Tgt Register address */
                d0 = (unsigned char)getCurrentChar();
                
                /* get type */
                hc = (char)getCurrentChar();
                
                /* Check type */
                if(hc == OP_VAR) {
                    
                    /* var */
                    regType = RegisterVar;
                    
                } else if(hc == OP_NUM) {
                    
                    /* num */
                    regType = RegisterNum;
                    
                } else if(hc == OP_STRING) {
                    
                    /* string */
                    regType = RegisterString;
                    
                } else if(hc == OP_TRUE) {
                    
                    /* bool - true (1) */
                    setNumRegister(d0, 1, RegisterBool);
                    break;
                    
                } else if(hc == OP_FALSE) {
                    
                    /* bool - false (0) */
                    setNumRegister(d0, 0, RegisterBool);
                    break;
                    
                } else if(hc == OP_ARRAY_VAR) {
                    
                    /* Setting from an array variable */
                    struct CoreObject *co;
                    regType = RegisterArrayVar;
                    
                    /* get variable */
                    incrementCounter = 0;
                    
                    while((hc = (unsigned char)getCurrentChar())) {
                        
                        variable[incrementCounter++] = (char)hc;
                    }
                    
                    variable[incrementCounter] = (char)hc;
                    
                    /* get key */
                    incrementCounter = 0;
                    
                    while((hc = (unsigned char)getCurrentChar())) {
                        
                        key[incrementCounter++] = (char)hc;
                    }
                    
                    key[incrementCounter] = (char)hc;
                    
                    co = getArrayValue(variable, key);
                    
                    if(co != NULL) {
                        
                        switch(co->_state) {
                                
                            case 0:
                                
                                /* num */
                                setNumRegister(d0, co->data._fvalue, RegisterNum);
                                break;
                                
                            case 1:
                                
                                /* string */
                                setStringRegister(d0, co->data._svalue, RegisterString);
                                break;
                                
                            case 2:
                                
                                /* bool */
                                setNumRegister(d0, co->data._bvalue ? 1 : 0, RegisterNum);
                                break;
                                
                            case 3:
                                
                                /* array */
                                setArrayRegister(d0, co, RegisterArray);
                                break;
                                
                            case 4:
                                
                                /* file */
                                setFileRegister(d0, co->data._file, RegisterFile);
                                break;
                                
                            case 5:
                                
                                /* connection */
                                setConnectionRegister(d0, (int)co->data._fvalue);
                                break;
                                
                            default:
                                
                                printf("Unrecognized data type to assign register to from array key value %s\n", variable);
                                exit(54314);
                                break;
                        }
                        
                        break;
                        
                        
                    } else {
                        
                        /* Error */
                        printf("Attempted to reference a null array key\n");
                        exit(9011);
                        
                    }
                    
                } else if(hc == OP_RGI) {
                    
                    /* register to register */
                    regType = RegisterNone;
                    
                } else if(hc == OP_POP) {
                    
                    /* Register from stack value, pop off, check type, and set to our register */
                    regType = getStackRegisterType();
                    
                    if(regType == RegisterBool || regType == RegisterNum) {
                        
                        /* num/bool */
                        setNumRegister(d0, popStackRegisterNum(), regType);
                        
                    } else if(regType == RegisterString) {
                        
                        /* string */
                        char *str = popStackRegisterString();
                        setStringRegister(d0, str, regType);
                        
                    } else if(regType == RegisterArray) {
                        
                        /* array */
                        setArrayRegister(d0, popStackRegisterArray(), regType);
                        
                    } else if(regType == RegisterFile) {
                        
                        /* file */
                        setFileRegister(d0, popStackRegisterFile(), regType);
                        
                    } else if(regType == RegisterNull) {
                        
                        /* null */
                        setNullRegister(d0);
                        /* Pop the null register off (no return value) */
                        popStackRegisterNull();
                        
                    } else if(regType == RegisterConnection) {
                        
                        /* connection */
                        setConnectionRegister(d0, popStackRegisterConnection());
                        
                    } else {
                        
                        /* error */
                        printf("Attempted to pop off value into register that was of an unrecognized type\n");
                        exit(6873);
                        
                    }
                    
                    break;
                    
                } else {
                    
                    printf("Unrecognized type in register set command\n");
                    exit(9410);
                }
                
                if(hc != OP_RGI) {
                    
                    /* Set Var and Fast forward */
                    if(regType != RegisterBool) {
                        
                        /* get variable */
                        incrementCounter = 0;
                        
                        while((hc = (unsigned char)getCurrentChar())) {
                            
                            variable[incrementCounter++] = (char)hc;
                        }
                        
                        variable[incrementCounter] = (char)hc;
                        
                    }
                    
                    if(regType == RegisterArrayVar) {
                        
                        /* Get key for array vars */
                        incrementCounter = 0;
                        
                        while((hc = (unsigned char)getCurrentChar())) {
                            
                            key[incrementCounter++] = (char)hc;
                        }
                        
                        key[incrementCounter] = (char)hc;
                        
                    }
                    
                    if(regType == RegisterNum) {
                        
                        /* Set num */
                        setNumRegister(d0, atof(variable), regType);
                        
                    } else if(regType == RegisterVar || regType == RegisterArrayVar) {
                        
                        /* Set from var or array key, match type as well */
                        struct CoreObject *co = regType == RegisterVar ? getValue(variable) : getArrayValue(variable, key);
                        
                        if(co != NULL) {
                            
                            switch(co->_state) {
                                    
                                case 0:
                                    
                                    /* num */
                                    setNumRegister(d0, co->data._fvalue, RegisterNum);
                                    break;
                                    
                                case 1:
                                    
                                    /* string */
                                    setStringRegister(d0, co->data._svalue, RegisterString);
                                    break;
                                    
                                case 2:
                                    
                                    /* bool */
                                    setNumRegister(d0, co->data._bvalue ? 1 : 0, RegisterNum);
                                    break;
                                    
                                case 3:
                                    
                                    /* array */
                                    setArrayRegister(d0, co, RegisterArray);
                                    break;
                                    
                                case 4:
                                    
                                    /* file */
                                    setFileRegister(d0, co->data._file, RegisterFile);
                                    break;
                                    
                                case 5:
                                    
                                    /* connection */
                                    setConnectionRegister(d0, (int)co->data._fvalue);
                                    break;
                                    
                                default:
                                    
                                    printf("Unrecognized state to assign register to from variable %s\n", variable);
                                    exit(54315);
                                    break;
                            }
                            
                        } else if(strlen(variable) == 4 &&
                                  *variable == 't'      &&
                                  *(variable+1) == 'r'  &&
                                  *(variable+2) == 'u'  &&
                                  *(variable+3) == 'e')
                        {
                            
                            /* True constant */
                            setNumRegister(d0, 1, RegisterBool);
                            
                        } else if(strlen(variable) == 5 &&
                                  *variable == 'f'      &&
                                  *(variable+1) == 'a'  &&
                                  *(variable+2) == 'l'  &&
                                  *(variable+3) == 's'  &&
                                  *(variable+4) == 'e')
                        {
                            
                            /* False constant */
                            setNumRegister(d0, 0, RegisterBool);
                            
                        } else if(strlen(variable) == 4 &&
                                  *variable == 'n'      &&
                                  *(variable+1) == 'u'  &&
                                  *(variable+2) == 'l'  &&
                                  *(variable+3) == 'l')
                        {
                            
                            /* null constant */
                            setNullRegister(d0);
                            
                        } else {
                            
                            /* null reference */
                            setNullRegister(d0);
                            
                        }
                        
                    } else if(regType == RegisterBool) {
                        
                        /* Bool set */
                        int nm;
                        
                        hc = getCurrentChar();
                        
                        if(hc == OP_TRUE) {
                            
                            nm = 1;
                            
                        } else if(hc == OP_FALSE) {
                            
                            nm = 0;
                            
                        } else {
                            
                            printf("Unrecognized boolean assignment to register, must be true (1) or false (0)\n");
                            exit(9248);
                            
                        }
                        
                        setNumRegister(d0, nm, RegisterBool);
                        
                    } else {
                        
                        /* Just set a string */
                        setStringRegister(d0, variable, regType);
                    }
                    
                } else {
                    
                    /* Set register to another register address */
                    d1 = (unsigned char)getCurrentChar();
                    
                    if(getRegisterType(d1) == RegisterNum) {
                        
                        /* num */
                        setNumRegister(d0, getRegisterNum(d1), RegisterNum);
                        
                    } else if(getRegisterType(d1) == RegisterBool) {
                        
                        /* bool */
                        setNumRegister(d0, getRegisterNum(d1), RegisterBool);
                        
                    } else if(getRegisterType(d1) == RegisterArray) {
                        
                        /* array */
                        setArrayRegister(d0, getRegisterArray(d1), RegisterArray);
                        
                    } else if(getRegisterType(d1) == RegisterFile) {
                        
                        /* file */
                        setFileRegister(d0, getRegisterFile(d1), RegisterFile);
                        
                    } else if(getRegisterType(d1) == RegisterString) {
                        
                        /* string */
                        setStringRegister(d0, getRegisterString(d1), RegisterString);
                        
                    } else if(getRegisterType(d1) == RegisterConnection) {
                        
                        /* connection */
                        setConnectionRegister(d0, getRegisterConnection(d1));
                        
                    } else {
                        
                        /* Wrong type */
                        printf("Unrecognized register type to set another register to!\n");
                        exit(7891);
                        
                    }
                    
                }
                
                break;
                
                
            case OP_SET:
                
                /* Set var from register */
                incrementCounter = 0;
                
                while((hc = (unsigned char)getCurrentChar())) {
                    
                    variable[incrementCounter++] = (char)hc;
                }
                
                variable[incrementCounter] = (char)hc;
                
                setVarFromRegister(variable, (unsigned char)getCurrentChar());
                
                break;
                
                
            case OP_SLEEP:
                
                if(getStackRegisterType() == RegisterNum) {
                    
                    Sys_Sleep((unsigned int)popStackRegisterNum());
                    
                } else {
                    
                    printf("Invalid value provided to sleep. Expected a number for time in seconds\n");
                    exit(1);
                    
                }
                break;
                
                
            case OP_PUSH_STACK:
                
                /* Pushes a new stack */
                stack_pushStack();
                break;
                
                
            case OP_POP_STACK:
                
                /* Pops an existing stack */
                stack_popStack();
                break;
                
                
            case OP_MULTI:
                
                h1 = (unsigned char)getCurrentChar();
                h2 = (unsigned char)getCurrentChar();
                h3 = (unsigned char)getCurrentChar();
                performMultiOp(h1,h2,h3);
                break;
                
                
            case OP_UNSET:
                
                /* Unsets a given variable */
                incrementCounter = 0;
                
                while((hc = (unsigned char)getCurrentChar())) {
                    
                    variable[incrementCounter++] = (char)hc;
                }
                
                variable[incrementCounter] = (char)hc;
                unsetValue(variable);
                
                break;
                
                
            case OP_UNSET_KEY:
                
                /* Unsets a given array key */
                
                /* set variable */
                incrementCounter = 0;
                
                while((hc = (unsigned char)getCurrentChar())) {
                    
                    variable[incrementCounter++] = (char)hc;
                }
                
                variable[incrementCounter] = (char)hc;
                
                /* set key */
                incrementCounter = 0;
                
                while((hc = (unsigned char)getCurrentChar())) {
                    
                    key[incrementCounter++] = (char)hc;
                }
                
                key[incrementCounter] = (char)hc;
                
                unsetArrayKeyValue(variable, key);
                break;
                
                
            case OP_JUMP:
                
                /* get address */
                offset[0] = (char)getCurrentChar();
                offset[1] = (char)getCurrentChar();
                offset[2] = (char)getCurrentChar();
                offset[3] = (char)getCurrentChar();
                
                /* Convert to unsigned int, address (base 16) */
                address = (int)strtol(offset, NULL, 16);
                
                /* Reset to this new address */
                resetCurrentFileIndex(address);
                
                break;
                
                
            case OP_EQUAL:
                
                h1 = (unsigned char)getCurrentChar();
                h2 = (unsigned char)getCurrentChar();
                h3 = (unsigned char)getCurrentChar();
                performEqualOp(h1,h2,h3);
                break;
                
                
            case OP_NOT_EQUAL:
                
                h1 = (unsigned char)getCurrentChar();
                h2 = (unsigned char)getCurrentChar();
                h3 = (unsigned char)getCurrentChar();
                performNotEqualOp(h1,h2,h3);
                break;
                
                
            case OP_LESST:
                
                h1 = (unsigned char)getCurrentChar();
                h2 = (unsigned char)getCurrentChar();
                h3 = (unsigned char)getCurrentChar();
                performLessThanOp(h1,h2,h3);
                break;
                
                
            case OP_LESST_EQUAL:
                
                h1 = (unsigned char)getCurrentChar();
                h2 = (unsigned char)getCurrentChar();
                h3 = (unsigned char)getCurrentChar();
                performLessThanEqualToOp(h1,h2,h3);
                break;
            
                
            case OP_GREATRT:
                
                h1 = (unsigned char)getCurrentChar();
                h2 = (unsigned char)getCurrentChar();
                h3 = (unsigned char)getCurrentChar();
                performGreaterThanOp(h1,h2,h3);
                break;
                
                
            case OP_GTRTHN_EQL:
                
                h1 = (unsigned char)getCurrentChar();
                h2 = (unsigned char)getCurrentChar();
                h3 = (unsigned char)getCurrentChar();
                performLessThanEqualToOp(h1,h2,h3);
                break;
                
                
            case OP_CALL:
                
                incrementCounter = 0;
                
                while((hc = (unsigned char)getCurrentChar())) {
                    
                    variable[incrementCounter++] = (char)hc;
                }
                
                variable[incrementCounter] = (char)hc;
                
                priorFileIndex = getPositionFromFuncLabel(variable);
                
                if(priorFileIndex != -1) {
                    
                    /* Valid, push our current state and reset to this index */
                    pushFunctionRevertState(getCurrentFileIndex());
                    resetCurrentFileIndex(priorFileIndex);
                    
                } else {
                    
                    /* Invalid */
                    printf("Invalid sequence to call found, %ld\n", priorFileIndex);
                    exit(490);
                    
                }
                break;
                
                
            case OP_CLOSE:
                
                if(getStackRegisterType() == RegisterFile) {
                    
                    Sys_Close(popStackRegisterFile());
                    #pragma message("By doing this we're leaving an invalid reference in our memory tree, to a non-existant file, may or may not be an issue")
                    
                } else {
                    
                    printf("Unexpected file close error, invalid first arg, must be a variable to an open file handle\n");
                    exit(1);
                    
                }
                break;
                
                
            case OP_DIVI:
                
                h1 = (unsigned char)getCurrentChar();
                h2 = (unsigned char)getCurrentChar();
                h3 = (unsigned char)getCurrentChar();
                performDiviOp(h1,h2,h3);
                break;
                
                
            case OP_EXEC:
                
                /* XLang Execution */
                
                if(interpreterState[interpreterIndex] == ISTATE_NONE) {
                    
                    /* Clear register 1 to type 'String' on first run */
                    setStringRegister(1, "", RegisterString);
                }
                
                while((incrementCounter = getCurrentChar()) != OP_EXEC && incrementCounter != EOF) {
                    
                    hc = (int)incrementCounter;
                    
                    if(hc == '\0') {
                        
                        char *out;
                        
                        xlangPointer[xlangMarker] = (char)hc;
                        xlangMarker = 0;
                        
                        /* Dispatch! */
                        out = executeLang(xlangPointer);
                        
                        setStringRegister(2, out, RegisterString);
                        performAddOp(1, 2, 1);
                        
                    } else {
                        
                        xlangPointer[xlangMarker++] = (char)hc;
                        
                    }
                }
                
                if(incrementCounter == EOF) {
                    
                    /* Toggle into XLANG state */
                    interpreterState[interpreterIndex] = ISTATE_XLANG;
                    
                } else {
                    
                    /* Toggle out of XLANG state */
                    interpreterState[interpreterIndex] = ISTATE_NONE;
                    
                }
                
                break;
                
                
            case OP_EXPONENT:
                
                h1 = (unsigned char)getCurrentChar();
                h2 = (unsigned char)getCurrentChar();
                h3 = (unsigned char)getCurrentChar();
                performExpOp(h1,h2,h3);
                break;
                
                
            case OP_FIND:
                
                /* Find chars in chars */
                
                if(getStackRegisterType() == RegisterString) {
                    
                    variable = popStackRegisterString();
                    
                } else {
                    
                    printf("Unexpected string find error, invalid arg\n");
                    exit(1);
                    
                }
                
                if(getStackRegisterType() == RegisterString) {
                    
                    key = popStackRegisterString();
                    
                } else {
                    
                    printf("Unexpected string find error, invalid arg\n");
                    exit(1);
                    
                }
                
                Sys_Find(key, variable);
                
                setStringRegister(0, getSysResult(), RegisterString);
                pushRegister(0);
                
                break;
                
                
            case OP_FUNC:
                
                /* The next item is a function name, push it as a label on the stack with the current read index + 1 past it */
                if(interpreterState[interpreterIndex] == ISTATE_NONE) {
                    
                    /* Capture name on first pass */
                    incrementCounter = 0;
                    
                    while((hc = (unsigned char)getCurrentChar())) {
                        
                        variable[incrementCounter++] = (char)hc;
                    }
                    
                    variable[incrementCounter] = (char)hc;
                    pushFuncLabel(getCurrentFileIndex(), variable);
                    /* pushFunctionRevertState(-1); */
                    
                }
                
                /* fast forward to the end of this func */
                for(;;) {
                    
                    incrementCounter = getCurrentChar();
                    
                    if(incrementCounter == EOF) {
                        
                        /* set func state */
                        interpreterState[interpreterIndex] = ISTATE_FUNC;
                        break;
                        
                    } else if(incrementCounter == OP_FUNC) {
                        
                        /* Push & Capture additional func name */
                        incrementCounter = 0;
                        
                        while((hc = (unsigned char)getCurrentChar())) {
                            
                            variable[incrementCounter++] = (char)hc;
                        }
                        
                        variable[incrementCounter] = (char)hc;
                        pushFuncLabel(getCurrentFileIndex(), variable);
                        /* pushFunctionRevertState(-1); */
                        interpreterState[++interpreterIndex] = ISTATE_FUNC;
                        funcCounter++;
                        
                    } else if(incrementCounter == OP_FUNC_END && funcCounter > 0) {
                        
                        /* One of the functions in our function was closed, decrement our count */
                        funcCounter--;
                        /* set normal state */
                        interpreterState[interpreterIndex] = ISTATE_NONE;
                        
                        /* Decrement our interpreter state */
                        if(interpreterIndex > 0) {
                            
                            interpreterIndex--;
                        }
                        
                    } else if(incrementCounter == OP_FUNC_END && funcCounter == 0) {
                        
                        /* End of the line, break out */
                        break;
                        
                    }
                }
                
                if(incrementCounter == OP_FUNC_END && funcCounter == 0) {
                    
                    /* set normal state */
                    interpreterState[interpreterIndex] = ISTATE_NONE;
                    
                    /* Decrement our interpreter state */
                    if(interpreterIndex > 0) {
                        
                        interpreterIndex--;
                    }
                    
                }
                
                break;
                
                
            case OP_COND_JUMP:
                
                /* Conditional GOTO, if the following statement is false GOTO the following address */
                
                /* get register num */
                registerNum = (unsigned char)getCurrentChar();
                
                /* get type */
                gotoType = (unsigned char)getCurrentChar();
                
                if(((int)getRegisterNum((unsigned char)registerNum) == 0 && gotoType == 0) ||
                   ((int)getRegisterNum((unsigned char)registerNum) == 1 && gotoType == 1))
                {
                    
                    /* Perform GOTO */
                    /* get address */
                    offset[0] = (char)getCurrentChar();
                    offset[1] = (char)getCurrentChar();
                    offset[2] = (char)getCurrentChar();
                    offset[3] = (char)getCurrentChar();
                    
                    /* Convert to unsigned int, address (base 16) */
                    address = (int)strtol(offset, NULL, 16);
                    
                    /* Reset to this new address */
                    resetCurrentFileIndex(address);
                    
                } else {
                    
                    /* Consume address chars and continue */
                    getCharByOffsetFromCurrent(3);
                    
                }
                break;
                
                
            case OP_INPUT:
                
                Sys_Input();
                setStringRegister(0, getSysResult(), RegisterString);
                pushRegister(0);
                break;
                
                
            case OP_SUB:
                
                h1 = (unsigned char)getCurrentChar();
                h2 = (unsigned char)getCurrentChar();
                h3 = (unsigned char)getCurrentChar();
                performSubOp(h1,h2,h3);
                break;
                
                
            case OP_MODULUS:
                
                h1 = (unsigned char)getCurrentChar();
                h2 = (unsigned char)getCurrentChar();
                h3 = (unsigned char)getCurrentChar();
                performModOp(h1,h2,h3);
                break;
                
                
            case OP_PARENC:
                
                printf("ParenC << Not Yet Implemented >>\n");
                exit(1);
                break;
                
                
            case OP_PARENO:
                
                printf("ParenO << Not Yet Implemented >>\n");
                exit(1);
                break;
                
                
            case OP_ADD:
                
                h1 = (unsigned char)getCurrentChar();
                h2 = (unsigned char)getCurrentChar();
                h3 = (unsigned char)getCurrentChar();
                performAddOp(h1,h2,h3);
                break;
                
                
            case OP_CONTINUE:
                
                /* Fast forward to the next JUMP instruction */
                for(;;) {
                    
                    incrementCounter = getCurrentChar();
                    
                    if(incrementCounter == EOF) {
                        
                        /* set func state */
                        interpreterState[interpreterIndex] = ISTATE_CONTINUE;
                        break;
                        
                    } else if(incrementCounter == OP_JUMP) {
                        
                        /* get & check address */
                        offset[0] = (char)getCurrentChar();
                        offset[1] = (char)getCurrentChar();
                        offset[2] = (char)getCurrentChar();
                        offset[3] = (char)getCurrentChar();
                        
                        /* Convert to unsigned int, address (base 16) */
                        address = (int)strtol(offset, NULL, 16);
                        
                        
                        if(address < getCurrentFileIndex()) {
                            
                            /* Found a jump back, break out! */
                            break;
                            
                        }
                        
                    }
                }
                
                if(incrementCounter == OP_JUMP) {
                    
                    getCharByOffsetFromCurrent(-6);
                    /* set back to normal state */
                    interpreterState[interpreterIndex] = ISTATE_NONE;
                    
                }
                break;
                
                
            case OP_BREAK:
                
                /* Fast forward past the next JUMP instruction */
                for(;;) {
                    
                    incrementCounter = getCurrentChar();
                    
                    if(incrementCounter == EOF) {
                        
                        /* set func state */
                        interpreterState[interpreterIndex] = ISTATE_BREAK;
                        break;
                        
                    } else if(incrementCounter == OP_JUMP) {
                        
                        /* get & check address */
                        offset[0] = (char)getCurrentChar();
                        offset[1] = (char)getCurrentChar();
                        offset[2] = (char)getCurrentChar();
                        offset[3] = (char)getCurrentChar();
                        
                        /* Convert to unsigned int, address (base 16) */
                        address = (int)strtol(offset, NULL, 16);
                        
                        if(address < getCurrentFileIndex()) {
                            
                            /* find a jump back, break out */
                            break;
                        }
                        
                    }
                }
                
                if(incrementCounter == OP_JUMP) {
                    
                    /* set back to normal state */
                    interpreterState[interpreterIndex] = ISTATE_NONE;
                    
                }
                break;
                
                
            case OP_PRINT:
                
                /* Print our last pushed arg to the screen */
                if(getStackRegisterType() == RegisterString || getStackRegisterType() == RegisterVar) {
                    
                    Sys_Print(popStackRegisterString());
                    
                } else if(getStackRegisterType() == RegisterNull) {
                    
                    Sys_Print("null");
                    popStackRegisterNull();
                    
                } else if(getStackRegisterType() == RegisterConnection || getStackRegisterType() == RegisterNum) {
                    
                    Sys_PrintNum(popStackRegisterNum());
                    
                } else if(getStackRegisterType() == RegisterArray) {
                    
                    Sys_Print("[array]");
                    popStackRegisterArray();
                    
                } else if(getStackRegisterType() == RegisterBool) {
                    
                    argNum1 = popStackRegisterNum();
                    Sys_Print(argNum1 == 0 ? "false" : "true");
                    
                } else {
                    
                    printf("\nAttempted to print an unacceptable value\n");
                    exit(1);
                }
                break;
                
                
            case OP_GC_RATE:
                
                if(getStackRegisterType() == RegisterNum) {
                    
                    Sys_GC((float)popStackRegisterNum());
                    
                } else {
                    
                    printf("Incorrect value provided for altering the garbage collector rate, must pass a number!");
                    exit(1);
                }
                break;
                
                
            case OP_PRINT_MEM:
                
                /* Prints Out System Memory in terms of allocated user-defined variables (very basic) */
                printMem();
                break;
                
                
            case OP_PURGE:
                
                Sys_Purge();
                break;
                
                
            case OP_PUSH:
                
                /* Pushes a given register value onto an arg stack */
                pushRegister((unsigned char)getCurrentChar());
                break;
                
                
            case OP_RAND:
                
                if(getStackRegisterType() == RegisterNum) {
                    
                    int result = Sys_Random((int)popStackRegisterNum());
                    setNumRegister(0, result, RegisterNum);
                    
                } else {
                    
                    printf("Invalid value provided to random. Expected a single boundary number to produce a value between 0 and the aforementioned number\n");
                    exit(1);
                    
                }
                break;
                
                
            case OP_READ:
                
                if(getStackRegisterType() == RegisterFile) {
                    
                    Sys_Read(popStackRegisterFile());
                    
                } else {
                    
                    printf("Unexpected file read error, invalid first arg, must be a variable to an open file handle\n");
                    exit(1);
                    
                }
                
                variable = getSysResult();
                
                if(didReadSafeNull() == 0 && strcmp(variable, "null") == 0) {
                    
                    /* NOT a safe null, clear what the safe null & set a NULL register */
                    setNullRegister(0);
                    setSafeNull(1);
                    
                } else {
                    
                    /* set string result from file */
                    setStringRegister(0, variable, RegisterString);
                    
                }
                /* push register */
                pushRegister(0);
                
                break;
                
                
            case OP_REG_MATCH:
                
                /* Attempts a regex match on a string */
                if(getStackRegisterType() == RegisterString) {
                    
                    key = popStackRegisterString();
                    
                } else {
                    
                    printf("Invalid first value provided to match. Expects a string to match and a regex string representation");
                    exit(1);
                    
                }
                
                if(getStackRegisterType() == RegisterString) {
                    
                    variable = popStackRegisterString();
                    
                } else {
                    
                    printf("Invalid second value provided to match. Expects a string to match and a regex string representation");
                    exit(1);
                    
                }
                
                /* Perform the regex */
                d0 = regex(variable, key);
                setNumRegister(0, d0, RegisterNum);
                
                break;
                
                
            case OP_GET_CAP:
                
                /* Retrieves the last regex capture and sets it to the given variable */
                if(getStackRegisterType() == RegisterNum) {
                    
                    /* Fetch the capture at the provided index */
                    key = getCaptureByIndex((unsigned char)popStackRegisterNum());
                    
                    if(key != NULL) {
                        
                        /* valid capture */
                        setStringRegister(0, key, RegisterString);
                        
                    } else {
                        /* no capture */
                        setNullRegister(0);
                        
                    }
                } else {
                    printf("Invalid value provided to capture. Expects a single numeric value to return the regex capture at that index.");
                    exit(1);
                    
                }
                break;
                
                
            case OP_REMOVE:
                
                if(getStackRegisterType() == RegisterString) {
                    
                    Sys_Remove(popStackRegisterString());
                    
                } else {
                    
                    printf("Unexpected file remove error, invalid first arg, must be a name of to an existing file\n");
                    exit(1);
                    
                }
                break;
                
                
            case OP_WRITE:
                /* Write to a var holding a file name */
                if(getStackRegisterType() == RegisterString) {
                    
                    key = popStackRegisterString();
                    
                } else {
                    
                    printf("Unexpected file write error, invalid second arg, must be a valid file mode (w ,r ,a ,r+ ,w+ ,a+ ,wb+ ,rb+ ,etc. )\n");
                    exit(1);
                }
                
                
                if(getStackRegisterType() == RegisterFile) {
                    
                    tmpFile = popStackRegisterFile();
                    
                } else {
                    
                    printf("Unexpected file write error, invalid first arg, must be a variable to an open file handle\n");
                    exit(1);
                }
                
                /* Write this line to the file */
                fprintf( tmpFile, "%s\n", key);
                break;
                
                
            case OP_REPLACE:
                
                /* 4 args, 3 string and 1 num */
                if(getStackRegisterType() == RegisterNum) {
                    
                    argNum1 = popStackRegisterNum();
                } else {
                    
                    argNum1 = 0;
                }
                
                if(getStackRegisterType() == RegisterString) {
                    
                    /* String replace */
                    if(getStackRegisterType() == RegisterString) {
                        
                        key = popStackRegisterString();
                        
                    } else {
                        
                        printf("Unexpected string replace error, invalid third arg\n");
                        exit(1);
                    }
                    
                    if(getStackRegisterType() == RegisterString) {
                        
                        arg3 = popStackRegisterString();
                        
                    } else {
                        
                        printf("Unexpected string replace error, invalid second arg\n");
                        exit(1);
                        
                    }
                    
                    if(getStackRegisterType() == RegisterString) {
                        
                        variable = popStackRegisterString();
                        
                    } else {
                        
                        printf("Unexpected string replace error, invalid first arg\n");
                        exit(1);
                    }
                    
                    Sys_Replace(variable, arg3, key, (char)argNum1);
                    
                } else {
                    
                    /* Error */
                    printf("Unsupported string operation!\n");
                    exit(1);
                    
                }
                
                setStringRegister(0, getSysResult(), RegisterString);
                pushRegister(0);
                break;
                
                
            case OP_SUBSTR:
                
                if(getStackRegisterType() == RegisterNum) {
                    
                    argNum1 = popStackRegisterNum();
                    
                } else {
                    
                    argNum1 = -1;
                    
                }
                
                /* String Sub */
                if(getStackRegisterType() == RegisterNum) {
                    
                    argNum2 = popStackRegisterNum();
                    
                } else if(getStackRegisterType() == RegisterString) {
                    
                    variable = popStackRegisterString();
                    argNum2 = argNum1;
                    argNum1 = -1;
                    
                    Sys_Substr(variable, (int)argNum2, (int)argNum1);
                    
                    setStringRegister(0, getSysResult(), RegisterString);
                    pushRegister(0);
                    
                    break;
                    
                } else {
                    
                    printf("Unexpected string sub error, invalid second arg\n");
                    exit(1);
                    
                }
                
                if(getStackRegisterType() == RegisterString) {
                    
                    variable = popStackRegisterString();
                    
                } else {
                    
                    printf("Unexpected string sub error, invalid first arg\n");
                    exit(1);
                    
                }
                
                Sys_Substr(variable, (int)argNum2, (int)argNum1);
                
                setStringRegister(0, getSysResult(), RegisterString);
                pushRegister(0);
                
                break;
                
                
            case OP_OPEN:
                
                /* Open and return a file handle */
                if(getStackRegisterType() == RegisterString) {
                    
                    variable = popStackRegisterString();
                    
                } else {
                    
                    printf("Unexpected file open error, invalid second arg\n");
                    exit(1);
                    
                }
                
                if(getStackRegisterType() == RegisterString) {
                    
                    key = popStackRegisterString();
                    
                } else {
                    
                    printf("Unexpected file open error, invalid first arg\n");
                    exit(1);
                    
                }
                
                setFileRegister(0, Sys_Open(key, variable), RegisterFile);
                pushRegister(0);
                
                break;
                
                
            case OP_RETURN:
                
                d1 = (unsigned char)getCurrentChar();
                pushRegister((unsigned char)d1);
                
                if(d1 != 0) {
                    
                    d0 = 0;
                    
                    if(getRegisterType(d1) == RegisterNum) {
                        
                        /* num */
                        setNumRegister(d0, getRegisterNum(d1), RegisterNum);
                        
                    } else if(getRegisterType(d1) == RegisterBool) {
                        
                        /* bool */
                        setNumRegister(d0, getRegisterNum(d1), RegisterBool);
                        
                    } else if(getRegisterType(d1) == RegisterArray) {
                        
                        /* array, copy it to avoid null reference on stack 'pop' */
                        struct CoreObject *co = copyArrayCoreObject(getRegisterArray(d1));
                        setArrayRegister(d0, co, RegisterArray);
                        
                    } else if(getRegisterType(d1) == RegisterFile) {
                        
                        /* file */
                        setFileRegister(d0, getRegisterFile(d1), RegisterFile);
                        
                    } else if(getRegisterType(d1) == RegisterString) {
                        
                        /* string */
                        setStringRegister(d0, getRegisterString(d1), RegisterString);
                        
                    } else if(getRegisterType(d1) == RegisterConnection) {
                        
                        /* connection */
                        setConnectionRegister(d0, getRegisterConnection(d1));
                        
                    } else {
                        
                        /* Wrong type */
                        printf("Unrecognized register type to set another register to!\n");
                        exit(7891);
                        
                    }
                }
                
                /* Treat this like OP_FUNC_END and return control to the caller */
                if((priorFileIndex = popFunctionRevertState()) != -1) {
                    
                    /* Valid pop, reset to this index */
                    resetCurrentFileIndex(priorFileIndex);
                    
                } else {
                    
                    printf("Read an invalid reverting state, %ld\n", priorFileIndex);
                    exit(581);
                    
                }
                break;
                
                
            case OP_RETURN_NONE:
                
                /* Empty return */
                /* Treat this like OP_FUNC_END and return control to the caller */
                if((priorFileIndex = popFunctionRevertState()) != -1) {
                    
                    /* Valid pop, reset to this index */
                    resetCurrentFileIndex(priorFileIndex);
                    
                } else {
                    
                    printf("Read an invalid reverting state, %ld\n", priorFileIndex);
                    exit(581);
                    
                }
                break;
                
                
            case OP_SEED_RAND:
                
                if(getStackRegisterType() == RegisterNum) {
                    
                    Sys_RandomSeed((unsigned int)popStackRegisterNum());
                    
                } else {
                    
                    printf("Invalid value provided to seed. Expected a number\n");
                    exit(1);
                    
                }
                break;

                
            case OP_ARRAY_SET:
                
                /* Set array key from register */
                
                /* get variable */
                incrementCounter = 0;
                
                while((hc = (unsigned char)getCurrentChar())) {
                    
                    variable[incrementCounter++] = (char)hc;
                }
                
                variable[incrementCounter] = (char)hc;
                
                hc = (char)getCurrentChar();
                
                if(hc > 2) {
                    
                    /* Normal string val provided */
                    
                    /* get full key */
                    incrementCounter = 0;
                    key[incrementCounter++] = (char)hc;
                    
                    while((hc = (unsigned char)getCurrentChar())) {
                        
                        key[incrementCounter++] = (char)hc;
                    }
                    
                    key[incrementCounter] = (char)hc;
                    
                    /* Set */
                    setArrayVarFromRegister(variable, key, (unsigned char)getCurrentChar());
                    
                } else {
                    
                    /* Register provided */
                    if(getRegisterType((unsigned char)hc) == RegisterString) {
                        
                        /* Valid type, get our key */
                        key = getRegisterString((unsigned char)hc);
                        key = LATstrdup(key);
                        /* Set */
                        setArrayVarFromRegister(variable, key, (unsigned char)getCurrentChar());
                        LATDealloc(key);
                        
                    } else {
                        
                        /* Invalid type */
                        printf("Invalid type provided for array key\n");
                        exit(5181);
                        
                    }
                }
                break;
                
                
            case OP_FUNC_END:
                
                if((priorFileIndex = popFunctionRevertState()) != -1) {
                    
                    /* Valid pop, reset to this index */
                    resetCurrentFileIndex(priorFileIndex);
                    
                } else {
                    
                    printf("Read an invalid reverting state, %ld\n", priorFileIndex);
                    exit(581);
                    
                }
                break;
                
                
            case OP_LOAD:
                
                /* Loads an lra/lrac file to run at this point, as if it were written in here */
                if(getStackRegisterType() == RegisterString) {
                    
                    executeLatriaFile(popStackRegisterString());
                    
                } else {
                    
                    printf("Invalid value provided to load. Expected a string representation of a latria script file.");
                    exit(1);
                    
                }
                break;
                
                
            case OP_PLATFORM:
                
                /* Gets the platform name and returns it */
                Sys_Platform();
                setStringRegister(0, getSysResult(), RegisterString);
                break;
                
                
            case OP_TIME:
                
                /* Returns the current system time in seconds */
                setNumRegister(0, (double)Sys_Time(), RegisterNum);
                #pragma message("Formerly some functions perform pushRegister(0) after called, does this help with inline calling")
                break;
                
                
            case OP_STRT_SERV:
                
                /* Starts a server */
                if(getStackRegisterType() == RegisterNum) {
                    
                    /* Start our server on this port */
                    argNum1 = Sys_StartServer((int)popStackRegisterNum());
                    setConnectionRegister(0, (int)argNum1);
                    
                } else {
                    
                    /* Error */
                    printf("Invalid value provided to startServer. Expected a numeric value, representing the desired port number to bind to\n");
                    exit(1);
                    
                }
                break;
                
                
            case OP_SEND_DATA:
                
                /* Sends data to a given connection */
                if(getStackRegisterType() == RegisterString) {
                    
                    variable = popStackRegisterString();
                    
                } else {
                    
                    printf("Invalid second parameter passed to sendData. Expects a numerical connection value and a string value to send as a message via that connection\n");
                    exit(1);
                    
                }
                
                if(getStackRegisterType() == RegisterConnection) {
                    
                    argNum1 = popStackRegisterNum();
                    
                } else {
                    
                    printf("Invalid first parameter passed to sendData. Expects a numerical connection value and a string value to send as a message via that connection\n");
                    exit(1);
                    
                }
                
                /* Send Data using this connection */
                argNum1 = Sys_SendData((int)argNum1, variable);
                /* Store the result */
                setNumRegister(0, argNum1, RegisterNum);
                break;
                
                
            case OP_READ_DATA:
                
                /* Reads data from a given connection */
                if(getStackRegisterType() == RegisterConnection) {
                    
                    argNum1 = popStackRegisterNum();
                    
                } else {
                    
                    printf("Invalid parameter passed to readData. Expects a numerical connection value to read data from and return it as a string\n");
                    exit(1);
                    
                }
                
                /* Read and store the string data */
                Sys_ReadData((int)argNum1);
                variable = getSysResult();
                
                if(variable != NULL) {
                    
                    /* String return value */
                    setStringRegister(0, variable, RegisterString);
                    
                } else {
                    
                    /* Null return value */
                    setNullRegister(0);
                    
                }
                break;
                
                
            case OP_CLOSE_CONN:
                
                /* Closes a given connection */
                if(getStackRegisterType() == RegisterConnection) {
                    
                    argNum1 = popStackRegisterNum();
                    
                } else {
                    
                    printf("Invalid parameter passed to closeConnection. Expects a numerical connection value to indicate the connection to close\n");
                    exit(1);
                    
                }
                
                /* Close this connection */
                Sys_CloseConnection((int)argNum1);
                break;
                
                
            case OP_CONNECT:
                
                /* Connects to a given address */
                if(getStackRegisterType() == RegisterNum) {
                    
                    argNum1 = popStackRegisterNum();
                    
                } else {
                    
                    printf("Invalid second parameter passed to connect. Expects a string representation of an ip address (ipv4) and a port number as a numerical value\n");
                    exit(1);
                    
                }
                
                if(getStackRegisterType() == RegisterString) {
                    
                    variable = popStackRegisterString();
                    
                } else {
                    
                    printf("Invalid first parameter passed to connect. Expects a string representation of an ip address (ipv4) and a port number as a numerical value\n");
                    exit(1);
                    
                }
                
                /* Attempt to connect to the given address, and store the result */
                setConnectionRegister(0, Sys_Connect(variable, (int)argNum1));
                break;
                
                
            case OP_BITNOT:
                
                /* Bitwise NOT */
                if(getStackRegisterType() == RegisterNum) {
                    
                    /* Perform Bit Not */
                    setNumRegister(0, Sys_bit_not((int)popStackRegisterNum()), RegisterNum);
                    
                } else {
                    
                    printf("Invalid value provided to not. Takes one number argument & returns the bitwise NOT result");
                    exit(1);
                    
                }
                break;
                
                
            case OP_BITAND:
                
                /* Bitwise AND */
                if(getStackRegisterType() == RegisterNum) {
                    
                    double dd0 = popStackRegisterNum();
                    
                    if(getStackRegisterType() == RegisterNum) {
                        
                        /* Perform Bit AND */
                        setNumRegister(0, Sys_bit_and((int)popStackRegisterNum(), (int)dd0), RegisterNum);
                        
                    } else {
                        
                        printf("Invalid value provided to and. Takes two number arguments & returns the bitwise AND result");
                        exit(1);
                    }
                    
                } else {
                    
                    printf("Invalid value provided to and. Takes two number arguments & returns the bitwise AND result");
                    exit(1);
                    
                }
                break;
                
                
            case OP_BITOR:
                
                /* Bitwise OR */
                if(getStackRegisterType() == RegisterNum) {
                    
                    double dd0 = popStackRegisterNum();
                    
                    if(getStackRegisterType() == RegisterNum) {
                        
                        /* Perform Bit OR */
                        setNumRegister(0, Sys_bit_or((int)popStackRegisterNum(), (int)dd0), RegisterNum);
                        
                    } else {
                        
                        printf("Invalid value provided to or. Takes two number arguments & returns the bitwise OR result");
                        exit(1);
                    }
                    
                } else {
                    
                    printf("Invalid value provided to or. Takes two number arguments & returns the bitwise OR result");
                    exit(1);
                    
                }
                break;
                
                
            case OP_BITXOR:
                
                /* Bitwise XOR */
                if(getStackRegisterType() == RegisterNum) {
                    
                    double dd0 = popStackRegisterNum();
                    
                    if(getStackRegisterType() == RegisterNum) {
                        
                        /* Perform Bit XOR */
                        setNumRegister(0, Sys_bit_xor((int)popStackRegisterNum(), (int)dd0), RegisterNum);
                        
                    } else {
                        
                        printf("Invalid value provided to xor. Takes two number arguments & returns the bitwise XOR result");
                        exit(1);
                    }
                    
                } else {
                    
                    printf("Invalid value provided to xor. Takes two number arguments & returns the bitwise XOR result");
                    exit(1);
                    
                }
                break;
                
                
            case OP_SQRT:
                
                /* Square Root Function */
                if(getStackRegisterType() == RegisterNum) {
                    
                    /* Perform square root with value */
                    setNumRegister(0, Sys_SquareRoot(popStackRegisterNum()), RegisterNum);
                    
                } else {
                    
                    printf("Invalid value provided to sqrt. Takes a single numeric argument");
                    exit(1);
                    
                }
                break;
                
                
            case OP_RUN:
                
                /* Compiles and runs a line of latria code */
                if(getStackRegisterType() == RegisterString) {
                    
                    
                    #ifdef INCLUDECOMPILER
                    /* Handle this line as a dismissable dynamic session */
                    openDynamicInterpreterFile();
                    handleInput(popStackRegisterString());
                    closeDynamicInterpreterFile();
                    #else
                    printf("\n\nWarning!! Unable to run line of code '%s', compiler not present to handle input\n\n", popStackRegisterString());
                    #endif
                    
                } else {
                    
                    printf("Invalid value provided to run. Takes a single string that is interpreted as latria code");
                    exit(1);
                }
                break;
                
                
            default:
                
                /* Read something that was not a registered op code! */
                printf("Failed on Instruction # %d\n", instructionCounter);
                /* printf("0x%02x\n",getCharByOffsetFromCurrent(-2)); */
                /* printf("0x%02x\n",(unsigned char)getCurrentChar()); */
                printf("Unrecognized OP Code: 0x%02x\n", (unsigned char)getCharByOffsetFromCurrent(-1));
                /* printf("0x%02x\n",(unsigned char)getCurrentChar()); */
                /* printf("0x%02x\n",(unsigned char)getCurrentChar()); */
                exit(1490);
                break;
                
        }
    }
    
}


/* Strips out undesired symbols from the given input, UNLESS they are enclosed in quotes */
void stripOutUnwantedSymbols(char *input) {
    
    char *origI, *origT, *tmp = NULL;
    int finalSize;
    LATBool isInQuotes = false, first=true;
    unsigned long inputLen = strlen(input);
    
    /* Check if we even need to do this first */
    if(!strchr(input, '\t') && !strchr(input, '\r')) {
        
        return;
    }
    
    origI=input;
    
    origT = tmp = setCharTablePointerByLEN( 10, inputLen*sizeof(char));
    
    /* Use our buffer to copy over our input WITHOUT the symbols we wish to cut out*/
    
    while(*input) {
        
        if(*input == '"' || *input=='\'') {
            
            if(first) {
                
                isInQuotes=true;
            
            } else if(*(input-1) != '\\') {
                
                isInQuotes = (isInQuotes)?false:true;
            }
            
        }
        
        /* Omitted characters are included in the 'if' below*/
        if((*input != '\t' && *input != '\r') || isInQuotes) {
            
            *tmp++ = *input;
        }
            
        if(first) {
            
            first = false;
        }
        
        input++;
    }
    
    *tmp = '\0';
    /* Copy our buffer back over to our input*/
    input = origI;
    tmp = origT;
    
    finalSize = (int)strlen(tmp);
    
    strncpy(input, tmp, (size_t)finalSize);
    input[finalSize] = '\0';
}


char* fetchXDeclaration(char *input) {
    
    char *start,*end;
    
    if(input == NULL) {
        
        return NULL;
    }
    
    start = strstr(input, OPENING_MARK);
    /* Closing mark will be ##> */
    end = strstr(input, CLOSING_MARK);
    
    if(start == NULL && end == NULL && !InLang_isLangBlockActive()) {
        
        /* No start & no end & NOT in an active block, so we skip out */
        return input;
    }
    
    if(start != NULL && !InLang_isLangBlockActive()) {
        
        /* We have a start sequence & NOT in an active language block */
        
        char *cmdlist = NULL;
        /* extract varName */
        char *varName = LATsubstring( input, 0, (int)(start-input));
        
        if(end == NULL) {
            
            /* Copy everything past our start sequence */
            cmdlist = LATsubstring(start, OML, (int)strlen(start));
        } else {
            
            /* strip away terminator sequence and copy what's left */
            cmdlist = LATsubstring(start, OML, (int)strlen(start)-CML);
        }
        
        InLang_startLangBlock(varName);
        InLang_appendToCMDList(cmdlist);
        
        LATDealloc(cmdlist), cmdlist = NULL;
        
        if(end != NULL) {
            
            char *retVal = executeLang(InLang_getCMDList());
            
            /* Sets the output based on return value flag being set*/
            if(stack_getReturnValue() == NULL) {
                
                char *inLangVarName = InLang_getVarName();
                setValue(inLangVarName, retVal);
            } else {
                
                stack_setReturnValue(retVal);
            }
            
            InLang_freeLangBlock();
        }
        
    } else if(InLang_isLangBlockActive()) {
        
        /* An active lang block */
        
        if(end != NULL) {
            
            /* End sequence found, let's wrap this up */
            char *retVal;
            
            /* determine if there is a command piece on here*/
            if(strlen(input) != 3) {
                
                /* Extract the additional items */
                char *tmp = LATsubstring(input, 0, (int)strlen(input)-3);
                InLang_appendToCMDList(tmp);
                LATDealloc(tmp), tmp = NULL;
            }
            
            retVal = executeLang(InLang_getCMDList());
            
            /* Sets the output based on return value flag being set*/
            if(stack_getReturnValue() == NULL) {
                
                setValue(InLang_getVarName(), retVal);
            } else {
                
                stack_setReturnValue(retVal);
            }
            
            InLang_freeLangBlock();
            
        } else {
            
            /* No end or start sequence, simply append this command */
            InLang_appendToCMDList(input);
        }
    }
    
    return NULL;
}


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
        
        
        /* if we did NOT block out in this line, and we ARE blocked out, this line is dead to us, mark it as so*/
        if(!blockedOutThisLine && getBlockCommentState()) {
            
            *origI='\0';
        }
    }
}



/* Executes the following code under a NEW given language*/
char * executeLang(char *xlangSource) {
    
    char *cmdlist;
    FILE *pipe;
    char charBuff[1024] = {0};
    int buffLen = sizeof(charBuff), reallocdCML = 0, curLen = 0, maximumStorageLength=STARTING_MAXIMUM_STORAGE_LENGTH;
    char *tableRef = NULL, *origRef;
    cmdlist = LATstrdup(xlangSource);
    
    if(strstr(cmdlist, INJECT_S) && strstr( cmdlist, INJECT_E)) {
        
        /* 1. There may be more than one...so we perform a while LOOP to the beginning of char[1] from INJECT_S (set a counter mark here as well)*/
        char *tc = cmdlist, *beginRemove = NULL, *extractedPV = NULL;
        
        char *ssMark = NULL, *seMark = NULL;
        
        while((ssMark = strstr(cmdlist, INJECT_S)) && (seMark = strstr(cmdlist, INJECT_E))) {
            
            char tmpChar;
            char *extractedVal = NULL;
            char *evi;
            char *advancedPointer;
            size_t x,y,z;
            size_t replaceCount = 0;
            char *apORIG;
            
            /* fast forward to our first item*/
            beginRemove = tc+=(ssMark-cmdlist);
            /*2. Once we've found that keep advancing along until we have determined what we found is a valid INJECT_S section
             *3. At this point mark RIGHT AFTER the section, this will be our pointer to our var/function declaration*/
            extractedPV = tc+=ISL; /* move forward to the beginning of our var/function, mark it*/
            /* 4. Keep advancing until we reach the section matching INJECT_E*/
            tc = cmdlist;
            tc+=(seMark-cmdlist);
            
            /* 9. Extract out our var/function*/
            tmpChar = *tc;
            *tc = '\0';
            
            #pragma message("hmmm, in-line subs may need to be a bit tricky here...")
            /*extractedVal = fetchFunctionDeclaration(extractedPV);*/
            
            *tc = tmpChar;
            evi = extractedVal;
            
            /* warning safety checkes, especially when setting evi to null, i think this was when i introduced getting a CoreObject back from getValue instead of just the value to try and save memory. So most likely just suspicion, but noted nonetheless */
            if(extractedVal == NULL) {
                
                struct CoreObject *evs;
                *tc = '\0';
                evs = getValue(extractedPV);
                
                if(evs == NULL) {
                    
                    /* Null lang ref */
                    printf("Attempted to interpolate a non-assigned variable value '%s' in '%s'\n", extractedPV,xlangSource);
                    exit(1);
                    
                }
                
                switch (evs->_state) {
                        
                    case 0:
                        
                        /* FLOAT*/
                        extractedVal = convertFloatToString(evs->data._fvalue);
                        evi = extractedVal;
                        break;
                        
                        
                    case 1:
                        
                        /* STRING*/
                        extractedVal = evs->data._svalue;
                        evi = NULL;
                        break;
                        
                        
                    case 2:
                        
                        /* BOOL*/
                        extractedVal = (evs->data._bvalue)?"true":"false";
                        evi = NULL;
                        break;
                        
                }
                
                *tc = tmpChar;
            }
            
            /* getValue Mod the two lines below this*/
            if(extractedVal == NULL) {
                
                extractedVal = "0";
            }
            
            /*10. Get the length of:
             *      x-The counter mark (size of the injection section that will be removed)*/
            x = (size_t)((tc-beginRemove)+IEL);
            
            /*      y-The length of the output from the var/function*/
            y = strlen(extractedVal);
            
            /*      z-The length of the cmd being passed in*/
            z = strlen(cmdlist);
            
            /*11. If z - x + y > z then make sure to REALLOC to a higher size*/
            if(z-x+y > z) {
                
                /* warning reallocation throws things off, again */
                cmdlist = LATAlloc( cmdlist, z*sizeof(char), (z-x+y+1)*sizeof(char)); /* Realloc to a higher size!*/
                reallocdCML = 1;
                
                /* NULLify our lang block, as we have messed it up by reallocating here*/
                InLang_NULLLangBlock();
                InLang_appendToCMDList(cmdlist);
                
                /* reset the following marks */
                ssMark = strstr( cmdlist, INJECT_S);
                seMark = strstr( cmdlist, INJECT_E);
                
            }
            /*12. Create a new pointer from the existing input, and fast forward (while setting to our new pointer) to the beginning of our INJECT_S*/
            tc = cmdlist; /* reset*/
            tc+=(ssMark-cmdlist); /* fast forward to removal section*/
            
            /*13. Place in the output we received*/
            
            apORIG = setCharTablePointer(6, seMark);
            
            advancedPointer = apORIG;
            
            while(*extractedVal) {
                
                *tc++ = *extractedVal;
                extractedVal++;
                replaceCount++;
            }
            
            /*14. Fast forward another counter the end of INJECT_E, do NOT copy any data over while doing this*/
            advancedPointer+=IEL;
            
            /*15. Copy the rest of the data in and call again to check if there are any more items requiring injection*/
            
            /* only need to readjust if everything else if we're not on spot*/
            if(replaceCount != x) {
                
                while(*advancedPointer) {
                    
                    *tc++ = *advancedPointer++;
                }
                
                *tc = '\0';
            }
            
            if(evi != NULL) {
                
                LATDealloc(evi), evi = NULL;
            }
        }
    }
    
    /* MAC/OSX piping */
    #ifdef _WIN32
    pipe = _popen(cmdlist, "r");
    #else
    pipe = popen(cmdlist, "r");
    #endif
    
    if(!pipe) {
        
        printf("Error occured attempting to execute another language!\n");
        
        /* check to free our cmdlist if reallocd*/
        if(reallocdCML == 1) {
            
            LATDealloc(cmdlist);
        }
        
        /* free our command list */
        LATDealloc(cmdlist);
        
        return tableRef;
    }
    
    /* Set up table ref */
    origRef = tableRef = setCharTablePointerByLEN( 33, STARTING_MAXIMUM_STORAGE_LENGTH);
    
    /* While not at end of file */
    while(!feof(pipe)) {
        
        /* Not at end of file */
        if(fgets( charBuff, buffLen, pipe) != NULL) {
            /* Handle what we read */
            
            int charbuffLen = (int)strlen(charBuff);
            /* Get size of what we read */
            
            /* Bump up the curLen */
            curLen+=charbuffLen;
            
            if(curLen > maximumStorageLength) {
                
                /* Realloc, go to next increment of STARTING_MAXIMUM_STORAGE_LENGTH */
                maximumStorageLength = STARTING_MAXIMUM_STORAGE_LENGTH * ((curLen / STARTING_MAXIMUM_STORAGE_LENGTH) + 1);
                origRef = tableRef = setCharTablePointerByLEN( 33, (unsigned long)maximumStorageLength);
                tableRef+=curLen-charbuffLen;
            }
            
            /* Copy what we read into our tableRef */
            tableRef = strncpy(tableRef, charBuff, (size_t)charbuffLen);
            tableRef[charbuffLen] = '\0';
            
            /* Bump tableRef by size of what we read */
            tableRef+=charbuffLen;
        }
    }
    
    fflush(pipe);
    
    #ifdef _WIN32
    if(_pclose(pipe) == -1) {
    #else
    if(pclose(pipe) == -1) {
    #endif
        
        printf(">>: Soft Error closing pipe, '%s', continuing execution~\n", cmdlist);
    }
    
    /* free our command list */
    LATDealloc(cmdlist);
    
    /* free our lang block */
    InLang_freeLangBlock();
    
    /* Return our table ref backed up to the start */
    return origRef;
}

    
/*Checks for a return and/or a value specified in the input
 returns true if there is
 return false if there isn't*/
LATBool checkForReturnValue(char *input) {
    
    char *se = strstr(input, "return");
    if(se != NULL) {
        
        if(strlen(se)>5) { /* check to see if there is something after this return*/
            
            if(!isspace(*(se+6))) { /* if following character is not a space, this is not a return statement*/
                
                return false;
            }
        }
        
        if((se-input) > 0) { /* check to see if there is something before this return*/
            
            if(!isspace(*(se-1))) { /* if preceding character is not a space, this is not a return statement*/
                
                return false;
            }
        }
        
        input+=6;
        
        /* set this scope's return value if a valid num was passed in, otherwise simply kill line of execution*/
        if(*input) {
            
            stack_setReturnValue(input);
        }
        
        return true;
    }
    return false;
}

    
/* Exteriorly referenced by prePerformMath for 'normal addition' */
char additionOutput[25];
    

/* Extracts and sets a register value to a variable */
void setVarFromRegister(char *varName, unsigned char registerNum) {
    
    RegisterType type = getRegisterType(registerNum);
    
    if(type == RegisterNum) {
        
        /* Number */
        char stringVal[26];
        sprintf(stringVal, "%.9f", getRegisterNum(registerNum));
        setValue(varName, stringVal);
        
    } else if(type == RegisterString) {
        
        /* String */
        setValue(varName, getRegisterString(registerNum));
        
    } else if(type == RegisterBool) {
        
        /* Bool */
        setBoolValue(varName, getRegisterNum(registerNum) == 1 ? true : false);
        
    } else if(type == RegisterVar) {
        
        /* Var */
        char stringVal[20];
        struct CoreObject *co = getValue(varName);
        
        switch(co->_state) {
                
                
            case 0:
                
                /* float */
                sprintf(stringVal, "%g", co->data._fvalue);
                setValue(varName, stringVal);
                break;
                
                
            case 1:
                
                /* string */
                setValue(varName, co->data._svalue);
                break;
                
                
            case 2:
                
                /* bool */
                setBoolValue(varName, co->data._bvalue);
                break;
                
                
            case 3:
                
                /* array */
                printf("This involves copying an entire array tree, something we cannot do just yet\n");
                exit(5438);
                break;
                
                
            case 4:
                
                /* file */
                setFileValue(varName, co->data._file);
                break;
                
                
            default:
                
                printf("Unrecognized core object state\n");
                exit(8252);
                break;
                
        }
        
    } else if(type == RegisterFile) {
        
        /* File */
        setFileValue(varName, getRegisterFile(registerNum));
        
    } else if(type == RegisterArray) {
        
        /* Array */
        setArrayObject(varName, getRegisterArray(registerNum));
        
    } else if(type == RegisterNull) {
        
        /* Null, Unset */
        unsetValue(varName);
        
    } else if(type == RegisterConnection) {
        
        /* Socket Connection */
        setConnectionValue(varName, getRegisterConnection(registerNum));
        
    } else {
        
        /* Unrecognized Type */
        printf("Unrecognized Register Type\n");
        exit(187);
        
    }
}
    

/* Extracts and sets a register vaue to an array key */
void setArrayVarFromRegister(char *varName, char *key, unsigned char registerNum) {
    
    RegisterType type = getRegisterType(registerNum);
    
    if(type == RegisterNum) {
        
        /* Number */
        char stringVal[20];
        sprintf(stringVal, "%g", getRegisterNum(registerNum));
        setArrayValue(varName, key, stringVal);
        
    } else if(type == RegisterString) {
        
        /* String */
        setArrayValue(varName, key, getRegisterString(registerNum));
        
    } else if(type == RegisterBool) {
        
        /* Bool */
        setArrayBoolValue(varName, key, getRegisterNum(registerNum) == 1 ? true : false);
        
    } else if(type == RegisterVar) {
        
        /* Var */
        char stringVal[20];
        struct CoreObject *co = getValue(varName);
        
        switch(co->_state) {
                
                
            case 0:
                
                /* float */
                sprintf(stringVal, "%g", co->data._fvalue);
                setArrayValue(varName, key, stringVal);
                break;
                
                
            case 1:
                
                /* string */
                setArrayValue(varName, key, co->data._svalue);
                break;
                
                
            case 2:
                
                /* bool */
                setArrayBoolValue(varName, key, co->data._bvalue);
                break;
                
                
            case 3:
                
                /* array */
                printf("This involves copying an entire array tree into an array key (nested), something we cannot do just yet\n");
                exit(5438);
                break;
                
                
            case 4:
                
                /* file */
                setArrayFileValue(varName, key, co->data._file);
                break;
                
                
            default:
                
                printf("Unrecognized core object state\n");
                exit(8252);
                break;
                
        }
        
    } else if(type == RegisterConnection) {
        
        /* Connection */
        setArrayConnectionValue(varName, key, getRegisterConnection(registerNum));
        
    } else {
        
        /* Unrecognized Type */
        printf("Unrecognized Register Type\n");
        exit(18765);
        
    }
}
