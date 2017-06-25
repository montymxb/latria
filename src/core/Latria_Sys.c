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
 *  latria_sys.c
 *  Created by Benjamin Friedman on 1/22/15.
 */

#include "latria_sys.h"

/* Holds the current system function status */
Latria_Sys_Status SysStatus;

/* Carries output */
char carrier[200];
/* Holds output to write to the screen */
char batched_writer[1000];
char *batched_index = batched_writer;

/* Stores results of find */
char findResult[100];

/* Null EOF reference */
char *nullEOF = "null";

/* Safe Null reference */
char safeNull = 0;


/* Returns the safe null reference (null vs. "null") */
char didReadSafeNull() {
    
    return safeNull;
}


/* Sets safe null reference */
void setSafeNull(char c) {
    safeNull = c;
}


/* Returns the current system status */
Latria_Sys_Status getSysStatus() {
    return SysStatus;
}

/* Sets the current system status */
void setSystemStatus(Latria_Sys_Status status) {
    SysStatus = status;
    
}


/* Replace a portion of a string with another string */
void Sys_Replace(char *arg1, char *arg2, char *arg3, char arg4) {
    
    /* Call substr & set value */
    char *result, *charTBRef;
    result = str_replace(arg1, arg2, arg3, arg4);
    charTBRef = setCharTablePointer(17, result);
    setSysResult(charTBRef);
    LATDealloc(result);
    
    SysStatus = HAS_RESULT;
    
}


/* Extract a substring out */
void Sys_Substr(char *arg1, int arg2, int arg3) {
    
    /* Call substr & set value */
    char *result,*charTBRef;
    result = LATsubstring( arg1, arg2, arg3);
    charTBRef = setCharTablePointer(16, result);
    setSysResult(charTBRef);
    LATDealloc(result);
    SysStatus = HAS_RESULT;
}


/* Alter the VM GC Rate */
void Sys_GC(float input) {
    
    adjustGCRate(input);
    SysStatus = SUCCEEDED;
}


/* Flushes all memory */
void Sys_Purge() {
    
    freeObjects();
    SysStatus = SUCCEEDED;
}


/* Prints out our memory structure (debugging feature mostly) */
void Sys_PrintMem() {
    
    /* Flush existing writes first */
    Flush_Batched_Write();
    printMem();
    SysStatus = SUCCEEDED;
    
}


/* String find, returns true/false on result */
void Sys_Find(char *arg1, char *arg2) {
    
    char *found = strstr(arg1, arg2);
    
    /* Set our result if NOT null */
    if(found != NULL) {
        sprintf(findResult,"%ld",found-arg1);
    }
    
    /* Mark we have a result */
    SysStatus = HAS_RESULT;
    
    /* Echo out -1 or the location we found the sub string at */
    setSysResult(found!=NULL?findResult:"-1");
    
}


/* Prints out to the console */
void Sys_Print(char *input) {
    
    /* Print the contents of the function call out to the screen */
    
    /* Write to batched out */
    batched_index = strcat(batched_index, input); /* used to be 'arg' */
    batched_index = strcat(batched_index, "\n");
    
    /* Check if adding our new contents would go over our limit, or we are in NO batch mode */
    if(strlen(batched_writer)+strlen(carrier) >= 999 || getPrintCacheMode() == 0) {
        
        /* Flush contents to the screen */
        Flush_Batched_Write();
    }
    
    SysStatus = SUCCEEDED;
    
}


/* Holds a reference for printing numbers on the screen instead of strings */
char holder[20];


void Sys_PrintNum(double num) {
    
    /* Write to batched out */
    sprintf(holder, "%g", num);
    batched_index = strcat(batched_index, holder); /* used to be 'arg' */
    batched_index = strcat(batched_index, "\n");
    if(strlen(batched_writer)+strlen(carrier) >= 999 || getPrintCacheMode() == 0) {
        Flush_Batched_Write();
    }
    
    SysStatus = SUCCEEDED;
}


/* Returns the system we are currently compiled for */
void Sys_Platform() {
    
    char *platform = "undefined";
    #if defined(MACOSX)
    platform = "mac";
    #elif defined(LINUXOS)
    platform = "linux";
    #elif defined(WINDOWOS)
    platform = "windows";
    #endif
    
    setSysResult(platform);
    SysStatus = HAS_RESULT;
}


/* Returns a random number */
int Sys_Random(int input) {
    return rand() % input;
    
}


/* Seeds the RNG */
void Sys_RandomSeed(unsigned int input) {
    srand(input);
    SysStatus = SUCCEEDED;
    
}


/* Reads a line of user input */
void Sys_Input() {
    char userinput[1024];
    int rez = scanf(" %[^\n]", userinput); /* fetch input */
    
    if(rez == EOF) {
        setSysResult(NULL);
        SysStatus = HAS_RESULT;
    }
    
    rez = scanf("%*c");
    
    if(rez == EOF) {
        setSysResult(NULL);
        SysStatus = HAS_RESULT;
    }
    
    setSysResult(userinput);
    SysStatus = HAS_RESULT;
    
}


/* Opens a file to read */
FILE *Sys_Open(char *fileName, char *mode) {
    
    /* Open file */
    FILE *file = fopen(fileName, mode);
    if(file == NULL) {
        /* Could not find the file, spit out this error and crash */
        printf("\n::> The file you provided \"%s\" could not be found\n\n", fileName);
        exit(1);
    }
    
    SysStatus = ASSIGNMENT_SAFE;
    
    return file;
}


/* Holds lines read from Sys_Read */
char line[1024];


/* Reads a given file */
void Sys_Read(FILE *file) {
    
    char *lip = line;
    
    if(file == NULL) {
        /* Somehow an invalid file pointer */
        printf("\n\n>>: Cannot read. The associated file has either been closed or is otherwise inaccesible\n\n");
        exit(591);
    }
    
    /* Indicate we have a result from this operation */
    SysStatus = HAS_RESULT;
    
    /* Get our line */
    if(fgets( lip, 1024, file)) {
        
        /* Strip off the line break on the end, or not */
        if(*(lip+strlen(lip)-1) == '\n') {
            
            *(lip+strlen(lip)-1) = 0;
            
        } else {
            
            *(lip+strlen(lip)) = 0;
            
        }
        
        /* Decrement our lip */
        lip--;
        
        /* Check if we read a SAFE null */
        if(strcmp(line, "null")==0) {
            
            /* We read a safe null, set it */
            safeNull = 1;
        }
        
        /* Set output for operation */
        setSysResult(line);
        
    } else {
        
        /* No more */
        *--lip = '\0';
        
        /* NOT a safe null */
        safeNull = 0;
        
        /* Indicate EOF */
        setSysResult((char *)nullEOF);
        
    }
}


/* Closes a given file */
void Sys_Close(FILE *file) {
    
    if(file == NULL) {
        
        /* An invalid file pointer */
        printf("\n\nCannot close. The associated file has either already been closed or is otherwise inaccesible\n\n");
        exit(591);
    }
    
    /* Closed */
    fclose(file);
    
    SysStatus = SUCCEEDED;
    
}


/* Removes a given a file from the file system */
void Sys_Remove(char *fileName) {
    
    int unlinkResult = unlink(fileName);
    
    if(unlinkResult != 0) {
        /* Failure of some sort */
        printf(">>: File Name not found or otherwise unable to be deleted: %s\n Takes 1 filename and cannot remove a directory", fileName);
    }
    
    SysStatus = SUCCEEDED;
    
}


/* Writes the batched write calls we have to the screen & resets all that good stuff */
void Flush_Batched_Write() {
    
    printf("%s", batched_writer);
    batched_writer[0] = '\0';
    batched_index = batched_writer;
}


#ifdef INCLUDECOMPILER
/* Strictly for testing our print functionality */
char * Read_Batched_Write() {
    
    return batched_writer;
}
#endif


#ifdef LAT_TESTS
/* Just empties the batched writer */
void Flush_Batched_Write_NO_PRINT() {
    
    batched_writer[0] = '\0';
    batched_index = batched_writer;
}
#endif


/* Sleep Call in Latria */
void Sys_Sleep(unsigned int sleepVal) {
    
    #ifdef _WIN32
    Sleep(sleepVal);
    #else
    sleep(sleepVal);
    #endif
}


/* Returns current system time (since epoch) */
unsigned long Sys_Time() {
    
    return (unsigned long)time(0);
}

/* not */
int Sys_bit_not(int i1) {
    
    return ~i1;
}

/* and */
int Sys_bit_and(int i1, int i2) {
    
    return i1 & i2;
}

/* or */
int Sys_bit_or(int i1, int i2) {
    
    return i1 | i2;
}

/* xor */
int Sys_bit_xor(int i1, int i2) {
    
    return i1 ^ i2;
}

/* square root */
double Sys_SquareRoot(double i) {
    
    return sqrt(i);
}

