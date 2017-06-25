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
 *  latria_connection.c
 *  Created by Benjamin Friedman on 06/24/17.
 */

#include "latria_connection.h"

/* Connection buffer standard size */
#define LAT_CONNECTION_BUFFER_SIZE 1024

/* Buffers for communication over sockets */
char commOutBuff[LAT_CONNECTION_BUFFER_SIZE];
char commInBuff[LAT_CONNECTION_BUFFER_SIZE];


/* Writes data to an established connection */
int latriaConnection_write(int connId, char *message) {
    
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


/* Read data from an established connection */
void latriaConnection_read(int connId) {
    
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
            setSystemStatus(HAS_RESULT);
            return;
            
        } else if(data != NULL) {
            
            /* Append */
            commInBuff[n] = 0;
            origData = LATAlloc(origData, sizeof(char) * strlen(origData), (size_t)(n+1) + strlen(origData));
            data = strcat(data, commInBuff);
            
            if(n < 1024) {
                
                /* Done, return */
                setSysResult(origData);
                setSystemStatus(HAS_RESULT);
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
        setSysResult(commInBuff);
        setSystemStatus(HAS_RESULT);
        LATDealloc(origData);
        
    }
}


/* Closes an established connection */
void latriaConnection_close(int connId) {
    
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
