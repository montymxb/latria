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
//  Latria_C_StateStack.h
//  Latria
//
//  Created by Benjamin Friedman on 11/5/15.
*/

#ifndef Latria_C_StateStack_h
#define Latria_C_StateStack_h

#include "Latria_C_Libs.h"

/* enum for jump types */
typedef enum {
    JUMP_TYPE_INCOMPLETE_JUMP,
    JUMP_TYPE_JUMP,
    JUMP_TYPE_COND_JUMP,
    JUMP_TYPE_GOTO,
    JUMP_TYPE_EXIT
} JumpType;

/* holds a jump type & the index (such as LABEL 2 or GOTO 5) */
typedef struct {
    JumpType jumpType;
    unsigned int jumpIndex;
    unsigned char *finishingSequence;
    unsigned int finishingSequenceSize;
    unsigned int jumpGroup;
} JumpInstance;

#define JUMP_STACK_SIZE 200

/* void pushGoto(unsigned int index); */
void pushForGoto(unsigned int index, unsigned char *finishingSequence, unsigned int finishingSequenceSize);
void pushIncompleteJump(unsigned int originalAddr, JumpType type);
JumpInstance *getJumpType();
JumpInstance *popJumpType();

unsigned int getJumpNum();

/*** JUMP UPDATES ***/
typedef struct {
    /* address in bytecode to modify */
    unsigned int bytecodeAddr;
    /* address to modify with */
    unsigned int jumpAddr;
}JumpUpdate;


JumpUpdate *popJumpUpdate();
void pushJumpUpdate(unsigned int bytecodeAddr, unsigned int jumpAddr);

/* Grouping Function */
void incrementJumpGroup();
unsigned int getJumpGroupNum();

/*** JUMP START STACK FUNCS ***/

/* pushes a new jump start to the jump start stack (of JumpInstances) */
void pushJumpStart(unsigned int bytecodeAddr);
/* Peeks at the value at the top of the stack */
JumpInstance *getJumpStartTop();
/* pops jump instance of the jump start stack (of JumpInstances) */
JumpInstance *popJumpStart();

void deallocStackStates();

#endif /* Latria_C_StateStack_h */
