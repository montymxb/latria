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
 *  latria_server.c
 *  Created by Benjamin Friedman on 06/24/17.
 */

#include "latria_server.h"

/* Starts listening for connections on the given port */
int latriaServer_listen(int port) {
    
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
