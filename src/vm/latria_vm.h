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
 *  latria_vm.h
 *  Created by Benjamin Friedman on 3/1/15.
 */

#ifndef latria_vm_latria_vm_h
#define latria_vm_latria_vm_h

#include "core/latria_register.h"
#include "memory/latria_memory_block.h"

/* Latria VM space */
typedef struct {
    
    /* Primary chain of memory blocks */
    struct MemoryBlock *memoryChain;
    
    /* The last item on our primary chain, for adding new blocks */
    struct MemoryBlock *lastMemoryChain;
    
    /* Last block we freed */
    struct MemoryBlock *freeBlockChain[LATRIA_FREE_BLOCK_CHAIN_SIZE];
    
    char *charTable[LATRIA_CHAR_TABLE_SIZE];
    
    /* Last result from the SYS Module */
    char *SYS_LastResult;
    
    unsigned long charTableSize[LATRIA_CHAR_TABLE_SIZE];
    int latriaCurrentMaxMemSize;
    int currentMemSize;
    unsigned char isCommentedOut;
    unsigned char isOptimizedPrintMode;
    
    /* Our 3 operations registers */
    Register *registers;
    
    /* 10 argument registers (to start) */
    Register *argRegisters;
    
    unsigned int argRegisterIndex;
    unsigned int maxArgRegisterIndex;
    
} LATVM;

#endif /* latria_vm_latria_vm_h */
