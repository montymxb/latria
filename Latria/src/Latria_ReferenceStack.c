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
        
        if(stack->latfp != 0) {
            
            /* Make sure to properly free all of this */
            LATF_freeAllObjects(stack->latfp);
            stack->latfp = 0;
        }
        
        /* Free any func names left along, and null them out */
        if(stack->curLATFuncList != NULL) {
            
            LATDealloc(stack->curLATFuncList);
            stack->curLATFuncList = NULL;
        }
        
        /* Zero out the bracket count, just in case */
        stack->latfBrackCount = 0;
        
        /* set our return value to NULL for now, it is freed elsewhere*/
        LATDealloc(stack->returnValue);
        stack->returnValue = NULL;
        
    } else {
        
        /* Create a new reference stack */
        stack = LATAlloc(NULL, 0, sizeof(struct LATReference_Stack));
        stack->cop = NULL;
        stack->latfp = 0;
        stack->returnValue = 0;
        stack->_tailCallAssociatedName = NULL;
        stack->curLATFuncList = NULL;
        stack->latfBrackCount = 0;
    }
    
    tmp = primaryRefStack;
    stack->cs = tmp;
    primaryRefStack = stack;
}


/* Sets the core object for the primary object*/
void stack_setCoreObject(struct CoreObject *_cop) {
    
    primaryRefStack->cop = _cop;
}


/* Sets the lat function for the primary object*/
void stack_setLATFunction(struct LATFunction *_latf) {
    
    if(_latf == 0 || _latf == NULL) {
        
        primaryRefStack->latfp = 0;
    } else {
        
        primaryRefStack->latfp = _latf;
    }
}


/* Returns whether a function is being constructed or not currently */
LATBool getFunctionBlockActive() {
    
    return (primaryRefStack->curLATFuncList != NULL);
}


/* Returns the current function block name */
char * getFunctionBlockName() {
    
    return primaryRefStack->curLATFuncList;
}


/* Sets the current function block name */
void setFunctionBlockName(char *_in) {
    
    primaryRefStack->curLATFuncList = _in;
}


/* Sets the current bracket count */
void setBracketCount(unsigned int i) {
    
    primaryRefStack->latfBrackCount = i;
}


/* Returns the current bracket count */
unsigned int getBracketCount() {
    
    return primaryRefStack->latfBrackCount;
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


/* Retrieves the top level function from the primary stack item*/
struct LATFunction * stack_getLATFunction() {
    
    return primaryRefStack->latfp;
}


/* Gets the primary CoreObject for a specific stack*/
struct CoreObject *_stack_getCoreObject(struct LATReference_Stack *stack) {
    
    return stack->cop;
}


/* Gets the primary LATFunction for a specific stack*/
struct LATFunction *_stack_getLATFunction(struct LATReference_Stack *stack) {
    
    return stack->latfp;
}


/* Returns the next stack down from the passed in stack*/
struct LATReference_Stack * getNextStack(struct LATReference_Stack *stack) {
    
    return stack->cs;
}


/* Returns the primary stack */
struct LATReference_Stack * getPrimaryStack() {
    
    return primaryRefStack;
}


/* Sets the return value for this scope*/
void stack_setReturnValue(char *i) {
    
    if(primaryRefStack->returnValue != NULL) {
        
        /* We already have a return value, let's proceed appropriately */
        
        if(!strcmp( primaryRefStack->returnValue, i)) {
            
            /* DONT DO ANYTHING, these values are EXACTLY the same*/
            return;
        }
        
        LATDealloc(primaryRefStack->returnValue);
        primaryRefStack->returnValue = LATstrdup(i);
        
    } else {
        
        /* Set a NEW return value*/
        primaryRefStack->returnValue = LATstrdup(i);
    }
}


/* Sets the return value for this scope without freeing the previous item (used exclusively for reallocation to a larger size) */
void OVERRIDE_stack_setReturnValue(char *i) {
    
    primaryRefStack->returnValue = LATstrdup(i);
}


/* Gets the return value for this scope*/
char *stack_getReturnValue() {
    
    return primaryRefStack->returnValue;
}


struct CoreObject *returnArrayObj = NULL;


/* Sets this stack's return object */
void stack_setReturnObject(struct CoreObject *co) {
    
    returnArrayObj = co;
}


/* Gets the current return object on this stack */
struct CoreObject *stack_getReturnObject() {
    
    return returnArrayObj;
}


/* Sets the 'name' for this tail call to associate to*/
void setTailCallName(char *tcn) {
    
    char *pp = primaryRefStack->_tailCallAssociatedName;
    
    if(pp != NULL) {
        
        if(!strcmp( pp, tcn)) {
            
            /* DONT DO ANYTHING, these values are EXACTLY the same*/
            return;
        }
        
        LATDealloc(primaryRefStack->_tailCallAssociatedName);
    }
    
    primaryRefStack->_tailCallAssociatedName = LATstrdup(tcn);
}


/* Returns the name this tail call will associate to*/
char *getTailCallName() {
    
    return primaryRefStack->_tailCallAssociatedName;
}


/* Clears the name this tail call will associate to*/
void clearTailCallName() {
    
    if(primaryRefStack->_tailCallAssociatedName != NULL) {
        
        LATDealloc(primaryRefStack->_tailCallAssociatedName), primaryRefStack->_tailCallAssociatedName = NULL;
    }
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
    
    if(primaryRefStack->latfp != 0) {
        
        /* Make sure to properly free all of this*/
        LATF_freeAllObjects(primaryRefStack->latfp);
        primaryRefStack->latfp = 0;
    }
    
    /* Free the tail call info if applicable*/
    if(primaryRefStack->_tailCallAssociatedName != NULL)
        LATDealloc(primaryRefStack->_tailCallAssociatedName);
    
    
    /* Free return value if applicable */
    if(primaryRefStack->returnValue != NULL)
        LATDealloc(primaryRefStack->returnValue);
    
    
    /* Free any function bits hanging on */
    if(primaryRefStack->curLATFuncList != NULL)
        LATDealloc(primaryRefStack->curLATFuncList);
    
    
    tmpRS = primaryRefStack->cs;
    LATDealloc(primaryRefStack);
    primaryRefStack = tmpRS;
    
    while(primaryRefStack != 0) {
        
        if(primaryRefStack->cop != 0) {
            
            /* Pop ALL items out from our core object*/
            freeAllObjects(primaryRefStack->cop);
            primaryRefStack->cop = 0;
        }
        
        if(primaryRefStack->latfp != 0) {
            
            /* Make sure to properly free all of this*/
            LATF_freeAllObjects(primaryRefStack->latfp);
            primaryRefStack->latfp = 0;
        }
        
        
        /* Free the tail call info if applicable*/
        if(primaryRefStack->_tailCallAssociatedName != NULL)
            LATDealloc(primaryRefStack->_tailCallAssociatedName);
        
        
        /* Free return value if applicable */
        if(primaryRefStack->returnValue != NULL)
            LATDealloc(primaryRefStack->returnValue);
        
        
        /* Free any function bits hanging on */
        if(primaryRefStack->curLATFuncList != NULL)
            LATDealloc(primaryRefStack->curLATFuncList);
        
        
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
        
        if(primaryRefHeap->latfp != 0) {
            
            /* Make sure to properly free all of this*/
            LATF_freeAllObjects(primaryRefHeap->latfp);
            primaryRefHeap->latfp = 0;
        }
        
        
        /* Free the tail call info if applicable*/
        if(primaryRefHeap->_tailCallAssociatedName != NULL)
            LATDealloc(primaryRefHeap->_tailCallAssociatedName);
        
        
        /* Free return value if applicable */
        if(primaryRefHeap->returnValue != NULL)
            LATDealloc(primaryRefHeap->returnValue);
        
        
        /* Free any function bits hanging on */
        if(primaryRefHeap->curLATFuncList != NULL)
            LATDealloc(primaryRefHeap->curLATFuncList);
        
        
        tmpRS = primaryRefHeap->cs;
        LATDealloc(primaryRefHeap);
        primaryRefHeap = tmpRS;
        
        
        while(primaryRefHeap != 0) {
            
            if(primaryRefHeap->cop != 0) {
                
                /* Pop ALL items out from our core object*/
                freeAllObjects(primaryRefHeap->cop);
                primaryRefHeap->cop = 0;
            }
            
            if(primaryRefHeap->latfp != 0) {
                
                /* Make sure to properly free all of this*/
                LATF_freeAllObjects(primaryRefHeap->latfp);
                primaryRefHeap->latfp = 0;
            }
            
            
            /* Free the tail call info if applicable*/
            if(primaryRefHeap->_tailCallAssociatedName != NULL)
                LATDealloc(primaryRefHeap->_tailCallAssociatedName);
            
            
            /* Free return value if applicable */
            if(primaryRefHeap->returnValue != NULL)
                LATDealloc(primaryRefHeap->returnValue);
            
            
            /* Free any function bits hanging on */
            if(primaryRefHeap->curLATFuncList != NULL)
                LATDealloc(primaryRefHeap->curLATFuncList);
            
            
            tmpRS = primaryRefHeap->cs;
            LATDealloc(primaryRefHeap);
            primaryRefHeap = tmpRS;
        }
    }
    
    /* Finally set them out*/
    primaryRefHeap = 0;
    primaryRefStack = 0;
}
