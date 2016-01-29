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

#include "Latria_Vars.h"

struct CoreObject *replacementObject = 0;
struct CoreObject *coreObjectHeap = 0;
int coreObjectHeapSize = 0;

/*ATOF -> double*/
/*STRTOF -> float*/

void setValue(char *name, char *val) {
    struct CoreObject *co = stack_getCoreObject();
    /* Set a regular object value */
    setValueForKey(name, val, &co);
    /* Update the stack here */
    stack_setCoreObject(co);
    
}

/* Frees a value */
void unsetValue(char *name) {
    struct CoreObject *co = stack_getCoreObject();
    co = freeObjectWithKey(name, co);
    stack_setCoreObject(co);
    
}

/* Frees a given array's key value combo */
void unsetArrayKeyValue(char *varName, char *key) {
    struct CoreObject *co = stack_getCoreObject();
    co = freeArrayObjectWithNameKey(varName,key,co);
}

/* Sets the value to a boolean specifically, doesn't free, just sets.
Freeing can be done via the same key and a standard set */
void setBoolValue(char *name, LATBool val) {
    struct CoreObject *co = stack_getCoreObject();
    setBoolValueForKey(name, val, &co);
    stack_setCoreObject(co);
}

/* Sets the array key to a boolean specifically */
void setArrayBoolValue(char *varName, char *key, LATBool val) {
    struct CoreObject *co = stack_getCoreObject();
    setBoolValueForArrayKey(varName, key, val, &co);
    stack_setCoreObject(co);
}

/* Goes over each item and attempts to find a matching array and existing key value, otherwise it creates a new one */
void setBoolValueForArrayKey(char *varName, char *key, LATBool value, struct CoreObject **_node) {
    if((*_node) != NULL) { /* (*node) */
        int compare = LAT_STRCMP((*_node)->key,varName);
        if(compare == 0 && (*_node)->_state == 3) {
            /* now dig in until we find our key, return NULL on failure */
            LATBool didUpdate = false;
            struct CoreObject *tmpObject = *_node;
            /* Bump along until we find a value or NULL out */
            while((tmpObject = tmpObject->lobj) != NULL) {
                
                /* Check for a match */
                if(LAT_STRCMP( tmpObject->key, key) == 0) {
                    
                    /* Update our matched object */
                    setBoolValueForKey(key, value, _node);
                    
                    didUpdate = true;
                    
                    break;
                }
            }
            
            /* Check to see if we updated */
            if(!didUpdate) {
                struct CoreObject *tmptmp;
                /* If we did not update, make a new core object to add in */
                struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
                
                (*_newNode).lobj = NULL;
                (*_newNode).robj = NULL;
                (*_newNode).key = LATstrdup(key);
                (*_newNode)._state = 2;
                (*_newNode).data._bvalue = value;
                
                /* Set this as the FIRST array element, set all others as it's children */
                tmptmp = (*_node)->lobj;
                (*_node)->lobj = _newNode;
                _newNode->lobj = tmptmp;
            }
            
            /* Done */
            
        } else if(compare == 0 && (*_node)->_state != 3) {
            /* This item exists but is NOT an array, let's repurpose it */
            
            /* Create a new key value in our array */
            struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
            (*_newNode).lobj = NULL;
            (*_newNode).robj = NULL;
            (*_newNode).key = LATstrdup(key);
            (*_newNode)._state = 2;
            (*_newNode).data._bvalue = value;
            
            /* repurpose the existing obj */
            _setArrayObjectForKey(varName, _newNode, &(*_node));
            
        } else if(compare < 0) {
            if((*_node)->_state != 3) {
                /* Try left node, normally */
                setBoolValueForArrayKey(varName, key, value, &(*_node)->lobj);
                
            } else {
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                setBoolValueForArrayKey(varName, key, value, &(*_node)->robj);
                
            }
        } else {
            setBoolValueForArrayKey(varName, key, value, &(*_node)->lobj);
            
        }
    } else {
        /* Create a new key value in our new array */
        struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
        (*_newNode).lobj = NULL;
        (*_newNode).robj = NULL;
        (*_newNode).key = LATstrdup(key);
        (*_newNode)._state = 2;
        (*_newNode).data._bvalue = value;
        
        /* Create our new array */
        _setArrayObjectForKey(varName, _newNode, &(*_node));
        
    }
}

/* Sets a key to a file value, doesn't free, just sets. Freeing can be done via same key and simply assigning to null */
void setFileValue(char *name, FILE *val) {
    struct CoreObject *co = stack_getCoreObject();
    setFileValueForKey(name, val, &co);
    stack_setCoreObject(co);
}

/* Sets an array key to a file value */
void setArrayFileValue(char *varName, char *key, FILE *val) {
    struct CoreObject *co = stack_getCoreObject();
    setFileValueForArrayKey(varName, key, val, &co);
    stack_setCoreObject(co);
}

/* Finds and sets an array key to a file value inside an existing array */
void setFileValueForArrayKey(char *varName, char *key, FILE *value, struct CoreObject **_node) {
    if((*_node) != NULL) { /* (*node)*/
        int compare = LAT_STRCMP((*_node)->key,varName);
        if(compare == 0 && (*_node)->_state == 3) {
            /* now dig in until we find our key, return NULL on failure */
            LATBool didUpdate = false;
            struct CoreObject *tmpObject = *_node;
            /* Bump along until we find a value or NULL out */
            while((tmpObject = tmpObject->lobj) != NULL) {
                
                /* Check for a match */
                if(LAT_STRCMP( tmpObject->key, key) == 0) {
                    
                    /* Update our matched object */
                    setFileValueForKey(key, value, _node);
                    
                    didUpdate = true;
                    
                    break;
                }
            }
            
            /* Check to see if we updated */
            if(!didUpdate) {
                struct CoreObject *tmptmp;
                /* If we did not update, make a new core object to add in */
                struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
                
                (*_newNode).lobj = NULL;
                (*_newNode).robj = NULL;
                (*_newNode).key = LATstrdup(key);
                (*_newNode).data._file = value;
                (*_newNode)._state = 4;
                
                /* Set this as the FIRST array element, set all others as it's children */
                tmptmp = (*_node)->lobj;
                (*_node)->lobj = _newNode;
                _newNode->lobj = tmptmp;
            }
            
            /* Done */
            
        } else if(compare == 0 && (*_node)->_state != 3) {
            /* This item exists but is NOT an array, let's repurpose it */
            
            /* Create a new key value in our array */
            struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
            (*_newNode).lobj = NULL;
            (*_newNode).robj = NULL;
            (*_newNode).key = LATstrdup(key);
            (*_newNode)._state = 4;
            (*_newNode).data._file = value;
            
            /* repurpose the existing obj */
            _setArrayObjectForKey(varName, _newNode, &(*_node));
            
        } else if(compare < 0) {
            if((*_node)->_state != 3) {
                /* Try left node, normally */
                setFileValueForArrayKey(varName, key, value, &(*_node)->lobj);
                
            } else {
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                setFileValueForArrayKey(varName, key, value, &(*_node)->robj);
                
            }
        } else {
            setFileValueForArrayKey(varName, key, value, &(*_node)->lobj);
            
        }
    } else {
        /* Create a new key value in our new array */
        struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
        (*_newNode).lobj = NULL;
        (*_newNode).robj = NULL;
        (*_newNode).key = LATstrdup(key);
        (*_newNode)._state = 4;
        (*_newNode).data._file = value;
        
        /* Create our new array */
        _setArrayObjectForKey(varName, _newNode, &(*_node));
        
    }
}

/* Sets a value to an array */
void setArrayValue(char *varName, char *key, char *value) {
    struct CoreObject *co = stack_getCoreObject();
    setArrayValueForKey(varName, key, value, &co);
    stack_setCoreObject(co);
}

/*
 * Returns the associated CoreObject or NULL
 */
struct CoreObject * getValue(char *name) {
    struct CoreObject *co = stack_getCoreObject();
    struct CoreObject *obj = NULL;
    
    /* Find an object value */
    obj = findObjectWithKey(name, &co);
    
    if(obj != NULL) {
        return obj;
    } else {
        if(getPrimaryStack() != 0)
            return _getValue( name, getPrimaryStack());
        else
            return NULL;
    }
}

/* Returns the associated array CoreObject or NULL */
struct CoreObject * getArrayValue(char *varName, char *key) {
    struct CoreObject *co = stack_getCoreObject();
    struct CoreObject *obj = NULL;
    obj = findArrayObjectWithVarKey(varName, key, &co);
    
    if(obj != NULL) {
        return obj;
        
    } else {
        if(getPrimaryStack() != 0) {
            return _getArrayValue(varName, key, getPrimaryStack());
            
        } else {
            return NULL;
            
        }
    }
}

/* Checks to see if there are any other matching vars in any other stacks in our stack */
struct CoreObject * _getValue(char *name, struct LATReference_Stack *stack) {
    struct CoreObject *co = _stack_getCoreObject(stack);
    struct CoreObject *obj = NULL;
    
    /* Find an obj value */
    obj = findObjectWithKey(name, &co);
    
    if(obj != NULL && co != NULL) {
        return obj;
    } else {
        struct LATReference_Stack *nxtStack = getNextStack(stack);
        if(nxtStack != 0)
            return _getValue( name, nxtStack);
        else
            return NULL;
    }
}

struct CoreObject * _getArrayValue(char *varName, char *key, struct LATReference_Stack *stack) {
    struct CoreObject *co = _stack_getCoreObject(stack);
    struct CoreObject *obj = NULL;
    obj = findArrayObjectWithVarKey(varName, key, &co);
    
    if(obj != NULL && co != NULL) {
        return obj;
    } else {
        struct LATReference_Stack *nxtStack = getNextStack(stack);
        if(nxtStack != 0)
            return _getArrayValue(varName, key, nxtStack);
        else
            return NULL;
    }
}

/* Fetches one of our tree objects */
struct CoreObject * findObjectWithKey(char *key, struct CoreObject **node) {
    if((*node) != NULL) { /* (*node) */
        int compare = LAT_STRCMP((*node)->key,key);
        if(compare == 0) {
            return *node;
        } else if(compare < 0) {
            if((*node)->_state != 3) {
                /* Try left node, normally */
                return findObjectWithKey(key, &(*node)->lobj);
            } else {
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                return findObjectWithKey(key, &(*node)->robj);
            }
        } else {
            return findObjectWithKey(key, &(*node)->robj);
        }
    } else
        return *node; /* *node*/
}

/* Fetches one of our Array Tree objects */
struct CoreObject * findArrayObjectWithVarKey(char *varName, char *key, struct CoreObject **node) {
    if((*node) != NULL) { /* (*node)*/
        int compare = LAT_STRCMP((*node)->key,varName);
        if(compare == 0 && (*node)->_state == 3) {
            struct CoreObject *tmpNode = (*node);
            /* Now dig in until we find our key, return NULL on failure */
            /* Bump along until we find a value or NULL out */
            
            while((tmpNode = tmpNode->lobj) != NULL) {
                /* Check for a match */
                if(LAT_STRCMP( tmpNode->key, key) == 0) {
                    
                    /* Return the match */
                    return tmpNode;
                }
            }
            
            /* No match, return NULL */
            return NULL;
            
        } else if(compare < 0) {
            if((*node)->_state != 3) {
                /* Try left node, normally */
                return findArrayObjectWithVarKey(varName, key, &(*node)->lobj);
                
            } else {
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                return findArrayObjectWithVarKey(varName, key, &(*node)->robj);
                
            }
        } else {
            /* Try right node */
            return findArrayObjectWithVarKey(varName, key, &(*node)->robj);
            
        }
    } else
        return *node; /* *node*/
}

/* Finds & Updates one of our Array Tree objects */
void setArrayValueForKey(char *varName, char *key, char *value, struct CoreObject **node) {
    if((*node) != NULL) { /* (*node)*/
        int compare = LAT_STRCMP((*node)->key, varName);
        if(compare == 0 && (*node)->_state == 3) {
            /* now dig in until we find our key, return NULL on failure */
            LATBool didUpdate = false;
            
            struct CoreObject *tmpObject = *node;
            /* Bump along until we find a value or NULL out */
            while((tmpObject = tmpObject->lobj) != NULL) {
                
                /* Check for a match */
                if(strcmp( tmpObject->key, key) == 0) {
                    
                    /* Update our matched object */
                    _updateCoreObjectWithVal( &tmpObject, value);
                    
                    didUpdate = true;
                    
                    break;
                }
            }
            
            /* Check to see if we updated */
            if(!didUpdate) {
                /* If we did not update, make a new core object to add in */
                struct CoreObject *tmptmp;
                struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
                
                /* Set this as the FIRST array element, set all others as it's children */
                tmptmp = (*node)->lobj;
                (*node)->lobj = _newNode;
                
                _constructCoreObjectWithKeyVal(&_newNode, key, value);
                _newNode->lobj = tmptmp;
                
            }
            
            /* Done */
            
        } else if(compare == 0 && (*node)->_state != 3) {
            /* This item exists but is NOT an array, let's repurpose it */
            
            /* Create a new key value in our array */
            struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
            
            _constructCoreObjectWithKeyVal(&_newNode, key, value);
            
            /* repurpose the existing obj */
            _setArrayObjectForKey(varName, _newNode, &(*node));
            
        } else if(compare < 0) {
            if((*node)->_state != 3) {
                /* Try left node, normally */
                setArrayValueForKey(varName, key, value, &(*node)->lobj);
                
            } else {
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                setArrayValueForKey(varName, key, value, &(*node)->robj);
                
            }
        } else {
            setArrayValueForKey(varName, key, value, &(*node)->robj);
            
        }
    } else {
        /* Create a new key value in our new array */
        struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
        
        _constructCoreObjectWithKeyVal(&_newNode, key, value);
        
        /* Create our new array */
        _setArrayObjectForKey(varName, _newNode, &(*node));
        
    }
}

/* Sets a CoreObject matching a key to a boolean value*/
void setBoolValueForKey(char *key, LATBool value, struct CoreObject **_node) {
    if((*_node) != NULL) {
        int compare = LAT_STRCMP((*_node)->key, key);
        if(compare == 0) {
            /* clear out any previous string value if present*/
            if((*_node)->_state == 1) {
                LATDealloc((*_node)->data._svalue);
            } else if((*_node)->_state == 3) {
                freeAllObjects((*_node)->lobj);
                (*_node)->lobj = NULL;
            } else if((*_node)->_state == 4) {
                /* Active file, close & null */
                /*fclose((*_node)->data._file);*/
                (*_node)->data._file = NULL;
            } else if((*_node)->_state == 0 || (*_node)->_state == 2) {
                /* Number or Bool, do nothing */
                
            } else {
                printf("Unrecognized data type found as core object: %d\n", (*_node)->_state);
                exit(1);
                
            }
            
            /* set bool value*/
            (*_node)->data._bvalue = value;
            /* set state*/
            (*_node)->_state = 2;
            
        } else if(compare < 0) {
            if((*_node)->_state != 3) {
                /* Try left node, normally */
                setBoolValueForKey(key, value, &(*_node)->lobj);
            } else {
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                setBoolValueForKey(key, value, &(*_node)->robj);
            }
        } else {
            /* Try right node*/
            setBoolValueForKey(key, value, &(*_node)->robj);
        }
    } else {
        /*create new node in tree and set values*/
        (*_node) = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
        /*zero out child nodes*/
        (*_node)->lobj = NULL;
        (*_node)->robj = NULL;
        
        (*_node)->key = LATstrdup(key);
        
        /* set boolean value*/
        (*_node)->data._bvalue = value;
        /* set state value*/
        (*_node)->_state = 2;
    }
}

/* Called to set a CoreObject directly as the LEFT component of an existing OR new CO */
void setArrayObject(char *name, struct CoreObject *array) {
    struct CoreObject *co = stack_getCoreObject();
    _setArrayObjectForKey(name, array, &co);
    stack_setCoreObject(co);
}

/* Digs down until an existing or new object is created, then sets the left CO to the array passed */
void _setArrayObjectForKey(char *key, struct CoreObject *array, struct CoreObject **_node) {
    if((*_node) != NULL) {
        int compare = LAT_STRCMP((*_node)->key, key);
        if(compare == 0) {
            /* clear out any previous string value if present*/
            if((*_node)->_state == 1) {
                LATDealloc((*_node)->data._svalue);
            } else if((*_node)->_state == 3) {
                freeAllObjects((*_node)->lobj);
                (*_node)->lobj = NULL;
            } else if((*_node)->_state == 4) {
                /* Active file, close & null */
                /*fclose((*_node)->data._file);*/
                (*_node)->data._file = NULL;
            } else if((*_node)->_state == 0 || (*_node)->_state == 2) {
                /* Number or Bool, do nothing */
                
            } else {
                printf("Unrecognized data type found as core object: %d\n", (*_node)->_state);
                exit(1);
                
            }
            
            /* LOBJ relocation logic */
            if((*_node)->lobj != NULL) {
                /* We have Items we need to relocate */
                if((*_node)->robj == NULL) {
                    /* Nothing on the right branch, shift LEFT to RIGHT */
                    (*_node)->robj = (*_node)->lobj;
                } else {
                    /* We need to relocate our left leaf further down the chain on the right one */
                    struct CoreObject *nxtObj = (*_node)->robj;
                    while(nxtObj) {
                        int lCompare = LAT_STRCMP((*_node)->lobj->key, nxtObj->key);
                        if(lCompare < 0) {
                            /* Try setting left */
                            if(nxtObj->lobj == NULL) {
                                nxtObj->lobj = (*_node)->lobj;
                                break;
                            } else {
                                nxtObj = nxtObj->lobj;
                            }
                        } else {
                            /* Try setting right */
                            if(nxtObj->robj == NULL) {
                                nxtObj->robj = (*_node)->lobj;
                                break;
                            } else {
                                nxtObj = nxtObj->robj;
                            }
                        }
                    }
                }
            }
            
            /* set array obj */
            (*_node)->lobj = array;
            /* set state */
            (*_node)->_state = 3;
            
        } else if(compare < 0) {
            if((*_node)->_state != 3) {
                /* Try left node, normally */
                _setArrayObjectForKey(key, array, &(*_node)->lobj);
            } else {
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                _setArrayObjectForKey(key, array, &(*_node)->robj);
            }
        } else {
            /* Try right node*/
            _setArrayObjectForKey(key, array, &(*_node)->robj);
        }
    } else {
        /*create new node in tree and set values*/
        (*_node) = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
        /*zero out child nodes*/
        (*_node)->lobj = NULL;
        (*_node)->robj = NULL;
        
        (*_node)->key = LATstrdup(key);
        
        /* set array obj */
        (*_node)->lobj = array;
        /* set state */
        (*_node)->_state = 3;
    }
    
}

/* Sets a CoreObject matching a key to a file value */
void setFileValueForKey(char *key, FILE *value, struct CoreObject **_node) {
    if((*_node) != NULL) {
        int compare = LAT_STRCMP((*_node)->key, key);
        if(compare == 0) {
            /* clear out any previous string value if present*/
            if((*_node)->_state == 1) {
                LATDealloc((*_node)->data._svalue);
            } else if((*_node)->_state == 3) {
                freeAllObjects((*_node)->lobj);
                (*_node)->lobj = NULL;
            } else if((*_node)->_state == 4) {
                /* Active file, close & null */
                if((*_node)->data._file != value) {
                    
                    /* (FORMER) Non-conflicting files, safe to close */
                    
                    #pragma message("files must be closed manually (they are not garbage collected) or they will be leaked out. This is a courtesy notice.")
                    /*fclose((*_node)->data._file);*/
                    
                    (*_node)->data._file = NULL;
                    
                }
            } else if((*_node)->_state == 0 || (*_node)->_state == 2) {
                /* Number or Bool, do nothing */
                
            } else {
                printf("Unrecognized data type found as core object: %d\n", (*_node)->_state);
                exit(1);
                
            }
            
            /* set file value*/
            (*_node)->data._file = value;
            /* set state */
            (*_node)->_state = 4;
            
        } else if(compare < 0) {
            if((*_node)->_state != 3) {
                /* Try left node, normally */
                setFileValueForKey(key, value, &(*_node)->lobj);
            } else {
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                setFileValueForKey(key, value, &(*_node)->robj);
            }
        } else {
            /* Try right node*/
            setFileValueForKey(key, value, &(*_node)->robj);
        }
    } else {
        (*_node) = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values*/
        /* zero out child nodes */
        (*_node)->lobj = NULL;
        (*_node)->robj = NULL;
        
        (*_node)->key = LATstrdup(key);
        
        /* set file value */
        (*_node)->data._file = value;
        /* set state value */
        (*_node)->_state = 4;
    }
}

/* Sets a connection value to a variable */
void setConnectionValue(char *name, int val) {
    struct CoreObject *co = stack_getCoreObject();
    setConnectionValueForKey(name, val, &co);
    stack_setCoreObject(co);
}

/* Sets a connection value for a given key */
void setConnectionValueForKey(char *name, int val, struct CoreObject **_node) {
    if((*_node) != NULL) {
        int compare = LAT_STRCMP((*_node)->key, name);
        if(compare == 0) {
            /* clear out any previous string value if present*/
            if((*_node)->_state == 1) {
                LATDealloc((*_node)->data._svalue);
                
            } else if((*_node)->_state == 3) {
                freeAllObjects((*_node)->lobj);
                (*_node)->lobj = NULL;
                
            } else if((*_node)->_state == 4) {
                /* Active file, close & null */
                /*fclose((*_node)->data._file);*/
                (*_node)->data._file = NULL;
                
            } else if((*_node)->_state == 0 || (*_node)->_state == 2) {
                /* Number or Bool, do nothing */
                
            } else {
                printf("Unrecognized data type found as core object: %d\n", (*_node)->_state);
                exit(1);
                
            }
            
            /* set connection value */
            (*_node)->data._fvalue = val;
            /* set state */
            (*_node)->_state = 5;
            
        } else if(compare < 0) {
            if((*_node)->_state != 3) {
                /* Try left node, normally */
                setConnectionValueForKey(name, val, &(*_node)->lobj);
            } else {
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                setConnectionValueForKey(name, val, &(*_node)->robj);
            }
        } else {
            /* Try right node*/
            setConnectionValueForKey(name, val, &(*_node)->robj);
        }
    } else {
        /*create new node in tree and set values*/
        (*_node) = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
        /*zero out child nodes*/
        (*_node)->lobj = NULL;
        (*_node)->robj = NULL;
        
        (*_node)->key = LATstrdup(name);
        
        /* set connection value*/
        (*_node)->data._fvalue = val;
        /* set state value*/
        (*_node)->_state = 5;
    }
    
}

/* Sets a connection value to a key of an array */
void setArrayConnectionValue(char *varName, char *key, int val) {
    struct CoreObject *co = stack_getCoreObject();
    setConnectionValueForArrayKey(varName, key, val, &co);
    stack_setCoreObject(co);
}

/* Sets a connection value to a given key of an array */
void setConnectionValueForArrayKey(char *varName, char *key, int val, struct CoreObject **_node) {
    if((*_node) != NULL) { /* (*node)*/
        int compare = LAT_STRCMP((*_node)->key,varName);
        if(compare == 0 && (*_node)->_state == 3) {
            /* now dig in until we find our key, return NULL on failure */
            LATBool didUpdate = false;
            struct CoreObject *tmpObject = *_node;
            /* Bump along until we find a value or NULL out */
            while((tmpObject = tmpObject->lobj) != NULL) {
                
                /* Check for a match */
                if(LAT_STRCMP( tmpObject->key, key) == 0) {
                    /* Update our matched object */
                    setConnectionValueForKey(key, val, _node);
                    didUpdate = true;
                    break;
                    
                }
            }
            
            /* Check to see if we updated */
            if(!didUpdate) {
                struct CoreObject *tmptmp;
                /* If we did not update, make a new core object to add in */
                struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
                
                (*_newNode).lobj = NULL;
                (*_newNode).robj = NULL;
                (*_newNode).key = LATstrdup(key);
                (*_newNode).data._fvalue = val;
                (*_newNode)._state = 5;
                
                /* Set this as the FIRST array element, set all others as it's children */
                tmptmp = (*_node)->lobj;
                (*_node)->lobj = _newNode;
                _newNode->lobj = tmptmp;
            }
            
            /* Done */
            
        } else if(compare == 0 && (*_node)->_state != 3) {
            /* This item exists but is NOT an array, let's repurpose it */
            
            /* Create a new key value in our array */
            struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
            (*_newNode).lobj = NULL;
            (*_newNode).robj = NULL;
            (*_newNode).key = LATstrdup(key);
            (*_newNode)._state = 5;
            (*_newNode).data._fvalue = val;
            
            /* repurpose the existing obj */
            _setArrayObjectForKey(varName, _newNode, &(*_node));
            
        } else if(compare < 0) {
            if((*_node)->_state != 3) {
                /* Try left node, normally */
                setConnectionValueForArrayKey(varName, key, val, &(*_node)->lobj);
                
            } else {
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                setConnectionValueForArrayKey(varName, key, val, &(*_node)->robj);
                
            }
        } else {
            setConnectionValueForArrayKey(varName, key, val, &(*_node)->lobj);
            
        }
    } else {
        /* Create a new key value in our new array */
        struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
        (*_newNode).lobj = NULL;
        (*_newNode).robj = NULL;
        (*_newNode).key = LATstrdup(key);
        (*_newNode)._state = 5;
        (*_newNode).data._fvalue = val;
        
        /* Create our new array */
        _setArrayObjectForKey(varName, _newNode, &(*_node));
        
    }
}

/*
 * Sets a value for a given key
 */
char setValueForKey(char *key, char *value, struct CoreObject **_node) {
    if((*_node) != NULL) {
        int compare = LAT_STRCMP((*_node)->key, key);
        if(compare == 0) {
            /* Update an existing core object */
            _updateCoreObjectWithVal( _node, value);
            return 0;
        } else if(compare < 0) {
            if((*_node)->_state != 3) {
                /* Try left node, normally */
                return setValueForKey(key, value, &(*_node)->lobj);
            } else {
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                return setValueForKey(key, value, &(*_node)->robj);
            }
        } else {
            /* Try right node*/
            return setValueForKey(key, value, &(*_node)->robj);
        }
    } else {
        /* Create new node in tree and set values */
        (*_node) = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
        return _constructCoreObjectWithKeyVal( &(*_node), key, value);
    }
}

/* Updates an existing CoreObject with the given key/value combination */
void _updateCoreObjectWithVal(struct CoreObject **node, char *value) {
    
    /* If we're reassigning AND we have an array object, flush the array's contents (it may be empty in which case this finishes near instantly). */
    if((*node)->_state == 3) {
        freeAllObjects((*node)->lobj);
        (*node)->lobj = NULL;
    } else if((*node)->_state == 4) {
        /* Active file, close & null */
        /*fclose((*node)->data._file);*/
        (*node)->data._file = NULL;
    }
    
    /*
    if(*value == '{' && *(value+strlen(value)-1) == '}') {
        // Tis an array!
        
        // clear out the string if needed
        if((*node)->_state == 1)
            LATDealloc((*node)->data._svalue);
        
        // we have an array initializer here (meta!)
        (*node)->_state = 3;
        
        
        // LOBJ relocation logic
        if((*node)->lobj != NULL) {
            // We have Items we need to relocate
            if((*node)->robj == NULL) {
                // Nothing on the right branch, shift LEFT to RIGHT
                (*node)->robj = (*node)->lobj;
            } else {
                // We need to relocate our left leaf further down the chain on the right one
                struct CoreObject *nxtObj = (*node)->robj;
                while(nxtObj) {
                    int lCompare = LAT_STRCMP((*node)->lobj->key, nxtObj->key);
                    if(lCompare < 0) {
                        // Try setting left
                        if(nxtObj->lobj == NULL) {
                            nxtObj->lobj = (*node)->lobj;
                            break;
                        } else {
                            nxtObj = nxtObj->lobj;
                        }
                    } else {
                        // Try setting right
                        if(nxtObj->robj == NULL) {
                            nxtObj->robj = (*node)->lobj;
                            break;
                        } else {
                            nxtObj = nxtObj->robj;
                        }
                    }
                }
            }
        }
        
        _buildArrayFromInitializer( node, value);
    } else
    */

    if(isNumeric(value)) {
        /* Assign a number value */
        double ival = strtod(value, NULL);
        /* clear out the string if need be*/
        if((*node)->_state == 1)
            LATDealloc((*node)->data._svalue);
        
        (*node)->data._fvalue = ival;
        (*node)->_state = 0;
    } else {
        /* Assign a string value */
        if((*node)->_state == 1) {
            unsigned long valLen = strlen(value);
            if(valLen > strlen((*node)->data._svalue)) {
                /* New string is longer, clear it out and set duplicate the new one in*/
                LATDealloc((*node)->data._svalue);
                (*node)->data._svalue = LATstrdup(value);
            } else {
                /* New string is <= length of old string, we'll copy that instead*/
                strncpy((*node)->data._svalue, value, valLen);
                (*node)->data._svalue[valLen] = '\0';
            }
        } else {
            (*node)->data._svalue = LATstrdup(value);
            (*node)->_state = 1;
        }
    }
}

/* Sets up a new CoreObject with given inputs */
char _constructCoreObjectWithKeyVal(struct CoreObject **node, char *key, char *value) {
    /*zero out child nodes*/
    (*node)->lobj = NULL;
    (*node)->robj = NULL;
    
    (*node)->key = LATstrdup(key);
    
    if(isNumeric(value)) {
        double ival = strtod(value, NULL);/*(value != 0)?atof(value):atof(value);*/
        (*node)->data._fvalue = ival;
        (*node)->_state = 0;
        return 0;
    } else {
        (*node)->data._svalue = LATstrdup(value);
        (*node)->_state = 1;        
        return 1;
    }
}

/* Constructs an array from a given initializer input, and places it into the given node */
void _buildArrayFromInitializer(struct CoreObject **node, char *input) {
    char *key;
    char *value;
    int move;
    struct CoreObject *lastNode;
    input++;
    
    /* empty array intializer check */
    if(*input == '}') {
        (*node)->lobj = NULL;
        return;
    }
    
    lastNode = *node;
    
    while(*input) {
        char tmp;
        unsigned char didStartWithQuote = 0;
        struct CoreObject *_node;
        
        /* Get Key */
        key = input;
        move = findUnquotedCharacter( ':', input);
        if(move == -1) {
            printf("Error, malformed array initializer :>> %s\n", input);
            exit(1029);
        }
        input+=move;
        *input='\0';
        key = LATstrdup(key);
        *input++=':';
        
        /* Get Value */
        
        if(*input == '"') {
            /* If starts with a quote, tear the quote off, we don't want it */
            value = input+1;
            didStartWithQuote = 1;
        } else {
            /* Normal set */
            value = input;
        }
        
        move = findUnquotedCharacter( ',', input);
        if(move < 0) {
            if(didStartWithQuote == 1) {
                /* Strip off closing TWO spaces from the tail */
                move = (int)strlen(input)-2;
            } else {
                /* Just Strip off the one */
                move = (int)strlen(input)-1;
            }
        }
        input+=move;
        tmp=*input;
        *input='\0';
        
        if(*(input-1) == '"') {
            /* Closing quote, tear it off */
            *(input-1) = '\0';
        }
        
        value = LATstrdup(value);
        
        if(*(input-1) == '\0') {
            /* We set this here, replace it */
            *(input-1) = '"';
        }
        
        /* Apply! */
        _node = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /* create new node in tree and set values */
        _constructCoreObjectWithKeyVal( &_node, key, value);
        
        LATDealloc(value);
        LATDealloc(key);
        
        lastNode->lobj = _node;
        lastNode = _node;
        
        /* Put back the character we extracted we removed for a temporary end point (most likely a comma) and continue */
        *input++=tmp;
        
        /* Double check to make sure we're not at the end */
        if(*input == '}') {
            break;
        }
    }
}

/* Converts a given float to a char pointer
 * strdup DOES LATAlloc memory, needs to be cleared out
 */
char * convertFloatToString(double value) {
    char str[20];
    int len = sprintf(str, "%g", value);
    /* Attempted to replace with a table pointer, caused system instability*/
    char *finalRez = LATAlloc(NULL, 0, (size_t)(len+1) * sizeof(char));
    strncpy(finalRez, str, (size_t)len);
    finalRez[len] = '\0';
    return finalRez;
}

/*
 * Frees an object matching a key
 */
struct CoreObject * freeObjectWithKey(char *key, struct CoreObject *node) {
    if(node != NULL) {
        if(LAT_STRCMP(node->key, key) == 0) {
            struct CoreObject *obj, *leftObj, *rightObj;
            /*get counts of nodes*/
            int nodeCount = 0;
            if(node->lobj != NULL) nodeCount++;
            if(node->robj != NULL) nodeCount++;
            
            if(node->_state == 3) {
                /* This is an array! Dealloc the entire list below it and decrement the node count (if we had anything) */
                if(node->lobj != NULL) {
                    freeAllObjects(node->lobj);
                    nodeCount--;
                    node->lobj = NULL;
                    
                }
            }
                
            
            switch(nodeCount) {
                case 0:
                    if(node->_state == 1)
                        LATDealloc(node->data._svalue);
                    else if(node->_state == 4) {
                        /*fclose(node->data._file);*/
                        node->data._file = NULL;
                    }
                    LATDealloc(node->key);
                    LATDealloc(node);
                    return 0;
                case 1:
                    obj = (node->lobj != 0)?node->lobj:node->robj;
                    
                    if(node->_state == 1)
                        LATDealloc(node->data._svalue);
                    else if(node->_state == 4) {
                        /*fclose(node->data._file);*/
                        node->data._file = NULL;
                    }
                    LATDealloc(node->key);
                    LATDealloc(node);
                    return obj;
                case 2:
                    leftObj = node->lobj;
                    rightObj = generateReplacement(node->robj);
                    
                    if(node->_state == 1)
                        LATDealloc(node->data._svalue);
                    else if(node->_state == 4) {
                        /*fclose(node->data._file);*/
                        node->data._file = NULL;
                    }
                    LATDealloc(node->key);
                    LATDealloc(node);
                    
                    node = replacementObject;
                    node->lobj = leftObj;
                    node->robj = rightObj;
                    
                    /*
                    Replace this with the lowest 'high' number
                    first we will check if the rightObj has ANY 'left' child nodes
                    if there are none, we'll copy it, delete the old one, and move it up here, pretty straight forward
                    if there's a left node, do the same check on that node, if it passes use it instead
                    NOTE if there's a right node present it _MUST_ replace the node we are about to remove
                    the example below demonstrates why
                    
                              x
                             / \
                            x  (x) <- removing this node
                               / \
                              x  [x] <- this node is used as our 'cursor', all changes will be propogated to it
                                 / \
                       using -> x   x
                                 \
                                  x <- and shift this node one up to the parent's spot
                    
                     AGAIN if there is NO right node we can simply copy, delete the original, and replace the node we're removing with it
                    
                    we'll be returning our rightObj
                     */
                    return node;
            }
        } else {
            node->lobj = freeObjectWithKey(key, node->lobj);
            if(node->_state != 3) {
                node->robj = freeObjectWithKey(key, node->robj);
            }
        }
    }
    return node;
}

/* Frees a specific array key value */
struct CoreObject * freeArrayObjectWithNameKey(char *varName, char *key, struct CoreObject *node) {
    if(node != NULL) {
        if(LAT_STRCMP(node->key, varName) == 0) {
            
            if(node->_state == 3) {
                /* Use our existing function to delete our item */
                node->lobj = freeObjectWithKey(key, node->lobj);
                
            } else {
                /* Wrong type to use this on */
                printf("Attempted to delete array key value on variable that was not an array to begin with\n");
                exit(9281);
                
            }
        } else {
            node->lobj = freeObjectWithKey(key, node->lobj);
            if(node->_state != 3) {
                node->robj = freeObjectWithKey(key, node->robj);
            }
        }
    }
    return node;
}

/* Creates a deep copy of an array core object */
struct CoreObject *copyArrayCoreObject(struct CoreObject *co) {
    struct CoreObject *copyCo = LATAlloc(NULL, 0, sizeof(struct CoreObject));
    /* Set items */
    copyCo->_state = co->_state;
    copyCo->data = co->data;
    copyCo->key = LATstrdup(co->key);
    
    copyCo->robj = NULL;
    copyCo->lobj = __copyArrayCoreObject(co->lobj);
    
    return copyCo;
}

/* Recursively iterates and returns on each element of this array */
struct CoreObject *__copyArrayCoreObject(struct CoreObject *co) {
    if(co != NULL) {
        /* Create new core object to copy to */
        struct CoreObject *copyCo = LATAlloc(NULL, 0, sizeof(struct CoreObject));
        /* Set items */
        copyCo->_state = co->_state;
        copyCo->data = co->data;
        copyCo->key = LATstrdup(co->key);
        
        copyCo->lobj = __copyArrayCoreObject(co->lobj);
        copyCo->robj = __copyArrayCoreObject(co->lobj);
        
        return copyCo;
    } else {
        return NULL;
    }
}

/* Takes a node, and digs down all the until no left nodes are present
 * That node is copied to an EXTERNAL VARIABLE, removed, and if a right element is present it is set to the old node and returned
 * At that time it proceeds to return all changes*/
struct CoreObject * generateReplacement(struct CoreObject *node) {
    if(node->lobj != NULL) {
        return generateReplacement(node->lobj); /*valid, dig down one more*/
    } else {
        /* invalid, this is our node, copy it*/
        replacementObject = node;
        return (node->robj != NULL)?node->robj:NULL;
    }
}

/*
 * Frees all objects in the tree structure, called when stacks or popped or when program is shutting down
 * Objects are moved from the stack to the heap and vice versa
 */
void freeAllObjects( struct CoreObject *node) {
    if(node != NULL) {
        freeAllObjects(node->lobj);
        freeAllObjects(node->robj);
        
        if(node->_state == 1) {
            /* String value to free */
            LATDealloc(node->data._svalue);
            
        } else if(node->_state == 4) {
            /* Open file to close */
            /*fclose(node->data._file);*/
            node->data._file = NULL;
            
        }
        LATDealloc(node->key);
        node->lobj = NULL;
        node->robj = NULL;
        LATDealloc(node);
        node = NULL;
    }
}

/* Frees all CoreObjects in the heap, called on system shutdown*/
void freeAllHeapObjects() {
    /* 1. Get a handle to the primary heap object, FREE it, and then move onto the next one */
    if(coreObjectHeap != 0) {
        struct CoreObject *node = NULL;
        while(coreObjectHeap != 0) {
            node = coreObjectHeap->lobj;
            LATDealloc( coreObjectHeap);
            coreObjectHeap = node;
        }
    }
}

/* Pushes a given object onto the heap (only uses the left node to creaet a linked list for heap storage)*/
/* DEPRECATED for now
void pushCoreObjectOnHeap(struct CoreObject *node) {
    if(coreObjectHeap != 0) {
        struct CoreObject *tmpNode = coreObjectHeap;
        coreObjectHeap = node;
        coreObjectHeap->lobj = tmpNode;
    } else {
        coreObjectHeap = node;
    }
    coreObjectHeapSize++;
}
 */

/* Pops a given object from the heap DEPRECATED for now*/
/*
struct CoreObject *popCoreObjectOnHeap() {
    if(coreObjectHeap != 0) {
        struct CoreObject *tmpNode = coreObjectHeap->lobj;
        struct CoreObject *node = coreObjectHeap;
        coreObjectHeap = tmpNode;
        coreObjectHeapSize--;
        return node;
    } else {
        return NULL;
    }
}
 */

void Core_printMem() {
    /* int mem; */
    printf("======CO Layout======\n");
    /* mem = Core__printMem(stack_getCoreObject(), 0); */
    Core__printMem(stack_getCoreObject(), 0);
    printf("=====================\n\n");
    /* printf("\nMemory Used by CoreObjects: %d bytes\n\n", mem); */
}

int Core__printMem(struct CoreObject *node, int mem) {
    if(node != 0) {
        char *key;
        if(node->_state == 3) {
            printf("-%s\n", node->key);
        } else {
            printf("%s\n",node->key);
            if(node->_state == 1) {
                printf("    %s\n", node->data._svalue);
            }
        }
        mem+=(int)sizeof(node);
        key = node->key;
        mem+=(int)(sizeof(key) * strlen(key));
        mem+=Core__printMem(node->lobj, 0);
        mem+=Core__printMem(node->robj, 0);
        return mem;
    } else
        return mem;
}

/*
 * Starts mem freeing
 */
void freeObjects() {
    freeAllObjects(stack_getCoreObject());
    freeAllHeapObjects();
    LATF_freeObjects();
    stack_setCoreObject(0);
    
}


