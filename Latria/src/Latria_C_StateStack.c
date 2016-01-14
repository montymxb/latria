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
//  Latria_C_StateStack.c
//  Latria
//
//  Created by Benjamin Friedman on 11/5/15.
*/

#include "Latria_C_StateStack.h"

/* Standard Jump Stack */
int jumpStackIndex = -1;
JumpInstance *jumpStack = NULL;
JumpInstance *origJumpStack = NULL;

/* Jump Start Stack, for ORs */
JumpInstance *jumpStartStack = NULL;
unsigned int jumpStartStackIndex = 0;
unsigned int jumpStartStackSize = 0;

/* Jump Grouping num, allows jumps from multiple control flows to coexist on the same stack without popping off the wrong ones at the wrong time */
unsigned int jumpGroupingNum = 0;

unsigned int currentJumpNum = 0;

/* Used by for loop to push a GOTO with a finishing sequence (incrementor) that is added right before the GOTO */
void pushForGoto(unsigned int index, unsigned char *finishingSequence, unsigned int finishingSequenceSize) {
    
    if(jumpStackIndex >= JUMP_STACK_SIZE-1) {
        printf("jump stack overflow\n");
        exit(190);
        
    }
    
    if(jumpStack == NULL) {
        jumpStack = origJumpStack = malloc(sizeof(JumpInstance) * JUMP_STACK_SIZE);
    }
    
    jumpStack->jumpIndex = index;
    jumpStack->jumpType = JUMP_TYPE_GOTO;
    jumpStack->finishingSequence = finishingSequence;
    jumpStack->finishingSequenceSize = finishingSequenceSize;
    jumpStack->jumpGroup = jumpGroupingNum;
    
    jumpStack++;
    jumpStackIndex++;
    currentJumpNum++;
}

/* Adds a label to the jump stack */
void pushIncompleteJump(unsigned int originalAddr, JumpType type) {
    
    #ifdef LAT_DEBUG
    printf("Pushing Incomplete Jump # %d: %d\n\n",jumpStackIndex,originalAddr);
    #endif
    
    if(jumpStackIndex >= JUMP_STACK_SIZE-1) {
        printf("jump stack overflow\n");
        exit(190);
        
    }
    
    if(jumpStack == NULL) {
        jumpStack = origJumpStack = malloc(sizeof(JumpInstance) * JUMP_STACK_SIZE);
    }
    
    jumpStack->jumpIndex = originalAddr;
    jumpStack->jumpType = type;
    jumpStack->jumpGroup = jumpGroupingNum;
    
    jumpStack++;
    jumpStackIndex++;
    currentJumpNum++;
}

/* Returns the top most value on the stack without popping it, a regular jump to note as well */
JumpInstance *getJumpType() {
    if(jumpStackIndex >= 0) {
        return jumpStack-1;
    } else {
        return NULL;
    }
}

/* Pops a value off the stack and returns it */
JumpInstance *popJumpType() {
    
    if(jumpStackIndex < 0) {
        return NULL;
        
    }
    
    jumpStackIndex--;
    jumpStack--;
    
    return jumpStack;
}

/* Returns the current jump index */
unsigned int getJumpNum() {
    return currentJumpNum;
}

/* Jump Update Stack */
JumpUpdate *jumpUpdateStack = NULL;
int jumpUpdateStackIndex = 0;
int jumpUpdateStackSize = 0;

JumpUpdate *popJumpUpdate() {
    if(jumpUpdateStackIndex > 0) {
        return &jumpUpdateStack[jumpUpdateStackIndex--];
    } else {
        return NULL;
    }
}

void pushJumpUpdate(unsigned int bytecodeAddr, unsigned int jumpAddr) {
    jumpUpdateStackIndex++;
    if(jumpUpdateStackIndex >= jumpUpdateStackSize) {
        /* Too large */
        if(jumpUpdateStack != NULL) {
            /* realloc */
            jumpUpdateStackSize += JUMP_UPDATE_INCREMENT;
            jumpUpdateStack = realloc(jumpUpdateStack, sizeof(JumpUpdate) * (unsigned long)jumpUpdateStackSize);
            
        } else {
            /* malloc */
            jumpUpdateStackSize = JUMP_UPDATE_INCREMENT;
            jumpUpdateStack = malloc(sizeof(JumpUpdate) * JUMP_UPDATE_INCREMENT);
            
        }
    }
    
    /* Add it */
    jumpUpdateStack[jumpUpdateStackIndex].bytecodeAddr = bytecodeAddr;
    jumpUpdateStack[jumpUpdateStackIndex].jumpAddr = jumpAddr;
}


/* Pushes a new jump start to the jump start stack (of JumpInstances) */
void pushJumpStart(unsigned int bytecodeAddr) {
    jumpStartStackIndex++;
    if(jumpStartStackIndex >= jumpStartStackSize) {
        /* Too large */
        if(jumpStartStack != NULL) {
            /* malloc */
            jumpStartStackSize = JUMP_UPDATE_INCREMENT;
            jumpStartStack = malloc(sizeof(JumpInstance) * JUMP_UPDATE_INCREMENT);
            
        } else {
            /* realloc */
            jumpStartStackSize += JUMP_UPDATE_INCREMENT;
            jumpStartStack = realloc(jumpStartStack, sizeof(JumpInstance) * jumpStartStackSize);
            
        }
    }
    
    /* Add it */
    jumpStartStack[jumpStartStackIndex].jumpIndex = bytecodeAddr;
    jumpStartStack[jumpStartStackIndex].jumpGroup = jumpGroupingNum;
}

/* Look at the top value without popping it */
JumpInstance *getJumpStartTop() {
    if(jumpStartStackIndex > 0) {
        return &jumpStartStack[jumpStartStackIndex];
    } else {
        return NULL;
    }
}

/* Pops jump instance of the jump start stack (of JumpInstances) */
JumpInstance *popJumpStart() {
    if(jumpStartStackIndex > 0) {
        return &jumpStartStack[jumpStartStackIndex--];
    } else {
        return NULL;
    }
}

/* Increments the current jump group */
void incrementJumpGroup() {
    jumpGroupingNum++;
}

/* Gets the jump grouping num, for seeing where the current grouping is at */
unsigned int getJumpGroupNum() {
    return jumpGroupingNum;
}

/* Deallocates all stacks and related objects */
void deallocStackStates() {
    free(origJumpStack), origJumpStack = NULL;
    free(jumpStartStack), jumpStartStack = NULL;
    free(jumpUpdateStack), jumpUpdateStack = NULL;
}

