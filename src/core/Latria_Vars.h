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
 *  latria_vars.h
 */

#ifndef latria_core_latria_vars_h
#define latria_core_latria_vars_h

#include "latria_core.h"
#include "latria_referencestack.h"
#include "latria_function.h"
#include "memory/latria_gc.h"

void setValue(char *name, char *val);
void unsetValue(char *name);
void unsetArrayKeyValue(char *varName, char *key);

/* Bool setting */
void setBoolValue(char *name, LATBool val);
void setBoolValueForKey(char *key, LATBool value, struct CoreObject **_node);

/* Array Bool setting */
void setArrayBoolValue(char *varName, char *key, LATBool val);
void setBoolValueForArrayKey(char *varName, char *key, LATBool value, struct CoreObject **_node);

/* File setting */
void setFileValue(char *name, FILE *val);
void setFileValueForKey(char *key, FILE *value, struct CoreObject **_node);

/* Array file setting */
void setArrayFileValue(char *varName, char *key, FILE *val);
void setFileValueForArrayKey(char *varName, char *key, FILE *value, struct CoreObject **_node);

/* Connection setting */
void setConnectionValue(char *name, int val);
void setConnectionValueForKey(char *name, int val, struct CoreObject **_node);

/* Array connection setting */
void setArrayConnectionValue(char *varName, char *key, int val);
void setConnectionValueForArrayKey(char *varName, char *key, int val, struct CoreObject **_node);

/* Generic array setting */
void setArrayValue(char *varName, char *key, char *value);
void setArrayValueForKey(char *varName, char *key, char *value, struct CoreObject **node);

/* Array to array setting */
void setArrayObject(char *name, struct CoreObject *array);
void _setArrayObjectForKey(char *key, struct CoreObject *array, struct CoreObject **_node);

/* Internal object getting */
struct CoreObject * _getValue(char *name, struct LATReference_Stack *stack);
struct CoreObject * _getArrayValue(char *varName, char *key, struct LATReference_Stack *stack);

/* Array object getting */
struct CoreObject * findObjectWithKey(char *key, struct CoreObject **node);
struct CoreObject * findArrayObjectWithVarKey(char *varName, char *key, struct CoreObject **node);

/* Generic value setting */
char setValueForKey(char *key, char *value, struct CoreObject **_node);

/* CoreObject construction */
char _constructCoreObjectWithKeyVal(struct CoreObject **node, char *key, char *value);

/* Internal core object updating */
void _updateCoreObjectWithVal(struct CoreObject **node, char *value);

/* CoreObject replacement */
struct CoreObject * generateReplacement(struct CoreObject *node);

/* Array Value freeing */
struct CoreObject * freeObjectWithKey(char *key, struct CoreObject *node);

/* Internal array value freeing */
struct CoreObject * freeArrayObjectWithNameKey(char *varName, char *key, struct CoreObject *node);

/* For Copying CoreObjects that act as arrays */
struct CoreObject *__copyArrayCoreObject(struct CoreObject *co);

void freeObjects();
void freeAllHeapObjects();

struct CoreObject *copyArrayCoreObject(struct CoreObject *co);

void freeAllObjects( struct CoreObject *node);

void Core_printMem();
int Core__printMem(struct CoreObject *node, int mem);

#endif /* latria_core_latria_vars_h */
