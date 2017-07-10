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
 *  latria_core.h
 */

#ifndef latria_core_latria_core_h
#define latria_core_latria_core_h

/* Current latria version */
#define LATRIA_VERSION_NUMBER "0.3.6"

/* 4 */
#define LAT_ADDRESS_SIZE 8

/* %04x */
#define LAT_ADDRESS_FORMAT_STRING "%08x"

/* 5 */
#define JUMP_UPDATE_INCREMENT 9

/* The size of instructions for the interpreter */
#define LAT_I_ADDRESS_SIZE 8

/* (Hacky) Prevents a popen warning in linux, one of many approaches to address this */
#define _POSIX_C_SOURCE 2

/* Free block chain size for VM */
#define LATRIA_FREE_BLOCK_CHAIN_SIZE 10

/* Argument register stack increment for VM */
#define LATRIA_ARG_REGISTER_STACK_INCREMENT  100

/* Maximum Mem Pressure before GC triggers for VM */
#define LATRIA_CURRENT_MAX_MEM_SIZE_BASELINE 655360

/* Initial size for VM */
#define LATRIA_START_STACK                   50000

/* Initial character table size */
#define LATRIA_CHAR_TABLE_SIZE               35

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#else
/* 
 _Win32 is usually defined by compilers targeting Windows.
 _Win32 is generally provided even when compiling on a 64 bit machine
 for backwards compatibility */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

/* must link with ws2_32.lib */
#pragma comment(lib,"ws2_32.lib")
#endif

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

#include "latria_bool.h"
#include "latria_register_type.h"
#include "latria_register.h"
#include "latria_core_object.h"

#endif /* latria_core_latria_core_h */
