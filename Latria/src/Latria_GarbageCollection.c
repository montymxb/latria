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
 *  Latria_GarbageCollection.c
 *  Latria
 *
 *  Created by Benjamin Friedman on 3/1/15.
 */

#include "Latria_GarbageCollection.h"

#define LATRIA_ARG_REGISTER_STACK_INCREMENT  100

#define LATRIA_CURRENT_MAX_MEM_SIZE_BASELINE 1024

#define LATRIA_START_STACK                   50000

#define LATRIA_CHAR_TABLE_SIZE               35


/* Latria's Block Chain */
struct MemoryBlock {
    
    /* The next block in the chain */
    struct MemoryBlock *nextBlock;
    
    /* The start of the memory block this points to */
    void *blockPointer;
    
    /* The size of this block */
    size_t blockSize;
    
    /* Marker for whether this block has been allocated or not */
    unsigned char isAllocated;
    
};

/* Latria VM space */
typedef struct {
    struct ControlFlowBlock *_controlFlowBlock;
    
    struct MemoryBlock *memoryChain;
    struct MemoryBlock *lowFreeChain;
    struct MemoryBlock *lastMemoryChain;
    
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


LATVM *constructNewVM();


/* Constructs a new VM instance */
LATVM* constructNewVM() {
    
    int x = 0;
    
    /* Allocate our VM */
    LATVM *latVM = malloc(sizeof(LATVM));
    
    latVM->latriaCurrentMaxMemSize = LATRIA_CURRENT_MAX_MEM_SIZE_BASELINE;
    latVM->currentMemSize          = 0;
    latVM->_controlFlowBlock       = NULL;
    latVM->isCommentedOut          = 0;
    latVM->SYS_LastResult          = NULL;
    latVM->memoryChain             = NULL;
    latVM->lowFreeChain            = NULL;
    latVM->isOptimizedPrintMode    = 0;
    
    /* Set 3 registers */
    latVM->registers = malloc(sizeof(Register) * 3);
    
    /* Set the default types */
    for(;x<3;x++) {
        
        latVM->registers[x].type = RegisterNone;
    }
    
    x = 0;
    
    /* Set 10 argument registers (for functions pushing and popping args) */
    latVM->argRegisters = malloc(sizeof(Register) * LATRIA_ARG_REGISTER_STACK_INCREMENT);
    latVM->argRegisterIndex = 0;
    latVM->maxArgRegisterIndex = LATRIA_ARG_REGISTER_STACK_INCREMENT;
    
    /* set defaults */
    for(;x<LATRIA_ARG_REGISTER_STACK_INCREMENT;x++) {
        
        latVM->argRegisters[x].type = RegisterNone;
    }
    
    return latVM;
}

/* Current VM Instance */
LATVM *currentVM = NULL;


/* Crawls through our LOCAL stack first, attempts to recycle as much as possible before the VM decides to dump everything */
void *lmalloc(size_t sn) {
    
    /* Crawl our stack FIRST */
    struct MemoryBlock *mb = NULL;
    void *ptr;
    
    if(currentVM->memoryChain != NULL) {
        
        /* Crawl down starting at the primary memory chain */
        struct MemoryBlock *nxtBlock = currentVM->memoryChain;
        
        /* Loop through out blocks to see if we have any pointers of DECENT size that are available */
        
        /* Try the lowFree item */
        if(currentVM->lowFreeChain != NULL) {
            
            if(currentVM->lowFreeChain->blockPointer != NULL) {
                
                if(currentVM->lowFreeChain->blockSize >= sn) {
                    
                    /* Bingo! Mark this as allocated again and return it */
                    nxtBlock = currentVM->lowFreeChain;
                    nxtBlock->isAllocated = 1;
                    currentVM->lowFreeChain = NULL;
                    return nxtBlock->blockPointer;
                }
            }
        }
        
        while(nxtBlock != NULL) {
            
            if(nxtBlock->isAllocated == 0) {
                
                /* Not allocated, make sure the pointer hasn't already been freed */
                if(nxtBlock->blockPointer != NULL) {
                    
                    if(nxtBlock->blockSize >= sn) {
                        
                        /* Bingo! Mark this as allocated again and return it */
                        nxtBlock->isAllocated = 1;
                        return nxtBlock->blockPointer;
                    }
                    
                } else if(mb == NULL) {
                    
                    /* Store this for working with if we fail here */
                    mb = nxtBlock->blockPointer;
                }
            }
            
            /* Bump along to the next block */
            nxtBlock = nxtBlock->nextBlock;
        }
    }
    
    /* No Blocks that matched, allocate the memory */
    ptr = malloc(sn);
    
    /* Create a NEW MemoryBlock to track this allocation, if we didn't find an empty one */
    if(mb == NULL) {
        
        mb = malloc(sizeof(struct MemoryBlock));
        mb->isAllocated = 1;
        mb->blockSize = sn;
        mb->blockPointer = ptr;
        mb->nextBlock = NULL;
        
        if(currentVM->memoryChain != NULL) {
            
            /* Place it on the end of our chain */
            currentVM->lastMemoryChain->nextBlock = mb;
            currentVM->lastMemoryChain = mb;
        } else {
            
            /* First block, set it up for start & end */
            currentVM->memoryChain = mb;
            currentVM->lastMemoryChain = mb;
        }
    } else {
        
        /* Recycle our empty one */
        mb->isAllocated = 1;
        mb->blockSize = sn;
        mb->blockPointer = ptr;
    }
    
    /* Bump the vm size */
    currentVM->currentMemSize+=(int)sn;
    
    /* Return this newly allocated memory */
    return ptr;
}


/* Reallocate a pointer we have somewhere here */
void *lrealloc(void *ptr, size_t sn) {
    
    struct MemoryBlock *mb = currentVM->memoryChain;
    
    while(mb != NULL) {
        
        /* Find the matching block to realloc */
        if(mb->blockPointer == ptr) {
            
            /* Found it! Now reallocate it */
            ptr = realloc(ptr, sn);
            /* Update our block pointer */
            mb->blockPointer = ptr;
            /* Update our size */
            mb->blockSize = sn;
            /* Return our reallocated pointer */
            return ptr;
        }
        mb = mb->nextBlock;
    }
    /* Not found, return NULL */
    
    return NULL;
}


/* Marks a block in our stack space as freed! */
void lfree(void *ptr) {
    
    struct MemoryBlock *mb = currentVM->memoryChain;
    
    while(mb != NULL) {
        
        /* Find the matching block to mark as freed */
        if(mb->blockPointer == ptr) {
            
            /* Found it! Now update it */
            mb->isAllocated = 0;
            
            if(currentVM->lowFreeChain == NULL) {
                
                /* Save the last freed item, we can use it */
                currentVM->lowFreeChain = mb;
            }
            break;
        }
        
        mb = mb->nextBlock;
    }
}


/* Latria Allocation function */
void *LATAlloc(void *ptr, size_t so, size_t sn) {
    
    if(so == 0) {
        
        /* Fresh Allocation */
        if((ptr = lmalloc(sn)) == NULL) {
            
            printf("OOM error in Malloc\n");
            exit(115);
        }
        
        return ptr;
    } else {
        
        /* Reallocation */
        if((ptr = lrealloc(ptr, sn)) == NULL) {
            
            printf("OOM error in Realloc\n");
            exit(115);
        }
        
        currentVM->currentMemSize+=(int)(sn-so);
        return ptr;
    }
}


/* Latria Dealloc function NOTE:: This can trigger a system wide GC Operation, which can stall up things as the garbage collector is concurrent*/
void LATDealloc(void *ptr) {
    
    /* 'Free' our pointer*/
    lfree(ptr);
    
    if(currentVM->currentMemSize >= currentVM->latriaCurrentMaxMemSize) {
        
        /* Free all available memory blocks */
        forceMemoryFree();
    }
}


/* Flushes pointers lined up for release, but does NOT free the linked list blocks*/
void forceMemoryFree() {
    
    struct MemoryBlock *mb = currentVM->memoryChain;
    
    /* Iterate over the entire memory chain */
    while(mb != NULL) {
        
        if(mb->isAllocated == 0) {
            
            /* Found a non-allocated block */
            if(mb->blockPointer != NULL) {
                
                /* Found a valid pointer, let's free it & null it */
                free(mb->blockPointer), mb->blockPointer = NULL;
                currentVM->currentMemSize-=(int)mb->blockSize;
            }
        }
        
        /* Bump along to next block */
        mb = mb->nextBlock;
    }
}


/* Frees all memory that has been marked as able to do so, AND frees all containing memory blocks as well*/
void forceALLMemoryFree() {
    
    /* Flush the entire structure */
    
    struct MemoryBlock *mb = currentVM->memoryChain;
    struct MemoryBlock *tmpBlock;
    
    /* Iterate over the entire memory chain */
    while(mb != NULL) {
        
        if(mb->isAllocated == 0) {
            
            /* Found a non-allocated block */
            if(mb->blockPointer != NULL) {
                
                /* Found a valid pointer, let's free it & null it */
                free(mb->blockPointer), mb->blockPointer = NULL;
                currentVM->currentMemSize-=(int)mb->blockSize;
            }
        }
        
        tmpBlock = mb->nextBlock;
        free(mb);
        mb = tmpBlock;
    }
}


/* Adjusts the GC rate*/
void adjustGCRate(float rate) {
    
    currentVM->latriaCurrentMaxMemSize = (int)(LATRIA_CURRENT_MAX_MEM_SIZE_BASELINE * rate);
}


/* Constructs the latria VM*/
void createLatriaVM() {
    
    int x = 0;
    currentVM = constructNewVM();
    
    /* Set up our char table array*/
    for(; x < LATRIA_CHAR_TABLE_SIZE; x++) {
        
        currentVM->charTable[x] = NULL;
    }
}

/* Sets the char * at the index in our table by length*/
char *setCharTablePointerByLEN(int index, unsigned long len) {
    
    if(index >= LATRIA_CHAR_TABLE_SIZE) {
        
        printf("Error! Attempt to set a value in the char table that is beyond the maximum of %d!\n", LATRIA_CHAR_TABLE_SIZE);
        exit(115);
    }
    
    if(currentVM->charTable[index] != NULL) {
        
        char *item = currentVM->charTable[index];
        
        size_t itemLen = currentVM->charTableSize[index];
        
        if(len > itemLen) {
            
            /* Realloc first to match the new size passed in*/
            item = lrealloc( item, (size_t)(len+1) * sizeof(char));
            currentVM->charTableSize[index] = len;
        }
        
        currentVM->charTable[index] = item;
        /* Just copy it back over*/
        
    } else {
        
        char *tmp = lmalloc((size_t)(len+1)*sizeof(char));
        currentVM->charTable[index] = tmp;
        currentVM->charTableSize[index] = len;
    }
    
    return currentVM->charTable[index];
}


/* Sets the char * at the index in our table*/
char *setCharTablePointer(int index, char *ptr) {
    
    if(index >= LATRIA_CHAR_TABLE_SIZE) {
        
        printf("Error! Attempt to set a value in the char table that is beyond the maximum of %d!\n", LATRIA_CHAR_TABLE_SIZE);
        exit(115);
    }
    
    if(currentVM->charTable[index] != NULL) {
        
        size_t ptrLen;
        size_t itemLen;
        char *item = currentVM->charTable[index];
        
        /* Check if the input is the same as our output, if so junk this request*/
        if(!strcmp( item, ptr)) {
            
            return currentVM->charTable[index];
        }
        
        ptrLen = strlen(ptr);
        itemLen =  currentVM->charTableSize[index];
        
        if(ptrLen > itemLen) {
            
            /* Realloc first to match the new size passed in */
            item = lrealloc( item, (ptrLen+1) * sizeof(char));
            currentVM->charTableSize[index] = ptrLen; /*TODO add a +1 potentially to the end of ptrLen, not quite right?*/
        }
        
        strncpy(item, ptr, ptrLen);
        item[ptrLen] = '\0';
        
        currentVM->charTable[index] = item;
        /* Just copy it back over */
        
    } else {
        
        size_t ptrLen = strlen(ptr);
        char *tmp = (char *)lmalloc((ptrLen+1)*sizeof(char));
        
        strncpy(tmp, ptr, ptrLen);
        tmp[ptrLen] = '\0';
        
        currentVM->charTable[index] = tmp;
        currentVM->charTableSize[index] = ptrLen;
    }
    
    return currentVM->charTable[index];
}


/* Sets the result for a system call in the VM */
void setSysResult(char *rez) {
    
    currentVM->SYS_LastResult = rez;
}

/* Gets the last result from a a system call in the vm (NOT ALLOCATED, SAFE FOR NOW) */
char *getSysResult() {
    
    char *poppedResult = currentVM->SYS_LastResult;
    currentVM->SYS_LastResult = NULL;
    return poppedResult;
}


void deconstructLATVM() {
    
    struct ControlFlowBlock *cfb;
    int x = 0;
    /* Deconstruct our char table array*/
    
    for(; x < LATRIA_CHAR_TABLE_SIZE; x++) {
        
        if(currentVM->charTable[x] != NULL)
            lfree(currentVM->charTable[x]);
    }
    
    /* free the registers */
    __freeAllRegisterValues();
    free(currentVM->registers);
    free(currentVM->argRegisters);
    
    forceALLMemoryFree();
    
    cfb = currentVM->_controlFlowBlock;
    
    while(cfb != NULL) {
        
        if(cfb->content)
            LATDealloc(cfb->content);
        
        if(cfb->initializer)
            LATDealloc(cfb->initializer);
        
        if(cfb->limit)
            LATDealloc(cfb->limit);
        
        if(cfb->step)
            LATDealloc(cfb->step);
        
        cfb = cfb->nxtBlock;
    }
    
    free(currentVM);
}


/* Gets the current control flow block for this VM */
struct ControlFlowBlock * getControlFlowBlock() {
    
    return currentVM->_controlFlowBlock;
}


/* Sets the current control flow block for this VM */
void setControlFlowBlock(struct ControlFlowBlock *cfb) {
    
    currentVM->_controlFlowBlock = cfb;
}


/* sets the current block comment state for this VM */
void setBlockCommentState(unsigned char ns) {
    
    currentVM->isCommentedOut = ns;
}


/* Returns the current block comment state for this VM */
unsigned char getBlockCommentState() {
    
    return currentVM->isCommentedOut;
}


void setPrintCacheMode(unsigned char c) {
    
    currentVM->isOptimizedPrintMode = c;
}


unsigned char getPrintCacheMode() {
    
    return currentVM->isOptimizedPrintMode;
}


/* Sets a string value to a register */
void setStringRegister(unsigned char registerNum, char *string, RegisterType type) {
    
    if(registerNum <= 2) {
        
        if(currentVM->registers[registerNum].type == RegisterString || currentVM->registers[registerNum].type == RegisterVar) {
            
            /* Existing string to be freed first */
            LATDealloc(currentVM->registers[registerNum].value.cvalue);
            
        }
        
        /* Set new type & value */
        currentVM->registers[registerNum].type = type;
        currentVM->registers[registerNum].value.cvalue = LATstrdup(string);
        
    } else {
        
        /* Tried to index a non-existant register */
        printf("Attempted to access register num at index %c with '%s', beyond the max register stack size\n",registerNum,string);
        exit(1490);
        
    }
}


/* Sets a num value to a register */
void setNumRegister(unsigned char registerNum, double num, RegisterType type) {
    
    if(registerNum <= 2) {
        
        if(currentVM->registers[registerNum].type == RegisterString || currentVM->registers[registerNum].type == RegisterVar) {
            
            /* Existing string to be freed first */
            LATDealloc(currentVM->registers[registerNum].value.cvalue);
            
        }
        
        /* Set new type & value */
        currentVM->registers[registerNum].type = type;
        currentVM->registers[registerNum].value.dvalue = num;
        
    } else {
        
        /* Tried to index a non-existant register */
        printf("Attempted to access register num at index %d with %f, beyond the max register stack size\n",registerNum,num);
        exit(1490);
        
    }
}


/* Sets a register to hold a FILE value */
void setFileRegister(unsigned char registerNum, FILE *file, RegisterType type) {
    
    if(registerNum <= 2) {
        
        if(currentVM->registers[registerNum].type == RegisterString || currentVM->registers[registerNum].type == RegisterVar) {
            
            /* Existing string to be freed first */
            LATDealloc(currentVM->registers[registerNum].value.cvalue);
            
        }
        
        /* Set new type & value */
        currentVM->registers[registerNum].type = type;
        currentVM->registers[registerNum].value.file = file;
        
    } else {
        
        /* Tried to index a non-existant register */
        printf("Attempted to access register num at index %d with a file, beyond the max register stack size\n",registerNum);
        exit(1490);
        
    }
}


/* Sets a register to hold an entire array */
void setArrayRegister(unsigned char registerNum, struct CoreObject *array, RegisterType type) {
    
    if(registerNum <= 2) {
        
        if(currentVM->registers[registerNum].type == RegisterString || currentVM->registers[registerNum].type == RegisterVar) {
            
            /* Existing string to be freed first */
            LATDealloc(currentVM->registers[registerNum].value.cvalue);
            
        }
        
        /* Set new type & value */
        currentVM->registers[registerNum].type = type;
        currentVM->registers[registerNum].value.array = array;
        
    } else {
        
        /* Tried to index a non-existant register */
        printf("Attempted to access register num at index %d with an entire array, beyond the max register stack size\n",registerNum);
        exit(1490);
        
    }
}


/* Sets a register to a NULL value */
void setNullRegister(unsigned char registerNum) {
    
    if(registerNum <= 2) {
        
        if(currentVM->registers[registerNum].type == RegisterString || currentVM->registers[registerNum].type == RegisterVar) {
            
            /* Existing string to be freed first */
            LATDealloc(currentVM->registers[registerNum].value.cvalue);
            
        }
        
        /* Set new type, but NO value */
        currentVM->registers[registerNum].type = RegisterNull;
        
    } else {
        
        /* Tried to index a non-existant register */
        printf("Attempted to access register num at index %d with null value, beyond the max register stack size\n",registerNum);
        exit(1490);
        
    }
}


/* Sets a register to a NULL value */
void setConnectionRegister(unsigned char registerNum, int connId) {
    
    if(registerNum <= 2) {
        
        if(currentVM->registers[registerNum].type == RegisterString || currentVM->registers[registerNum].type == RegisterVar) {
            
            /* Existing string to be freed first */
            LATDealloc(currentVM->registers[registerNum].value.cvalue);
            
        }
        
        /* Set type & value */
        currentVM->registers[registerNum].type = RegisterConnection;
        currentVM->registers[registerNum].value.dvalue = connId;
        
    } else {
        
        /* Tried to index a non-existant register */
        printf("Attempted to access register num at index %d with a register value, beyond the max register stack size\n",registerNum);
        exit(1490);
        
    }
}


/* Get register type */
RegisterType getRegisterType(unsigned char registerNum) {
    
    if(registerNum <= 2) {
        
        return currentVM->registers[registerNum].type;
        
    } else {
        
        printf("Attempted to access register num at index %d, beyond the max register stack size\n", registerNum);
        exit(4928);
        
    }
}


/* Get register num value */
double getRegisterNum(unsigned char registerNum) {
    
    if(registerNum <= 2) {
        
        return currentVM->registers[registerNum].value.dvalue;
        
    } else {
        
        printf("Attempted to access register num at index %d, beyond the max register stack size\n", registerNum);
        exit(4928);
        
    }
}


/* Get register string value */
char *getRegisterString(unsigned char registerNum) {
    
    if(registerNum <= 2) {
        
        return currentVM->registers[registerNum].value.cvalue;
        
    } else {
        
        printf("Attempted to access register num at index %d, beyond the max register stack size\n", registerNum);
        exit(4928);
        
    }
}


/* Get register file value */
FILE *getRegisterFile(unsigned char registerNum) {
    
    if(registerNum <= 2) {
        
        return currentVM->registers[registerNum].value.file;
        
    } else {
        
        printf("Attempted to access register num at index %d for a file, beyond the max register stack size\n", registerNum);
        exit(4928);
        
    }
}


int getRegisterConnection(unsigned char registerNum) {
    
    if(registerNum <= 2) {
        
        return (int)currentVM->registers[registerNum].value.dvalue;
        
    } else {
        
        printf("Attempted to access register num at index %d for a connection, beyond the max register stack size\n", registerNum);
        exit(4928);
        
    }
}


/* Get register array value */
struct CoreObject *getRegisterArray(unsigned char registerNum) {
    
    if(registerNum <= 2) {
        
        return currentVM->registers[registerNum].value.array;
        
    } else {
        
        printf("Attempted to access register num at index %d for an array, beyond the max register stack size\n", registerNum);
        exit(4928);
        
    }
}


/* Private Register Functions */
void pushStringStackRegister(char *string, RegisterType type);
void pushNumStackRegister(double num, RegisterType type);
void pushFileStackRegister(FILE *file, RegisterType type);
void pushArrayStackRegister(struct CoreObject *array, RegisterType type);
void pushNullStackRegister(RegisterType type);
void pushConnectionStackRegister(int connId);


/* Pushes the contents of an operations register to a stack register */
void pushRegister(unsigned char registerNum) {
    
    if(registerNum <= 2) {
        
        Register r = currentVM->registers[registerNum];
        
        if(r.type == RegisterNum || r.type == RegisterBool) {
            
            /* numeric */
            pushNumStackRegister(r.value.dvalue, r.type);
            
        } else if(r.type == RegisterString) {
            
            /* string */
            pushStringStackRegister(r.value.cvalue, r.type);
            
        } else if(r.type == RegisterFile) {
            
            /* file */
            pushFileStackRegister(r.value.file, r.type);
            
        } else if(r.type == RegisterArray) {
            
            /* array */
            /* Create a deep copy of our array ref, leaving our old ref behind */
            pushArrayStackRegister(r.value.array, r.type);
            
            #pragma message("Thought this was need to prevent references to freed arrays when, say, stacks are popped 'off'. Apparently not the case.")
            /* pushArrayStackRegister(copyArrayCoreObject(r.value.array), r.type); */
            
        } else if(r.type == RegisterNull) {
            
            /* null */
            pushNullStackRegister(r.type);
            
        } else if(r.type == RegisterConnection) {
            
            /* connection */
            pushConnectionStackRegister((int)r.value.dvalue);
            
        } else {
            
            /* Unrecognized */
            printf("Attempted to push register of unrecognized type to variable on stack.\n");
            exit(65000);
            
        }
        
        __pushStackRegister();
        
    } else {
        
        printf("Attempted to access register num at index %d, beyond the max register stack size\n", registerNum);
        exit(4928);
        
    }
}


/* Adds a string stack register item */
void pushStringStackRegister(char *string, RegisterType type) {
    
    if(currentVM->argRegisters[currentVM->argRegisterIndex].type == RegisterString ||
       currentVM->argRegisters[currentVM->argRegisterIndex].type == RegisterVar)
    {
        
        /* Existing string to be freed first */
        LATDealloc(currentVM->argRegisters[currentVM->argRegisterIndex].value.cvalue);
        
    }
    
    currentVM->argRegisters[currentVM->argRegisterIndex].type = type;
    currentVM->argRegisters[currentVM->argRegisterIndex].value.cvalue = LATstrdup(string);
}


/* Adds a num stack register item */
void pushNumStackRegister(double num, RegisterType type) {
    
    if(currentVM->argRegisters[currentVM->argRegisterIndex].type == RegisterString ||
       currentVM->argRegisters[currentVM->argRegisterIndex].type == RegisterVar)
    {
        
        /* Existing string to be freed first */
        LATDealloc(currentVM->argRegisters[currentVM->argRegisterIndex].value.cvalue);
        
    }
    
    currentVM->argRegisters[currentVM->argRegisterIndex].type = type;
    currentVM->argRegisters[currentVM->argRegisterIndex].value.dvalue = num;
}


/* Adds a file stack register item */
void pushFileStackRegister(FILE *file, RegisterType type) {
    
    if(currentVM->argRegisters[currentVM->argRegisterIndex].type == RegisterString ||
       currentVM->argRegisters[currentVM->argRegisterIndex].type == RegisterVar)
    {
        
        /* Existing string to be freed first */
        LATDealloc(currentVM->argRegisters[currentVM->argRegisterIndex].value.cvalue);
        
    }
    
    currentVM->argRegisters[currentVM->argRegisterIndex].type = type;
    currentVM->argRegisters[currentVM->argRegisterIndex].value.file = file;
}


/* Adds an array stack register item */
void pushArrayStackRegister(struct CoreObject *array, RegisterType type) {
    
    if(currentVM->argRegisters[currentVM->argRegisterIndex].type == RegisterString ||
       currentVM->argRegisters[currentVM->argRegisterIndex].type == RegisterVar)
    {
        
        /* Existing string to be freed first */
        LATDealloc(currentVM->argRegisters[currentVM->argRegisterIndex].value.cvalue);
        
    }
    
    currentVM->argRegisters[currentVM->argRegisterIndex].type = type;
    currentVM->argRegisters[currentVM->argRegisterIndex].value.array = array;
}


/* Adds a null stack register item */
void pushNullStackRegister(RegisterType type) {
    
    if(currentVM->argRegisters[currentVM->argRegisterIndex].type == RegisterString ||
       currentVM->argRegisters[currentVM->argRegisterIndex].type == RegisterVar)
    {
        
        /* Existing string to be freed first */
        LATDealloc(currentVM->argRegisters[currentVM->argRegisterIndex].value.cvalue);
        
    }
    
    /* Type only */
    currentVM->argRegisters[currentVM->argRegisterIndex].type = type;
}


void pushConnectionStackRegister(int connId) {
    
    if(currentVM->argRegisters[currentVM->argRegisterIndex].type == RegisterString ||
       currentVM->argRegisters[currentVM->argRegisterIndex].type == RegisterVar)
    {
        
        /* Existing string to be freed first */
        LATDealloc(currentVM->argRegisters[currentVM->argRegisterIndex].value.cvalue);
        
    }
    
    /* Type & Value */
    currentVM->argRegisters[currentVM->argRegisterIndex].type = RegisterConnection;
    currentVM->argRegisters[currentVM->argRegisterIndex].value.dvalue = connId;
}


/* Actually pushes register vals on the stack. Reallocs up if we need to as well */
void __pushStackRegister() {
    
    /* Increment */
    currentVM->argRegisterIndex++;
    
    /* Check */
    if(currentVM->argRegisterIndex >= currentVM->maxArgRegisterIndex-1) {
        
        /* Realloc Up to next size */
        #pragma message("this is a rather nasty bug. When this is called the program is dead, it will inevitably segfault due to the realloc not taking place, to some degree or another. As to 'why', I have a clue, but it continues to alude me. Reduce the constant stack size and fix this when i'm ready")
        currentVM->maxArgRegisterIndex+=LATRIA_ARG_REGISTER_STACK_INCREMENT;
        *(currentVM->argRegisters) = *(Register *)realloc(&currentVM->argRegisters, currentVM->maxArgRegisterIndex);
    }
}


/* Get the type of the current register type on the stack */
RegisterType getStackRegisterType() {
    
    return currentVM->argRegisterIndex > 0 ? currentVM->argRegisters[currentVM->argRegisterIndex-1].type : RegisterNone;
}


/* Pop and return the number the of the current register item */
double popStackRegisterNum() {
    
    return currentVM->argRegisters[--currentVM->argRegisterIndex].value.dvalue;
}


/* Pop and return the string value of the current register item */
char *popStackRegisterString() {
    
    return currentVM->argRegisters[--currentVM->argRegisterIndex].value.cvalue;
}


/* Pops and returns the array value of hte current register item */
struct CoreObject *popStackRegisterArray() {
    
    return currentVM->argRegisters[--currentVM->argRegisterIndex].value.array;
}


/* Pops and returns the file value of the current register item */
FILE *popStackRegisterFile() {
    
    return currentVM->argRegisters[--currentVM->argRegisterIndex].value.file;
}


/* Simply pops the current 'null' register item, disregarding the item itself */
void popStackRegisterNull() {
    
    currentVM->argRegisterIndex--;
}


int popStackRegisterConnection() {
    
    return (int)currentVM->argRegisters[--currentVM->argRegisterIndex].value.dvalue;
}


/* Frees any strings on registers of string or var type */
void __freeAllRegisterValues() {
    
    unsigned char x = 0;
    
    for(;;) {
        
        if(currentVM->registers[x].type == RegisterVar || currentVM->registers[x].type == RegisterString) {
            
            LATDealloc(currentVM->registers[x].value.cvalue);
        }
        
        if(x >= 2) {
            
            break;
        }
            
        x++;
    }
    
    x = 0;
    
    for(;;) {
        
        if(currentVM->argRegisters[x].type == RegisterVar || currentVM->argRegisters[x].type == RegisterString) {
            
            LATDealloc(currentVM->argRegisters[x].value.cvalue);
        }
        
        if(x >= currentVM->maxArgRegisterIndex-1) {
            
            break;
        }
        
        x++;
    }
}


/* Prints Memory held under this VM, but not the memory used by the VM instance itself*/
void printMem() {
    
    printf("\n\nMemory Used: %d bytes\n%.3f kbs\n\n", currentVM->currentMemSize, currentVM->currentMemSize/1024.0);
    /* Print memory from latria CoreObjects */
    Core_printMem();
}
