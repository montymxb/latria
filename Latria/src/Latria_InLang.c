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
 * Latria_InLang.c
 * Latria
 *
 * Created by Benjamin Friedman on 2/11/15.
 */

#include "Latria_InLang.h"

#define CMD_ENDING '\n'

char *langBlock = NULL;
char *varName = NULL;

/* Allocates a new lang block, frees the old one if necessary */
void InLang_startLangBlock(char *_varName) {
    varName = _varName;
}

/* Nulls out the current lang block */
void InLang_NULLLangBlock() {
    langBlock = NULL;
}

/* Appends a cmd to the CMDList, allocates if necessary*/
void InLang_appendToCMDList(char *cmd) {
    
    size_t cmdLen = strlen(cmd);
    if(cmdLen != 0) {
        /* Not an empty line */
        
        if(langBlock != NULL) {
            /* Append to existing block */
            char *mark;
            size_t len1 = strlen(langBlock);
            langBlock = LATAlloc(langBlock, len1, len1+cmdLen+2);
            mark = LATstrcat(langBlock, cmd);
            /* use the ending we received to append the proper line terminator*/
            *mark = CMD_ENDING;
            *++mark = '\0';
        } else {
            /* Create a new block */
            langBlock = LATAlloc(langBlock, 0, cmdLen+2);
            
            strncpy(langBlock, cmd, cmdLen);
            
            /* Append line ending & terminator */
            langBlock[cmdLen] = CMD_ENDING;
            langBlock[cmdLen+1] = '\0';
        }
    }
}

LATBool InLang_isLangBlockActive() {
    return(varName != NULL);
}

/* Returns the CMDList */
char *InLang_getCMDList() {
    return(langBlock != NULL)?langBlock:NULL;
}

/* Returns the varName this will be ultimately assigned to*/
char *InLang_getVarName() {
    return(varName != NULL)?varName:NULL;
}

/* Frees the entire lang block */
void InLang_freeLangBlock() {
    if(langBlock != NULL) {
        LATDealloc( langBlock), langBlock = NULL;
    }
    if(varName != NULL)
        LATDealloc(varName), varName = NULL;
}
