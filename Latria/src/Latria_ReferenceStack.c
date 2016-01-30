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
 *  Latria_ReferenceStack.c
 *  Latria
 *
 *  Created by Benjamin Friedman on 2/11/15.
 */

#include "Latria_ReferenceStack.h"

/* Utilized stack*/
struct LATReference_Stack *primaryRefStack = 0;

/* Discarded Heap*/
struct LATReference_Stack *primaryRefHeap = 0;


/* Creates a new stack and puts it on the big'o stack*/
void stack_pushStack() {
    
    struct LATReference_Stack *tmp;
    struct LATReference_Stack *stack = NULL;
    
    if(primaryRefHeap != 0) {
        
        /* Pop the top item off our heap stack */
        stack = primaryRefHeap;
        /* Reset our heap to the next item */
        primaryRefHeap = primaryRefHeap->cs;
        
        /* Free any lingering items from this reference stack item */
        if(stack->cop != 0) {
            
            /* Pop ALL items out from our core object */
            freeAllObjects(stack->cop);
            stack->cop = 0;
        }
        
        
    } else {
        
        /* Create a new reference stack */
        stack = LATAlloc(NULL, 0, sizeof(struct LATReference_Stack));
        stack->cop = NULL;
    }
    
    tmp = primaryRefStack;
    stack->cs = tmp;
    primaryRefStack = stack;
}


/* Sets the core object for the primary object*/
void stack_setCoreObject(struct CoreObject *_cop) {
    
    primaryRefStack->cop = _cop;
}


/* Pops the current Stack of the stack*/
void stack_popStack() {
    
    struct LATReference_Stack *childStack = primaryRefStack->cs;
    
    if(primaryRefHeap == 0) {
        
        primaryRefHeap = primaryRefStack;
        primaryRefHeap->cs = 0;
        
    } else {
        
        struct LATReference_Stack *tmpStack = primaryRefHeap;
        primaryRefHeap = primaryRefStack;
        primaryRefHeap->cs = tmpStack;
    }
    
    primaryRefStack = childStack;
}


/* Retrieves the top level CoreObject from the primary stack item*/
struct CoreObject * stack_getCoreObject() {
    
    return primaryRefStack->cop;
}


/* Gets the primary CoreObject for a specific stack*/
struct CoreObject *_stack_getCoreObject(struct LATReference_Stack *stack) {
    
    return stack->cop;
}


/* Returns the next stack down from the passed in stack*/
struct LATReference_Stack * getNextStack(struct LATReference_Stack *stack) {
    
    return stack->cs;
}


/* Returns the primary stack */
struct LATReference_Stack * getPrimaryStack() {
    
    return primaryRefStack;
}


/*Frees the entire primary stack AND heap*/
void stack_freeHeapAndStack() {
    
    struct LATReference_Stack *tmpRS;
    
    /*////////////////////*/
    /* FREE STACK */
    /*///////////////////*/
    
    if(primaryRefStack->cop != 0) {
        
        /* Pop ALL items out from our core object*/
        freeAllObjects(primaryRefStack->cop);
        primaryRefStack->cop = 0;
    }
    
    
    tmpRS = primaryRefStack->cs;
    LATDealloc(primaryRefStack);
    primaryRefStack = tmpRS;
    
    while(primaryRefStack != 0) {
        
        if(primaryRefStack->cop != 0) {
            
            /* Pop ALL items out from our core object*/
            freeAllObjects(primaryRefStack->cop);
            primaryRefStack->cop = 0;
        }
        
        
        tmpRS = primaryRefStack->cs;
        LATDealloc(primaryRefStack);
        primaryRefStack = tmpRS;
    }
    
    
    /*////////////////////*/
    /* FREE HEAP */
    /*///////////////////*/
    
    if(primaryRefHeap != NULL) {
        
        if(primaryRefHeap->cop != 0) {
            
            /* Pop ALL items out from our core object*/
            freeAllObjects(primaryRefHeap->cop);
            primaryRefHeap->cop = 0;
        }
        
        
        tmpRS = primaryRefHeap->cs;
        LATDealloc(primaryRefHeap);
        primaryRefHeap = tmpRS;
        
        
        while(primaryRefHeap != 0) {
            
            if(primaryRefHeap->cop != 0) {
                
                /* Pop ALL items out from our core object*/
                freeAllObjects(primaryRefHeap->cop);
                primaryRefHeap->cop = 0;
            }
            
            
            tmpRS = primaryRefHeap->cs;
            LATDealloc(primaryRefHeap);
            primaryRefHeap = tmpRS;
        }
    }
    
    /* Finally set them out*/
    primaryRefHeap = 0;
    primaryRefStack = 0;
}
