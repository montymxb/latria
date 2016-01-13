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
*  Latria_Sys.c
*  Latria
*
*  Created by Benjamin Friedman on 1/22/15.
*/

#include "Latria_Sys.h"

static Latria_Sys_Status SysStatus;

char carrier[200];
char batched_writer[1000];
char *batched_index = batched_writer;
char findResult[20];

char *nullEOF = "null";
char safeNull = 0;

char didReadSafeNull() {
    return safeNull;
}

void setSafeNull(char c) {
    safeNull = c;
}

Latria_Sys_Status getSysStatus() {
    return SysStatus;
}

/*
LATBool checkForSystemCommand(char *input, char *orig) {
    switch(*(input+1)) {
        case 'B':
             Print
             return Sys_Print(input);
        case 'A':
            PrintMem
             return Sys_PrintMem(); 
        case 'C':
             Sleep 
             return Sys_Sleep(input); 
        case 'D':
             Purge 
            return Sys_Purge();
        case 'E':
             GC Rate 
            return Sys_GC(input);
        case 'F':
             Find 
             return Sys_Find(input); 
        case 'G':
             Replace 
             return Sys_Replace(input); 
        case 'H':
             Substr 
             return Sys_Substr(input); 
        case 'I':
             Open 
             return Sys_Open(input, orig); 
        case 'J':
             Read 
             return Sys_Read(input); 
        case 'K':
             Write 
             return Sys_Write(input); 
        case 'L':
             Close 
             return Sys_Close(input); 
        case 'M':
             Remove 
             return Sys_Remove(input); 
        case 'N':
             Random 
             return Sys_Random(input); 
        case 'O':
             Seed Random 
             return Sys_RandomSeed(input); 
        case 'P':
             Read user line of input 
             return Sys_Input(input); 
        default:
             Fail on default 
            SysStatus = FAILED;
            return false;
    }
}
*/

/* Replace a portion of a string with another string */
void Sys_Replace(char *arg1, char *arg2, char *arg3, char arg4) {
    /*int argCount = fetchNumArgs(input);*/
    
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
    /* print the contents of the function call out to the screen */
    /* Write to batched out */
    batched_index = LATstrcat(batched_index, input); /* used to be 'arg' */
    batched_index = LATstrcat(batched_index, "\n");
    if(strlen(batched_writer)+strlen(carrier) >= 999 || getPrintCacheMode() == 0) {
        Flush_Batched_Write();
    }
    
    SysStatus = SUCCEEDED;
    
}

char holder[20];

void Sys_PrintNum(double num) {
    /* Write to batched out */
    sprintf(holder, "%g", num);
    batched_index = LATstrcat(batched_index, holder); /* used to be 'arg' */
    batched_index = LATstrcat(batched_index, "\n");
    if(strlen(batched_writer)+strlen(carrier) >= 999 || getPrintCacheMode() == 0) {
        Flush_Batched_Write();
    }
    
    SysStatus = SUCCEEDED;
}

/* Returns the architecture we are currently compiled for */
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
    scanf(" %[^\n]", userinput); /*fetch input*/
    scanf("%*c");
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

/* For sys Read */
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
    /* *lip++='"'; */
    if(fgets( lip, 1024, file)) {
        /* Strip off the line break on the end, or not */
        if(*(lip+strlen(lip)-1) == '\n') {
            *(lip+strlen(lip)-1) = 0;
        } else {
            *(lip+strlen(lip)) = 0;
        }
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
        /* Somehow an invalid file pointer */
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

#if defined(LAT_TESTS)
/* Strictly for testing our print functionality */
char * Read_Batched_Write() {
    return batched_writer;
}
/* Just empties the batched writer */
void Flush_Batched_Write_NO_PRINT() {
    batched_writer[0] = '\0';
    batched_index = batched_writer;
}
#endif

/* Returns whether or not an unquoted character 'c' exists in given char pointer 's' */
int findUnquotedCharacter(char c, char *s) {
    char *orig = s;
    unsigned char passedFirst = 0;
    char *yc = strchr(s, c);
    
    if(yc == NULL) {
        /* NO target character found in this string, return -1 */
        return -1;
    }
    
    if(strchr(s,'"') == NULL && strchr(s, '\'') == NULL) {
        /* NO double or single quotes, return the first index of this character */
        int index = (int)(yc-s);
        return index;
    }
    
    /* While loop over our input */
    while(*s) {
        /* If tgt string found */
        if(*s == c) {
            /* The index of what we found */
            int index = (int)(s-orig);
            return index;
            
        } else if(*s == '"') {
            /* Double Quote */
            if((passedFirst == 1 && *(s-1) != '\\') || passedFirst == 0) {
                /* Not escaped or first pass */
                while(true) {
                    /* Loop until we escape double quotes */
                    yc=strchr(s+1, '"');
                    if(yc) {
                        /* We found double quotes */
                        if(*(yc-1) != '\\') {
                            /* Valid closing Double Quote, set out pointer to it and move along */
                            s = yc;
                            break;
                        }
                    } else {
                        /* Error, no closing double quote! */
                        printf("No closing double quote found :>> %s\n", orig);
                        exit(6918);
                    }
                }
            }
            
        } else if(*s == '\'') {
            /* Single Quote */
            if((passedFirst == 1 && *(s-1) != '\\') || passedFirst == 0) {
                /* Not escaped or first pass */
                while(true) {
                    /* Loop until we escape single quotes */
                    yc=strchr(s+1, '\'');
                    if(yc) {
                        /* We found single quotes */
                        if(*(yc-1) != '\\') {
                            /* Valid closing Single Quote, set out pointer to it and move along */
                            s = yc;
                            break;
                        }
                    } else {
                        /* Error, no closing single quote! */
                        printf("No closing single quote found :>> %s\n", s);
                        exit(6918);
                    }
                }
            }
        }
        passedFirst = 1;
        s++;
    }
    return -1;
}

/* Finds a character that is unquoted & unparenthesized, completely on it's own */
int findUncontainedChar(char c, char *s) {
    char *orig = s;
    unsigned char passedFirst = 0;
    char *yc = strchr(s, c);
    
    if(yc == NULL) {
        /* NO target character found in this string, return -1 */
        return -1;
    }
    
    if(strchr(s,'"') == NULL && strchr(s, '\'') == NULL && strchr(s, '(') == NULL && strchr(s, ')') == NULL) {
        /* NO double or single quotes & no opening or closing parentheses, return the first index of this character */
        int index = (int)(yc-s);
        return index;
    }
    
    /* While loop over our input */
    while(*s) {
        /* If tgt string found */
        if(*s == c) {
            /* The index of what we found */
            int index = (int)(s-orig);
            return index;
            
        } else if(*s == '"') {
            /* Double Quote found */
            if((passedFirst == 1 && *(s-1) != '\\') || passedFirst == 0) {
                /* Not escaped or first pass */
                while(true) {
                    yc=strchr(s+1, '"');
                    if(yc) {
                        /* We found double quotes */
                        if(*(yc-1) != '\\') {
                            /* Valid closing Double Quote, set out pointer to it and move along */
                            s = yc;
                            break;
                        }
                    } else {
                        /* Error, no closing double quote! */
                        printf("No closing double quote found :>> %s\n", orig);
                        exit(6918);
                    }
                }
            }
            
        } else if(*s == '(') {
            /* Opening Parenthese found */
            if((passedFirst == 1 && *(s-1) != '\\') || passedFirst == 0) {
                /* Not escaped or first pass */
                while(true) {
                    int indexFound;
                    yc=s+1;
                    indexFound=findUnquotedCharacter(')', s+1);
                    if(indexFound != -1) {
                        /* We found a closing parenthese */
                        yc+=indexFound;
                        /* Valid closing paren, set out pointer to it and move along */
                        s = yc;
                        break;
                    } else {
                        /* Error, no closing double quote! */
                        printf("No closing double quote found :>> %s\n", orig);
                        exit(6918);
                    }
                }
            }
            
        } else if(*s == '\'') {
            /* Single Quote found */
            if((passedFirst == 1 && *(s-1) != '\\') || passedFirst == 0) {
                /* Not escaped or first pass */
                while(true) {
                    yc=strchr(s+1, '\'');
                    if(yc) {
                        /* We found single quotes */
                        if(*(yc-1) != '\\') {
                            /* Valid closing Single Quote, set out pointer to it and move along */
                            s = yc;
                            break;
                        }
                    } else {
                        /* Error, no closing single quote! */
                        printf("No closing single quote found :>> %s\n", s);
                        exit(6918);
                    }
                }
            }
            
        }
        passedFirst = 1;
        s++;
    }
    return -1;
}

/* Sleep Call in Latria */
void Sys_Sleep(unsigned int sleepVal) {
    sleep(sleepVal);
}

/* Time in latria */
unsigned long Sys_Time() {
    return (unsigned long)time(0);
}

/* Starts a server */
int Sys_StartServer(int port) {
    int optval = 1, err, connection;
    struct sockaddr_in serv_addr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    memset( &serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    /* Accept connection from any address */
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Bind to given port */
    serv_addr.sin_port = htons(port);
    
    /* Allow this socket to be reusable, aka restart does not end up with you unable to reaccess your previous (and still running) spot */
    setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    
    /* bind it to our address */
    err = bind( sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(err == -1) {
        printf("Error attempting to bind server to address\n");
        exit(1);
        
    }
    
    /* listen for connections to this socket */
    err = listen(sock, 10);
    if(err == -1) {
        /* error */
        printf("Error attempting to listen for connections on a socket\n");
        exit(1);
        
    }
    
    /* Blocks until we receive a connection */
    connection = accept( sock, (struct sockaddr*)NULL, NULL);
    
    /* close our original socket */
    close(sock);
    
    /* return our newly established connection */
    return connection;
}

char commBuff[1024];

/* Sends data over an established connection */
int Sys_SendData(int connId, char *message) {
    ssize_t result = 0;
    while(strlen(message) < 1024) {
        /* Write out blocks of 1024 at a time */
        snprintf( commBuff, sizeof(commBuff), "%s", message);
        result = write( connId, commBuff, strlen(commBuff));
        if(result == -1) {
            /* Error, failed to write! */
            return 0;
        }
        
        if(strlen(message) >= 1024) {
            /* Increment message by 1024 */
            message+=1024;
            
        } else {
            /* End of message */
            break;
            
        }
    }
    return (int)result;
}

/* Reads data from an established connection */
void Sys_ReadData(int connId) {
    ssize_t n;
    char *origData = NULL;
    char *data = NULL;
    while(( n = read(connId, commBuff, sizeof(commBuff)-1)) > 0) {
        
        if(n <= sizeof(commBuff)-1 && data == NULL) {
            /* Fits in one run, simply set the result and leave */
            commBuff[n] = 0;
            setSysResult(commBuff);
            SysStatus = HAS_RESULT;
            return;
            
        } else if(data != NULL) {
            /* Append */
            commBuff[n] = 0;
            origData = LATAlloc(origData, sizeof(char) * strlen(origData), n+1 + strlen(origData));
            data = LATstrcat(data, commBuff);
            
            if(n < 1024) {
                /* Done, return */
                setSysResult(origData);
                SysStatus = HAS_RESULT;
                LATDealloc(origData);
                return;
                
            }
            
        } else if(data == NULL) {
            /* Allocate, first run */
            data = origData = LATAlloc(NULL, 0, n+1);
            data = LATstrcat(data, commBuff);
            
        }
    }
    
    if(origData != NULL) {
        /* Indicate we have a result to return */
        SysStatus = HAS_RESULT;
        setSysResult(commBuff);
        LATDealloc(origData);
        
    }
}

/* Closes an established connection */
void Sys_CloseConnection(int connId) {
    close(connId);
}

/* Attempts to create an established connection */
int Sys_Connect(char *address, int port) {
    struct sockaddr_in serv_addr;
    int sock;
    ssize_t err;
    
    memset( &serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if(sock < 0) {
        printf("An error occurred creating a socket for latria to connect via\n");
        exit(1);
        
    }
    
    /* Convert address to a network address structure in the af address family (binary form), and copy it into serv_addr.sin_addr */
    err = inet_pton(AF_INET, address, &serv_addr.sin_addr);
    
    if(err == 0) {
        /* Invalid address provided */
        printf("Invalid address provided to connect to, %s:%d\n", address,port);
        exit(1);
        
    } else if(err == -1) {
        /* Error converting address to binary format*/
        printf("Error occurred converting provided address to a binary form, %s:%d\n",address,port);
        exit(1);
        
    }
    
    /* Attempt to connect this socket to the given address */
    err = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    
    
    if(err == 0) {
        /* return our connected socket */
        return sock;
        
    } else {
        /* return failure */
        return -1;
        
    }
}

#warning Set up coroutines in latria

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

