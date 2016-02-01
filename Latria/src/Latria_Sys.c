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
 *  Latria_Sys.c
 *  Latria
 *
 *  Created by Benjamin Friedman on 1/22/15.
 */

#include "Latria_Sys.h"

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


/* Starts a server */
int Sys_StartServer(int port) {
    
    #ifdef _WIN32
    /* Windows Server (far back as Vista) */
    
    /* Init winsock */
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult != NO_ERROR) {
        
        /* failed init */
        printf("\n\nWSAStartup failed with err: %ld\n\n", iResult);
        return -1;
        
    }
    
    /* Create a socket to listen for incoming connections */
    SOCKET listenSock;
    listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if(listenSock == INVALID_SOCKET) {
        
        /* Invalid socket */
        printf("\n\nSocket failed with err: %ld\n\n", WSAGetLastError());
        WSACleanup();
        return -1;
        
    }
    
    /* Setup the address family, ip, and port */
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    
    if(bind(listenSock, (SOCKADDR *) &serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        
        /* Failed to bind socket */
        printf("\n\nBind failed with err: %ld\n\n", WSAGetLastError());
        WSACleanup();
        return -1;
        
    }
    
    /* Listen for incoming connection requests */
    if(listen(listenSock, 1) == SOCKET_ERROR) {
        
        printf("\n\nlisten failed with err: %ld\n\n", WSAGetLastError());
        closesocket(listenSock);
        WSACleanup();
        return -1;
        
    }
    
    /* Create a sock for accepting incoming requests */
    SOCKET acceptSock;
    
    /* accept a connection */
    acceptSock = accept(listenSock, NULL, NULL);
    
    if(acceptSock == INVALID_SOCKET) {
        
        /* accept failed */
        printf("\n\naccept failed with error: %ld\n\n", WSAGetLastError());
        closesocket(listenSock);
        WSACleanup();
        return -1;
        
    }
    
    /* Connected to client! */
    
    /* close our original server socket */
    closesocket(listenSock);
    
    /* cleanup */
    WSACleanup();
    
    /* return our newly established connection */
    return acceptSock;
    
    #else
    /* Linux/Mac Server */
    
    
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
    err = listen(sock, 1);
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
    #endif
}


/* Buffer for communication over sockets */
char commOutBuff[1024];
char commInBuff[1024];


/* Sends data over an established connection */
int Sys_SendData(int connId, char *message) {
    
    #ifdef _WIN32
    /* Windows implementation */
    
    int result = 0;
    sprintf(commOutBuff, "%s", message);
    commOutBuff[strlen(message)] = '\0';
    result = send(connId, commOutBuff, strlen(commOutBuff), 0);
    if(result == SOCKET_ERROR) {
        
        /* Failed to send! */
        return 0;
        
    }
    
    return result;
    
    
    #else
    /* linux/mac implementation */
    
    
    ssize_t result = 0;
    size_t msgLen = strlen(message);
        
    sprintf(commOutBuff, "%s", message);
    commOutBuff[msgLen] = '\0';
    
    result = write( connId, commOutBuff, msgLen);
    if(result == -1) {
        
        /* Error, failed to write! */
        return 0;
    }
    
    return (int)result;
    #endif
}


/* Reads data from an established connection */
void Sys_ReadData(int connId) {
    
    char *origData = NULL;
    char *data = NULL;
    
    #ifdef _WIN32
    /* int for windows */
    int n;
    #else
    /*  ssize_t for linux/mac */
    ssize_t n;
    #endif
    
    while(( n = recv(connId, commInBuff, sizeof(commInBuff)-1, 0)) > 0) {
        
        if((size_t)n <= (size_t)sizeof(commInBuff)-1 && data == NULL) {
            
            /* Fits in one run, cap our end, simply set the result and leave */
            commInBuff[n] = 0;
            setSysResult(commInBuff);
            SysStatus = HAS_RESULT;
            return;
            
        } else if(data != NULL) {
            
            /* Append */
            commInBuff[n] = 0;
            origData = LATAlloc(origData, sizeof(char) * strlen(origData), (size_t)(n+1) + strlen(origData));
            data = strcat(data, commInBuff);
            
            if(n < 1024) {
                
                /* Done, return */
                setSysResult(origData);
                SysStatus = HAS_RESULT;
                LATDealloc(origData);
                return;
                
            }
            
        } else if(data == NULL) {
            
            /* Allocate, first run */
            data = origData = LATAlloc(NULL, 0, (size_t)(n+1));
            data = strcat(data, commInBuff);
            
        }
    }
    
    if(origData != NULL) {
        
        /* Indicate we have a result to return */
        SysStatus = HAS_RESULT;
        setSysResult(commInBuff);
        LATDealloc(origData);
        
    }
}


/* Closes an established connection */
void Sys_CloseConnection(int connId) {
    
    #ifdef _WIN32
    /* Windows */
    
    int result = shutdown(connId, SD_SEND);
    if(result == SOCKET_ERROR) {
        
        printf("\n\nShutdown failed with err: %d\n\n", WSAGetLastError());
        
    }
    
    closesocket(connId);
    WSACleanup();
    
    
    #else
    /* Mac/Linux */
    
    close(connId);
    #endif
}


/* Attempts to create an established connection */
int Sys_Connect(char *address, int port) {
    
    #ifdef _WIN32
    /* Windows */
    
    WSADATA wsaData;
    /* init winsock */
    int result = WSAStartup(MAKEWORD(2,2), &wsaData);
    
    if(result != NO_ERROR) {
        
        /* failed to init */
        printf("\n\nWSAStartup function failed with err: %d\n\n", result);
        return -1;
        
    }
    
    /* create socket to connect to server */
    SOCKET connSock;
    connSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if(connSock == INVALID_SOCKET) {
        
        /* failed to create socket */
        printf("\n\nsocket failed with err: %ld\n\n", WSAGetLastError());
        WSACleanup();
        return -1;
        
    }
    
    /* Specify address family, ip and port of server to connect to */
    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr(address);
    sock_addr.sin_port = htons(port);
    
    /* attempt connect to server */
    result = connect(connSock, (SOCKADDR *) &sock_addr, sizeof(sock_addr));
    
    if(result == SOCKET_ERROR) {
        
        /* failed to connect */
        printf("\n\nconnect failed with err: %ld\n\n", WSAGetLastError());
        result = closesocket(connSock);
        
        if(result == SOCKET_ERROR) {
            
            /* failed to close the socket */
            printf("\n\nclosesocket failed with err: %ld\n\n", WSAGetLastError());
            
        }
        
        WSACleanup();
        return -1;
        
    }
    
    /* Connected to server! Return our connected socket */
    return connSock;
    
    
    #else
    
    
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
    #endif
}


#pragma message("Set up coroutines in latria")


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

