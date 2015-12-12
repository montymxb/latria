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
 *  Latria (Atria Lang)
 */

#include "Latria_Function.h"

/*
 * latria function 'class'
 *
 //you may declare a function with the following syntax
 @function(arg1, arg2) {
    contents...
    x = arg1
    return x
 }
 
 //and you may later call it using
 function()
 
 --can return a value
 --can pass in inputs, WILL have scope
 
 */
struct LATFunction *LATFreplacementObject = 0;

long int *revertStates = NULL;
int revertStatesMax = 0;
int revertStatesIndex = 0;

/* Adds a function revert state */
void pushFunctionRevertState(long int index) {
    if(revertStatesIndex > revertStatesMax-1) {
        if(revertStates != NULL) {
            /* realloc */
            revertStatesMax+=10;
            revertStates = LATAlloc(revertStates, (size_t)(revertStatesMax-10) * sizeof(long int), (size_t)revertStatesMax * sizeof(long int));
            
        } else {
            /* malloc */
            revertStatesMax+=10;
            revertStates = LATAlloc(NULL, 0, (size_t)revertStatesMax * sizeof(long int));
            
        }
    }
    
    /* Set this revert state */
    revertStates[revertStatesIndex] = index;
    /* bump */
    revertStatesIndex++;
    
}

/* Returns the last function revert state */
long int popFunctionRevertState() {
    return revertStates[--revertStatesIndex];
}


/* Creates a block with given key*/
struct LATFunction* createLATFunction(char *key) {
    struct LATFunction *f = LATAlloc(NULL, 0, sizeof(struct LATFunction));
    f->key = LATstrdup(key);
    f->content = 0;
    f->lobj = 0;
    f->robj = 0;
    f->coreParam = 0;
    return f;
}

/* Appends given char * to a block*/
void appendCharsToLATFunction(struct LATFunction *function, char *input) {
    int i2c;
    int i2=0;
    char *input2;
    /* LATAlloc our input +1 for a \n*/
    if(!input) return; /* for internal function stacking*/
    
    i2c = (int)strlen(input);
    input2 = LATAlloc(NULL, 0, sizeof(char)*(size_t)(i2c+2)); /* HISTORY issue with the +2 just before this, used to be +1, not quite long enough for the \0 being addded about 6 lines down, massive memory issues there*/
    
    while(i2 < i2c) {
        input2[i2] = input[i2];
        i2++;
    }
    
    input2[i2c] = '\n';
    input2[i2c+1] = '\0';
    if(function->content == 0) {
        char *ppt;
        /* memory not allocated, let's put it out there initially*/
        function->content = input2;
        ppt = function->content;
        ppt+=strlen(ppt);
        function->eol = ppt;
    } else {
        int fcLen, newLen;
        /* memory already exits,get existing size, LATAlloc to increase it up to the new desired bounds, and append our item using the Latria_Lib append function*/
        
        /* make i2c match our new size*/
        i2c+=2;
        fcLen = (int)strlen(function->content);
        
        newLen = (int)(i2c + fcLen + 1);
        function->content = LATAlloc(function->content, (size_t)fcLen * sizeof(char), (size_t)newLen * sizeof(char));
        function->eol = LATstrcat(function->content,input2);
        LATDealloc(input2), input2 = NULL;
    }
}

/* Destroys a given block*/
void destroyLATFunction(struct LATFunction *function) {
    if(function->content != 0)
        LATDealloc(function->content), function->content = 0;
    
    LATDealloc(function->key);
    LATDealloc(function);
}

/* ATOF -> double*/
/* STRTOF -> float*/

void LATF_setValue(char *name, char *val) {
    if(LAT_STRCMP(val,"null") == 0) {
        struct LATFunction *po = stack_getLATFunction();
        stack_setLATFunction(LATF_freeObjectWithKey(name, po));
    } else {
        struct LATFunction *po = stack_getLATFunction();
        LATF_setValueForKey(name, val, &po);
        stack_setLATFunction(po);
    }
}

struct LATFunction * LATF_findObject(char *key) {
    struct LATFunction *po = stack_getLATFunction();
    struct LATFunction *rez = LATF_findObjectWithKey( key, &po);
    if(rez == 0) {
        if(getPrimaryStack() != 0)
            return LATF_findObjectWithStack( key, getPrimaryStack());
        else
            return rez;
    } else
        return rez;
}

struct LATFunction * LATF_findObjectWithStack(char *key, struct LATReference_Stack *stack) {
    struct LATFunction *po = _stack_getLATFunction(stack);
    struct LATFunction *rez = LATF_findObjectWithKey( key, &po);
    if(rez == 0) {
        if(getNextStack(stack) != 0)
            return LATF_findObjectWithStack( key, getNextStack(stack));
        else
            return rez;
    } else
        return rez;
}

/* Fetches one of our tree objects*/
struct LATFunction * LATF_findObjectWithKey(char *key, struct LATFunction **node) {
    if((*node) != 0) {
        int compare = LAT_STRCMP((*node)->key,key);
        if(compare == 0)
            return *node;
        else if(compare < 0)
            return LATF_findObjectWithKey(key, &(*node)->lobj);
        else
            return LATF_findObjectWithKey(key, &(*node)->robj);
    } else
        return *node;
}

/*
 * Frees an object matching a key
 */
struct LATFunction * LATF_freeObjectWithKey(char *key, struct LATFunction *node) {
    if(node != 0) {
        if(LAT_STRCMP(node->key, key) == 0) {
            struct LATFunction *obj, *leftObj, *rightObj;
            /* get counts of nodes*/
            int nodeCount = 0;
            if(node->lobj != 0) nodeCount++;
            if(node->robj != 0) nodeCount++;
            switch(nodeCount) {
                case 0:
                    LATF_freeParams(key);
                    if(node->content != 0) LATDealloc(node->content);
                    LATDealloc(node->key);
                    LATDealloc(node);
                    return 0;
                case 1:
                    LATF_freeParams(key);
                    if(node->lobj != 0) obj = node->lobj;
                    else obj = node->robj;
                    
                    if(node->content != 0) LATDealloc(node->content);
                    LATDealloc(node->key);
                    LATDealloc(node);
                    return obj;
                case 2:
                    LATF_freeParams(key);
                    leftObj = node->lobj;
                    rightObj = LATF_generateReplacement(node->robj);
                    if(node->content != 0) LATDealloc(node->content);
                    LATDealloc(node->key);
                    LATDealloc(node);
                    node = LATFreplacementObject;
                    node->lobj = leftObj;
                    node->robj = rightObj;
                    return node;
            }
        } else {
            node->lobj = LATF_freeObjectWithKey(key, node->lobj);
            node->robj = LATF_freeObjectWithKey(key, node->robj);
        }
    }
    return node;
}

/*
 * Takes a node, and digs down all the until no left nodes are present
 * That node is copied to an EXTERNAL VARIABLE, removed, and if a right element is present it is set to the old node and returned
 * At that time it proceeds to return all changes
 */
struct LATFunction * LATF_generateReplacement(struct LATFunction *node) {
    if(node->lobj != 0) {
        /*valid, dig down one more*/
        return LATF_generateReplacement(node->lobj);
    } else {
        /* invalid, this is our node, copy it*/
        LATFreplacementObject = node;
        return (node->robj != 0)?node->robj:0;
    }
}

/*
 * Frees all objects in the tree structure (assuming program is shutting down)
 */
void LATF_freeAllObjects( struct LATFunction *node) {
    if( node != 0 ) {
        LATF_freeAllObjects(node->lobj);
        LATF_freeAllObjects(node->robj);
        
        if(node->coreParam != 0) {
            _LATF_freeParams(node->coreParam);
            node->coreParam = 0;
        }
        
        LATDealloc(node->key);
        LATDealloc(node->content);
        LATDealloc(node);
    }
}

/*
 * Sets a value for a given key
 */
void LATF_setValueForKey(char *key, char *value, struct LATFunction **node) {
    if(*node != 0) {
        int compare = LAT_STRCMP((*node)->key, key);
        if(compare == 0)
            appendCharsToLATFunction( *node, value); /* match case*/
        else if(compare < 0)
            LATF_setValueForKey(key, value, &(*node)->lobj); /* left node*/
        else if(compare > 0)
            LATF_setValueForKey(key, value, &(*node)->robj); /* right node*/
    } else {
        *node = createLATFunction(key); /*create new node in tree and set values*/
        (*node)->lobj = 0;
        (*node)->robj = 0;
        appendCharsToLATFunction( *node, value);
    }
}

/*Adds an input param for our function*/
void LATF_addParam(char *paramName, char *functionName) {
    struct LATFunction *f = LATF_findObject(functionName);
    if(f == 0) {
        /* function does NOT exist, let's create it be setting it first, THEN recovering it*/
        struct LATFunction *po = stack_getLATFunction();
        LATF_setValueForKey(functionName, NULL, &po);
        stack_setLATFunction(po);
        f = LATF_findObject(functionName);
    }
    if(f->coreParam != 0)
        _LATF_addParam( paramName, &f->coreParam);
    else {
        struct LATFParameter *param = LATAlloc( NULL, 0, sizeof(struct LATFParameter));
        param->valName = paramName;
        param->param = 0;
        f->coreParam = param;
    }
}

/*Adds an input param to a previous input param for our function*/
void _LATF_addParam(char *paramName, struct LATFParameter **parentParam) {
    if((*parentParam)->param != 0)
        _LATF_addParam( paramName, &(*parentParam)->param);
    else {
        struct LATFParameter *param = LATAlloc( NULL, 0, sizeof(struct LATFParameter));
        param->valName = paramName;
        param->param = 0;
        (*parentParam)->param = param;
    }
}

/*Gets the topmost param and returns it*/
/*
struct LATFParameter * LATF_getParam(char *functionName) {
    struct LATFunction *f = LATF_findObject(functionName);
    if(f != 0) {
        if(f->coreParam != 0)
            return f->coreParam;
        else
            return _LATF_getParam(f->coreParam);
    } else
        return 0;
}
 */

/*Gets the following param and returns it*/
/*
struct LATFParameter * _LATF_getParam(struct LATFParameter *parentParam) {
    if(parentParam->param != 0 && parentParam->param != NULL)
        return parentParam->param;
    else
        return _LATF_getParam(parentParam->param);
}
 */

/*Frees all params (called once done copying all params to the new stack)*/
void LATF_freeParams(char *functionName) {
    struct LATFunction *f = LATF_findObject(functionName);
    if(f != 0) {
        if(f->coreParam != 0) {
            _LATF_freeParams(f->coreParam);
            f->coreParam = 0;
        }
    }
}

/*Frees param from given param (if needed)*/
void _LATF_freeParams(struct LATFParameter *parentParam) {
    if(parentParam->param != 0) {
        _LATF_freeParams(parentParam->param);
    }
    /* valgrind leak patch, the two lines below were inside the if statement above, resulting in the last param block not being freed */
    LATDealloc(parentParam->valName);
    LATDealloc(parentParam);
}

/* Starts mem freeing */
void LATF_freeObjects() {
    LATDealloc(revertStates), revertStates = NULL;
    LATF_freeAllObjects(stack_getLATFunction());
    stack_setLATFunction(0);
}

void LATF_printMem() {
    int mem;
    mem = LATF__printMem(stack_getLATFunction(), 0);
    printf("\nMemory Used by Functions: %d bytes\n\n", mem);
}

int LATF__printMem(struct LATFunction *node, int mem) {
    if(node != 0) {
        char *key, *stt;
        printf("%s\n",node->key);
        mem+=sizeof(node);
        key = node->key;
        stt = node->content;
        mem+=(sizeof(key) * strlen(key));
        if(stt != 0)
            mem+=(sizeof(stt) * strlen(stt));
        else
            mem+=sizeof(stt);
        mem+=LATF__printMem(node->lobj, 0);
        mem+=LATF__printMem(node->robj, 0);
        return mem;
    } else
        return mem;
}
