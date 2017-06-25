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
 *  latria_client.c
 *  Created by Benjamin Friedman on 06/24/17.
 */

#include "latria_client.h"


/* Attempts to connect to a server at the given address and port */
int latriaClient_connect(char *address, int port) {
    
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
