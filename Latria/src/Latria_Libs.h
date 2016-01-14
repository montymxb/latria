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

#ifndef __Latria_Libs__
#define __Latria_Libs__

/* (Hacky) Prevents a popen warning in linux, one of many approaches to address this */
#define _POSIX_C_SOURCE 2

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#else /* _Win32 is usually defined by compilers targeting Windows. 32 is provided even when compiling on a 64 bit machine for backwards compatibility */
#include <windows.h>
#endif

#include <time.h>

/* Strictly for sqrt */
#include <math.h>

/* For sockets */
#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

/* string comparison macro */

#define LAT_STRCMP(a, b)  (*(a) != *(b) ? \
(int) ((unsigned char) *(a) - \
       (unsigned char) *(b)) : \
strcmp((a), (b)))

/* string duplication */
char *LATstrdup(char *s);
/* string concatenation */
char *LATstrcat(char *d, char *s);

/* bool is not defined on some systems without including the proper header, this is the substitute bool */
typedef enum { false, true } LATBool;

/*** REGISTER ENUM ***/

typedef enum {
    RegisterVar,        /* var                  */
    RegisterNum,        /* num                  */
    RegisterString,     /* str                  */
    RegisterBool,       /* bool (just a #)      */
    RegisterNone,       /* empty                */
    RegisterArray,      /* entire array         */
    RegisterArrayVar,   /* sets from array key  */
    RegisterFile,       /* file                 */
    RegisterConnection, /* connection           */
    RegisterNull        /* null                 */
}RegisterType;

/*** END REGISTER ENUM ***/


/*Stores all core data types, numeric (float) & string for now*/
struct CoreObject {
    /* left object (when an array ONLY the left object is used) */
    /* NOTE, array type is a linked list in it's simplest form */
    struct CoreObject *lobj;
    /* right object (when an array this is always null) */
    struct CoreObject *robj;
    
    /* Shared memory for values */
    union {
        /* FILE * */
        FILE *_file;
        /* Number */
        double _fvalue;
        /* String */
        char *_svalue;
        /* Bool */
        LATBool _bvalue;
    } data;
    
    /* Key to fetch this value by */
    char *key;
    
    unsigned char _state; /*
                           0=float
                           1=string
                           2=bool
                           3=array
                           4=file
                           5=connection
                           */
};

/*** REGISTER STRUCT START ***/

typedef struct {
    /* Value */
    union {
        double dvalue;
        char *cvalue;
        FILE *file;
        struct CoreObject *array;
    }value;
    
    /* Type */
    RegisterType type;
    
}Register;

/*** REGISTER STRUCT END ***/

/*Basic Latria Function object*/
struct LATFunction {
    struct LATFunction *lobj;
    struct LATFunction *robj;
    struct LATFParameter *coreParam;
    char * key;
    char * content;
    char * eol;
};

/*Linked list for passed in vars to a function as it is being created
 these variables are copied into that function's stack space at the appropriate time
 */
struct LATFParameter {
    struct LATFParameter *param;
    char *valName;
};

struct LATReference_Stack{
    struct CoreObject *cop; /* CoreObject Primary */
    struct LATFunction *latfp; /* LATFunction Primary */
    struct LATReference_Stack *cs; /*Child Stack*/
    
    char *returnValue; /*Stack return value*/
    
    char *_tailCallAssociatedName; /* Tail call return name (if applicable) for the current reference stack*/
    
    char *curLATFuncList; /* Used for building up functions */
    unsigned int latfBrackCount; /* Used to avoid confusion with brackets which may otherwise terminate a function too soon */
};

char *str_replace(char *orig, char *rep, char *with, char removeAll);
char *str_replaceNCPY(char *orig, char*rep, char*with);
LATBool isNumeric(char *val);
char * stripWhitespace(char *input);

/* forwardly declared from Latria_GC */
void *LATAlloc(void *ptr, size_t so, size_t sn);
void LATDealloc(void *ptr);
void *lmalloc(size_t sn);
void *lrealloc(void *ptr, size_t sn);
void lfree(void *ptr);

/* Forwardly Declared from Latria_Lexical */
void stripComments(char *input);

#endif /* defined(__Latria_Libs__) */
