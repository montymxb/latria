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
//  Latria_C_Lexical.c
//  Latria
//
//  Created by Benjamin Friedman on 11/3/15.
*/

#include "Latria_C_Lexical.h"

/* Compiler state, for, well, everything! */
#define MAX_COMPILER_STACK_STATE 25
unsigned short _compilerState[MAX_COMPILER_STACK_STATE] = {LATC_START};
int compilerStateIndex = 0;

/* Current line # */
unsigned int lineNumber = 0;

/* Operand Stack */
ExpressionElement *operandStack;
short operandStackIndex = -1;
short operandStackMax   = 0;

/* Operator Stack */
ExpressionElement *operatorStack;
short operatorStackIndex = -1;
short operatorStackMax   = 0;
short operatorParenCount = 0;

/* Relational Operator Stack */
ExpressionElement *relationalOperatorStack;
short relationalOperatorStackIndex = -1;
short relationalOperatorStackMax   = 0;
short relationalOperatorParenCount = 0;


/* Current register index (out of 255) */
unsigned char baselineRegisterNum = 0;
unsigned char _lastRegistersSet[2];

#define MAXIMUM_REGISTER_COUNT 3

/* Holds the last value set on any given register, if we attempt to set a redundant value the compiler omits that redundant set instruction */
char *lastRegisterValues[MAXIMUM_REGISTER_COUNT] = {NULL};
unsigned short lastRegisterType[MAXIMUM_REGISTER_COUNT] = {9999};
unsigned short lastRegisterOps[MAXIMUM_REGISTER_COUNT] = {999};
size_t lastRegisterValuesLen[MAXIMUM_REGISTER_COUNT] = {0};

#define FUNC_NAME_STACK_MAX 15
char *funcNameStack[FUNC_NAME_STACK_MAX] = {NULL};
int funcNameStackIndex = 0;

/* Tracks first dispatch */
unsigned char isFirstDispatch = 0;

/* capture */
char *__captureIndex;
char *__savedCapture = NULL;
unsigned int __savedCaptureLength = 0;

/* Holds the originally passed line */
char *originalInput;

/* Holds the left hand value of an equals expression, keeping it away from the operands */
char *leftElement;

/* Loop Flag */
char shouldBreak;

/* Analyzes a given input to determine how to compile it, once it's done it either passes it off to be compiled by a function or it returns */
void analyzeInputToCompile(char *input) {
    
    unsigned char registerResetCounter;
    ExpressionElement *ee;
    
    /* For debugging */
    originalInput = input;
    
    /* Clear Loop flag */
    shouldBreak = 0;
    
    /* reset first dispatch */
    isFirstDispatch = 0;
    
    /* increment line # */
    lineNumber++;
    
    /* Strips comments and handles block comments as well */
    stripComments(input);
    
    /* Trim from the back */
    stripLineEndings(input);
    
    /* Trim from the front */
    while(*input == '\t' || *input == '\n' || *input == '\r' || isspace(*input)) {
        input++;
    }
    
    if(!*input) {
        return;
        
    }
    
    /* Begin Instructions */
    startInstructions();
    
    /* Set capture level */
    resetCapturedInput(input);
    
    /* Make sure this is not a crosslang exec */
    if(getCompilerState() != LATC_EXEC_END || strstr(input, "#>")) {
        stripWhitespace(input);
    }
    
    for(;;) {
        
        switch (getCompilerState()) {
            case LATC_START:
                input = analyzeStart(input);
                break;
                
            case LATC_VAL:
                /* Variable/Function Call of some sort */
                input = analyzeValue(input);
                break;
                
            case LATC_EQUALS:
                /* We have read an equals sign */
                input = analyzeEquals(input);
                break;
                
            case LATC_ASSIGN:
                /* Writes out set or array set as needed */
                input = analyzeAssignment(input);
                break;
                
            case LATC_ARRAY_INIT:
                input = analyzeArrayInit(input);
                break;
                
            case LATC_RVAL_START:
                /* Start of rvalue */
                input = analyzeRValueStart(input);
                break;
                
            case LATC_NUM:
                /* Numeric value being processed */
                input = analyzeNum(input);
                break;
                
            case ASSIGN_NUM:
                /* Done and Single Number to assign */
                #ifdef LAT_DEBUG
                assignNum(input);
                #else
                assignNum();
                #endif
                break;
                
            case ASSIGN_STR:
                /* Done and single string assign */
                #ifdef LAT_DEBUG
                assignString(input);
                #else
                assignString();
                #endif
                break;
                
            case ASSIGN_VAR:
                /* Done and Single Var to assign */
                #ifdef LAT_DEBUG
                assignVar(input);
                #else
                assignVar();
                #endif
                break;
                
            case ASSIGN_GENERIC:
                /* Generic assign */
                #ifdef LAT_DEBUG
                assignGeneric(input);
                #else
                assignGeneric();
                #endif
                break;
                
            case LATC_LPOST:
                /* For post reading a var on the left, a [,=, or ( can follow */
                input = analyzeLPost(input);
                break;
                
            case LATC_DONE:
                /* exit */
                shouldBreak = 1;
                /* resetCompilerState(); */
                popCompilerState();
                pushCompilerState(LATC_START);
                break;
                
            case LATC_VAL_ARRAY:
                /* Array assigner */
                input = analyzeValArray(input);
                break;
                
            case LATC_CALL:
                /* Generic function call */
                input = analyzeFunctionCall(input);
                break;
                
            case LATC_CALL_SETLAST:
                /* Makes the last value on the register a PUSH item */
                printf("'analyzeFunctionCallArg' called from state machine loop. This is the one function that does NOT conform to our standard, should NOT be calling this\n");
                exit(9140);
                /*input = analyzeFunctionCallArg(input);*/
                break;
                
            case LATC_VAL_ARRAY_C:
                /* Looking for closing mark to array ref */
                input = analyzeValArrayClose(input);
                break;
                
            case LATC_DQUOTE:
                /* Double Quoted string */
                input = analyzeQuoted(input, '"');
                break;
                
            case LATC_SQUOTE:
                /* Single Quoted String */
                input = analyzeQuoted(input, '\'');
                break;
                
            case LATC_RPOST:
                /* For analyzing after just matching a right value */
                input = analyzeRPost(input);
                break;
                
            case LATC_OPERATOR:
                /* Analyzes an operator that was found */
                input = analyzeOperator(input);
                break;
                
            case LATC_DEFFUNC_NAME:
                /* Analyzes function name in a definition */
                input = analyzeFunctionDefinitionName(input);
                break;
                
            case LATC_DEFFUNC_ARGS:
                /* Analyzes function args in a definition */
                input = analyzeFunctionDefinitionArgs(input);
                break;
                
            case LATC_DEFFUNC_START:
                /* Analyzes for a starting sequence to a function */
                input = analyzeFunctionDefinitionStart(input);
                break;
                
            case LATC_DEFFUNC_END:
                /* Analyzes for a terminating sequence to a function */
                input = analyzeFunctionDefinitionEnd(input);
                break;
                
            case LATC_IF:
                /* Analyze if conditions */
                input = analyzeIf(input);
                break;
                
            case LATC_RETURN:
                /* Analyzes a return statement for a current function */
                input = analyzeReturn(input);
                break;
                
            case LATC_RETURN_NONE:
                /* Analyzes an empty return statement */
                input = analyzeReturnNone(input);
                break;
                
            case LATC_IF_START:
                /* Anayze the start of an if */
                input = analyzeIfStart(input);
                break;
                
            case LATC_IF_END:
                /* Analyze end of an if */
                input = analyzeIfEnd(input);
                break;
                
            case LATC_IF_POST:
                /* Analyze post if */
                input = analyzeIfPost(input);
                break;
                
            case LATC_ELSEIF:
                /* Analyze ElseIf */
                input = analyzeElseIf(input);
                break;
                
            case LATC_ELSEIF_START:
                /* Analyze ElseIf Start */
                input = analyzeElseIfStart(input);
                break;
                
            case LATC_ELSEIF_END:
                /* Analyze ElseIf End */
                input = analyzeElseIfEnd(input);
                break;
                
            case LATC_ELSE_POST:
                /* Analyze Else(if) Post */
                input = analyzeElseIfPost(input);
                break;
                
            case LATC_ELSE_START:
                /* Analyze Else Start */
                input = analyzeElseStart(input);
                break;
                
            case LATC_ELSE_END:
                /* Analyze Else End */
                input = analyzeElseEnd(input);
                break;
                
            case LATC_FOR_DECLARATION:
                /* Analyze the beginning of a for loop */
                input = analyzeForDeclaration(input);
                break;
                
            case LATC_FOR_LIMIT:
                /* Analyze the limit of a for loop */
                input = analyzeForLimit(input);
                break;
                
            case LATC_FOR_INCREMENT:
                /* Analyze the incrementing component of a for loop */
                input = analyzeForIncrement(input);
                break;
                
            case LATC_FOR_START:
                /* Analyze the opening bracket of a for loop */
                input = analyzeForStart(input);
                break;
                
            case LATC_FOR_END:
                /* Analyze for the closing bracket of a for loop */
                input = analyzeForEnd(input);
                break;
                
            case LATC_WHILE_PRE:
                /* Prepares the VM for the upcoming while loop */
                input = prepareWhile(input);
                break;
                
            case LATC_WHILE:
                /* Analyze the limit of a while loop (and push a label) */
                input = analyzeWhile(input);
                break;
                
            case LATC_WHILE_START:
                /* Analye for the start of a while loop */
                input = analyzeWhileStart(input);
                break;
                
            case LATC_WHILE_END:
                /* Analyze for the end of a while loop */
                input = analyzeWhileEnd(input);
                break;
                
            case LATC_EXEC_END:
                /* Analyze the end of exec */
                input = analyzeExecEnd(input);
                break;
                
            default:
                #ifdef LAT_DEBUG
                setCompileError("Unrecognized input on the following line","", originalInput);
                #else
                setCompileError();
                #endif
                break;
        }
        
        if(getCompileError() != 0) {
            /* Compiler Error Occurred */
            exit(1);
            
        } else if(shouldBreak == 1) {
            break;
        }
        
    }
    
    /* Check for any stray operands, if we find any it's an error */
    if((ee = popOperand()) != NULL) {
        /* Stray operand found! */
        printf("Stray operand found:%s\n",ee->element);
        exit(632);
    }
    
    if((ee = popOperator()) != NULL) {
        /* Stray operator found! */
        printf("Stray operator found:%hu\n",ee->op);
        exit(1940);
    }
    
    /* Free the left element */
    freeLeftHandValue();
    
    /* Reset values and free as needed */
    for(registerResetCounter = 0; registerResetCounter < MAXIMUM_REGISTER_COUNT; registerResetCounter++) {
        if(lastRegisterValues[registerResetCounter] != NULL) {
            free(lastRegisterValues[registerResetCounter]);
            lastRegisterValues[registerResetCounter] = NULL;
        }
        lastRegisterType[registerResetCounter] = 9999;
        lastRegisterOps[registerResetCounter] = 999;
        lastRegisterValuesLen[registerResetCounter] = 0;
    }
    
    _lastRegistersSet[0] = 0;
    _lastRegistersSet[1] = 0;
    
    /* Finish and submit instructions */
    dispatchInstructions();
}



/**** COMPILER STATE FUNCTIONS ****/

/* Analyzes the start of a line */
char *analyzeStart(char *in) {
    
    #ifdef LAT_DEBUG
    printf("Analyze Normal:\n%s\n\n",in);
    #endif
    
    if(isalpha(*in) || *in == '_') {
        
        if(*in && *(in+1) && *(in+2) && *(in+3) && *(in+4) && *(in+5) && *in == 'r' && *(in+1) == 'e' && *(in+2) == 't' && *(in+3) == 'u' && *(in+4) == 'r' && *(in+5) == 'n') {
            /* return mark */
            resetCapturedInput(in+=6);
            popCompilerState();
            pushCompilerState(LATC_RETURN);
            pushCompilerState(LATC_RVAL_START);
            return in;
            
        } else if(strcmp(in, "continue") == 0) {
            /* Continue */
            popCompilerState();
            pushCompilerState(LATC_DONE);
            writeOutCode(OP_CONTINUE);
            return in;
                
        } else if(strcmp(in, "break") == 0) {
            /* Break */
            popCompilerState();
            pushCompilerState(LATC_DONE);
            writeOutCode(OP_BREAK);
            return in;
            
        } else {
            /* name */
            resetCapturedInput(in);
            popCompilerState();
            pushCompilerState(LATC_VAL);
            return in;
            
        }
        
    } else if(isWhitespace(*in) == 1) {
        /* continue */
        return ++in;
    
    } else if(*in == '@') {
        /* function declaration */
        resetCapturedInput(++in);
        
        if(isalpha(*in)) {
            popCompilerState();
            pushCompilerState(LATC_DEFFUNC_NAME);
            
        } else {
            /* Function names can only start with alpha-only characters! */
#ifdef LAT_DEBUG
            setCompileError("Function declaration started with non-alpha character", in, originalInput);
#else
            setCompileError();
#endif
            
        }
        
        return in;
        
    } else if(*in == '}' && (getPriorCompilerState() == LATC_DEFFUNC_END ||
                             getPriorCompilerState() == LATC_IF_END ||
                             getPriorCompilerState() == LATC_ELSEIF_END ||
                             getPriorCompilerState() == LATC_ELSE_END ||
                             getPriorCompilerState() == LATC_FOR_END ||
                             getPriorCompilerState() == LATC_WHILE_END)) {
        
        /* Pop to handle function def end */
        popCompilerState();
        return in;
        
    } else if(*in == '#' && *(in+1) && *(in+1) == '#') {
        /* Start of inlang execution (write left hand value to batched queue) */
        
        /*
        // Set Batched 
        setWriteMode(WRITE_MODE_BATCHED);
        
        // Write assignment to register 0
        writeOutCode((char)OP_SET);
        writeOutCharacters(getCapturedInput(in));
        writeOutCode((char)1);
        
        // Set normal write
        setWriteMode(WRITE_MODE_NORMAL);
        */
        
        in+=2;
        
        if(*in) {
#ifdef LAT_DEBUG
            setCompileError(":>> Syntax Error. You may not follow an xlang declaration with anything other than a line break. Use the following lines to put xlang data in.", in, originalInput);
#else
            setCompileError();
#endif
            
        }
        
        popCompilerState();
        pushCompilerState(LATC_EXEC_END);
        
        /* Write out an exec op code */
        writeOutCode((char)OP_EXEC);
        
        return in;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized start character provided",in, originalInput);
#else
        setCompileError();
#endif
        return NULL;
    }
}

/* LATC_LVAL */
char *analyzeValue(char *in) {
    
    #ifdef LAT_DEBUG
    printf("Analyze Value:\n%s\n\n",in);
    #endif
    
    if(isalnum(*in) || *in == '_') {
        /* continue */
        return ++in;
        
    } else if((*in == '=' || *in == ')') && (getPriorCompilerState() == LATC_IF || getPriorCompilerState() == LATC_ELSEIF)) {
        /* Push & Pop Back */
        pushOperand(getCapturedInput(in), OP_VAR, 0);
        popCompilerState();
        return in;
        
    } else if(*in == '=' && *(in+1) != '=') {
        /* Convert to Equals, then Right side mode */
        popCompilerState();
        pushCompilerState(LATC_EQUALS);
        return in;
        
    } else if(isWhitespace(*in) == 1) {
        /* space terminates name */
        popCompilerState();
        return ++in;
    
    } else if(*in == '(') {
        /* start analyzing function call or control flow */
        
        char *capIn = getCapturedInput(in);
        
        if(strlen(capIn) == 2 && *capIn == 'i' && *(capIn+1) == 'f') {
            /* IF */
            resetCapturedInput(++in);
            popCompilerState();
            /* increment the grouping num of all following jumps */
            incrementJumpGroup();
            pushCompilerState(LATC_IF);
            
        } else if(strlen(capIn) == 3 && *capIn == 'f' && *(capIn+1) == 'o' && *(capIn+2) == 'r') {
            /* FOR */
            resetCapturedInput(++in);
            popCompilerState();
            /* increment the grouping num of all following jumps */
            incrementJumpGroup();
            pushCompilerState(LATC_FOR_DECLARATION);
            
        } else if(strlen(capIn) == 5 && *capIn == 'w' && *(capIn+1) == 'h' && *(capIn+2) == 'i' && *(capIn+3) == 'l' && *(capIn+4) == 'e') {
            /* WHILE */
            resetCapturedInput(++in);
            popCompilerState();
            /* increment the grouping num of all following jumps */
            incrementJumpGroup();
            pushCompilerState(LATC_WHILE_PRE);
            
        } else {
            pushFuncName(getCapturedInput(in));
            resetCapturedInput(++in);
            
            popCompilerState();
            pushCompilerState(LATC_CALL);
            
        }
        return in;
        
    } else if(*in == ')' && (getPriorCompilerState() == LATC_CALL || getPriorCompilerState() == LATC_DEFFUNC_ARGS || getPriorCompilerState() == LATC_IF)) {
        /* Pop to be handled by prior state */
        popCompilerState();
        return in;
        
    } else if(*in == ')' && getPriorCompilerState() == LATC_WHILE) {
        /* Push operand */
        pushOperand(getCapturedInput(in), OP_VAR, 0);
        /* Pop to be handled by while */
        popCompilerState();
        return in;
        
    } else if(*in == ',' && (getPriorCompilerState() == LATC_DEFFUNC_ARGS || getPriorCompilerState() == LATC_CALL)) {
        /* Pop to have this arg handled */
        popCompilerState();
        return in;
        
    } else if(*in == '[') {
        /* analyzing an lval array element */
        pushOperand(getCapturedInput(in), OP_VAR, 1);
        if(getPriorCompilerState() != LATC_IF && getPriorCompilerState() != LATC_ELSEIF) {
            popCompilerState();
            pushCompilerState(LATC_LPOST);
        } else {
            popCompilerState();
        }
        pushCompilerState(LATC_VAL_ARRAY);
        resetCapturedInput(++in);
        return in;
        
    } else if(isArithmeticOperator(*in) == 1) {
        /* Let's get analyzing! */
        if(isalnum(*(in-1))) {
            pushOperand(getCapturedInput(in), OP_VAR, 0);
        }
        popCompilerState();
        pushCompilerState(LATC_OPERATOR);
        return in;
        
    } else if(*in == '\0' || *in == '\n' || *in == '\r') {
        
        if(leftElement == NULL && getLastOperator() == NULL && findParentCompilerState(LATC_RETURN) == 0) {
            /* Variable by itself with nothing to do! */
#ifdef LAT_DEBUG
            setCompileError("A variable by itself is not a valid statement!", in, originalInput);
#else
            setCompileError();
#endif
            
        }
        
        if(getLastOperator() == NULL || findParentCompilerState(LATC_RETURN)) {
            /* Push what we have */
            pushOperand(getCapturedInput(in), OP_VAR, 0);
            
        }
        
        popCompilerState();
        return in;
        
    } else if(isConditionalOperator(in)) {
        pushOperand(getCapturedInput(in), OP_VAR, 0);
        popCompilerState();
        return in;
        
    } else if((*in == '}' ||  *in == ',' || *in == ':') && getPriorCompilerState() == LATC_ARRAY_INIT) {
        /* Part of an array initializer */
        pushOperand(getCapturedInput(in), OP_VAR, 0);
        popCompilerState();
        return in;
        
    } else if(*in == ']' && getPriorCompilerState() == LATC_VAL_ARRAY_C) {
        /* End of an array, pop back! */
        pushOperand(getCapturedInput(in), OP_VAR, 2);
        popCompilerState();
        return in;
        
    } else if(*in == '#' && *(in+1) && *(in+1) == '#') {
        /* Start of inlang execution (write left hand value to batched queue) */
        
        /* Set Batched */
        setWriteMode(WRITE_MODE_BATCHED);
        
        /* Write assignment to register 0 */
        writeOutCode((char)OP_SET);
        writeOutCharacters(getCapturedInput(in));
        writeOutCode((char)1);
        
        /* Set normal write */
        setWriteMode(WRITE_MODE_NORMAL);
        
        in+=2;
        
        if(*in) {
#ifdef LAT_DEBUG
            setCompileError(":>> Syntax Error. You may not follow an xlang declaration with anything other than a line break. Use the following lines to put xlang data in.", in, originalInput);
#else
            setCompileError();
#endif
            
        }
        
        popCompilerState();
        pushCompilerState(LATC_EXEC_END);
        
        /* Write out an exec op code */
        writeOutCode((char)OP_EXEC);
        
        return in;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized left value provided",in, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

/* LATC_EQUALS */
char *analyzeEquals(char *in) {
    
    #ifdef LAT_DEBUG
    printf("Analyze Equals:\n%s\n\n",in);
    #endif
    
    /* left hand side expressions are always variables, and as such simply need be recorded as a string value */
    assignLeftHandValue(in);
    popCompilerState();
    pushCompilerState(LATC_ASSIGN);
    pushCompilerState(LATC_RVAL_START);
    ++in;
    
    in = analyzeRValueStart(in);
    
    return in;
}

/* LATC_RVAL_START */
char *analyzeRValueStart(char *in) {
    
    #ifdef LAT_DEBUG
    printf("Analyze Right Value Start:\n%s\n\n",in);
    #endif
    
    if(isdigit(*in) || *in == '-' || *in == '+') {
        resetCapturedInput(in);
        if(*in == '-' || *in == '+') {
            in++;
        }
        if(getPriorCompilerState() != LATC_RETURN) {
            popCompilerState();
            pushCompilerState(ASSIGN_NUM);
        } else {
            popCompilerState();
        }
        pushCompilerState(LATC_RPOST);
        pushCompilerState(LATC_NUM);
        return in;
        
    } else if(isalpha(*in) || *in == '_') {
        resetCapturedInput(in);
        if(getPriorCompilerState() != LATC_RETURN) {
            popCompilerState();
            pushCompilerState(ASSIGN_VAR);
        } else {
            popCompilerState();
        }
        pushCompilerState(LATC_RPOST);
        pushCompilerState(LATC_VAL);
        return in;
        
    } else if(isWhitespace(*in) == 1) {
        /* continue */
        return ++in;
      
    } else if(*in == '"') {
        /* Double quote rval */
        if(getPriorCompilerState() != LATC_RETURN) {
            popCompilerState();
            pushCompilerState(ASSIGN_STR);
        } else {
            popCompilerState();
        }
        pushCompilerState(LATC_RPOST);
        pushCompilerState(LATC_DQUOTE);
        resetCapturedInput(in+1);
        return ++in;
        
    } else if(*in == '\'') {
        /* Single quote rval */
        if(getPriorCompilerState() != LATC_RETURN) {
            popCompilerState();
            pushCompilerState(ASSIGN_STR);
        } else {
            popCompilerState();
        }
        pushCompilerState(LATC_RPOST);
        pushCompilerState(LATC_SQUOTE);
        resetCapturedInput(in+1);
        return ++in;
        
    } else if(*in == '(') {
        /* Starting paren (for compound expressions) */
        if(getPriorCompilerState() != LATC_RETURN) {
            popCompilerState();
            pushCompilerState(ASSIGN_GENERIC);
        } else {
            popCompilerState();
        }
        pushCompilerState(LATC_RPOST);
        pushCompilerState(LATC_OPERATOR);
        return in;
        
    } else if(*in == '{') {
        /* Array Initializer! (Not followed by anything else) */
        popCompilerState();
        if(getCompilerState() == LATC_ASSIGN) {
            /* Strip an assign state, it's redundant at this point */
            popCompilerState();
        }
        pushCompilerState(LATC_DONE);
        pushCompilerState(LATC_ARRAY_INIT);
        resetCapturedInput(++in);
        return in;
        
    } else if(!*in && getPriorCompilerState() == LATC_RETURN) {
        popCompilerState();
        popCompilerState();
        /* Push up empty return state */
        pushCompilerState(LATC_RETURN_NONE);
        return in;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized right value provided",in, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

/* LATC_RNUM, For a numeric value on the right */
char *analyzeNum(char *in) {
    
    #ifdef LAT_DEBUG
    printf("Analyzing Number:\n%s\n\n",in);
    #endif
    
    if(isalpha(*in)) {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized digit",in, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    } else if(isdigit(*in) || *in == '.') {
        /* continue */
        return ++in;
        
    } else if (*in == '\0' || *in == '\n' || *in == '\r') {
        
        popCompilerState();
        
        if(*(in-1) != ')') {
            /* line terminated */
            pushOperand(getCapturedInput(in), OP_NUM, 0);
            
        }
        return in;
        
    } else if(isWhitespace(*in) == 1) {
        /* continue */
        return ++in;
        
    } else if(isConditionalOperator(in)) {
        /* Pop Back */
        pushOperand(getCapturedInput(in), OP_NUM, 0);
        popCompilerState();
        return in;
        
    } else if(*in == ')' && (getPriorCompilerState() == LATC_CALL)) {
        /* Pop to be handled by prior state */
        popCompilerState();
        return in;
        
    } else if(*in == ')' && (getPriorCompilerState() == LATC_IF || getPriorCompilerState() == LATC_ELSEIF) && relationalOperatorParenCount == 0 && operatorParenCount == 0) {
        /* Pop Back */
        pushOperand(getCapturedInput(in), OP_NUM, 0);
        popCompilerState();
        return in;
        
    } else if(*in == ')' && (findParentCompilerState(LATC_FOR_INCREMENT) == 1 || getPriorCompilerState() == LATC_WHILE)) {
        /* Pop Back */
        pushOperand(getCapturedInput(in), OP_NUM, 0);
        popCompilerState();
        return in;
        
    } else if(isArithmeticOperator(*in) == 1) {
        /* Convert to an analyzer */
        if(isalnum(*(in-1))) {
            pushOperand(getCapturedInput(in), OP_NUM, 0);
        }
        popCompilerState();
        pushCompilerState(LATC_OPERATOR);
        return in;
        
    } else if((*in == '}' || *in == ',' || *in == ':') && getPriorCompilerState() == LATC_ARRAY_INIT) {
        /* Array init state, just pop */
        pushOperand(getCapturedInput(in), OP_NUM, 0);
        popCompilerState();
        return in;
        
    } else if(*in == ',') {
        /* Take a chance! */
        if(findParentCompilerState(LATC_FOR_DECLARATION) == 1 || findParentCompilerState(LATC_FOR_LIMIT) == 1) {
            /* Push up for for declaration */
            pushOperand(getCapturedInput(in), OP_NUM, 0);
            
        }
        popCompilerState();
        return in;
        
    } else if(*in == '}') {
        /* Pop Back */
        pushOperand(getCapturedInput(in), OP_NUM, 0);
        popCompilerState();
        return in;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognied right number",in, originalInput);
#else
        setCompileError();
#endif
        return NULL;
    }
}

/* LATC_LPOST, For after reading a name on the left */
char *analyzeLPost(char *in) {
    
#ifdef LAT_DEBUG
    printf("Analyze Left Post:\n%s\n\n",in);
#endif
    if(*in == '[') {
        pushOperand(getCapturedInput(in), OP_VAR, 0);
        pushCompilerState(LATC_VAL_ARRAY);
        return ++in;
        
    } else if(*in == '(') {
        pushCompilerState(LATC_CALL);
        return ++in;
        
    } else if(*in == '=') {
        /* assignment */
        popCompilerState();
        pushCompilerState(LATC_EQUALS);
        return in;
        
    } else if(isWhitespace(*in) == 1) {
        /* continue */
        return ++in;
        
    } else if(*in == ')' && getPriorCompilerState() == LATC_CALL) {
        /* In function call, pop state */
        popCompilerState();
        return in;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized character after left name value",in, originalInput);
#else
        setCompileError();
#endif
        return NULL;
    }
}

/* For after reading anything new on the right */
char *analyzeRPost(char *in) {
    
    #ifdef LAT_DEBUG
    printf("Analyze Right Post Value:\n%s\n\n",in);
    #endif
    
    if(isWhitespace(*in) == 1) {
        /* continue */
        return ++in;
        
    } else if(*in == '\n' || *in == '\r' || *in == '\0') {
        /* All done, time to write this out */
        if(getLastOperator() != NULL && getPriorCompilerState() != LATC_RETURN) {
            /* We have at least one operator */
            popCompilerState();
            popCompilerState();
            pushCompilerState(ASSIGN_GENERIC);
            
        } else {
            /* Single value, assign to last state */
            popCompilerState();
            
        }
        
        if(*in == '\n' || *in == '\r') {
            return ++in;
        } else {
            return in;
        }
        
    } else if(*in == ')' && findParentCompilerState(LATC_FOR_INCREMENT) == 1) {
        /* Dispatch */
        dispatchInstructionsFromExpression(0);
        /* Pop Back */
        popCompilerState();
        return in;
        
    } else if(isArithmeticOperator(*in) == 1) {
        /* More to do */
        if(*in == '+' || *in == '-') {
            if(isArithmeticOperator(*(in-1)) == 1 && *(in-1) != ')') {
                /* It's a sign, thus a number! */
                resetCapturedInput(in);
                pushCompilerState(LATC_NUM);
                return ++in;
            }
        }
        pushCompilerState(LATC_OPERATOR);
        return in;
        
    } else if(isdigit(*in)) {
        /* A number */
        resetCapturedInput(in);
        pushCompilerState(LATC_NUM);
        return in;
        
    } else if(isalpha(*in) || *in == '_') {
        /* A var or function call */
        resetCapturedInput(in);
        pushCompilerState(LATC_VAL);
        return in;
        
    } else if(*in == '\'') {
        /* Single quotes */
        pushCompilerState(LATC_SQUOTE);
        resetCapturedInput(in+1);
        return ++in;
        
    } else if(*in == '"') {
        /* Double Quotes */
        pushCompilerState(LATC_DQUOTE);
        resetCapturedInput(in+1);
        return ++in;
        
    } else if(*in == ',') {
        /* Take a chance! */
        popCompilerState();
        return in;
        
    } else if(*in == '}') {
        /* Pop Back */
        if(getLastOperator() != NULL) {
            dispatchInstructionsFromExpression(0);
        }
        popCompilerState();
        return in;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized symbol after having read a right value",in, originalInput);
#else
        setCompileError();
#endif
        return NULL;
    }
}

/* Analyze an lval array, x['val']... */
char *analyzeValArray(char *in) {
    
    #ifdef LAT_DEBUG
    printf("Analyze Array Reference:\n%s\n\n",in);
    #endif
    
    if(isWhitespace(*in) == 1) {
        /* skip */
        return ++in;
        
    } else if(*in == '\'') {
        popCompilerState();
        pushCompilerState(LATC_VAL_ARRAY_C);
        pushCompilerState(LATC_SQUOTE);
        pushOperator(OP_PARENO, 0);
        resetCapturedInput(++in);
        return in;
        
    } else if(*in == '"') {
        popCompilerState();
        pushCompilerState(LATC_VAL_ARRAY_C);
        pushCompilerState(LATC_DQUOTE);
        pushOperator(OP_PARENO, 0);
        resetCapturedInput(++in);
        return in;
        
    } else if(isArithmeticOperator(*in) == 1) {
        /* More to do */
        pushCompilerState(LATC_OPERATOR);
        pushOperator(OP_PARENO, 0);
        return in;
        
    } else if(isalnum(*in) ||  *in == '_') {
        /* Continue? */
        popCompilerState();
        pushCompilerState(LATC_VAL_ARRAY_C);
        pushOperator(OP_PARENO, 0);
        if(isdigit(*in)) {
            pushCompilerState(LATC_NUM);
        } else {
            pushCompilerState(LATC_VAL);
        }
        return in;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized character used in array reference",in, originalInput);
#else
        setCompileError();
#endif
        return NULL;
    }
}

/* Analyze Left hand side array close ']' */
char *analyzeValArrayClose(char *in) {
    
    #ifdef LAT_DEBUG
    printf("Analyze Left Array Close:\n%s\n\n",in);
    #endif
    
    if(isWhitespace(*in) == 1) {
        /* continue */
        return ++in;
        
    } else if(*in == ']') {
        
        if(getLastOperator()->op == OP_PARENO) {
            /* Pop and repush last as our key (NOTE No free, strdup has some sort of optimization where it's simply recycling our value, pretty nifty too) */
            getLastOperand()->isArrayOp = 2;
            
        } else {
            /* Dispatch up to our paren */
            dispatchInstructionsFromExpression(0);
            /* Preserve this value on the stack */
            writeOutCode((char)OP_PUSH);
            writeOutCode((char)getLastRegisterUsedAtIndex(0));
            
        }
        
        /* Pop our blocking paren */
        popOperator();
        
        popCompilerState();
        
        /* See if we're done (for left hand assignment to array values) */
        if(getCompilerState() == LATC_DONE) {
            /* No further states, push up generic assign and equals */
            pushCompilerState(ASSIGN_GENERIC);
            pushCompilerState(LATC_EQUALS);
            
        }
        
        resetCapturedInput(++in);
        return in;
        
    } else if(isArithmeticOperator(*in) == 1) {
        /* Is artihmetic operator */
        pushCompilerState(LATC_OPERATOR);
        return in;
        
    } else if(*in == '\'') {
        /* Single Quote */
        pushCompilerState(LATC_SQUOTE);
        resetCapturedInput(++in);
        return in;
        
    } else if(*in == '"') {
        /* Double Quote */
        pushCompilerState(LATC_DQUOTE);
        resetCapturedInput(++in);
        return in;
        
    } else {
        /* Error */
#ifdef LAT_DEBUG
        setCompileError("Found another symbol than expected array reference closer",in, originalInput);
#else
        setCompileError();
#endif
        return NULL;
    }
}

/* Analyze Function Call on either side of an equation */
char *analyzeFunctionCall(char *in) {
    
    #ifdef LAT_DEBUG
    printf("Analyzing Function Call:\n%s\n\n",in);
    #endif
    
    if(*in == ',') {
        /* Param Break, dispatch EVERYTHING we have into a 'push' */
        in = analyzeFunctionCallArg(in);
        resetCapturedInput(++in);
        return in;
        
    } else if(*in == ')') {
        char *funcName;
        unsigned short callCode;
        char holder = *in;
        *in = '\0';
        /* Closing paren, dispatch everything we have into a PUSH */
        
        /* Validate we actually had any args */
        if(*(in-1) != '(') {
            /* Capture our last arg */
            in = analyzeFunctionCallArg(in);
            *in = holder;
            resetCapturedInput(++in);
        }
        
        if(getPriorCompilerState() != LATC_RPOST) {
            popCompilerState();
            
            /* Check if we have a closing bracket tagging along */
            if(*in != '}') {
                /* Push Done */
                pushCompilerState(LATC_DONE);
            }
            pushCompilerState(LATC_RPOST);
            
        } else {
            popCompilerState();
            
        }
        
        funcName = popFuncName();
        
        if((callCode = fetchSystemFunctionCall(funcName)) == 0) {
            writeOutCode((char)OP_CALL);
            /* Push Completed Function Call name */
            writeOutCharacters(funcName);
            
        } else {
            /* Make a system function call, name on its own */
            writeOutCode((char)callCode);
            
        }
        
        /* free func name */
        free(funcName);
        
        return in;
        
    } else if(isWhitespace(*in) == 1) {
        /* Just whitespace */
        return ++in;
    
    } else if(isdigit(*in)) {
        /* Number of some sort */
        pushCompilerState(LATC_NUM);
        return in;
        
    } else if(isalnum(*in) || *in == '_') {
        /* Value of some sort */
        pushCompilerState(LATC_VAL);
        return in;
        
    } else if(isArithmeticOperator(*in) == 1) {
        /* Is artihmetic operator */
        pushCompilerState(LATC_OPERATOR);
        return in;
        
    } else if(*in == '\'') {
        /* Single quoted */
        pushCompilerState(LATC_SQUOTE);
        resetCapturedInput(++in);
        return in;
        
    } else if(*in == '"') {
        /* Double Quoted */
        pushCompilerState(LATC_DQUOTE);
        resetCapturedInput(++in);
        return in;
        
    } else if(*in == '[') {
        /* Beginnings of array item */
        pushCompilerState(LATC_VAL_ARRAY);
        return ++in;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized item in function call",in, originalInput);
#else
        setCompileError();
#endif
        
    }
    
    return ++in;
}

/* Uses LAST register action to create a PUSH on the stack for whatever we just read... */
char *analyzeFunctionCallArg(char *i) {
    
    char *capIn = getCapturedInput(i);
    
    #ifdef LAT_DEBUG
    printf("Analyzing Function Call Arg : %s\n\n",i);
    #endif
    
    /* Check for captures other than string, which are already pushed */
    if(*capIn) {
        /* Check to extract a num or var */
        if(isNumeric(capIn)) {
            pushOperand(getCapturedInput(i), OP_NUM, 0);
        } else {
            pushOperand(getCapturedInput(i), OP_VAR, 0);
        }
        
    }
    
    if(getLastOperator() != NULL) {
        /* Dispatch */
        dispatchInstructionsFromExpression(0);
        
    } else if(getOperandCount() == 1) {
        /* An array value was passed */
        dispatchInstructionsFromExpression(0);
        
    } else {
        /* Manual register assignment */
        ExpressionElement *ee = popOperand();
        
        if(ee != NULL) {
            /* write out register (not really string, misnomer) */
            writeOutRegisterStrAssign(2, ee->precedence, ee->element);
            /* push it onto our last used stack */
            pushLastRegisterUsed(2);
            free(ee->element);
            
        } else {
            /* Empty string args provided, push as empty string */
            writeOutRegisterStrAssign(2, OP_STRING, "");
            pushLastRegisterUsed(2);
            
        }
        
    }
    
    /* Write a PUSH for this last used register item now */
    writeOutCode((char)OP_PUSH);
    writeOutCode((char)getLastRegisterUsedAtIndex(0));
    
    resetCapturedInput(i);
    
    return i;
}

/* Analyzes an array initialier. Is ALWAYS on its own, never followed by anything */
char *analyzeArrayInit(char *i) {
    
    ExpressionElement *eValue=NULL,*eKey=NULL;
    
    #ifdef LAT_DEBUG
    printf("Analyzing Array Init : %s\n\n", i);
    #endif
    
    if(*i == ':') {
        
        if(getLastOperator() != NULL) {
            /* Dispatch! */
            dispatchInstructionsFromExpression(0);
            
            /* PUSH the result */
            writeOutCode((char)OP_PUSH);
            writeOutCode((char)0);
        }
        
        resetCapturedInput(++i);
        return i;
        
    } else if(*i == ',') {
        
        /* Fetch both operands, and write an ARRAY_SET */
        
        if(getLastOperator() != NULL) {
            /* Dispatch! */
            dispatchInstructionsFromExpression(0);
            
            /* Forcibly reassign register 0 to register 2 */
            writeOutCode((char)OP_RGI);
            writeOutCode((char)2);
            writeOutCode((char)OP_RGI);
            writeOutCode((char)0);
        }
        
        /* Value */
        eValue = popOperand();
        /* Key */
        eKey = popOperand();
        
        if(eKey == NULL) {
            writeOutCode((char)OP_RGI);
            writeOutCode((char)0);
            writeOutCode((char)OP_POP);
        }
        
        /* If not null, set to 2, otherwise we have already set to register 2 above */
        if(eValue != NULL) {
            /* Set value to register 2 */
            writeOutCode((char)OP_RGI);
            writeOutCode((char)2);
            /* Type */
            writeOutCode((char)eValue->precedence);
            writeOutCharacters(eValue->element);
        }
        
        writeOutCode((char)OP_ARRAY_SET);
        writeOutCharacters(leftElement);
        /* Either assign to chars or to an address */
        eKey != NULL ? writeOutCharacters(eKey->element) : writeOutCode((char)0);
        /* Assign from register 2 */
        writeOutCode((char)2);
        
        if(eValue != NULL) {
            free(eValue->element);
        }
        if(eKey != NULL) {
            free(eKey->element);
        }
        
        /* Reset capture and return */
        resetCapturedInput(++i);
        return i;
    
    } else if(*i == '}') {
        
        /* Fetch both operands, and write an ARRAY_SET */
        
        if(getLastOperator() != NULL) {
            /* Dispatch! */
            dispatchInstructionsFromExpression(0);
            
            /* Forcibly reassign register 0 to register 2 */
            writeOutCode((char)OP_RGI);
            writeOutCode((char)2);
            writeOutCode((char)OP_RGI);
            writeOutCode((char)0);
        }
        
        /* Value */
        eValue = popOperand();
        /* Key */
        eKey = popOperand();
        
        /* If not null, set to 2, otherwise we have already set to register 2 above */
        if(eValue != NULL) {
            /* Set value to register 2 */
            writeOutCode((char)OP_RGI);
            writeOutCode((char)2);
            /* Type */
            writeOutCode((char)eValue->precedence);
            writeOutCharacters(eValue->element);
        }
        
        if(eKey == NULL) {
            writeOutCode((char)OP_RGI);
            writeOutCode((char)0);
            writeOutCode((char)OP_POP);
        }
        
        writeOutCode((char)OP_ARRAY_SET);
        writeOutCharacters(leftElement);
        /* Either assign to chars or to an address */
        eKey != NULL ? writeOutCharacters(eKey->element) : writeOutCode((char)0);
        /* Assign from register 2 */
        writeOutCode((char)2);
        
        if(eValue != NULL) {
            free(eValue->element);
        }
        if(eKey != NULL) {
            free(eKey->element);
        }
        
        /* Reset capture, pop state, return */
        resetCapturedInput(++i);
        popCompilerState();
        return i;
        
    } else if(isArithmeticOperator(*i) == 1) {
        /* More to do */
        if(*i == '+' || *i == '-') {
            if(isArithmeticOperator(*(i-1)) == 1 && *(i-1) != '(' && *(i-1) != ')') {
                /* Skip */
                return ++i;
            }
        }
        pushCompilerState(LATC_OPERATOR);
        return i;
        
    } else if(isdigit(*i)) {
        /* A number */
        resetCapturedInput(i);
        pushCompilerState(LATC_NUM);
        return i;
        
    } else if(isalpha(*i) || *i == '_') {
        /* A var or function call */
        resetCapturedInput(i);
        pushCompilerState(LATC_VAL);
        return i;
        
    } else if(*i == '\'') {
        /* Single quotes */
        pushCompilerState(LATC_SQUOTE);
        resetCapturedInput(++i);
        return i;
        
    } else if(*i == '"') {
        /* Double Quotes */
        pushCompilerState(LATC_DQUOTE);
        resetCapturedInput(++i);
        return i;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized symbol read inside array initializer",i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
    }
}

/* Analyzes a function definition name */
char *analyzeFunctionDefinitionName(char *i) {
    
#ifdef LAT_DEBUG
    printf("Analyzing Function Def Name : %s\n\n", i);
#endif
    
    if(*i == '(') {
        /* Write out the intial declaration */
        writeOutCode((char)OP_FUNC);
        writeOutCharacters(getCapturedInput(i));
        
        /* Write out command to push a new stack */
        writeOutCode((char)OP_PUSH_STACK);
        
        resetCapturedInput(++i);
        
        /* Pop this state and push deffunc args */
        popCompilerState();
        pushCompilerState(LATC_DEFFUNC_ARGS);
        return i;
        
    } else if(isalnum(*i)) {
        /* continue */
        return ++i;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized character in function definition name", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

/* Analyzes the arguments provided to a function definition name */
char *analyzeFunctionDefinitionArgs(char *i) {
    
#ifdef LAT_DEBUG
    printf("Analyzing Def Args : %s\n\n", i);
#endif
    
    if(isalpha(*i)) {
        /* Value of some sort */
        pushCompilerState(LATC_VAL);
        return i;
        
    } else if(*i == ',') {
        /* acceptable median character */
        pushOperand(getCapturedInput(i), OP_VAR, 0);
        resetCapturedInput(++i);
        return i;
        
    } else if(*i == ')') {
        
        ExpressionElement *ee;
        
        if(*(i-1) != '(') {
            /* write out final arg as well */
            pushOperand(getCapturedInput(i), OP_VAR, 0);
            
        }
        
        /* End of declaration! Write out all argument sets from last to first to match order they are popped off */
        while((ee = popOperand()) != NULL) {
            /* Pop to register */
            writeOutCode((char)OP_RGI);
            writeOutCode((char)2);
            writeOutCode((char)OP_POP);
            
            /* Set operand value to register */
            writeOutCode((char)OP_SET);
            writeOutCharacters(ee->element);
            writeOutCode((char)2);
            
            /* free */
            free(ee->element);
        }
        
        /* Indicate we now need an opening { to continue */
        popCompilerState();
        pushCompilerState(LATC_DEFFUNC_START);
        
        return ++i;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized character in function definition arguments", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

char *analyzeFunctionDefinitionStart(char *i) {
    
#ifdef LAT_DEBUG
    printf("Analyzing Function Def Start : %s\n\n",i);
#endif
    
    if(*i == '{') {
        /* Entering function body, push a state to indicate we need to close this function at some point and continue with a start code */
        popCompilerState();
        pushCompilerState(LATC_DEFFUNC_END);
        if(*(i+1)) {
            pushCompilerState(LATC_START);
            
        } else {
            pushCompilerState(LATC_DONE);
            
        }
        
        return ++i;
        
    } else if(*i == '\0') {
        /* continue across lines */
        shouldBreak = 1;
        return i;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized character found instead of expected '{' for starting function body", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

char *analyzeFunctionDefinitionEnd(char *i) {
    
    #ifdef LAT_DEBUG
    printf("Analyzing Function Def End : %s\n\n",i);
    #endif
    
    if(*i == '}') {
        
        /* Write command to pop the current stack */
        writeOutCode((char)OP_POP_STACK);
        
        /* Wrap up the current function */
        writeOutCode((char)OP_FUNC_END);
        popCompilerState();
        
        if(*(i+1)) {
            pushCompilerState(LATC_START);
            
        } else {
            pushCompilerState(LATC_DONE);
            
        }
        
        return ++i;
        
    } else if(*i == '\0') {
        /* continue across lines */
        shouldBreak = 1;
        return i;
        
    } else {
        /* Did not read end yet, just another line */
        pushCompilerState(LATC_START);
        return i;
        
    }
}

/* Analyzes and pushes up a return value if we have any */
char *analyzeReturn(char *i) {
    
    #ifdef LAT_DEBUG
    printf("Analyzing Return: %s\n\n",i);
    #endif
    
    #pragma message("if this starts to build up (like for array refs) just export this functionality into another function, dub it condenseValuesToRegister, effectively takes any number of values and combinations and reduces it to the result at the last set register address")
    
    if(getLastOperator() != NULL) {
        dispatchInstructionsFromExpression(0);
        
    } else if(getLastOperand() != NULL) {
        /* Set and return solo value */
        ExpressionElement *ee = popOperand();
        writeOutCode((char)OP_RGI);
        writeOutCode((char)2);
        writeOutCode((char)ee->precedence);
        writeOutCharacters(ee->element);
        
        pushLastRegisterUsed(2);
        
        free(ee->element);
        
    }
    
    /* Get last register used, and return */
    writeOutCode((char)OP_RETURN);
    writeOutCode((char)getLastRegisterUsedAtIndex(0));
    
    popCompilerState();
    return i;
    
}

/* Analyzes an empty return statement, just puts up an appropriate op code */
char *analyzeReturnNone(char *i) {
    writeOutCode((char)OP_RETURN_NONE);
    popCompilerState();
    return i;
    
}

/* Performs an assignment to either a variable or an array ref */
char *analyzeAssignment(char *i) {
    
    #ifdef LAT_DEBUG
    printf("Analyzing Assignment: %s\n\n",i);
    #endif
    
    if(!*leftElement) {
        /* This is a single assign array ref */
        ExpressionElement *keyHolder = popOperand();
        ExpressionElement *captureHolder = popOperand();
        
        if(captureHolder == NULL) {
            /* Our key was precomputed, load it from a register on our pseudo stack */
            writeOutCode((char)OP_RGI);
            writeOutCode((char)2);
            writeOutCode((char)OP_POP);
            
            /* Var name (not key */
            writeOutCode((char)OP_ARRAY_SET);
            writeOutCharacters(keyHolder->element);
            free(keyHolder->element);
            
            /* Key (now at address 0x02) */
            writeOutCode((char)2);
            
        } else {
            /* Standard Var Name */
            writeOutCode((char)OP_ARRAY_SET);
            writeOutCharacters(captureHolder->element);
            free(captureHolder->element);
            
            /* Key */
            writeOutCharacters(keyHolder->element);
            free(keyHolder->element);
            
        }
        
        /* write out what we are assigning from */
        writeOutCode((char)getLastRegisterUsedAtIndex(0));
        
    } else {
        /* Write out var assign */
        writeOutCode((char)OP_SET);
        writeOutCharacters(leftElement);
        freeLeftHandValue();
        writeOutCode((char)0);
        
    }
    
    if(!*i) {
        popCompilerState();
        pushCompilerState(LATC_DONE);
    
    } else if(*i == ',' || (*i == ')' && findParentCompilerState(LATC_FOR_INCREMENT) == 1)) {
        popCompilerState();
        
    } else {
        popCompilerState();
        
    }
    
    return i;
}

/* Analyzes an if statment */
char *analyzeIf(char *i) {
    
    #ifdef LAT_DEBUG
    printf("Analyzing If: %s\n\n", i);
    #endif
    
    if(isdigit(*i)) {
        /* Analyze this number */
        resetCapturedInput(i);
        pushCompilerState(LATC_NUM);
        return i;
        
    } else if(*i == '\'') {
        /* Single Quoted */
        pushCompilerState(LATC_SQUOTE);
        resetCapturedInput(++i);
        return i;

    } else if(*i == '"') {
        /* Double Quoted */
        pushCompilerState(LATC_DQUOTE);
        resetCapturedInput(++i);
        return i;
        
    } else if(isalpha(*i)) {
        /* Analyze this variable/bool/function name */
        resetCapturedInput(i);
        pushCompilerState(LATC_VAL);
        return i;
        
    } else if(*i == ')' && relationalOperatorParenCount == 0 && operatorParenCount == 0) {
        /* Closing paren for 'if' */
        if(getLastOperator() != NULL) {
            dispatchInstructionsFromExpression(0);
        }
        
        /* (save this for later, solo true)
         dispatchRelationalExpression(OP_EQUAL);
         
        // Shorthand TRUE condition, push operator, push operand
        pushRelationalOperator(OP_EQUAL, 0);
        pushOperand("true", OP_TRUE, 0);
        
        resetCapturedInput(i);
        */
        
        /* Dispatch the prior expression */
        dispatchRelationalExpression(0);
        
        popCompilerState();
        
        /* At this point we are at the END of this if statement, push up an expectant START segment for this if */
        pushCompilerState(LATC_IF_START);
        
        return ++i;
        
    } else if(isConditionalOperator(i)) {
        
        unsigned char conditionalOpCode = getConditionalOPCode(i);
        
        /* Dispatch the prior expression */
        dispatchRelationalExpression(conditionalOpCode);
        
        /* Push Conditional op, get conditional op and push as an operator to work with */
        pushRelationalOperator(conditionalOpCode, 0);
        
        i++;
        if(*i == '=' || *i == '&' || *i == '|') {
            ++i;
        }
        
        return i;
        
    } else if(*i == '!') {
        /* Shorthand FALSE condition, push operand */
        pushOperand("false", OP_FALSE, 0);
        /* Dispatch the prior expression */
        dispatchRelationalExpression(OP_EQUAL);
        /* Push Conditional op, get conditional op and push as an operator to work with */
        pushRelationalOperator(OP_EQUAL, 0);
        resetCapturedInput(++i);
        return i;
        
    } else if(*i == '(') {
        /* Grouping paren, discard */
        relationalOperatorParenCount++;
        return ++i;
        
    } else if(*i == ')') {
        /* Closing group paren, dispatch and decrement */
        relationalOperatorParenCount--;
        return ++i;
        
    } else {
        /* Error */
#ifdef LAT_DEBUG
        setCompileError("Unrecognized input to analyzing an if statement", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

/* Looks for the starting bracket to an if body */
char *analyzeIfStart(char *i) {
    
    #ifdef LAT_DEBUG
    printf("Analyzing If Start : %s\n\n",i);
    #endif
    
    if(*i == '{') {
        /* Entering if body, place our OR jump */
        JumpInstance *ji = popJumpStart();
        if(ji != NULL) {
            pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
            
        }
        
        popCompilerState();
        /* Push close mark */
        pushCompilerState(LATC_IF_END);
        
        if(*(i+1)) {
            pushCompilerState(LATC_START);
            
        } else {
            pushCompilerState(LATC_DONE);
            
        }
        
        return ++i;
        
    } else if(*i == '\0') {
        /* continue across lines */
        shouldBreak = 1;
        return i;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized character found instead of expected '{' for starting if body", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
    
}

/* Analyzes the end of an if body */
char *analyzeIfEnd(char *i) {
    
    #ifdef LAT_DEBUG
    printf("Analyzing If End : %s\n\n",i);
    #endif
    
    if(*i == '}') {
        popCompilerState();
        pushCompilerState(LATC_IF_POST);
        
        return ++i;
        
    } else if(*i == '\0') {
        /* continue across lines */
        shouldBreak = 1;
        return i;
        
    } else {
        /* Did not read end yet, just another line */
        pushCompilerState(LATC_START);
        return i;
        
    }
}

/* Analyzes if 'post' */
char *analyzeIfPost(char *i) {
    
    if(strlen(i) >= 6 && *i == 'e' && *(i+1) == 'l' && *(i+2) == 's' && *(i+3) == 'e' && *(i+4) == 'i' && *(i+5) == 'f') {
        /* ELSEIF */
        
        handleControlFlowTransfer(0);
        
        i+=7;
        resetCapturedInput(i);
        popCompilerState();
        
        /* increment the grouping num of all following jumps */
        incrementJumpGroup();
        pushCompilerState(LATC_ELSEIF);
        
        return i;
        
    } else if(strlen(i) >= 4 && *i == 'e' && *(i+1) == 'l' && *(i+2) == 's' && *(i+3) == 'e') {
        /* ELSE */
        handleControlFlowTransfer(0);
        
        i+=4;
        resetCapturedInput(i);
        popCompilerState();
        pushCompilerState(LATC_ELSE_START);
        
        return i;
        
    } else {
        /* Check following lines */
        if(*i) {
            
            /* Done, place our final jump to exit out via */
            JumpInstance *ji = popJumpType();
            pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
            
            /* Continue jump from prior failure, from here we have nothing more to do */
            /*
             ji = popJumpType();
             pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
             */
            
            /* pop */
            popCompilerState();
            
            /* Content, pop everything and setup start! */
            pushCompilerState(LATC_START);
            
        } else {
            /* No content, continue... */
            shouldBreak = 1;
            
        }
        
        return i;
    }
}

char *analyzeElseIf(char *i) {
#ifdef LAT_DEBUG
    printf("Analyzing ElseIf: %s\n\n", i);
#endif
    
    if(isdigit(*i)) {
        /* Analyze this number */
        resetCapturedInput(i);
        pushCompilerState(LATC_NUM);
        return i;
        
    } else if(*i == '\'') {
        /* Single Quoted */
        pushCompilerState(LATC_SQUOTE);
        resetCapturedInput(++i);
        return i;
        
    } else if(*i == '"') {
        /* Double Quoted */
        pushCompilerState(LATC_DQUOTE);
        resetCapturedInput(++i);
        return i;
        
    } else if(isalpha(*i)) {
        /* Analyze this variable/bool/function name */
        resetCapturedInput(i);
        pushCompilerState(LATC_VAL);
        return i;
        
    } else if(*i == ')' && relationalOperatorParenCount == 0 && operatorParenCount == 0) {
        /* Closing paren for 'else if' */
        if(getLastOperator() != NULL) {
            dispatchInstructionsFromExpression(0);
        }
        
        /* Dispatch the prior expression */
        dispatchRelationalExpression(0);
        
        popCompilerState();
        
        /* At this point we are at the END of this if statement, push up an expectant START segment for this if */
        pushCompilerState(LATC_ELSEIF_START);
        
        return ++i;
        
    } else if(isConditionalOperator(i)) {
        
        unsigned char conditionalOpCode = getConditionalOPCode(i);
        
        /* Dispatch the prior expression */
        dispatchRelationalExpression(conditionalOpCode);
        
        /* Push Conditional op, get conditional op and push as an operator to work with */
        pushRelationalOperator(conditionalOpCode, 0);
        
        i++;
        if(*i == '=' || *i == '&' || *i == '|') {
            ++i;
        }
        
        return i;
        
    } else if(*i == '(') {
        /* Grouping paren, discard */
        relationalOperatorParenCount++;
        return ++i;
        
    } else if(*i == ')') {
        /* Closing group paren, dispatch and decrement */
        relationalOperatorParenCount--;
        return ++i;
        
    } else {
        /* Error */
#ifdef LAT_DEBUG
        setCompileError("Unrecognized input to analyzing an else if statement", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

/* Analyze Else If Start */
char *analyzeElseIfStart(char *i) {
    #ifdef LAT_DEBUG
    printf("Analyzing ElseIf Start : %s\n\n",i);
    #endif
    
    if(*i == '{') {
        /* Entering else if body, push up a label to jumping OUT of this if we failed above */
        JumpInstance *ji = popJumpStart();
        if(ji != NULL) {
            pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
            
        }
        
        popCompilerState();
        /* Push close mark */
        pushCompilerState(LATC_ELSEIF_END);
        
        if(*(i+1)) {
            pushCompilerState(LATC_START);
            
        } else {
            pushCompilerState(LATC_DONE);
            
        }
        
        return ++i;
        
    } else if(*i == '\0') {
        /* continue across lines */
        shouldBreak = 1;
        return i;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized character found instead of expected '{' for starting else if body", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

/* Analyze Else If End Sequence */
char *analyzeElseIfEnd(char *i) {
    #ifdef LAT_DEBUG
    printf("Analyzing ElseIf End : %s\n\n",i);
    #endif
    
    if(*i == '}') {
        popCompilerState();
        pushCompilerState(LATC_ELSE_POST);
        
        return ++i;
        
    } else if(*i == '\0') {
        /* continue across lines */
        shouldBreak = 1;
        return i;
        
    } else {
        /* Did not read end yet, just another line */
        pushCompilerState(LATC_START);
        return i;
        
    }
}

/* Analyze Else If Post */
char *analyzeElseIfPost(char *i) {
    if(strlen(i) >= 6 && *i == 'e' && *(i+1) == 'l' && *(i+2) == 's' && *(i+3) == 'e' && *(i+4) == 'i' && *(i+5) == 'f') {
        /* ELSEIF */
        
        handleControlFlowTransfer(1);
        
        i+=7;
        resetCapturedInput(i);
        popCompilerState();
        pushCompilerState(LATC_ELSEIF);
        
        return i;
        
    } else if(strlen(i) >= 4 && *i == 'e' && *(i+1) == 'l' && *(i+2) == 's' && *(i+3) == 'e') {
        /* ELSE */
        
        handleControlFlowTransfer(1);
        
        i+=4;
        resetCapturedInput(i);
        popCompilerState();
        pushCompilerState(LATC_ELSE_START);
        
        return i;
        
    } else {
        
        /* Check following lines */
        if(*i) {
            
            /* Done, place our final jump to exit out via */
            JumpInstance *ji = popJumpType();
            pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
            
            /* Continue jump from prior failure, from here we have nothing more to do */
            ji = popJumpType();
            pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
            
            /* pop */
            popCompilerState();
            
            /* Content, pop everything and setup start! */
            pushCompilerState(LATC_START);
            
        } else {
            /* No content, continue... */
            shouldBreak = 1;
            
        }
        
        return i;
    }
    
}

/* Analyze Else Start */
char *analyzeElseStart(char *i) {
    if(*i == '{') {
        /* Entering else body */
        popCompilerState();
        /* Push close mark */
        pushCompilerState(LATC_ELSE_END);
        
        if(*(i+1)) {
            pushCompilerState(LATC_START);
            
        } else {
            pushCompilerState(LATC_DONE);
            
        }
        
        return ++i;
        
    } else if(*i == '\0') {
        /* continue across lines */
        shouldBreak = 1;
        return i;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized character found instead of expected '{' for starting else body", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

/* Analyze Else End */
char *analyzeElseEnd(char *i) {
    if(*i == '}') {
        
        /* Done, update the final exit jump */
        JumpInstance *ji = popJumpType();
        pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
        
        popCompilerState();
        
        if(*(i+1)) {
            pushCompilerState(LATC_START);
            
        } else {
            pushCompilerState(LATC_DONE);
            
        }
        
        return ++i;
        
    } else if(*i == '\0') {
        /* continue across lines */
        shouldBreak = 1;
        return i;
        
    } else {
        /* Did not read end yet, just another line */
        pushCompilerState(LATC_START);
        return i;
        
    }
    
}

/* Analyze For Loop */
char *analyzeForDeclaration(char *i) {
    if(*i == ',') {
        /* Shift to limit */
        popCompilerState();
        pushCompilerState(LATC_FOR_LIMIT);
        
        /* Push label to mark point to loop back on */
        pushIncompleteJump((unsigned int)getProgramByteCount(), JUMP_TYPE_JUMP);
        
        return ++i;
    
    } else if(isalpha(*i)) {
        pushCompilerState(LATC_VAL);
        return i;
        
    } else {
        /* Unrecognized for declaration element */
#ifdef LAT_DEBUG
        setCompileError("Unrecognized intialization statement in the passed for loop", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

/* Analyzes the limit component of a for loop */
char *analyzeForLimit(char *i) {
    if(*i == ',') {
        
        /* End of limit statement */
        if(getLastOperator() != NULL) {
            dispatchInstructionsFromExpression(0);
        }
        
        /* Dispatch the prior expression */
        dispatchRelationalExpression(0);
        
        /* Shift to incrementor */
        popCompilerState();
        resetCapturedInput(++i);
        pushCompilerState(LATC_FOR_INCREMENT);
        return i;
        
    } else if(isdigit(*i)) {
        /* Analyze this number */
        resetCapturedInput(i);
        pushCompilerState(LATC_NUM);
        return i;
        
    } else if(*i == '\'') {
        /* Single Quoted */
        pushCompilerState(LATC_SQUOTE);
        resetCapturedInput(++i);
        return i;
        
    } else if(*i == '"') {
        /* Double Quoted */
        pushCompilerState(LATC_DQUOTE);
        resetCapturedInput(++i);
        return i;
        
    } else if(isalpha(*i)) {
        /* Analyze this variable/bool/function name */
        resetCapturedInput(i);
        pushCompilerState(LATC_VAL);
        return i;
        
    } else if(isConditionalOperator(i)) {
        
        unsigned char conditionalOpCode = getConditionalOPCode(i);
        
        /* Dispatch the prior expression */
        dispatchRelationalExpression(conditionalOpCode);
        
        /* Push Conditional op, get conditional op and push as an operator to work with */
        pushRelationalOperator(conditionalOpCode, 0);
        
        i++;
        if(*i == '=' || *i == '&' || *i == '|') {
            ++i;
        }
        
        return i;
        
    } else if(*i == '(') {
        /* Grouping paren, discard */
        relationalOperatorParenCount++;
        return ++i;
        
    } else if(*i == ')') {
        /* Closing group paren, dispatch and decrement */
        relationalOperatorParenCount--;
        return ++i;
        
    } else {
        /* Error */
#ifdef LAT_DEBUG
        setCompileError("Unrecognized input to analyzing an 'for' limit statement", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

/* Analyzes the incrementing part of a for loop */
char *analyzeForIncrement(char *i) {
    if(*i == ')') {
        
        /* End of statements, pop and push a 'for' start state */
        popCompilerState();
        pushCompilerState(LATC_FOR_START);
        
        return ++i;
        
    } else if(isalpha(*i)) {
        /* Set write mode to batched */
        setWriteMode(WRITE_MODE_BATCHED);
        pushCompilerState(LATC_VAL);
        return i;
        
    } else if(isdigit(*i)) {
        /* Set write mode to batched */
        setWriteMode(WRITE_MODE_BATCHED);
        pushCompilerState(LATC_NUM);
        return i;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized input passed to increment statement in 'for' loop", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
    
}

/* Analyze 'For' Start */
char *analyzeForStart(char *i) {
    if(*i == '{') {
        /* Entering for body */
        
        /* Update a cond-jump for catching alternation instances */
        JumpInstance *ji = popJumpStart();
        if(ji != NULL) {
            pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
            
        }
        
        /* Push goto with exit label address */
        pushForGoto(getJumpNum(), readBatchedData(), (unsigned int)readBatchedDataSize());
        
        /* NULL the batched data reference */
        nullifyBatchedData();
        
        /* Turn write mode back to normal */
        setWriteMode(WRITE_MODE_NORMAL);
        
        popCompilerState();
        /* Push 'for' close mark */
        pushCompilerState(LATC_FOR_END);
        
        if(*(i+1)) {
            pushCompilerState(LATC_START);
            
        } else {
            pushCompilerState(LATC_DONE);
            
        }
        
        return ++i;
        
    } else if(*i == '\0') {
        /* continue across lines */
        shouldBreak = 1;
        return i;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized character found instead of expected '{' for starting for body", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

/* Analyze 'For' End */
char *analyzeForEnd(char *i) {
    if(*i == '}') {
        
        /* Get our increment statement */
        JumpInstance *forEnd = popJumpType();
        /* Get our exit statement */
        JumpInstance *exitLabel = popJumpType();
        /* Get our loopback statement */
        JumpInstance *loopBack = popJumpType();
        
        /* Write out the increment sequence */
        writeRAW(forEnd->finishingSequence, (int)forEnd->finishingSequenceSize);
        
        /* free the finishing sequence */
        free(forEnd->finishingSequence);
        
        /* Yes the addresses are mixed below, get over it */
        
        /* Write out jump (account for op code before and 4 bytes read) */
        writeOutCode((char)OP_JUMP);
        writeOut4ByteAddress((unsigned int)loopBack->jumpIndex);
        
        /* Write out the exit label */
        pushJumpUpdate(exitLabel->jumpIndex, (unsigned int)getProgramByteCount());
        
        /* End of For */
        popCompilerState();
        
        if(*(i+1)) {
            pushCompilerState(LATC_START);
            
        } else {
            pushCompilerState(LATC_DONE);
            
        }
        
        return ++i;
        
    } else if(*i == '\0') {
        /* continue across lines */
        shouldBreak = 1;
        return i;
        
    } else {
        /* Did not read end yet, just another line */
        pushCompilerState(LATC_START);
        return i;
        
    }
    
}

/* Prepares the upcoming while loop by pushing up a label */
char *prepareWhile(char *i) {
    /* Push up an incomplete jump to jump back on to 'loop' */
    pushIncompleteJump((unsigned int)getProgramByteCount(), JUMP_TYPE_JUMP);
    popCompilerState();
    pushCompilerState(LATC_WHILE);
    return i;
    
}

/* Analyze the limit of a while loop */
char *analyzeWhile(char *i) {
    if(*i == ')') {
        
        /* End of limit statement */
        if(getLastOperator() != NULL) {
            dispatchInstructionsFromExpression(0);
        }
        
        /* Dispatch the prior expression */
        dispatchRelationalExpression(0);
        
        /* Shift to Start */
        popCompilerState();
        resetCapturedInput(++i);
        pushCompilerState(LATC_WHILE_START);
        
        return i;
        
    } else if(isdigit(*i)) {
        /* Analyze this number */
        resetCapturedInput(i);
        pushCompilerState(LATC_NUM);
        return i;
        
    } else if(*i == '\'') {
        /* Single Quoted */
        pushCompilerState(LATC_SQUOTE);
        resetCapturedInput(++i);
        return i;
        
    } else if(*i == '"') {
        /* Double Quoted */
        pushCompilerState(LATC_DQUOTE);
        resetCapturedInput(++i);
        return i;
        
    } else if(isalpha(*i)) {
        /* Analyze this variable/bool/function name */
        resetCapturedInput(i);
        pushCompilerState(LATC_VAL);
        return i;
        
    } else if(isConditionalOperator(i)) {
        
        unsigned char conditionalOpCode = getConditionalOPCode(i);
        
        /* Dispatch the prior expression */
        dispatchRelationalExpression(conditionalOpCode);
        
        /* Push Conditional op, get conditional op and push as an operator to work with */
        pushRelationalOperator(conditionalOpCode, 0);
        
        i++;
        if(*i == '=' || *i == '&' || *i == '|') {
            ++i;
        }
        
        return i;
        
    } else if(*i == '(') {
        /* Grouping paren, discard */
        relationalOperatorParenCount++;
        return ++i;
        
    } else if(*i == ')') {
        /* Closing group paren, dispatch and decrement */
        relationalOperatorParenCount--;
        return ++i;
        
    } else {
        /* Error */
#ifdef LAT_DEBUG
        setCompileError("Unrecognized input to analyzing a 'while' limit statement", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

/* Analyze the starting sequence of a while loop */
char *analyzeWhileStart(char *i) {
    if(*i == '{') {
        
        /* Update a cond-jump for catching alternation instances */
        JumpInstance *ji = popJumpStart();
        if(ji != NULL) {
            pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
            
        }
        
        /* Entering while body */
        popCompilerState();
        /* Push 'while' close mark */
        pushCompilerState(LATC_WHILE_END);
        
        if(*(i+1)) {
            pushCompilerState(LATC_START);
            
        } else {
            pushCompilerState(LATC_DONE);
            
        }
        
        return ++i;
        
    } else if(*i == '\0') {
        /* continue across lines */
        shouldBreak = 1;
        return i;
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Unrecognized character found instead of expected '{' for starting while body", i, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    }
}

/* Analyze the end point of a while loop */
char *analyzeWhileEnd(char *i) {
    if(*i == '}') {
        
        /* Get exit label */
        JumpInstance *exitLabel = popJumpType();
        /* Get increment and JUMP back */
        JumpInstance *whileEnd = popJumpType();
        
        /* Yes the addresses are mixed below, get over it */
        
        /* Write out jump to loop back (account for op code and 4 bytes composing it) */
        writeOutCode((char)OP_JUMP);
        /* writeOut4ByteAddress((unsigned int)((getProgramByteCount()-5) - whileEnd->jumpIndex)); */
        writeOut4ByteAddress((unsigned int)whileEnd->jumpIndex);
        
        /* Write out the exit label */
        pushJumpUpdate(exitLabel->jumpIndex, (unsigned int)getProgramByteCount());
        /* writeOutCode((char)OP_LABEL); */
        /* writeOut4ByteAddress(exitLabel->jumpIndex); */
        
        /* End of While */
        popCompilerState();
        
        if(*(i+1)) {
            pushCompilerState(LATC_START);
            
        } else {
            pushCompilerState(LATC_DONE);
            
        }
        
        return ++i;
        
    } else if(*i == '\0') {
        /* continue across lines */
        shouldBreak = 1;
        return i;
        
    } else {
        /* Did not read end yet, just another line */
        pushCompilerState(LATC_START);
        return i;
        
    }
    
}

/* Analyzes for the end of an execute 'inlang' block */
char *analyzeExecEnd(char *i) {
    if(*i == '#' && *(i+1) && *(i+1) == '>') {
        /* Terminating Exec */
        writeOutCode((char)OP_EXEC);
        /* Done, assign! */
        writeRAW(readBatchedData(), readBatchedDataSize());
        unsetBatchedData();
        popCompilerState();
        pushCompilerState(LATC_DONE);
        return i;
        
    } else if(*i) {
        /* Write out */
        writeOutCharacters(i);
        shouldBreak = 1;
        return i;
        
    } else {
        shouldBreak = 1;
        return i;
    }
}


/**** ASSIGN FUNCTIONS ****/

/* Assign a single num to our value */
#ifdef LAT_DEBUG
void assignNum(char *in) {
    printf("Assign Single Num:\n%s\n\n",in);
    exportSingleAssign(OP_NUM);
    
}
#else
void assignNum() {
    exportSingleAssign(OP_NUM);
    
}
#endif


/* Assign a single var to our value */
#ifdef LAT_DEBUG
void assignVar(char *in) {
    printf("Assign Var:\n%s\n\n",in);
    exportSingleAssign(OP_VAR);
}
#else
void assignVar() {
    exportSingleAssign(OP_VAR);
}
#endif


/* Assign a single string to our value */
#ifdef LAT_DEBUG
void assignString(char *in) {
    printf("Assign String:\n%s\n\n",in);
    exportSingleAssign(OP_STRING);
}
#else
void assignString() {
    exportSingleAssign(OP_STRING);
}
#endif


/* Generic Assign */
#ifdef LAT_DEBUG
void assignGeneric(char *in) {
    printf("Assign Generic:\n%s\n\n",in);
    /* Dispatch existing instructs */
    dispatchInstructionsFromExpression(0);
    popCompilerState();
}
#else
void assignGeneric() {
    /* Dispatch existing instructs */
    dispatchInstructionsFromExpression(0);
    popCompilerState();
}
#endif

/* Exports a single assignment (to var or bool) */
void exportSingleAssign(unsigned char type) {
    
    /* Capture Holer */
    ExpressionElement *captureHolder = popOperand();
    
    if(captureHolder != NULL) {
        /* Operand to work with */
        if(type == OP_VAR && strcmp(captureHolder->element, "null") == 0) {
            /* Null check, write out unset */
            writeOutCode((char)OP_UNSET);
            writeOutCharacters(leftElement);
            free(captureHolder->element);
            freeLeftHandValue();
            
            /* Duo pop! */
            popCompilerState();
            popCompilerState();
            pushCompilerState(LATC_DONE);
            
            return;
            
        }
        
        /* Write our register assign */
        if(type == OP_VAR && strcmp(captureHolder->element, "true") == 0) {
            /* true */
            writeOutRegisterStrAssign(0, OP_TRUE, NULL);
            
        } else if(type == OP_VAR && strcmp(captureHolder->element, "false") == 0) {
            /* false */
            writeOutRegisterStrAssign(0, OP_FALSE, NULL);
            
        } else {
            /* just a var */
            writeOutRegisterStrAssign(0, type, captureHolder->element);
        }
        
        /* free capture holder element */
        free(captureHolder->element);
        
    } else if(findParentCompilerState(LATC_FOR_INCREMENT) == 1 && getPriorCompilerState() != LATC_ASSIGN) {
        /* Function declaration item */
        writeOutCode((char)OP_RGI);
        writeOutCode((char)0);
        writeOutCode((char)OP_POP);
        
    }
    
    popCompilerState();
}

/* Bumps if not matching limit, else sets compiler state to provided state*/
char *analyzeQuoted(char *in, char limit) {
    
        #ifdef LAT_DEBUG
        printf("Analyze Quoted:\n%s\n\n",in);
        #endif
    
    if(*in == limit && *(in-1) != '\\') {
        /* Just in Case */
        popCompilerState();
        
        pushOperand(getCapturedInput(in), OP_STRING, 0);
        
        if(getLastOperator() != NULL && *(in+1) != ']' && getCompilerState() != LATC_ARRAY_INIT && !isArithmeticOperator(*(in+1))) {
            /* Generic Assign, pop prior state and push generic */
            popCompilerState();
            popCompilerState();
            pushCompilerState(ASSIGN_GENERIC);
            
        }
        
        resetCapturedInput(++in);
        
        return in;
        
    } else if (!*in) {
        /* We ran out of options! */
#ifdef LAT_DEBUG
        setCompileError("Mismatched quotes",in, originalInput);
#else
        setCompileError();
#endif
        return NULL;
        
    } else {
        /* continue */
        return ++in;
    }
}

/* PEMDAS expression parsing, uses a double stack technique */
char *analyzeOperator(char *input) {
    
    ExpressionElement *element;
    
    #ifdef LAT_DEBUG
    printf("Analyze Operator:\n%s\n\n",input);
    #endif
    resetCapturedInput(input+1);
    
    /* For the following either push our operator to the stack and bump, or dispatch instructions and NO bump */
    
    if(*input == '+') {
        if((element = getLastOperator()) != NULL && element->precedence >= 1) {
            if(element->precedence > 1) {
                /* Bump baseline register num */
                baselineRegisterNum++;
                
            }
            dispatchInstructionsFromExpression(0);
            if(element->precedence > 1) {
                /* Bump back down */
                baselineRegisterNum--;
                
            }
            resetCapturedInput(input);
            
        } else {
            pushOperator(OP_ADD, 1);
            input++;
            
        }
        
    } else if(*input == '-') {
        if((element = getLastOperator()) != NULL && element->precedence >= 1) {
            if(element->precedence > 1) {
                /* Bump baseline register num */
                baselineRegisterNum++;
            }
            dispatchInstructionsFromExpression(0);
            if(element->precedence > 1) {
                /* Bump back down */
                baselineRegisterNum--;
            }
            resetCapturedInput(input);
        } else {
            pushOperator(OP_SUB, 1);
            input++;
        }
        
    } else if(*input == '*') {
        if((element = getLastOperator()) != NULL && element->precedence >= 2) {
            /* Bump up the baseline */
            if(element->precedence > 2) {
                /* Bump baseline register num */
                baselineRegisterNum++;
            }
            dispatchInstructionsFromExpression(0);
            if(element->precedence > 2) {
                /* Bump back down */
                baselineRegisterNum--;
            }
            resetCapturedInput(input);
        } else {
            pushOperator(OP_MULTI, 2);
            input++;
        }
        
    } else if(*input == '/') {
        if((element = getLastOperator()) != NULL && element->precedence >= 2) {
            /* Bump up the baseline */
            if(element->precedence > 2) {
                /* Bump baseline register num */
                baselineRegisterNum++;
            }
            dispatchInstructionsFromExpression(0);
            if(element->precedence > 2) {
                /* Bump back down */
                baselineRegisterNum--;
            }
            resetCapturedInput(input);
        } else {
            pushOperator(OP_DIVI, 2);
            input++;
        }
        
    } else if(*input == '%') {
        if((element = getLastOperator()) != NULL && element->precedence >= 2) {
            /* Bump up the baseline */
            if(element->precedence > 2) {
                /* Bump baseline register num */
                baselineRegisterNum++;
            }
            dispatchInstructionsFromExpression(0);
            if(element->precedence > 2) {
                /* Bump back down */
                baselineRegisterNum--;
            }
            resetCapturedInput(input);
        } else {
            pushOperator(OP_MODULUS, 2);
            input++;
        }
        
    } else if(*input == '^') {
        if((element = getLastOperator()) != NULL && element->precedence >= 3) {
            /* Bump up the baseline */
            if(element->precedence > 3) {
                /* Bump baseline register num */
                baselineRegisterNum++;
            }
            dispatchInstructionsFromExpression(0);
            if(element->precedence > 3) {
                /* Bump back down */
                baselineRegisterNum--;
            }
            resetCapturedInput(input);
        } else {
            pushOperator(OP_EXPONENT, 3);
            input++;
        }
        
    } else if(*input == '(') {
        /* Always push up an opening paren, no matter what */
        if(*(input-1) == '&' || *(input-1) == '|') {
            /* Relational Paren */
            pushRelationalOperator(OP_PARENO, 0);
            /* increment paren count */
            relationalOperatorParenCount++;
            
        } else {
            /* Arithmetic Paren */
            pushOperator(OP_PARENO, 0);
            pushOperand("(", OP_PARENO, 0);
            /* increment paren count */
            operatorParenCount++;
        
        }
        input++;
        
    } else if(*input == ')') {
        
        if(((*(input+1) && *(input+1) != '&' && *(input+1) != '|' && operatorParenCount >= 1)) || (operatorParenCount >= 1)) {
            /* Dispatch Arithmetic Expression */
            unsigned char didFirstDispatch = 0;
            /* Dispatch to clear up to our last read opening paren */
            /* On our first time dispatch if we are working with a closing paren, clear it */
            if(isFirstDispatch == 0) {
                isFirstDispatch = 1;
                didFirstDispatch = 1;
            }
            baselineRegisterNum++;
            dispatchInstructionsFromExpression(1);
            baselineRegisterNum--;
            input++;
            
            /* Check to write a register->register assignment*/
            if(didFirstDispatch == 1) {
                /* Write out a register to register assignment, 0 -> 1 IF this is our first dispatch */
                writeOutRegisterNumAssign(0, OP_RGI, 1);
                
            }
            
            /* decrement paren count */
            operatorParenCount--;
            
        } else if((*(input+1) && (*(input+1) == '&' || *(input+1) == '|') && relationalOperatorParenCount >= 1) || (operatorParenCount < 1 && relationalOperatorParenCount >= 1)) {
            /* Dispatch Relational Expression */
            
            dispatchRelationalExpression(0);
            
            if(*(input+1) == '|') {
                
                #pragma message("bad logic here, the if below will always fail. before ji = NULL was not being set to anything")
                
                JumpInstance *ji = NULL;
                /* Put a success jump here */
                writeOutCode((char)OP_JUMP);
                pushIncompleteJump((unsigned int)getProgramByteCount(), JUMP_TYPE_INCOMPLETE_JUMP);
                writeOut4ByteAddress(0);
                
                /* Write out a failure interception here */
                if(ji != NULL) {
                    pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
                }
                /* writeOutCode((char)OP_LABEL); */
                /* writeOut4ByteAddress(getJumpNum()); */
            }
            
            input++;
            
            /* decrement paren count */
            relationalOperatorParenCount--;
        } else {
            /* Error, unabalanced expression */
#ifdef LAT_DEBUG
            setCompileError("Unbalanced expression found, parentheses did not match up", input, originalInput);
#else
            setCompileError();
#endif
            
        }
        
    } else if(*input == '=' && *(input+1) && *(input+1) == '=') {
        /* == */
        if((element = getLastRelationalOperator()) != NULL && element->precedence >= 1) {
            dispatchInstructionsFromExpression(0);
            resetCapturedInput(input);
            
        } else {
            dispatchRelationalExpression(0);
            pushRelationalOperator(OP_EQUAL, 1);
            input++;
            
        }
        
    } else if(*input == '<' && *(input+1) && *(input+1) != '=') {
        /* < */
        if((element = getLastRelationalOperator()) != NULL && element->precedence >= 1) {
            dispatchInstructionsFromExpression(0);
            resetCapturedInput(input);
            
        } else {
            dispatchRelationalExpression(0);
            pushRelationalOperator(OP_LESST, 1);
            input++;
            
        }
        
    } else if(*input == '>' && *(input+1) && *(input+1) != '=') {
        /* > */
        if((element = getLastRelationalOperator()) != NULL && element->precedence >= 1) {
            dispatchInstructionsFromExpression(0);
            resetCapturedInput(input);
            
        } else {
            dispatchRelationalExpression(0);
            pushRelationalOperator(OP_GREATRT, 1);
            input++;
            
        }
        
    } else if(*input == '!' && *(input+1) && *(input+1) != '=') {
        /* != */
        if((element = getLastRelationalOperator()) != NULL && element->precedence >= 1) {
            dispatchInstructionsFromExpression(0);
            resetCapturedInput(input);
            
        } else {
            dispatchRelationalExpression(0);
            pushRelationalOperator(OP_NOT_EQUAL, 1);
            input++;
            
        }
        
    } else if(*input == '>' && *(input+1) && *(input+1) == '=') {
        /* >= */
        if((element = getLastRelationalOperator()) != NULL && element->precedence >= 1) {
            dispatchInstructionsFromExpression(0);
            resetCapturedInput(input);
            
        } else {
            dispatchRelationalExpression(0);
            pushRelationalOperator(OP_GTRTHN_EQL, 1);
            input++;
            
        }
        
    } else if(*input == '<' && *(input+1) && *(input+1) == '=') {
        /* <= */
        if((element = getLastRelationalOperator()) != NULL && element->precedence >= 1) {
            dispatchInstructionsFromExpression(0);
            resetCapturedInput(input);
            
        } else {
            dispatchRelationalExpression(0);
            pushRelationalOperator(OP_LESST_EQUAL, 1);
            input++;
            
        }
        
    } else if(*input == '&' && *(input+1) && *(input+1) == '&') {
        /* && */
        if((element = getLastRelationalOperator()) != NULL && element->precedence >= 1) {
            dispatchInstructionsFromExpression(0);
            resetCapturedInput(input);
            
        } else {
            dispatchRelationalExpression(OP_AND);
            pushRelationalOperator(OP_AND, 1);
            input++;
            
        }
        
    } else if(*input == '|' && *(input+1) && *(input+1) == '|') {
        /* || */
        if((element = getLastRelationalOperator()) != NULL && element->precedence >= 1) {
            dispatchInstructionsFromExpression(0);
            resetCapturedInput(input);
            
        } else {
            dispatchRelationalExpression(OP_OR);
            pushRelationalOperator(OP_OR, 1);
            input++;
            
        }
        
    } else {
        /* Error */
#ifdef LAT_DEBUG
        setCompileError("Unknown operand in compound expression",input, originalInput);
#else
        setCompileError();
#endif
        
    }
    
    /* Return to previous compiler state */
    popCompilerState();
    
    return input;
}

/* Dispatches instructions from a given expression
 *  0 - Regular mode, runs until it empties all operands OR reads a '(', at which point it will stop, replace what it read, and return
 *  1 - Clear mode, run until it empties all operands OR read a '(', at which point it will stop and return without replacing what it read
 */
unsigned char didDispatchArrayRefs;
void dispatchInstructionsFromExpression(unsigned char mode) {
    
    /* Start our register counter at the baseline # */
    unsigned char currentRegisterNum = baselineRegisterNum, notDone = 0;
    
    ExpressionElement *poppedOperand1,*poppedOperand2;
    ExpressionElement *poppedOperator;
    
    #ifdef LAT_DEBUG
    printf("Dispatching Instructions\n");
    #endif
    
    didDispatchArrayRefs = 0;
    
    /* Check if this is our first run */
    if(isFirstDispatch == 0) {
        /* Forcibly set our currentRegister# to 0, and increment our first dispatch flag */
        currentRegisterNum = 0;
        isFirstDispatch = 1;
        
    }
    
    /* Check for special array refs */
    if(getLastOperand() != NULL && getLastOperand()->isArrayOp == 2) {
        /* We found a key, call the specialized dispatch for this, key is in plaintext */
        dispatchArrayExpression(0);
        
    } else if(getLastOperand() != NULL && getLastOperand()->isArrayOp == 1) {
        /* We found a name but no key. The key is stored on the arg stack */
        dispatchArrayExpression(1);
        
    }
    
    /* Make sure we have something to work with */
    if((poppedOperand1 = popOperand()) != NULL && (poppedOperand2 = popOperand()) != NULL) {
        
        int rgiIndex = 0;
        
        /* First run with 2 operands */
        
        /* Get an operator */
        poppedOperator = popOperator();
        
        if(poppedOperator == NULL && *poppedOperand1->element == '(') {
            /* This is an end ')' case () back to back with no way to connect what we just wrote, write a merge from 0 to 1 instead using ADD */
#ifdef LAT_DEBUG
            setCompileError("Dead code, this is a net","", originalInput);
#else
            setCompileError();
#endif
            return;
        }
        
        /* Write out register assign for operand 1 */
        if(*poppedOperand1->element != '(') {
            /* Regular operand */
            writeOutRegisterStrAssign(currentRegisterNum, poppedOperand1->precedence, poppedOperand1->element);
            
            #ifdef LAT_DEBUG
            printf(">%s and ",poppedOperand1->element);
            #endif
            
            if(!*poppedOperand1->element) {
#ifdef LAT_DEBUG
                setCompileError("Operand 1 was null while compiling","", originalInput);
#else
                setCompileError();
#endif
            }
            
        }
        #ifdef LAT_DEBUG
        else {
            /* O/PEN, use register 0x00 instead */
            printf("Using register for first operand instead\n");
        }
        #endif
        
        /*** Handle operand 2 ***/
        
        if(poppedOperand2->element == NULL) {
#ifdef LAT_DEBUG
            setCompileError("Null second argument to expression analyzer","", originalInput);
#else
            setCompileError();
#endif
        }
        
        if(*poppedOperand2->element != '(') {
            /* Write out register assign for operand 2 */
            writeOutRegisterStrAssign(2, poppedOperand2->precedence, poppedOperand2->element);
            
            #ifdef LAT_DEBUG
            printf("%s\n", poppedOperand2->element);
            #endif
        }
        #ifdef LAT_DEBUG
        else {
            printf("Using register for second operand instead\n");
        }
        #endif
        
        if(poppedOperator == NULL) {
#ifdef LAT_DEBUG
            setCompileError("No operator found for the first expression","", originalInput);
#else
            setCompileError();
#endif
        }
        
        if(poppedOperator->op == OP_PARENO) {
#ifdef LAT_DEBUG
            setCompileError("We read an opening paren, something we should not be reading to start","", originalInput);
#else
            setCompileError();
#endif
            
        } else if(poppedOperator->op == OP_PARENC) {
#ifdef LAT_DEBUG
            setCompileError("We read a closing paren, something we should never be reading","", originalInput);
#else
            setCompileError();
#endif
            
        } else {
            /* Write out the operation (ADD,SUB,DIVI,MULTI,EXP,MOD) */
            writeOutCode((char)poppedOperator->op);
            
        }
        
        /* Assign result to our current register # */
        writeOutCharacter((char)currentRegisterNum);
        
        if(poppedOperand1->element != NULL) {
            if(*poppedOperand1->element != '(') {
                /* Operand 1 */
                writeOutCharacter((char)currentRegisterNum);
            } else {
                /* O/PEN, defer to current address */
                writeOutCode((char)getLastRegisterUsedAtIndex((unsigned char)rgiIndex++));
            }
            
            /* free element */
            free(poppedOperand1->element);
            
        } else {
            /* No first operand value! */
#ifdef LAT_DEBUG
            setCompileError("First element not present","", originalInput);
#else
            setCompileError();
#endif
            
        }
        
        if(poppedOperand2 != NULL && poppedOperand2->element != NULL) {
            if(*poppedOperand2->element != '(') {
                /* Operand 2, always register 2 */
                writeOutCharacter((char)2);
            } else {
                /* O/PEN, defer to current address INVERTED */
                if(getLastRegisterUsedAtIndex((unsigned char)rgiIndex) == getLastRegisterUsedAtIndex(0) && rgiIndex == 1) {
                    writeOutCode((char)currentRegisterNum);
                } else {
                    writeOutCode((char)getLastRegisterUsedAtIndex((unsigned char)rgiIndex));
                }
            }
            
            /* free element */
            free(poppedOperand2->element);
            
        } else {
            /* No second operand value */
#ifdef LAT_DEBUG
            setCompileError("Second element not present","", originalInput);
#else
            setCompileError();
#endif
        }
        
        /* Push last register # */
        pushLastRegisterUsed(currentRegisterNum);
        
        /* Check our operator without popping it, before we continue */
        if((poppedOperator = getLastOperator()) == NULL) {
            /* must be done */
            notDone = 1;
            
        } else if(poppedOperator->op == OP_PARENO) {
            if(mode == 0) {
                /* Regular mode, return at this point */
                return;
            } else {
                /* Clear mode, pop this operator, and return */
                popOperator();
                return;
            }
        } else if(poppedOperator->op == OP_PARENC) {
#ifdef LAT_DEBUG
            setCompileError("We read a closing paren while looping, something we should never be reading","", originalInput);
#else
            setCompileError();
#endif
        }
        
        /* Fetch next operand */
        if(notDone == 0) {
            
            if(didDispatchArrayRefs == 1) {
                didDispatchArrayRefs = 0;
            }
            
            /* Check for special array refs */
            if(getLastOperand() != NULL && getLastOperand()->isArrayOp == 2) {
                /* We found a key, call the specialized dispatch for this */
                dispatchArrayExpression(0);
                
            } else if(getLastOperand() != NULL && getLastOperand()->isArrayOp == 1) {
                /* We found a name but no key. The key is stored on the arg stack */
                dispatchArrayExpression(1);
                
            }
            
            poppedOperand1 = popOperand();
        }
    }
    
    /* Proceed to go one at a time for every other item we have listed */
    while(poppedOperand1 != NULL && notDone == 0) {
        
        unsigned char operand1Val = getLastRegisterUsedAtIndex(0);
        
        #ifdef LAT_DEBUG
        printf("::%s\n",poppedOperand1->element);
        #endif
        
        /* Get an operator */
        poppedOperator = popOperator();
        
        if(poppedOperator == NULL && *poppedOperand1->element == '(') {
            /* free element */
            free(poppedOperand1->element);
            /* This is an end ')' case */
            return;
            
        } else if(*poppedOperand1->element != '(') {
            /* Write out register assign for operand 1 */
            operand1Val = getLastRegisterUsedAtIndex(0) == 2 ? 1 : 2;
            writeOutRegisterStrAssign(operand1Val, poppedOperand1->precedence, poppedOperand1->element);
            pushLastRegisterUsed(operand1Val);
            
            #ifdef LAT_DEBUG
            printf(">%s and register val\n",poppedOperand1->element);
            #endif
        } else {
            
            #ifdef LAT_DEBUG
            printf("Using register for sole element\n");
            #endif
            operand1Val = getLastRegisterUsedAtIndex(0);
        }
        
        /* Write out the operation (ADD,SUB,DIVI,MULTI,EXP,MOD) */
        writeOutCode((char)poppedOperator->op);
        
        /* Assign to our current register # */
        writeOutCharacter((char)currentRegisterNum);
        
        if(poppedOperand1->element != NULL) {
            /* Operand 1 */
            if(didDispatchArrayRefs == 0) {
                writeOutCharacter((char)operand1Val);
            } else {
                /* FLIP, for array refs */
                writeOutCharacter((char)getLastRegisterUsedAtIndex(1));
            }
            
            /* free element */
            free(poppedOperand1->element);
            
        } else {
            /* No first operand value! */
#ifdef LAT_DEBUG
            setCompileError("First element not present","", originalInput);
#else
            setCompileError();
#endif
            
        }
        
        /* Perform op with prior register or last used register item at the appropriate index */
        if(operand1Val != getLastRegisterUsedAtIndex(0)) {
            writeOutCharacter((char)getLastRegisterUsedAtIndex(0));
        } else {
            if(didDispatchArrayRefs == 0) {
                writeOutCharacter((char)getLastRegisterUsedAtIndex(1));
            } else {
                /* FLIP, for array refs */
                writeOutCharacter((char)operand1Val);
            }
        }
        
        /* Push last used */
        pushLastRegisterUsed(currentRegisterNum);
        
        /* Check our operator without popping it, before we continue */
        if((poppedOperator = getLastOperator()) == NULL) {
            /* must be done */
        } else if(poppedOperator->op == OP_PARENO) {
            if(mode == 0) {
                /* Regular mode, return at this point */
                return;
                
            } else {
                /* Check to push an operand of ( if we don't have one already */
                if(getLastOperand() == NULL || *getLastOperand()->element != '(') {
                    /* Push closing paren */
                    pushOperand("(", OP_PARENO, 0);
                    
                }
                
                /* Clear mode, pop this operator, and return */
                popOperator();
                return;
            }
        } else if(poppedOperator->op == OP_PARENC) {
#ifdef LAT_DEBUG
            setCompileError("We read a closing paren while looping, something we should never be reading","", originalInput);
#else
            setCompileError();
#endif
        }
        
        /* Check for special array refs */
        if(getLastOperand() != NULL && getLastOperand()->isArrayOp == 2) {
            /* We found a key, call the specialized dispatch for this */
            dispatchArrayExpression(0);
            
        } else if(getLastOperand() != NULL && getLastOperand()->isArrayOp == 1) {
            /* We found a name but no key. The key is stored on the arg stack */
            dispatchArrayExpression(1);
            
        }
        
        if(didDispatchArrayRefs == 1) {
            didDispatchArrayRefs = 0;
        }
        
        /* Retrieve next operand */
        poppedOperand1 = popOperand();
        
    }
    
    /* Finally check for a tailing lone operator to wrap up with */
    if((poppedOperator = popOperator()) != NULL) {
        
        /* Write out the operator that seperates this and a previous section */
        writeOutCode((char)poppedOperator->op);
        
        writeOutCharacter((char)0);
        writeOutCharacter((char)0);
        writeOutCharacter((char)1);
    }
}

/* Pushes an operand to our operand stack */
void pushOperand(char *operand, unsigned short opValue, unsigned short arrayOpStat) {
    
    char *newOperand;
    
    if(!*operand) {
        #ifdef LAT_DEBUG
        printf("Avoiding pushing null operand\n");
        #endif
        return;
        
    }
    
    newOperand = strdup(operand);
    
    operandStackIndex++;
    
    #ifdef LAT_DEBUG
    printf("Pushing Operand %d: %s (%d)\n",operandStackIndex,newOperand,arrayOpStat);
    #endif
    
    if(operandStack == NULL) {
        operandStackMax+=(short)10;
        operandStack = malloc(sizeof(ExpressionElement) * 10);
        
    } else if (operandStackIndex > operandStackMax - 1) {
        operandStackMax+=(short)10;
        operandStack = realloc(operandStack, sizeof(ExpressionElement) * (size_t)operandStackMax);
        
    }
    
    operandStack[operandStackIndex].element = newOperand;
    operandStack[operandStackIndex].precedence = opValue;
    operandStack[operandStackIndex].isArrayOp = arrayOpStat;
}

/* Pushes an operator to our operator stack */
void pushOperator(unsigned short operand, unsigned short precedence) {
    
    operatorStackIndex++;
    
    #ifdef LAT_DEBUG
    switch(operand) {
        case OP_ADD:
            printf("Pushing Operator %d: +\n",operatorStackIndex);
            break;
        
        case OP_SUB:
            printf("Pushing Operator %d: -\n",operatorStackIndex);
            break;
            
        case OP_MULTI:
            printf("Pushing Operator %d: *\n",operatorStackIndex);
            break;
            
        case OP_DIVI:
            printf("Pushing Operator %d: /\n",operatorStackIndex);
            break;
            
        case OP_MODULUS:
            printf("Pushing Operator %d: %%\n",operatorStackIndex);
            break;
            
        case OP_EXPONENT:
            printf("Pushing Operator %d: ^\n",operatorStackIndex);
            break;
            
        case OP_PARENO:
            printf("Pushing Operator %d: (\n",operatorStackIndex);
            break;
            
        default:
#ifdef LAT_DEBUG
            setCompileError("Unrecognized operator added","", originalInput);
#else
            setCompileError();
#endif
            break;
        
    }
    #endif
    
    if(operatorStack == NULL) {
        operatorStackMax+=(short)10;
        operatorStack = malloc(sizeof(ExpressionElement) * 10);
        
    } else if (operatorStackIndex > operatorStackMax - 1) {
        operatorStackMax+=(short)10;
        operatorStack = realloc(operatorStack, sizeof(ExpressionElement) * (size_t)operatorStackMax);
        
    }
    
    operatorStack[operatorStackIndex].op = operand;
    operatorStack[operatorStackIndex].precedence = precedence;
}

/* Pops an operand (+) from our operand stack */
ExpressionElement *popOperand() {
    /* printf("popAND:%d\n",operandStackIndex); */
    return operandStackIndex >= 0 ? &operandStack[operandStackIndex--] : NULL;
}

/* Pops an operator (x) */
ExpressionElement *popOperator() {
    /* printf("popTOR:%d\n",operatorStackIndex); */
    return operatorStackIndex >= 0 ? &operatorStack[operatorStackIndex--] : NULL;
}

/* Returns the last item on the operator stack we addded */
ExpressionElement *getLastOperator() {
    return operatorStackIndex >= 0 ? &operatorStack[operatorStackIndex] : NULL;
}

/* Returns the last item on the operand stack we added */
ExpressionElement *getLastOperand() {
    return operandStackIndex >= 0 ? &operandStack[operandStackIndex] : NULL;
}

/* Returns the current operand count - 1 */
short getOperandCount() {
    return operandStackIndex;
}

/* Pushes a new relational operator */
void pushRelationalOperator(unsigned short operand, unsigned short precedence) {
    relationalOperatorStackIndex++;
    
    #ifdef LAT_DEBUG
    switch(operand) {
            
        case OP_EQUAL:
            printf("Pushing Relational Operator %d EqualTo\n\n",relationalOperatorStackIndex);
            break;
            
        case OP_NOT_EQUAL:
            printf("Pushing Relational Operator %d NotEqualTo\n\n",relationalOperatorStackIndex);
            break;
            
        case OP_LESST:
            printf("Pushing Relational Operator %d LessThan\n\n",relationalOperatorStackIndex);
            break;
            
        case OP_LESST_EQUAL:
            printf("Pushing Relational Operator %d LessThanEqualTo\n\n",relationalOperatorStackIndex);
            break;
            
        case OP_GREATRT:
            printf("Pushing Relational Operator %d GreaterThan\n\n",relationalOperatorStackIndex);
            break;
            
        case OP_GTRTHN_EQL:
            printf("Pushing Relational Operator %d GreaterThanEqualTo\n\n",relationalOperatorStackIndex);
            break;
            
        case OP_AND:
            printf("Pushing Relational Operator %d And\n\n",relationalOperatorStackIndex);
            break;
            
        case OP_OR:
            printf("Pushing Relational Operator %d Or\n\n",relationalOperatorStackIndex);
            break;
            
        case OP_PARENO:
            printf("Pushing Relational Operator %d (\n\n",relationalOperatorStackIndex);
            break;
            
        default:
#ifdef LAT_DEBUG
            setCompileError("Unrecognized relational operator added","", originalInput);
#else
            setCompileError();
#endif
            break;
            
    }
    #endif
    
    if(relationalOperatorStack == NULL) {
        relationalOperatorStackMax+=(short)10;
        relationalOperatorStack = malloc(sizeof(ExpressionElement) * 10);
        
    } else if (relationalOperatorStackIndex > relationalOperatorStackMax - 1) {
        relationalOperatorStackMax+=(short)10;
        relationalOperatorStack = realloc(relationalOperatorStack, sizeof(ExpressionElement) * (size_t)relationalOperatorStackMax);
        
    }
    
    relationalOperatorStack[relationalOperatorStackIndex].op = operand;
    relationalOperatorStack[relationalOperatorStackIndex].precedence = precedence;
}

/* Pop the current relational operator off the stack */
ExpressionElement *popRelationalOperator() {
    #ifdef LAT_DEBUG
    printf("pop relational op:%d\n",relationalOperatorStackIndex);
    #endif
    return relationalOperatorStackIndex >= 0 ? &relationalOperatorStack[relationalOperatorStackIndex--] : NULL;
    
}

/* Gets the last pushed on relational operator from the stack */
ExpressionElement *getLastRelationalOperator() {
    return relationalOperatorStackIndex >= 0 ? &relationalOperatorStack[relationalOperatorStackIndex] : NULL;
}

/* Dispatch the pending relational expression */
void dispatchRelationalExpression(unsigned short nextRelOPCode) {
    /* Dispatch if we have operators */
    if(getLastOperator() != NULL || getOperandCount() == 1) {
        dispatchInstructionsFromExpression(0);
        
    } else {
        /* Take our only item, and assign it to a register */
        ExpressionElement *ee = popOperand();
        if(ee != NULL) {
            
            if(ee->precedence == OP_VAR && strcmp(ee->element, "true") == 0) {
                /* true */
                writeOutRegisterStrAssign(2, OP_TRUE, NULL);
                
            } else if(ee->precedence == OP_VAR && strcmp(ee->element, "false") == 0) {
                /* false */
                writeOutRegisterStrAssign(2, OP_FALSE, NULL);
                
            } else {
                /* Regular var assign */
                writeOutRegisterStrAssign(2, ee->precedence, ee->element);
                
            }
            
            free(ee->element);
            
            pushLastRegisterUsed(2);
            
        }
    }
    
    /* Push up our last register value we set */
    writeOutCode((char)OP_PUSH);
    writeOutCode((char)getLastRegisterUsedAtIndex(0));
    
    if(getLastRelationalOperator() != NULL && getLastRelationalOperator()->op != OP_AND && getLastRelationalOperator()->op != OP_OR) {
        /* A pending expression to perform */
        ExpressionElement *relOp = popRelationalOperator();
        
        /* Assign register 1 */
        writeOutCode((char)OP_RGI);
        writeOutCode((char)1);
        writeOutCode((char)OP_POP);
        
        /* Assign register 2 */
        writeOutCode((char)OP_RGI);
        writeOutCode((char)2);
        writeOutCode((char)OP_POP);
        
        /* Write out op, assign to 1 */
        writeOutCode((char)relOp->op);
        writeOutCode((char)1);
        writeOutCode((char)1);
        writeOutCode((char)2);
        
        /* Write out conditional goto for the prior 'equals' */
        if(nextRelOPCode != OP_OR) {
            /* Complete the prior exit case */
            if(getJumpType() != NULL && getJumpType()->jumpType == JUMP_TYPE_INCOMPLETE_JUMP && getJumpType()->jumpGroup == getJumpGroupNum()) {
                JumpInstance *ji = popJumpType();
                pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
            }
            
            /* Write out a failure case */
            writeOutCode((char)OP_COND_JUMP);
            writeOutCode((char)1);
            writeOutCode((char)0); /* fail case */
            /* Push a jump FAIL command */
            pushIncompleteJump((unsigned int)getProgramByteCount(), JUMP_TYPE_INCOMPLETE_JUMP);
            writeOut4ByteAddress(0);
        
        } else if(nextRelOPCode == OP_OR) {
            /* Complete the prior start OR jump */
            JumpInstance *ji = popJumpStart();
            if(ji != NULL) {
                pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
            }
            
            /* Write out a success case */
            writeOutCode((char)OP_COND_JUMP);
            writeOutCode((char)1);
            writeOutCode((char)1); /* success case */
            /* Push a jump START command, separate stack */
            pushJumpStart((unsigned int)getProgramByteCount());
            writeOut4ByteAddress(0);
            
            /* Complete the prior exit case */
            if(getJumpType() != NULL && getJumpType()->jumpType == JUMP_TYPE_INCOMPLETE_JUMP) {
                ji = popJumpType();
                pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
            }
            
        }
    }
}


/* Specific function for handling dispatch of Array refs. Called by the 'full' dispatch. It works through the name & key before returning*/
void dispatchArrayExpression(unsigned char mode) {
    
    #ifdef LAT_DEBUG
    printf("Dispatching Array Instructions\n");
    #endif
    
    /* Indicate we did dispatch array refs */
    didDispatchArrayRefs = 1;
    
    if(mode == 0) {
        /* Key as operand */
        ExpressionElement *key = popOperand();
        ExpressionElement *name = popOperand();
        /* write out register */
        writeOutRegisterArrayValueAssign(2, OP_ARRAY_VAR, name->element, key->element);
        /* push it onto our last used stack */
        pushLastRegisterUsed(2);
        
        free(key->element);
        free(name->element);
        
    } else if(mode == 1) {
        /* Key as arg on stack, pop to get it */
        ExpressionElement *name = popOperand();
        
        /* Pop key to register 0x02 */
        writeOutCode((char)OP_RGI);
        writeOutCode((char)2);
        writeOutCode((char)OP_POP);
        
        /* Write out array set */
        writeOutCode((char)OP_RGI);
        writeOutCode((char)2);
        writeOutCode((char)OP_ARRAY_VAR);
        /* var name */
        writeOutCharacters(name->element);
        /* key from register */
        writeOutCode((char)2);
        
        /* Clear last op */
        lastRegisterOps[2] = 999;
        /* set type */
        lastRegisterType[2] = OP_ARRAY_VAR;
        
        /* Clear any associated string as well */
        free(lastRegisterValues[2]),lastRegisterValues[2] = NULL;
        
        /* free name */
        free(name->element);
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Attempted to set mode to an undefined value %d. Only takes arg of 0 or 1 as an unsigned char", "", originalInput);
#else
        setCompileError();
#endif
        
    }
}

/* Resets the currently captured input */
void resetCapturedInput(char *input) {
    
#ifdef LAT_DEBUG
    printf("Reset Captured Input To:\n%s\n\n",input);
#endif
    
    __captureIndex = input;
}

char *getCapturedInput(char *i) {
    
    int len;
    
    if(__captureIndex == NULL) {
        return NULL;
        
    }
    len = (int)(i - __captureIndex);
    
    if(__savedCapture == NULL) {
        /* malloc */
        __savedCapture = malloc(sizeof(char) * (size_t)(len+2));
        __savedCaptureLength = (unsigned int)len + 1;
        
    } else if(__savedCaptureLength <= (unsigned int)len - 1) {
        /* realloc */
        __savedCapture = realloc(__savedCapture, sizeof(char) * (size_t)(len+2));
        __savedCaptureLength = (unsigned int)len + 1;
        
    }
    
    strncpy(__savedCapture, __captureIndex, (size_t)len);
    __savedCapture[len] = '\0';
    return __savedCapture;
}

/* Pushes function names up */
void pushFuncName(char *name) {
    
    if(funcNameStackIndex > FUNC_NAME_STACK_MAX-1) {
        printf("Func Name Stack overflow!\n");
        exit(14901);
    }
    
    funcNameStack[funcNameStackIndex++] = strdup(name);
}

/* Pops function names */
char *popFuncName() {
    char *dd = funcNameStack[funcNameStackIndex-1];
    funcNameStack[--funcNameStackIndex] = NULL;
    return dd;
}

/* Frees all function names [*/
void freeFuncNames() {
    int x = 0;
    while(x < FUNC_NAME_STACK_MAX) {
        free(funcNameStack[x]);
        x--;
    }
}


/* Sets the left hand value */
void assignLeftHandValue(char *cutIndex) {
    leftElement = getCapturedInput(cutIndex);
    leftElement = strdup(leftElement);
    resetCapturedInput(cutIndex+1);
    #ifdef LAT_DEBUG
    printf("Assigned lefthand Value:\n%s\n\n",leftElement);
    #endif
}

/* Frees the left hand value */
void freeLeftHandValue() {
    #ifdef LAT_DEBUG
    printf("Freed lefthand value\n\n");
    #endif
    free(leftElement),leftElement = NULL;
}

/* Adds a last used register # */
void pushLastRegisterUsed(unsigned char registerNum) {
    /* Shift existing */
    unsigned char tmp = _lastRegistersSet[0];
    _lastRegistersSet[1] = tmp;
    /* Set */
    _lastRegistersSet[0] = registerNum;
    
}

/* Returns one of two historical indexes used to hold previous register assignments */
unsigned char getLastRegisterUsedAtIndex(unsigned char index) {
    if(index <= 1) {
        return _lastRegistersSet[index];
    } else {
#ifdef LAT_DEBUG
        setCompileError("Attempted to access last register used beyond 2 slot history","", originalInput);
#else
        setCompileError();
#endif
        return 0;
    }
}

/* Returns an op code for a given input */
unsigned char getConditionalOPCode(char *i) {
    if(*i == '=') {
        if(*(i+1) && *(i+1) == '=') {
            return OP_EQUAL;
            
        }
        
    } else if(*i == '<') {
        if(*(i+1) && *(i+1) == '=') {
            return OP_LESST_EQUAL;
            
        } else {
            return OP_LESST;
            
        }
        
    } else if(*i == '>') {
        if(*(i+1) && *(i+1) == '=') {
            return OP_GTRTHN_EQL;
            
        } else {
            return OP_GREATRT;
            
        }
        
    } else if(*i == '!' && *(i+1) && *(i+1) == '=') {
        return OP_NOT_EQUAL;
        
    } else if(*i == '&' && *(i+1) && *(i+1) == '&') {
        return OP_AND;
        
    } else if(*i == '|' && *(i+1) && *(i+1) == '|') {
        return OP_OR;
        
    }
    
    printf("Unrecognized conditional sequence passed '%s'\n", i);
    exit(1455);
}

/* Writes out requested string assign register if needed */
void writeOutRegisterStrAssign(unsigned char regNum, unsigned short type, char *value) {
    
    if(regNum <= 2) {
        
        if(type == OP_TRUE || type == OP_FALSE) {
            /* Bool and Done */
            writeOutCode((char)OP_RGI);
            writeOutCode((char)regNum);
            writeOutCode((char)type);
            
            /* clear last code op */
            lastRegisterOps[regNum] = 999;
            /* set type */
            lastRegisterType[regNum] = type;
            
        }
        /*else if(lastRegisterValues[regNum] == NULL || (strcmp(lastRegisterValues[regNum],value) != 0 || lastRegisterType[regNum] != type || lastRegisterType[regNum] == OP_VAR || lastRegisterType[regNum] == OP_ARRAY_VAR)) {*/
        else {
            /* Not redundant! Let's set this value (note we are allowing vars to pass by though) */
            writeOutCode((char)OP_RGI);
            writeOutCode((char)regNum);
            writeOutCode((char)type);
            writeOutCharacters(value);
            
            /* clear last code op */
            lastRegisterOps[regNum] = 999;
            /* set type */
            lastRegisterType[regNum] = type;
            
            /* Cache this request */
            if(lastRegisterValues[regNum] != NULL) {
                /* realloc if needed and copy in */
                size_t newLen = strlen(value);
                if(lastRegisterValuesLen[regNum] < newLen) {
                    /* Realloc, copy, reset */
                    lastRegisterValues[regNum] = realloc(lastRegisterValues[regNum], (newLen+1) * sizeof(char));
                    lastRegisterValues[regNum] = strncpy(lastRegisterValues[regNum], value, newLen);
                    lastRegisterValues[regNum][newLen] = '\0';
                    lastRegisterValuesLen[regNum] = newLen;
                    
                }
                
            } else {
                /* set new */
                lastRegisterValues[regNum] = strdup(value);
                lastRegisterValuesLen[regNum] = strlen(value);
                
            }
        }
        
    } else {
        /* Too far! */
#ifdef LAT_DEBUG
        setCompileError("Attempted to set a register to string beyond the maximum of index 2",value,originalInput);
#else
        setCompileError();
#endif
        
    }
}

/* Register Array Value Assignment */
void writeOutRegisterArrayValueAssign(unsigned char regNum, unsigned short type, char *varName, char *key) {
    if(regNum <= 2) {
        /* Write it out */
        writeOutCode((char)OP_RGI);
        writeOutCode((char)regNum);
        writeOutCode((char)type);
        /* var name */
        writeOutCharacters(varName);
        /* key! */
        writeOutCharacters(key);
        
        /* Clear last op */
        lastRegisterOps[regNum] = 999;
        /* set type */
        lastRegisterType[regNum] = type;
        
        /* Clear any associated string as well */
        free(lastRegisterValues[regNum]),lastRegisterValues[regNum] = NULL;
        
    } else {
        /* Too far! */
#ifdef LAT_DEBUG
        setCompileError("Attempted to set a register to string beyond the maximum of index 2",varName,originalInput);
#else
        setCompileError();
#endif
        
    }
}

/* Writes out requested op code assign register (register to register) if needed */
void writeOutRegisterNumAssign(unsigned char regNum, unsigned short type, unsigned short opCode) {
    if(regNum <= 2) {
        
        if(lastRegisterOps[regNum] != opCode || lastRegisterType[regNum] != type) {
            /* Only use if not redundant */
            writeOutCode((char)OP_RGI);
            writeOutCode((char)regNum);
            writeOutCode((char)type);
            writeOutCode((char)opCode);
            
            /* Set our last used op on this register */
            lastRegisterOps[regNum] = opCode;
            
            /* set type */
            lastRegisterType[regNum] = type;
            
            /* Clear any associated string as well */
            free(lastRegisterValues[regNum]),lastRegisterValues[regNum] = NULL;
            
        }
    } else {
        /* Too far! */
#ifdef LAT_DEBUG
        setCompileError("Attempted to set a register to num beyond the maximum of index 2","",originalInput);
#else
        setCompileError();
#endif
        
    }
}

/* Used right when we're about to use OP_CALL, to check if we should simply use OP_SYSFUNC of one type or another instead, saves time on the other end */
unsigned short fetchSystemFunctionCall(char *i) {
    
    if(*i == '_' && *(i+1) == '_') {
        if(*(i+2) == 'p') {
            if(*(i+3) == 'r' && strcmp(i, "__printMem") == 0) {
                return OP_PRINT_MEM;
            } else if(*(i+3) == 'u' && strcmp(i, "__purge") == 0) {
                return OP_PURGE;
            }
            
        } else if(*(i+2) == 's' && strcmp(i, "__setGCRate") == 0) {
            return OP_GC_RATE;
        }
        
    } else if(*i == 'a' && strcmp(i, "and") == 0) {
        return OP_BITAND;
        
    } else if(*i == 'c') {
        if(*(i+1) == 'l') {
            if(strcmp(i, "closeConnection") == 0) {
                return OP_CLOSE_CONN;
            } else if(strcmp(i, "close") == 0) {
                return OP_CLOSE;
            }
            
        } else if(*(i+1) == 'a' && strcmp(i, "capture") == 0) {
            return OP_GET_CAP;
        } else if(*(i+1) == 'o' && strcmp(i, "connect") == 0) {
            return OP_CONNECT;
        }
        
    } else if(*i == 'f' && strcmp(i, "find") == 0) {
        return OP_FIND;
        
    } else if(*i == 'i' && strcmp(i, "input") == 0) {
        return OP_INPUT;
        
    } else if(*i == 'l' && strcmp(i, "load") == 0) {
        return OP_LOAD;
        
    } else if(*i == 'm' && strcmp(i, "match") == 0) {
        return OP_REG_MATCH;
        
    } else if(*i == 'n' && strcmp(i, "not") == 0) {
        return OP_BITNOT;
        
    } else if(*i == 'o') {
        if(*(i+1) == 'p' && strcmp(i, "open") == 0) {
            return OP_OPEN;
        } else if(*(i+1) == 'r' && !*(i+2)) {
            return OP_BITOR;
        }
        
    } else if(*i == 'p') {
        if(*(i+1) == 'r' && strcmp(i, "print") == 0) {
            return OP_PRINT;
        } else if(*(i+1) == 'l' && strcmp(i, "platformName") == 0) {
            return OP_PLATFORM;
        }
        
    } else if(*i == 'r') {
        if(*(i+1) == 'e') {
            if(*(i+2) == 'p' && strcmp(i, "replace") == 0) {
                return OP_REPLACE;
            } else if(*(i+2) == 'm' && strcmp(i, "remove") == 0) {
                return OP_REMOVE;
            } else if(*(i+2) == 'a') {
                if(strcmp(i, "readData") == 0) {
                    return OP_READ_DATA;
                } else if(strcmp(i, "read") == 0) {
                    return OP_READ;
                }
                
            }
            
        } else if(*(i+1) == 'a' && strcmp(i, "random") == 0) {
            return OP_RAND;
        } else if(*(i+1) == 'u' && strcmp(i, "run") == 0) {
            return OP_RUN;
        }
        
    } else if(*i == 's') {
        if(*(i+1) == 'l' && strcmp(i, "sleep") == 0) {
            return OP_SLEEP;
        } else if(*(i+1) == 'u' && strcmp(i, "substr") == 0) {
            return OP_SUBSTR;
        } else if(*(i+1) == 'e') {
            if(*(i+2) == 'e' && strcmp(i, "seed") == 0) {
                return OP_SEED_RAND;
            } else if(*(i+2) == 'n' && strcmp(i, "sendData") == 0) {
                return OP_SEND_DATA;
            }
            
        } else if(*(i+1) == 'q' && strcmp(i, "sqrt") == 0) {
            return OP_SQRT;
        } else if(*(i+1) == 't' && strcmp(i, "startServer") == 0) {
            return OP_STRT_SERV;
        }
        
    } else if(*i == 't' && strcmp(i, "time") == 0) {
        return OP_TIME;
        
    } else if(*i == 'w' && strcmp(i, "write") == 0) {
        return OP_WRITE;
        
    } else if(*i == 'x' && strcmp(i, "xor") == 0) {
        return OP_BITXOR;
        
    }
    
    /* nothing matched */
    return 0;
}

/* Special dispatch function, looks to see if our state is LATC_IF_POST or LATC_ELSEIF_POST, and dispatches as if we had following content */
void dispatchIfAndElseIf() {
    if(getCompilerState() == LATC_IF_POST) {
        /* If Done, place our final jump to exit out via */
        JumpInstance *ji = popJumpType();
        pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
        
    } else if(getCompilerState() == LATC_ELSE_POST) {
        /* Elseif Done, place our final jump to exit out via */
        JumpInstance *ji = popJumpType();
        pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
        
        /* Continue jump from prior failure, from here we have nothing more to do */
        ji = popJumpType();
        pushJumpUpdate(ji->jumpIndex, (unsigned int)getProgramByteCount());
        
    }
}

/* Gets the current compiler state */
unsigned short getCompilerState() {
    if(compilerStateIndex >= 0) {
        return _compilerState[(int)compilerStateIndex];
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Attempted to fetch a compiler state with no states on the stack.", "", originalInput);
#else
        setCompileError();
#endif
        return LATC_NO_STATE;
        
    }
}

/* (Stack peek) Returns the compiler state before this one, or null code */
unsigned short getPriorCompilerState() {
    if(compilerStateIndex >= 1) {
        return _compilerState[(int)compilerStateIndex-1];
    } else {
        return LATC_NO_STATE;
    }
}

/* (Stack Peek) Return the bottommost compiler state or a null code */
unsigned short getRootCompilerState() {
    if(compilerStateIndex >= 1) {
        return _compilerState[0];
    } else {
        return LATC_NO_STATE;
    }
}

/* Attempts to find a given parent compiler state in our stack, if it finds ONE instance of the state it will return 1, else 0 */
unsigned short findParentCompilerState(unsigned short parentState) {
    int ci = compilerStateIndex;
    while(ci >= 0) {
        if(_compilerState[ci] == parentState) {
            return 1;
        }
        ci--;
    }
    return 0;
}

/* Pushes a new compiler state */
void pushCompilerState(unsigned short state) {
    
#ifdef LAT_DEBUG
    printf("Push Compiler State:%d\n\n",compilerStateIndex+1);
#endif
    
    if(compilerStateIndex >= MAX_COMPILER_STACK_STATE-1) {
#ifdef LAT_DEBUG
        setCompileError("Compiler State Stack Overflow\n","", originalInput);
#else
        setCompileError();
#endif
        
    } else {
        compilerStateIndex++;
        _compilerState[compilerStateIndex] = state;
        #ifdef LAT_DEBUG
        printCompilerStackState();
        #endif
        
    }
}

/* Pops the last added compiler state */
unsigned short popCompilerState() {
    
#ifdef LAT_DEBUG
    printf("Pop Compiler State:%d\n\n",compilerStateIndex-1);
#endif
    
    if(compilerStateIndex >= 0) {
        compilerStateIndex--;
        #ifdef LAT_DEBUG
        printCompilerStackState();
        #endif
        return _compilerState[compilerStateIndex];
        
    } else {
#ifdef LAT_DEBUG
        setCompileError("Compiler State Stack Underflow\n","", originalInput);
#else
        setCompileError();
#endif
        return 0;
        
    }
}

/* Handles the transfer between if,else & elseif */
void handleControlFlowTransfer(unsigned char shouldFetchExitJump) {
    /* Get our continue from failure jump */
    JumpInstance *ji1 = popJumpType();
    
    if(shouldFetchExitJump == 1) {
        /* Get a potential prior exit jump to chain off of, thus giving us an exit path */
        JumpInstance *ji2 = popJumpType();
        
        if(ji2 != NULL) {
            pushJumpUpdate(ji2->jumpIndex, (unsigned int)getProgramByteCount());
            
        }
        
    }
    
    /* Exit Jump from prior success, do not execute following chained statements */
    writeOutCode((char)OP_JUMP);
    writeOut4ByteAddress(0);
    
    /* Continue jump from prior failure, from here we try the following else */
    pushJumpUpdate(ji1->jumpIndex, (unsigned int)getProgramByteCount());
    
    /* Push an incomplete jump to finish (after the ji1 update, otherwise this will overwrite it) */
    pushIncompleteJump((unsigned int)getProgramByteCount()-LAT_ADDRESS_SIZE, JUMP_TYPE_EXIT);
    
    /* increment the grouping num of all following jumps */
    incrementJumpGroup();
}

/* Frees all lexical allocations, called on compilation completion */
void freeLexicalAllocations() {
    /* Free any saved captures */
    int x = 0;
    free(__savedCapture),__savedCapture = NULL;
    
    /* Free any cached register values */
    for(;x<3;x++) {
        free(lastRegisterValues[x]),lastRegisterValues[x] = NULL;
        
    }
    
    /* free operand stack */
    free(operandStack),operandStack = NULL;
    /* free operator stack */
    free(operatorStack),operatorStack = NULL;
    /* free relational operator stack */
    free(relationalOperatorStack),relationalOperatorStack = NULL;
}

/**** DEBUG ****/

unsigned char compileError = 0;

/* Sets a compile error and */
#ifdef LAT_DEBUG
void setCompileError(char *error, char *instance, char *line) {
#else
void setCompileError() {
#endif
    #ifdef LAT_DEBUG
    printf("\n>>: %s at '%s' on line #%d\n %s\n\n",error,instance,lineNumber,line);
    #endif
    compileError = 1;
    exit(1);
}

/* */
unsigned char getCompileError() {
    return compileError;
}

/* Prints out the current stack state visually */
void printCompilerStackState() {
    #ifdef LAT_DEBUG
    int x = compilerStateIndex;
    
    printf(">Compiler States:\n");
    
    for(;x>=0;x--) {
        switch(_compilerState[x]) {
            case LATC_BOOL:
                printf("LATC_BOOL\n");
                break;
            case LATC_CALL:
                printf("LATC_CALL\n");
                break;
            case LATC_CALL_SETLAST:
                printf("LATC_CALL_SETLAST\n");
                break;
            case LATC_CALL_VAL:
                printf("LATC_CALL_VAL\n");
                break;
            case LATC_DEFFUNC_NAME:
                printf("LATC_DEFFUNC_NAME\n");
                break;
            case LATC_DEFFUNC_ARGS:
                printf("LATC_DEFFUNC_ARGS\n");
            case LATC_DEFFUNC_START:
                printf("LATC_DEFFUNC_START\n");
                break;
            case LATC_DEFFUNC_END:
                printf("LATC_DEFFUNC_END\n");
                break;
            case LATC_DONE:
                printf("LATC_DONE\n");
                break;
            case LATC_ASSIGN:
                printf("LATC_ASSIGN\n");
                break;
            case LATC_DQUOTE:
                printf("LATC_DQUOTE\n");
                break;
            case LATC_EQUALS:
                printf("LATC_EQUALS\n");
                break;
            case LATC_LPOST:
                printf("LATC_LPOST\n");
                break;
            case LATC_NUM:
                printf("LATC_NUM\n");
                break;
            case LATC_OPERATOR:
                printf("LATC_OPERATOR\n");
                break;
            case LATC_RPOST:
                printf("LATC_RPOST\n");
                break;
            case LATC_RVAL_START:
                printf("LATC_RVAL_START\n");
                break;
            case LATC_SQUOTE:
                printf("LATC_SQUOTE\n");
                break;
            case LATC_START:
                printf("LATC_START\n");
                break;
            case LATC_VAL:
                printf("LATC_VAL\n");
                break;
            case LATC_VAL_ARRAY:
                printf("LATC_VAL_ARRAY\n");
                break;
            case LATC_VAL_ARRAY_C:
                printf("LATC_VAL_ARRAY_C\n");
                break;
            case ASSIGN_GENERIC:
                printf("ASSIGN_GENERIC\n");
                break;
            case ASSIGN_NUM:
                printf("ASSIGN_NUM\n");
                break;
            case ASSIGN_STR:
                printf("ASSIGN_STR\n");
                break;
            case ASSIGN_VAR:
                printf("ASSIGN_VAR\n");
                break;
            case LATC_ARRAY_INIT:
                printf("LATC_ARRAY_INIT\n");
                break;
            case LATC_RETURN:
                printf("LATC_RETURN\n");
                break;
            case LATC_IF:
                printf("LATC_IF\n");
                break;
            case LATC_IF_START:
                printf("LATC_IF_START\n");
                break;
            case LATC_IF_END:
                printf("LATC_IF_END\n");
                break;
            case LATC_IF_POST:
                printf("LATC_IF_POST\n");
                break;
            case LATC_ELSEIF:
                printf("LATC_ELSEIF\n");
                break;
            case LATC_ELSEIF_START:
                printf("LATC_ELSEIF_START\n");
                break;
            case LATC_ELSEIF_END:
                printf("LATC_ELSEIF_END\n");
                break;
            case LATC_ELSE_POST:
                printf("LATC_ELSE_POST\n");
                break;
            case LATC_ELSE_START:
                printf("LATC_ELSE_START\n");
                break;
            case LATC_ELSE_END:
                printf("LATC_ELSE_END\n");
                break;
            case LATC_FOR_DECLARATION:
                printf("LATC_FOR_DECLARATION\n");
                break;
            case LATC_FOR_LIMIT:
                printf("LATC_FOR_LIMIT\n");
                break;
            case LATC_FOR_INCREMENT:
                printf("LATC_FOR_INCREMENT\n");
                break;
            case LATC_FOR_START:
                printf("LATC_FOR_START\n");
                break;
            case LATC_FOR_END:
                printf("LATC_FOR_END\n");
                break;
            case LATC_WHILE:
                printf("LATC_WHILE\n");
                break;
            case LATC_WHILE_END:
                printf("LATC_WHILE_END\n");
                break;
            case LATC_WHILE_START:
                printf("LATC_WHILE_START\n");
                break;
            case LATC_WHILE_PRE:
                printf("LATC_WHILE_PRE\n");
                break;
            case LATC_EXEC_END:
                printf("LATC_EXEC_END\n");
                break;
            case LATC_RETURN_NONE:
                printf("LATC_RETURN_NONE\n");
                break;
            default:
                printf("Unregnized compiler state passed to print debug function!\n");
                exit(4810);
                break;
        }
    }
    printf("\n");
    #endif
}
