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
 *  Latria_C_Lexical.h
 *  Latria
 *
 *  Created by Benjamin Friedman on 11/3/15.
 */

#ifndef Latria_Lexical_h
#define Latria_Lexical_h

#include "Latria_C_OpCodes.h"

#endif /* Latria_Lexical_h */

/**** ADDRESSING ****/

#define REGISTER_ADDRESS_SIZE 1

/**** COMPILER STATES ****/

#define LATC_START                  0x0000 /* On compiler start or new line to read */
#define LATC_ARRAY_INIT             0x0001 /* For an array initializer! */
#define LATC_EQUALS                 0x0002 /* When we have read an equals */

/* Right hand side SINGLE types */
#define LATC_SQUOTE                 0x0010 /* Single quoted Right Side string */
#define LATC_DQUOTE                 0x0012 /* Double quoted Right Side string */
#define LATC_NUM                    0x0003 /* for when we are reading a number on the right side */
#define LATC_VAL                    0x0004 /* For when we are reading */
#define LATC_BOOL                   0x0011 /* Right side boolean */

#define LATC_RPOST                  0x0013 /* For after having just matched a right side var */

#define LATC_RVAL_START             0x0005 /* For when we start reading a value on the right side (verifies no leading num followed by a character) */
#define LATC_DEFFUNC_NAME           0x0006 /* For when we read a starting @ */

#define ASSIGN_VAR                  0x0007 /* For when assigning a var */
#define ASSIGN_NUM                  0x0008 /* For when assigning a num */
#define ASSIGN_STR                  0x0016 /* For when assigning a string */
#define ASSIGN_GENERIC              0x0014 /* Generic right side value to assign (use what's on our stack) */

#define LATC_LPOST                  0x0009 /* For after we read something on the left already */
#define LATC_DONE                   0x000a /* Kills our current run */

/* Left Side Array Stuff */
#define LATC_VAL_ARRAY              0x000b /* Left Side Array reference Start */
#define LATC_NO_STATE               0x000d /* Empty State */
#define LATC_DEFFUNC_ARGS           0x000e /* Analysis of arguments provided in a function declaration */
#define LATC_VAL_ARRAY_C            0x000f /* Left Side Array reference Stop */

/* Function Call */
#define LATC_CALL                   0x000c /* Function call */
#define LATC_DEFFUNC_END            0x0017 /* Handles function closing case, looks specifically for a '}' character */
#define LATC_DEFFUNC_START          0x0018 /* For when we are expecting an opening { to continue, transcends line breaks as well */
#define LATC_CALL_VAL               0x0019 /* Function call var */
#define LATC_CALL_SETLAST           0x001a /* Function call special state, assigns last val to register and writes a PUSH to put it on the stack */

/* function return */
#define LATC_RETURN                 0x001b /* Handles returning whatever value we have computed for this item */

#define LATC_ASSIGN                 0x001c /* Handles the left hand equals work for most cases */

/* control flows */
#define LATC_IF                     0x001d /* If flow       */
#define LATC_ELSEIF                 0x001e /* Else If flow  */

/* For states */
#define LATC_FOR_DECLARATION        0x0020 /* For flow      */
#define LATC_FOR_LIMIT              0x0031 /* For limit */
#define LATC_FOR_INCREMENT          0x0032 /* For increment */
#define LATC_FOR_START              0x001f /* For Start     */
#define LATC_FOR_END                0x002e /* For end */

/* While states */
#define LATC_WHILE                  0x0021 /* While flow (implicitly the limit) */
#define LATC_WHILE_START            0x002f /* While Start */
#define LATC_WHILE_END              0x0030 /* While End */

/* if bounds */
#define LATC_IF_START               0x0022 /* if start section */
#define LATC_IF_END                 0x0023 /* if end section */
#define LATC_IF_POST                0x002c /* section that allows START/ elseif/ else to follow (pushes a new label on board if followed by else/elseif) */

/* elseif bounds */
#define LATC_ELSEIF_START           0x0024 /* elseif start */
#define LATC_ELSEIF_END             0x0025 /* elseif end */

/* else bounds */
#define LATC_ELSE_START             0x0026 /* else start */
#define LATC_ELSE_END               0x0027 /* else end */

#define LATC_ELSE_POST              0x002d /* sections that allows START/ elseif/ else to follow (used by 'elseif' and 'else') */

/* for bounds */
#define LATC_WHILE_PRE              0x0028 /* for preparing while */

#define LATC_EXEC_START             0x0029 /* execute start */
#define LATC_EXEC_END               0x002a /* execute end   */

/* while bounds */
#define LATC_RETURN_NONE            0x002b /* empty return */


/* We have an operator */
#define LATC_OPERATOR               0x0015 /* Operator found */

void analyzeInputToCompile(char *input);
char *analyzeOperator(char *input);

/* ElementStack */
typedef struct {
    char *element;
    unsigned short op;
    unsigned short precedence;
    
    unsigned short isArrayOp; /* 0=no,1=name,2=key */
}ExpressionElement;

/* State Proxy functions */
char *analyzeStart(char *in);
char *analyzeEquals(char *in);
char *analyzeRValueStart(char *in);
char *analyzeValue(char *in);
char *analyzeNum(char *in);
char *analyzeLPost(char *in);
char *analyzeRPost(char *in);
char *analyzeValArray(char *in);
char *analyzeValArrayClose(char *in);
char *analyzeFunctionCall(char *in);
char *analyzeFunctionCallArg(char *i);
char *analyzeArrayInit(char *i);
char *analyzeFunctionDefinitionName(char *i);
char *analyzeFunctionDefinitionArgs(char *i);
char *analyzeFunctionDefinitionStart(char *i);
char *analyzeFunctionDefinitionEnd(char *i);
char *analyzeReturn(char *i);
char *analyzeReturnNone(char *i);
char *analyzeAssignment(char *i);

char *analyzeIf(char *i);
char *analyzeIfStart(char *i);
char *analyzeIfEnd(char *i);
char *analyzeIfPost(char *i);

char *analyzeElseIf(char *i);
char *analyzeElseIfStart(char *i);
char *analyzeElseIfEnd(char *i);
char *analyzeElseIfPost(char *i);

char *analyzeElseStart(char *i);
char *analyzeElseEnd(char *i);

char *analyzeForDeclaration(char *i);
char *analyzeForLimit(char *i);
char *analyzeForIncrement(char *i);
char *analyzeForStart(char *i);
char *analyzeForEnd(char *i);

char *prepareWhile(char *i);
char *analyzeWhile(char *i);
char *analyzeWhileStart(char *i);
char *analyzeWhileEnd(char *i);

char *analyzeExecEnd(char *i);



/* Assign Functions */
#ifdef LAT_DEBUG
void assignNum(char *i);
void assignVar(char *i);
void assignString(char *i);
void assignGeneric(char *i);
#else
void assignNum();
void assignVar();
void assignString();
void assignGeneric();
#endif

/* Additional functions */
void exportSingleAssign(unsigned char type);
char *analyzeQuoted(char *in, char limit);
void dispatchInstructionsFromExpression(unsigned char mode);
void assignLeftHandValue(char *cutIndex);
void freeLeftHandValue();
void pushLastRegisterUsed(unsigned char registerNum);
unsigned char getLastRegisterUsedAtIndex(unsigned char index);
unsigned char getConditionalOPCode(char *i);

/* Pushing and popping from compiler state */
unsigned short getCompilerState();
unsigned short getPriorCompilerState();
unsigned short getRootCompilerState();
unsigned short findParentCompilerState(unsigned short parentState);
void pushCompilerState(unsigned short state);
unsigned short popCompilerState();

/* For writing out register string & num assigments */
void writeOutRegisterStrAssign(unsigned char regNum, unsigned short type, char *value);
void writeOutRegisterNumAssign(unsigned char regNum, unsigned short type, unsigned short opCode);
void writeOutRegisterArrayValueAssign(unsigned char regNum, unsigned short type, char *varName, char *key);

/* System Function detector */
unsigned short fetchSystemFunctionCall(char *i);

/* Expression Element Functions */
void pushOperand(char *operand, unsigned short opValue, unsigned short arrayOpStat);
ExpressionElement *popOperand();
short getOperandCount();
ExpressionElement *getLastOperand();

/* Operator Funcs */
void pushOperator(unsigned short operand, unsigned short precedence);
ExpressionElement *popOperator();
ExpressionElement *getLastOperator();

/* Relational Operator Funcs */
void pushRelationalOperator(unsigned short operand, unsigned short precedence);
ExpressionElement *popRelationalOperator();
ExpressionElement *getLastRelationalOperator();
void dispatchRelationalExpression(unsigned short nextRelOPCode);

/* Special Dispatch for closing if & elseif */
void dispatchIfAndElseIf();

/* Dispatches the current array expression (0=key as operand, 1=key as popped stack arg) */
void dispatchArrayExpression(unsigned char mode);

/* Input Processing Functions */
void resetCapturedInput(char *input);
char *getCapturedInput(char *i);

/*** Func Name Stack ***/
void pushFuncName(char *name);
char *popFuncName();
void freeFuncNames();

/* local copy */
char *localCopy(char *i);

/* Jump Group Dispatching */
void handleControlFlowTransfer(unsigned char shouldFetchExitJump);

/* debugging */
#ifdef LAT_DEBUG
void setCompileError(char *error, char *instance, char *line);
#else
void setCompileError();
#endif
unsigned char getCompileError();
void printCompilerStackState();

/* freeing */
void freeLexicalAllocations();

