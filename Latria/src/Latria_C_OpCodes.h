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
//  Latria_C_OpCodes.h
//  Latria
//
//  Created by Benjamin Friedman on 11/4/15.
*/

#ifndef Latria_C_OpCodes_h
#define Latria_C_OpCodes_h

#include "Latria_C_StateStack.h"

typedef enum {
    WRITE_MODE_NORMAL, /* normal writing */
    WRITE_MODE_BATCHED /* writes to an internal buffer and waits for user to interact with it */
} CompilerWriteMode;


/*** INSTRUCTION SEQUENCE SIZES ***/
#define HEADER_SEQUENCE_SIZE 1
#define ADDRESS_SEQUENCE_SIZE 1
#define COMMAND_SEQUENCE_SIZE 1
#define INDICATOR_SEQUENCE_SIZE 1 /* part of each sub sequence */

/*
 *  References to all of latria's op codes
 *
 */


/* Used for registers when assigning to an array ref */
#define OP_ARRAY_SET   0x01

/* sets a var following with a val or expression */
#define OP_SET         0x2b

/* defers execution to another lrac script, which, on completion returns to us */
#define OP_LOAD        0x02

/* true (for assignment) */
#define OP_TRUE        0x04
/* jump */
#define OP_JUMP        0x05
/* conditional jump */
#define OP_COND_JUMP   0x06

/* Code for a blank return */
#define OP_RETURN_NONE 0x07

/* Mark for an opening paren */
#define OP_PARENO      0x08
/* Mark for a closing paren */
#define OP_PARENC      0x09
/* function definition */
#define OP_FUNC        0x0a
/* push to our stack */
#define OP_PUSH        0x20
/* Removes something from our stack */
#define OP_POP         0x0c
/* Reference to our closure stack */
#define OP_PUSH_STACK  0x0d
/* return from function */
#define OP_RETURN      0x0e
/* print */
#define OP_PRINT       0x0f

/* returns the platform name we compiled for */
#define OP_PLATFORM    0x10

/* string find */
#define OP_FIND        0x11
/* string replace */
#define OP_REPLACE     0x12
/* substring */
#define OP_SUBSTR      0x13
/* match via regex */
#define OP_REG_MATCH   0x14
/* prints some memory info */
#define OP_PRINT_MEM   0x15
/* sleep */
#define OP_SLEEP       0x16
/* xlang execution */
#define OP_EXEC        0x17
/* flush all memory */
#define OP_PURGE       0x18
/* get random number */
#define OP_RAND        0x19
/* seed random */
#define OP_SEED_RAND   0x1a
/* read user input */
#define OP_INPUT       0x1b

/** file io **/
#define OP_OPEN        0x1c
#define OP_READ        0x1d
#define OP_WRITE       0x1e
#define OP_CLOSE       0x1f
/* deletes file from system */
#define OP_REMOVE      0x2a

/* Calls following function by name */
#define OP_CALL        0x21
/* Sets a var to NULL, unsets it */
#define OP_UNSET       0x22
/* Tells the VM to add a process from this current point */
#define OP_COROUTINE   0x23

/* math */
#define OP_SUB         0x24
#define OP_ADD         0x25
#define OP_MULTI       0x26
#define OP_DIVI        0x27
#define OP_MODULUS     0x28
#define OP_EXPONENT    0x29

/* defines break between sequences */
#define OP_FUNC_END    0x0b

/* relational comparators */
/* == */
#define OP_EQUAL       0x2c
/* && */
#define OP_AND         0x2d
/* | */
#define OP_OR          0x2e
/* < */
#define OP_LESST       0x2f
/* > */
#define OP_GREATRT     0x30
/* <= */
#define OP_LESST_EQUAL 0x31
/* >= */
#define OP_GTRTHN_EQL  0x03
/* != */
#define OP_NOT_EQUAL   0x32

/* false */
#define OP_FALSE       0x33

/* returns the current system time */
#define OP_TIME        0x34

/* array ops */
#define OP_ARRAY_VAR   0x35

/* unsets an array key value */
#define OP_UNSET_KEY   0x36

/* GC Rate */
#define OP_GC_RATE     0x37

/* var */
#define OP_VAR         0x38

/* string */
#define OP_STRING      0x39

/* num */
#define OP_NUM         0x3a

/* bool */
#define OP_BOOL        0x3b

/* EXIT */
#define OP_EXIT        0x3c

/* File */
#define OP_FILE        0x3d

/* Set Register Value Function */
#define OP_RGI         0x3e

/* Bitwise Ops */
#define OP_BITAND      0x3f
#define OP_BITOR       0x40
#define OP_BITXOR      0x41
#define OP_BITNOT      0x43

/* Pops a stack (used for functions) */
#define OP_POP_STACK   0x42

/* square root function */
#define OP_SQRT        0x44

/* Returns a capture from a prior regex (or null) */
#define OP_GET_CAP     0x45

/* Runs a line of latria code */
#define OP_RUN         0x46

/* Continue op code */
#define OP_CONTINUE    0x47

/* Break */
#define OP_BREAK       0x48

/* Start Server */
#define OP_STRT_SERV   0x49
#define OP_SEND_DATA   0x4a
#define OP_READ_DATA   0x4b
#define OP_CLOSE_CONN  0x4c
#define OP_CONNECT     0x4d


void startInstructions();
void writeOutCode(char code);
void writeOutCharacter(char code);
void writeOutCharacters(char *string);
void writeOutHexAddress(unsigned char *address, int size);
void writeRAW(unsigned char *string, int size);
void dispatchInstructions();

void setWriteMode(CompilerWriteMode mode);

unsigned char *readBatchedData();
int readBatchedDataSize();

void unsetBatchedData();
void nullifyBatchedData();

long int getProgramByteCount();

void writeOut4ByteAddress(unsigned int address);

/* forwardly declared from io */
void lat_io_compiler_writeOutput(unsigned char *output, int size);


#endif /* Latria_C_OpCodes_h */
