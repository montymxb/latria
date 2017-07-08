/*
The MIT License (MIT)

Copyright (c) 2017 Benjamin Wilson Friedman

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
 *  latria_vars.c
 */

#include "latria_vars.h"

/* holds a replacement object in the works */
struct CoreObject *replacementObject = 0;

/* Reference to the heap */
struct CoreObject *coreObjectHeap    = 0;

/* Reference to the heap object count */
int coreObjectHeapSize               = 0;


/* Sets key value */
void setValue(char *name, char *val) {
    
    /* Get the topmost CoreObject */
    struct CoreObject *co = stack_getCoreObject();
    
    /* Set a regular object value */
    setValueForKey(name, val, &co);
    
    /* Update the stack */
    stack_setCoreObject(co);
    
}


/* Frees a value */
void unsetValue(char *name) {
    
    /* Get the topmost CoreObject */
    struct CoreObject *co = stack_getCoreObject();
    
    /* Free our value that matches this key */
    co = freeObjectWithKey(name, co);
    
    /* Update the stack */
    stack_setCoreObject(co);
    
}


/* Frees a given array's key value combo */
void unsetArrayKeyValue(char *varName, char *key) {
    
    struct CoreObject *co = stack_getCoreObject();
    co = freeArrayObjectWithNameKey(varName,key,co);
}


/* Sets a boolean value with a given key */
void setBoolValue(char *name, LATBool val) {
    
    struct CoreObject *co = stack_getCoreObject();
    setBoolValueForKey(name, val, &co);
    stack_setCoreObject(co);
}


/* Sets an array key to a boolean value */
void setArrayBoolValue(char *varName, char *key, LATBool val) {
    
    struct CoreObject *co = stack_getCoreObject();
    setBoolValueForArrayKey(varName, key, val, &co);
    stack_setCoreObject(co);
}


/* Goes over each item and attempts to find a matching array and existing key value, otherwise it creates a new one */
void setBoolValueForArrayKey(char *varName, char *key, LATBool value, struct CoreObject **_node) {
    
    /* Check if this node is NULL */
    if((*_node) != NULL) {
        
        /* Compare this node's key against ours */
        int compare = LAT_STRCMP((*_node)->key,varName);
        
        /* Check if the key and object type match */
        if(compare == 0 && (*_node)->_state == 3) {
            
            /* Default to not having updated */
            LATBool didUpdate = false;
            
            /* Reference this node with a temporary one */
            struct CoreObject *tmpObject = *_node;
            
            /* Bump along until we find a value or NULL out */
            while((tmpObject = tmpObject->lobj) != NULL) {
                
                /* Check for a match */
                if(LAT_STRCMP( tmpObject->key, key) == 0) {
                    
                    /* Update our matched object */
                    setBoolValueForKey(key, value, _node);
                    
                    /* Indicate we updated and break */
                    didUpdate = true;
                    break;
                }
            }
            
            /* Check if we did not update */
            if(!didUpdate) {
                
                struct CoreObject *tmptmp;
                
                /* If we did not update, make a new core object to add in */
                struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
                
                /* Assign values */
                (*_newNode).lobj         = NULL;
                (*_newNode).robj         = NULL;
                (*_newNode).key          = LATstrdup(key);
                (*_newNode)._state       = 2;
                (*_newNode).data._bvalue = value;
                
                /* Set this as the FIRST array element, set all others as it's children */
                
                /* Indicate our tmptmp is our left leaf */
                tmptmp         = (*_node)->lobj;
                
                /* Indicate our left object is our new node created here */
                (*_node)->lobj = _newNode;
                
                /* Set the left object of our new node to that we extracted above from the original object */
                _newNode->lobj = tmptmp;
            }
            
            /* Done */
            
        /* Check if we mathed but our types differ */
        } else if(compare == 0 && (*_node)->_state != 3) {
            
            /* This item exists but is NOT an array, repurpose it */
            
            /* Create a new key value combo in our array and set it up */
            struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
            
            (*_newNode).lobj         = NULL;
            (*_newNode).robj         = NULL;
            (*_newNode).key          = LATstrdup(key);
            (*_newNode)._state       = 2;
            (*_newNode).data._bvalue = value;
            
            /* repurpose the existing obj */
            _setArrayObjectForKey(varName, _newNode, &(*_node));
        
        } else if(compare < 0) {
            
            /* We did not match, continue searching */
            
            if((*_node)->_state != 3) {
                
                /* Try left node, normally */
                setBoolValueForArrayKey(varName, key, value, &(*_node)->lobj);
                
            } else {
                
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right instead */
                setBoolValueForArrayKey(varName, key, value, &(*_node)->robj);
                
            }
        } else {
            
            /* Try the left node if greater */
            setBoolValueForArrayKey(varName, key, value, &(*_node)->lobj);
            
        }
    } else {
        
        /* Create a new key value in our new array */
        struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
        
        (*_newNode).lobj         = NULL;
        (*_newNode).robj         = NULL;
        (*_newNode).key          = LATstrdup(key);
        (*_newNode)._state       = 2;
        (*_newNode).data._bvalue = value;
        
        /* Create our new array */
        _setArrayObjectForKey(varName, _newNode, &(*_node));
        
    }
}


/* Set a file value */
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
    
    /* Check that this node is not NULL */
    if((*_node) != NULL) {
        
        /* compare this node's key and ours */
        int compare = LAT_STRCMP((*_node)->key,varName);
        
        /* Check if we matched */
        if(compare == 0 && (*_node)->_state == 3) {
            
            /* Match and of the same type, indicate we did not update by default */
            LATBool didUpdate = false;
            
            /* Set a tmp object to our matching node */
            struct CoreObject *tmpObject = *_node;
            
            /* Bump along until we find a value or NULL out */
            while((tmpObject = tmpObject->lobj) != NULL) {
                
                /* Check for a match */
                if(LAT_STRCMP( tmpObject->key, key) == 0) {
                    
                    /* Update our matched object */
                    setFileValueForKey(key, value, _node);
                    
                    /* Indicate we updated and break */
                    didUpdate = true;
                    break;
                }
            }
            
            /* Check if we did not update */
            if(!didUpdate) {
                
                struct CoreObject *tmptmp;
                
                /* If we did not update, make a new core object to add in */
                struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
                
                (*_newNode).lobj       = NULL;
                (*_newNode).robj       = NULL;
                (*_newNode).key        = LATstrdup(key);
                (*_newNode).data._file = value;
                (*_newNode)._state     = 4;
                
                /* Set tmptmp to the matching nodes left object */
                tmptmp = (*_node)->lobj;
                
                /* Set the matching node's left object to our newly created node */
                (*_node)->lobj = _newNode;
                
                /* Set tmptmp to our new node's left leaf, effectively putting it on top of the array list */
                _newNode->lobj = tmptmp;
                
            }
            
            /* Done */
            
        } else if(compare == 0 && (*_node)->_state != 3) {
            
            /* This item exists but is NOT an array, repurpose it */
            
            /* Create a new key value in our array */
            struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
            
            (*_newNode).lobj       = NULL;
            (*_newNode).robj       = NULL;
            (*_newNode).key        = LATstrdup(key);
            (*_newNode)._state     = 4;
            (*_newNode).data._file = value;
            
            /* repurpose the existing obj */
            _setArrayObjectForKey(varName, _newNode, &(*_node));
            
        } else if(compare < 0) {
            
            /* No match */
            
            if((*_node)->_state != 3) {
                
                /* Try left node, normally */
                setFileValueForArrayKey(varName, key, value, &(*_node)->lobj);
                
            } else {
                
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                setFileValueForArrayKey(varName, key, value, &(*_node)->robj);
                
            }
        } else {
            
            /* No match, try the left node */
            setFileValueForArrayKey(varName, key, value, &(*_node)->lobj);
            
        }
    } else {
        
        /* Create a new key value in our new array */
        struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
        
        (*_newNode).lobj       = NULL;
        (*_newNode).robj       = NULL;
        (*_newNode).key        = LATstrdup(key);
        (*_newNode)._state     = 4;
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


/* Return the CoreObject associated with the given key */
struct CoreObject * getValue(char *name) {
    
    struct CoreObject *co = stack_getCoreObject();
    struct CoreObject *obj = NULL;
    
    /* Find an object with the key passed */
    obj = findObjectWithKey(name, &co);
    
    if(obj != NULL) {
        
        /* Return the obj we found */
        return obj;
    } else {
        
        /* Did not find anything, try again with another scope */
        
        if(getPrimaryStack() != 0) {
            
            /* Try the next scope */
            return _getValue( name, getPrimaryStack());
        } else {
            
            /* No match, return null */
            return NULL;
        }
    }
}


/* Returns the associated array CoreObject or null */
struct CoreObject * getArrayValue(char *varName, char *key) {
    
    struct CoreObject *co = stack_getCoreObject();
    struct CoreObject *obj = NULL;
    
    /* Attempt to find an aray element with the given key */
    obj = findArrayObjectWithVarKey(varName, key, &co);
    
    if(obj != NULL) {
        
        /* Return the obj we found */
        return obj;
        
    } else {
        
        /* Did not find anything, try again with another scope */
        
        if(getPrimaryStack() != 0) {
            
            /* Try the next scope */
            return _getArrayValue(varName, key, getPrimaryStack());
        } else {
            
            /* No match, return null */
            return NULL;
        }
    }
}


/* Retturns the CoreObject with the associated key, polling across scopes */
struct CoreObject * _getValue(char *name, struct LATReference_Stack *stack) {
    
    struct CoreObject *co = _stack_getCoreObject(stack);
    struct CoreObject *obj = NULL;
    
    /* Find an obj matching the given key */
    obj = findObjectWithKey(name, &co);
    
    if(obj != NULL && co != NULL) {
        
        /* Return the obj we found */
        return obj;
    } else {
        
        /* Did not find anything, try in the next scope */
        struct LATReference_Stack *nxtStack = getNextStack(stack);
        
        if(nxtStack != 0) {
            
            /* Try in the next scope */
            return _getValue( name, nxtStack);
        } else {
            
            /* Nothing more to try, return null */
            return NULL;
        }
    }
}


/* Internally searches and attempts to return an array element with the given key */
struct CoreObject * _getArrayValue(char *varName, char *key, struct LATReference_Stack *stack) {
    
    struct CoreObject *co = _stack_getCoreObject(stack);
    struct CoreObject *obj = NULL;
    
    /* Find an array object matching the given key */
    obj = findArrayObjectWithVarKey(varName, key, &co);
    
    if(obj != NULL && co != NULL) {
        
        /* Return the obj we found */
        return obj;
    } else {
        
        /* Did not find anything, try in the next scope */
        struct LATReference_Stack *nxtStack = getNextStack(stack);
        
        if(nxtStack != 0) {
            
            /* Try in the next scope */
            return _getArrayValue(varName, key, nxtStack);
        } else {
            
            /* Nothing more to try, return null */
            return NULL;
        }
    }
}


/* Attempts to find and return an object with the provided key */
struct CoreObject * findObjectWithKey(char *key, struct CoreObject **node) {
    
    /* Check if this node is not NULL */
    if((*node) != NULL) {
        
        /* Compare this node's key and ours */
        int compare = LAT_STRCMP((*node)->key,key);
        
        /* Check if they match */
        if(compare == 0) {
            
            /* Return this matching node */
            return *node;
        } else if(compare < 0) {
            
            /* Did not match, try a lower node */
            if((*node)->_state != 3) {
                
                /* Try left node, normally */
                return findObjectWithKey(key, &(*node)->lobj);
            } else {
                
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                return findObjectWithKey(key, &(*node)->robj);
            }
        } else {
            
            /* Did not match, try the right node */
            return findObjectWithKey(key, &(*node)->robj);
        }
    } else {
        
        /* Null node, return it */
        return *node;
    }
}


/* Finds and return an array object with a given key */
struct CoreObject * findArrayObjectWithVarKey(char *varName, char *key, struct CoreObject **node) {
    
    /* Check if this node is not NULL */
    if((*node) != NULL) {
        
        /* Compare this node's key and ours */
        int compare = LAT_STRCMP((*node)->key,varName);
        
        /* Check if they matched */
        if(compare == 0 && (*node)->_state == 3) {
            
            /* Matched node and of same type*/
            
            /* Set tmpNode to our matching node */
            struct CoreObject *tmpNode = (*node);
            
            /* Iterate over the left elements (all part of the array) until we hit null */
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
            
            /* Did not match, try another node */
            
            if((*node)->_state != 3) {
                
                /* Try left node, normally */
                return findArrayObjectWithVarKey(varName, key, &(*node)->lobj);
                
            } else {
                
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                return findArrayObjectWithVarKey(varName, key, &(*node)->robj);
                
            }
        } else {
            
            /* Did not match. Try right node */
            return findArrayObjectWithVarKey(varName, key, &(*node)->robj);
            
        }
    } else {
        
        /* This node is NULL, return it */
        return *node;
    }
}


/* Sets an array value with the provided key */
void setArrayValueForKey(char *varName, char *key, char *value, struct CoreObject **node) {
    
    /* Check if this node is not null */
    if((*node) != NULL) {
        
        /* Compare this node's key and ours */
        int compare = LAT_STRCMP((*node)->key, varName);
        
        /* Check for a match */
        if(compare == 0 && (*node)->_state == 3) {
            
            /* Mached and of same type, indicate we have not updated by default */
            LATBool didUpdate = false;
            
            /* Assign the matching node to our tmpObj */
            struct CoreObject *tmpObject = *node;
            
            /* Bump along until we find a value or NULL out */
            while((tmpObject = tmpObject->lobj) != NULL) {
                
                /* Check for a match */
                if(strcmp( tmpObject->key, key) == 0) {
                    
                    /* Update our matched object */
                    _updateCoreObjectWithVal( &tmpObject, value);
                    
                    /* Indicate we updated and break */
                    didUpdate = true;
                    break;
                }
            }
            
            /* Check if we did not update */
            if(!didUpdate) {
                
                /* Make a new core object to add in */
                struct CoreObject *tmptmp;
                struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
                
                /* Set tmptmp to our matching node's left object */
                tmptmp        = (*node)->lobj;
                
                /* Set the matching node's left object to our new node */
                (*node)->lobj = _newNode;
                
                /* Construct a new core object with our new node */
                _constructCoreObjectWithKeyVal(&_newNode, key, value);
                
                /* Assing tmptmp to our new node's left object, putting it at the top of the array list */
                _newNode->lobj = tmptmp;
                
            }
            
            /* Done */
            
        } else if(compare == 0 && (*node)->_state != 3) {
            
            /* This item exists but is not an array, repurpose it */
            
            /* Create a new key value in our array */
            struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
            
            /* Construct a new object with the provided key and value */
            _constructCoreObjectWithKeyVal(&_newNode, key, value);
            
            /* repurpose the existing obj */
            _setArrayObjectForKey(varName, _newNode, &(*node));
            
        } else if(compare < 0) {
            
            /* Did not match */
            
            if((*node)->_state != 3) {
                
                /* Try left node, normally */
                setArrayValueForKey(varName, key, value, &(*node)->lobj);
                
            } else {
                
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                setArrayValueForKey(varName, key, value, &(*node)->robj);
                
            }
        } else {
            
            /* Did not match, try our right node */
            setArrayValueForKey(varName, key, value, &(*node)->robj);
            
        }
    } else {
        
        /* Create a new key value in our new array */
        struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject)); /*create new node in tree and set values */
        
        /* Construct a new object with the provided key and value */
        _constructCoreObjectWithKeyVal(&_newNode, key, value);
        
        /* Create our new array */
        _setArrayObjectForKey(varName, _newNode, &(*node));
        
    }
}


/* Sets a boolean value for a given key */
void setBoolValueForKey(char *key, LATBool value, struct CoreObject **_node) {
    
    /* Check if this node is not null */
    if((*_node) != NULL) {
        
        /* Compare this node's key and ours */
        int compare = LAT_STRCMP((*_node)->key, key);
        
        /* Check if we matched */
        if(compare == 0) {
            
            /* Matched, clear out any prior values */
            if((*_node)->_state == 1) {
                
                /* Delete prior string value */
                LATDealloc((*_node)->data._svalue);
                
            } else if((*_node)->_state == 3) {
                
                /* Delete this arrays' elements */
                freeAllObjects((*_node)->lobj);
                (*_node)->lobj = NULL;
                
            } else if((*_node)->_state == 4) {
                
                /* File, null it */
                (*_node)->data._file = NULL;
                
            } else if((*_node)->_state == 0 || (*_node)->_state == 2) {
                
                /* Number or Bool, do nothing */
                
            } else {
                
                /* Unrecognized data type, exit out */
                printf("Unrecognized data type found as core object: %d\n", (*_node)->_state);
                exit(1);
                
            }
            
            /* set bool value and state */
            (*_node)->data._bvalue = value;
            (*_node)->_state       = 2;
            
        } else if(compare < 0) {
            
            /* Did not match */
            
            if((*_node)->_state != 3) {
                
                /* Try left node, normally */
                setBoolValueForKey(key, value, &(*_node)->lobj);
            } else {
                
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                setBoolValueForKey(key, value, &(*_node)->robj);
            }
        } else {
            
            /* Did not match. Try right node */
            setBoolValueForKey(key, value, &(*_node)->robj);
        }
    } else {
        
        /* Create new node in tree and set values */
        (*_node) = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
        
        /* Setup */
        (*_node)->lobj         = NULL;
        (*_node)->robj         = NULL;
        (*_node)->key          = LATstrdup(key);
        (*_node)->data._bvalue = value;
        (*_node)->_state       = 2;
    }
}


/* Called to set a CoreObject directly as the LEFT component of an existing OR new CoreObject */
void setArrayObject(char *name, struct CoreObject *array) {
    
    struct CoreObject *co = stack_getCoreObject();
    _setArrayObjectForKey(name, array, &co);
    stack_setCoreObject(co);
}


/* Digs down until an existing or new object is created, then sets the left CoreObject to the array passed */
void _setArrayObjectForKey(char *key, struct CoreObject *array, struct CoreObject **_node) {
    
    /* Check if this node is not null */
    if((*_node) != NULL) {
        
        /* Compare this node's key and ours */
        int compare = LAT_STRCMP((*_node)->key, key);
        
        /* Check if we matched */
        if(compare == 0) {
            
            
            /* Matched, clear out any prior values */
            if((*_node)->_state == 1) {
                
                /* Free string */
                LATDealloc((*_node)->data._svalue);
                
            } else if((*_node)->_state == 3) {
                
                /* Free array elements */
                freeAllObjects((*_node)->lobj);
                (*_node)->lobj = NULL;
                
            } else if((*_node)->_state == 4) {
                
                /* File, null it out, user is responsible for freeing this beforehand (Files are NOT collected) */
                (*_node)->data._file = NULL;
                
            } else if((*_node)->_state == 0 || (*_node)->_state == 2) {
                
                /* Number or Bool, do nothing */
                
            } else {
                
                /* Unrecognized data type, error out */
                printf("Unrecognized data type found as core object: %d\n", (*_node)->_state);
                exit(1);
                
            }
            
            /* Left Object relocation logic, first check our left object is not null */
            if((*_node)->lobj != NULL) {
                
                /* It's not null, we need to relocate items */
                
                /* Check if our right object is null */
                if((*_node)->robj == NULL) {
                    
                    /* Nothing on the right branch, shift LEFT to RIGHT */
                    (*_node)->robj = (*_node)->lobj;
                    
                } else {
                    
                    /* Our right object is not null. We need to relocate our left leaf further down the chain on the right one */
                    struct CoreObject *nxtObj = (*_node)->robj;
                    
                    /* Iterate whle our nxtObj is not null */
                    while(nxtObj) {
                        
                        /* Compare our left object and right object's key */
                        int lCompare = LAT_STRCMP((*_node)->lobj->key, nxtObj->key);
                        
                        /* Check how we compared */
                        if(lCompare < 0) {
                            
                            /* Try setting left */
                            if(nxtObj->lobj == NULL) {
                                
                                /* Our left object is null, simply set it */
                                nxtObj->lobj = (*_node)->lobj;
                                break;
                            } else {
                                
                                /* Set our nxtObj to our lobj, it needs to be moved down as well */
                                nxtObj = nxtObj->lobj;
                            }
                            
                        } else {
                            
                            /* Try setting right */
                            if(nxtObj->robj == NULL) {
                                
                                /* Our right object is null, simply set it */
                                nxtObj->robj = (*_node)->lobj;
                                break;
                            } else {
                                
                                /* Set our nxtObj to our right object, it need to be moved down as well */
                                nxtObj = nxtObj->robj;
                            }
                        }
                    }
                }
            }
            
            /* set array obj and state to our matching node */
            (*_node)->lobj   = array;
            (*_node)->_state = 3;
            
        } else if(compare < 0) {
            
            /* Did not match */
            
            if((*_node)->_state != 3) {
                
                /* Try left node, normally */
                _setArrayObjectForKey(key, array, &(*_node)->lobj);
            } else {
                
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                _setArrayObjectForKey(key, array, &(*_node)->robj);
            }
        } else {
            
            /* Did not match. Try right node */
            _setArrayObjectForKey(key, array, &(*_node)->robj);
        }
    } else {
        
        /* Create new node in tree and set values */
        (*_node) = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
        
        /* Setup */
        (*_node)->lobj      = NULL;
        (*_node)->robj      = NULL;
        (*_node)->key       = LATstrdup(key);
        (*_node)->lobj      = array;
        (*_node)->_state    = 3;
    }
    
}


/* Sets a CoreObject matching a key to a file value */
void setFileValueForKey(char *key, FILE *value, struct CoreObject **_node) {
    
    /* Check if our node is not null */
    if((*_node) != NULL) {
        
        /* Compare this node's key and ours */
        int compare = LAT_STRCMP((*_node)->key, key);
        
        /* Check for a match */
        if(compare == 0) {
            
            /* Matched, clear out any previous string value if present*/
            if((*_node)->_state == 1) {
                
                /* Free prior string */
                LATDealloc((*_node)->data._svalue);
                
            } else if((*_node)->_state == 3) {
                
                /* Free prior array elements */
                freeAllObjects((*_node)->lobj);
                (*_node)->lobj = NULL;
                
            } else if((*_node)->_state == 4) {
                
                /* File, set null, not collected, user is responsible for freeing this before hand */
                if((*_node)->data._file != value) {
                    
                    /* Non-conflicting files, safe to remove */
                    
                    /* TODO files must be closed manually (they are not reclaimed) or they will be leaked out. This is a courtesy notice. */
                    (*_node)->data._file = NULL;
                    
                }
            } else if((*_node)->_state == 0 || (*_node)->_state == 2) {
                
                /* Number or Bool, do nothing */
                
            } else {
                printf("Unrecognized data type found as core object: %d\n", (*_node)->_state);
                exit(1);
                
            }
            
            /* set file value and type */
            (*_node)->data._file = value;
            (*_node)->_state     = 4;
            
        } else if(compare < 0) {
            
            /* Did not match */
            
            if((*_node)->_state != 3) {
                
                /* Try left node, normally */
                setFileValueForKey(key, value, &(*_node)->lobj);
            } else {
                
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                setFileValueForKey(key, value, &(*_node)->robj);
            }
        } else {
            
            /* Did not match. Try right node */
            setFileValueForKey(key, value, &(*_node)->robj);
        }
    } else {
        
        /* Create new node in tree and set values */
        (*_node) = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
        
        /* Setup */
        (*_node)->lobj         = NULL;
        (*_node)->robj         = NULL;
        (*_node)->key          = LATstrdup(key);
        (*_node)->data._file   = value;
        (*_node)->_state       = 4;
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
    
    /* Check if this node is not null */
    if((*_node) != NULL) {
        
        /* Compare this node's keys and ours */
        int compare = LAT_STRCMP((*_node)->key, name);
        
        /* Check if we matched */
        if(compare == 0) {
            
            /* Matched, clear out any previous values if present*/
            if((*_node)->_state == 1) {
                
                /* Free prior string */
                LATDealloc((*_node)->data._svalue);
                
            } else if((*_node)->_state == 3) {
                
                /* Free prior array elements */
                freeAllObjects((*_node)->lobj);
                (*_node)->lobj = NULL;
                
            } else if((*_node)->_state == 4) {
                
                /* Null prior file, not collected, up to the user to free this on their own before hand */
                (*_node)->data._file = NULL;
                
            } else if((*_node)->_state == 0 || (*_node)->_state == 2) {
                
                /* Number or Bool, do nothing */
                
            } else {
                
                /* Unrecognized type, error out */
                printf("Unrecognized data type found as core object: %d\n", (*_node)->_state);
                exit(1);
                
            }
            
            /* set connection value and state */
            (*_node)->data._fvalue  = val;
            (*_node)->_state        = 5;
            
        } else if(compare < 0) {
            
            /* Did not match  */
            
            if((*_node)->_state != 3) {
                
                /* Try left node, normally */
                setConnectionValueForKey(name, val, &(*_node)->lobj);
            } else {
                
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                setConnectionValueForKey(name, val, &(*_node)->robj);
            }
        } else {
            
            /* Did not match. Try right node */
            setConnectionValueForKey(name, val, &(*_node)->robj);
        }
    } else {
        
        /* Create new node in tree and set values */
        (*_node) = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
        
        /* Setup */
        (*_node)->lobj         = NULL;
        (*_node)->robj         = NULL;
        (*_node)->key          = LATstrdup(name);
        (*_node)->data._fvalue = val;
        (*_node)->_state       = 5;
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
    
    /* Check if this node is not null */
    if((*_node) != NULL) {
        
        /* Compare this node's key and ours */
        int compare = LAT_STRCMP((*_node)->key,varName);
        
        /* Check if we matched */
        if(compare == 0 && (*_node)->_state == 3) {
            
            /* Matched and of same type, indicate we did not update by default */
            LATBool didUpdate = false;
            struct CoreObject *tmpObject = *_node;
            
            /* Bump along until we find a value or NULL out */
            while((tmpObject = tmpObject->lobj) != NULL) {
                
                /* Check for a match */
                if(LAT_STRCMP( tmpObject->key, key) == 0) {
                    
                    /* Update our matched object */
                    setConnectionValueForKey(key, val, _node);
                    
                    /* Indicate we updated and break */
                    didUpdate = true;
                    break;
                    
                }
            }
            
            /* Check if we did not update */
            if(!didUpdate) {
                
                /* We did not update, make a new core object to add in */
                struct CoreObject *tmptmp;
                struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
                
                /* Setup our new node */
                (*_newNode).lobj         = NULL;
                (*_newNode).robj         = NULL;
                (*_newNode).key          = LATstrdup(key);
                (*_newNode).data._fvalue = val;
                (*_newNode)._state       = 5;
                
                /* Set tmptmp to our matching nodes left object */
                tmptmp = (*_node)->lobj;
                
                /* Set our matching node's left object to our new node */
                (*_node)->lobj = _newNode;
                
                /* Set our new node's left object to tmptmp, setting it at the top of the array list */
                _newNode->lobj = tmptmp;
                
            }
            
            /* Done */
            
        } else if(compare == 0 && (*_node)->_state != 3) {
            
            /* This item exists but is NOT an array, let's repurpose it */
            
            /* Create a new key value in our array */
            struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
            
            /* Setup */
            (*_newNode).lobj         = NULL;
            (*_newNode).robj         = NULL;
            (*_newNode).key          = LATstrdup(key);
            (*_newNode)._state       = 5;
            (*_newNode).data._fvalue = val;
            
            /* repurpose the existing obj */
            _setArrayObjectForKey(varName, _newNode, &(*_node));
            
        } else if(compare < 0) {
            
            /* Did not match */
            
            if((*_node)->_state != 3) {
                
                /* Try left node, normally */
                setConnectionValueForArrayKey(varName, key, val, &(*_node)->lobj);
                
            } else {
                
                /* This is an array item, ALL left nodes are reserved away from general stack space. Try right */
                setConnectionValueForArrayKey(varName, key, val, &(*_node)->robj);
                
            }
        } else {
            
            /* Did not match. Try the left node */
            setConnectionValueForArrayKey(varName, key, val, &(*_node)->lobj);
            
        }
    } else {
        
        /* Create a new key value in our new array */
        struct CoreObject *_newNode = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
        
        /* Setup our new node */
        (*_newNode).lobj         = NULL;
        (*_newNode).robj         = NULL;
        (*_newNode).key          = LATstrdup(key);
        (*_newNode)._state       = 5;
        (*_newNode).data._fvalue = val;
        
        /* Create our new array */
        _setArrayObjectForKey(varName, _newNode, &(*_node));
        
    }
}


/* Sets a value for a given key */
char setValueForKey(char *key, char *value, struct CoreObject **_node) {
    
    /* Check if this node is not null */
    if((*_node) != NULL) {
        
        /* Compare this node's key and ours */
        int compare = LAT_STRCMP((*_node)->key, key);
        
        /* Check if we matched */
        if(compare == 0) {
            
            /* Matched. Update an existing core object */
            _updateCoreObjectWithVal( _node, value);
            return 0;
            
        } else if(compare < 0) {
            
            /* Did not match */
            
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
        
        /* Create new node in tree */
        (*_node) = (struct CoreObject *)LATAlloc(NULL, 0, sizeof(struct CoreObject));
        
        /* Return this new core object with the given key and value */
        return _constructCoreObjectWithKeyVal( &(*_node), key, value);
    }
}


/* Updates an existing CoreObject with the given key/value combination */
void _updateCoreObjectWithVal(struct CoreObject **node, char *value) {
    
    /* Check this node's type */
    if((*node)->_state == 3) {
        
        /* Free array elements */
        freeAllObjects((*node)->lobj);
        (*node)->lobj = NULL;
        
    } else if((*node)->_state == 4) {
        
        /* Null the file reference, user is responsible for freeing this before hand */
        (*node)->data._file = NULL;
    }

    /* Check if our input is numeric */
    if(isNumeric(value)) {
        
        /* Assign a number value from the char pointer */
        double ival = strtod(value, NULL);
        
        /* Check if prior state was a string */
        if((*node)->_state == 1) {
            
            /* Free prior string */
            LATDealloc((*node)->data._svalue);
        }
        
        /* Set new number value and state (type) */
        (*node)->data._fvalue   = ival;
        (*node)->_state         = 0;
        
    } else {
        
        /* Assign a string value */
        
        /* Check if prior state was a string already */
        if((*node)->_state == 1) {
            
            /* Already a string, start by getting the length of our input value */
            unsigned long valLen = strlen(value);
            
            /* Check if we need to clear our old char pointer to fit this new string */
            if(valLen > strlen((*node)->data._svalue)) {
                
                /* New string is longer, clear it out and set duplicate the new one in*/
                LATDealloc((*node)->data._svalue);
                (*node)->data._svalue = LATstrdup(value);
                
            } else {
                
                /* New string is less than or equal to the length of old string, copy our value into it */
                strncpy((*node)->data._svalue, value, valLen);
                (*node)->data._svalue[valLen] = '\0';
            }
        } else {
            
            /* Not a string type, copy our string value in and set the state (type) */
            (*node)->data._svalue = LATstrdup(value);
            (*node)->_state = 1;
            
        }
    }
}


/* Sets up a new CoreObject with given inputs */
char _constructCoreObjectWithKeyVal(struct CoreObject **node, char *key, char *value) {
    
    /* Zero out child nodes */
    (*node)->lobj = NULL;
    (*node)->robj = NULL;
    
    /* Copy in the key */
    (*node)->key = LATstrdup(key);
    
    /* Check type of value */
    if(isNumeric(value)) {
        
        /* Number, convert to double and set in */
        double ival = strtod(value, NULL);
        
        /* set values */
        (*node)->data._fvalue = ival;
        (*node)->_state       = 0;
        
        /* Return 0, indicating we set a number type */
        return 0;
        
    } else {
        
        /* String, copy in value and set type */
        (*node)->data._svalue = LATstrdup(value);
        (*node)->_state       = 1;
        
        /* Return 1, indicating we set a string type */
        return 1;
    }
}


/* Converts a given dobule to a char pointer (NOTE: returns memory that must be freed) */
char * convertFloatToString(double value) {
    
    /* Maximum 20 digits of precision */
    char str[20];
    
    /* print our number into our str value */
    int len = sprintf(str, "%g", value);
    
    /* NOTE:: Attempted to replace with a table pointer for efficiency, caused server system instability */
    
    /* Allocate our final result */
    char *finalRez = LATAlloc(NULL, 0, (size_t)(len+1) * sizeof(char));
    
    /* Copy our value into our final result */
    strncpy(finalRez, str, (size_t)len);
    finalRez[len] = '\0';
    
    /* return our final result */
    return finalRez;
}


/* Frees an object matching a key */
struct CoreObject * freeObjectWithKey(char *key, struct CoreObject *node) {
    
    /* Check if our node is not null */
    if(node != NULL) {
        
        /* Check if this node's key and ours match */
        if(LAT_STRCMP(node->key, key) == 0) {
            
            /* Match */
            struct CoreObject *obj, *leftObj, *rightObj;
            
            /* Get counts of nodes */
            int nodeCount = 0;
            if(node->lobj != NULL) nodeCount++;
            if(node->robj != NULL) nodeCount++;
            
            /* Check thisn node's state */
            if(node->_state == 3) {
                
                /* This is an array! Dealloc the entire list below it and decrement the node count (if we had anything) */
                if(node->lobj != NULL) {
                    
                    /* Left object not null, free all elements */
                    freeAllObjects(node->lobj);
                    nodeCount--;
                    node->lobj = NULL;
                    
                }
            }
            
            switch(nodeCount) {
                    
                /* 0 nodes */
                case 0:
                    
                    if(node->_state == 1) {
                        
                        /* Free prior string value */
                        LATDealloc(node->data._svalue);
                        
                    } else if(node->_state == 4) {
                       
                        /* Null prior file value, user's responsibility to free this before hand */
                        node->data._file = NULL;
                    }
                    
                    /* Free key and node */
                    LATDealloc(node->key);
                    LATDealloc(node);
                    
                    return 0;
                    
                /* 1 Node */
                case 1:
                    
                    /* Set obj to the one node we have */
                    obj = (node->lobj != 0)?node->lobj:node->robj;
                    
                    if(node->_state == 1) {
                        
                        /* Free prior string */
                        LATDealloc(node->data._svalue);
                        
                    } else if(node->_state == 4) {
                        
                        /* Null prior file value, user's responsibility to free this before hand */
                        node->data._file = NULL;
                    }
                    
                    /* Free key and node */
                    LATDealloc(node->key);
                    LATDealloc(node);
                    
                    /* Return our one node */
                    return obj;
                    
                /* 2 Nodes */
                case 2:
                    
                    /* Get our left node */
                    leftObj = node->lobj;
                    
                    /* Generate a replacement for our right node */
                    rightObj = generateReplacement(node->robj);
                    
                    if(node->_state == 1) {
                        
                        /* Free prior string value */
                        LATDealloc(node->data._svalue);
                        
                    } else if(node->_state == 4) {
                        
                        /* Null prior file value, user's responsibility to free this before hand */
                        node->data._file = NULL;
                    }
                    
                    /* Free key and node */
                    LATDealloc(node->key);
                    LATDealloc(node);
                    
                    /* Set node to our replacement object */
                    node = replacementObject;
                    
                    /* Set left & right objects accordingly */
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
                    
                     ** If there is NO right node we can simply copy, delete the original, and replace the node we're removing with it
                    
                    we'll be returning our rightObj
                     */
                    
                    /* return our modified node */
                    return node;
            }
            
        } else {
            
            /* Did not match */
            
            /* Set our left node to the result of freeing the desired node with the given key within our left node */
            node->lobj = freeObjectWithKey(key, node->lobj);
            
            if(node->_state != 3) {
                
                /* Not an array, pass and set the right object to it's result */
                node->robj = freeObjectWithKey(key, node->robj);
            }
        }
    }
    
    /* Return the node */
    return node;
}


/* Frees a specific array key value */
struct CoreObject * freeArrayObjectWithNameKey(char *varName, char *key, struct CoreObject *node) {
    
    /* Check if this node is not null */
    if(node != NULL) {
        
        /* Check if this node's key and ours match */
        if(LAT_STRCMP(node->key, varName) == 0) {
            
            /* Match */
            
            if(node->_state == 3) {
                
                /* An array. Use our existing function to delete our item */
                node->lobj = freeObjectWithKey(key, node->lobj);
                
            } else {
                
                /* Wrong type to use this on! Error out */
                printf("Attempted to delete array key value on variable that was not an array to begin with\n\n");
                exit(9281);
                
            }
        } else {
            
            /* Did not match */
            
            /* Pass and set our left node further down */
            node->lobj = freeObjectWithKey(key, node->lobj);
            
            if(node->_state != 3) {
                
                /* Not an array, pass this node's right object and set the result to itself */
                node->robj = freeObjectWithKey(key, node->robj);
            }
        }
    }
    
    /* Return our node */
    return node;
}


/* Creates a deep copy of an array core object */
struct CoreObject *copyArrayCoreObject(struct CoreObject *co) {
    
    /* Create a new core object */
    struct CoreObject *copyCo = LATAlloc(NULL, 0, sizeof(struct CoreObject));
    
    /* Copy properties */
    copyCo->_state  = co->_state;
    copyCo->data    = co->data;
    copyCo->key     = LATstrdup(co->key);
    copyCo->robj    = NULL;
    
    /* Copy the rest of the array into our left object */
    copyCo->lobj    = __copyArrayCoreObject(co->lobj);
    
    /* return out copy */
    return copyCo;
}


/* Recursively iterates and returns on each element of this array */
struct CoreObject *__copyArrayCoreObject(struct CoreObject *co) {
    
    /* Check if the provided object is not null */
    if(co != NULL) {
        
        /* Create new core object to copy to */
        struct CoreObject *copyCo = LATAlloc(NULL, 0, sizeof(struct CoreObject));
        
        /* Set items */
        copyCo->_state  = co->_state;
        copyCo->data    = co->data;
        copyCo->key     = LATstrdup(co->key);
        
        /* Copy the rest of the array into our left object */
        copyCo->lobj    = __copyArrayCoreObject(co->lobj);
        /* Copy the rest of the array into our right object */
        copyCo->robj    = __copyArrayCoreObject(co->lobj);
        
        /* return our copy */
        return copyCo;
    } else {
        
        /* no object, return null */
        return NULL;
    }
}


/* Takes a node, and digs down all the until no left nodes are present
 * That node is copied to an EXTERNAL VARIABLE, removed, and if a right element is present it is set to the old node and returned
 * At that time it proceeds to return all changes
 */
struct CoreObject * generateReplacement(struct CoreObject *node) {
    
    /* Check if our left object is not null */
    if(node->lobj != NULL) {
        
        /* Call ourselves with the left node */
        return generateReplacement(node->lobj);
    } else {
        
        /* Null left object, this is our node, copy it */
        replacementObject = node;
        return (node->robj != NULL)?node->robj:NULL;
    }
}


/* Frees the given node and all it's children */
void freeAllObjects( struct CoreObject *node) {
    
    /* Check if the node is not null */
    if(node != NULL) {
        
        /* Recursively call with children */
        freeAllObjects(node->lobj);
        freeAllObjects(node->robj);
        
        /* Check state */
        if(node->_state == 1) {
            
            /* String value to free */
            LATDealloc(node->data._svalue);
            
        } else if(node->_state == 4) {
            
            /* File to null, it is up to the user to free files before hand */
            node->data._file = NULL;
            
        }
        
        /* Free this node's key */
        LATDealloc(node->key);
        
        /* Null children */
        node->lobj = NULL;
        node->robj = NULL;
        
        /* Free and null this node */
        LATDealloc(node);
        node = NULL;
    }
}


/* Frees all CoreObjects in the heap, called on system shutdown*/
void freeAllHeapObjects() {
    
    /* Get a handle to the primary heap object, FREE it, and then move onto the next one */
    
    /* Check we have a heap */
    if(coreObjectHeap != 0) {
        
        struct CoreObject *node = NULL;
        
        /* Iterate over heap while it is not 0 (null) */
        while(coreObjectHeap != 0) {
            
            /* Set node to the left object in our heap */
            node = coreObjectHeap->lobj;
            
            /* Free our current core object */
            LATDealloc( coreObjectHeap);
            
            /* Set our coreObjectHeap to our node (left child)v*/
            coreObjectHeap = node;
        }
    }
}


/* Prints out an list of the objects currently in memory, not complete and not accurate in terms of memory usage reflected */
void Core_printMem() {
    
    printf("======CO Layout======\n");
    
    Core__printMem(stack_getCoreObject(), 0);
    
    printf("=====================\n\n");
    
}


/* Prints details about the passed in node and updates the mem int passed */
int Core__printMem(struct CoreObject *node, int mem) {
    
    /* Check if this node is not null */
    if(node != 0) {
        
        /* Check this node's state */
        if(node->_state == 3) {
            
            /* Array. Print Var Name */
            printf("-%s\n", node->key);
            
        } else {
            
            /* Not an array */
            
            /* Print the key of this obj */
            printf("%s\n",node->key);
            
            if(node->_state == 0) {
            
                /* Number */
                printf("NUMBER: %f\n\n", node->data._fvalue);
                
            } else if(node->_state == 1) {
                
                /* String */
                printf("STRING: %s\n\n", node->data._svalue);
                
                /* Increment memory by size of string value */
                mem+=(int)(sizeof(node->data._svalue) * strlen(node->data._svalue));
                
            } else if(node->_state == 2) {
                
                /* Bool */
                printf("BOOL: %s\n\n", node->data._bvalue ? "true": "false");
                
            } else if(node->_state == 4) {
                
                /* File (print id) */
                printf("FILE: %d\n\n", node->data._file);
                
            } else if(node->_state == 5) {
                
                /* Connection */
                printf("CONNECTION: %d\n\n", (int)node->data._fvalue);
                
            } else {
                
                printf(":: unrecognized type ::\n\n");
                
            }
            
            
        }
        
        /* Increment memory by the size of this node */
        mem+=(int)sizeof(node);
        
        /* Increment memory by size of key */
        mem+=(int)(sizeof(node->key) * strlen(node->key));
        
        /* Call on left node */
        mem+=Core__printMem(node->lobj, 0);
        
        /* Call on right node */
        mem+=Core__printMem(node->robj, 0);
        
        /* Return memory computed */
        return mem;
    } else {
        
        /* Return current memory */
        return mem;
    }
}

/* Frees all underling objects and structures */
void freeObjects() {
    
    /* Free the stack */
    freeAllObjects(stack_getCoreObject());
    
    /* Free the heap */
    freeAllHeapObjects();
    
    /* Free all function objects */
    LATF_freeObjects();
    
    /* Set our stack to 0 */
    stack_setCoreObject(0);
    
}


