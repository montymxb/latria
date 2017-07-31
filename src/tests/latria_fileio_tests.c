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
 *  latria_fileio_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_fileio_tests.h"


void test_FileIO();

/**
 * Run these tests
 */
void latria_fileio_tests() {
    test_FileIO();
}

/* Tests File IO */
void test_FileIO() {
    char *rez;
    ctest_begin("file io");
    
    strcpy((getTestLineHolder()), "x=open(\"latria-test-file-for-functional-testing.txt\",\"w\")");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "write(x,\"line1\")");
    handleInput((getTestLineHolder()));
    strcpy((getTestLineHolder()), "write(x,\"null\")");
    handleInput((getTestLineHolder()));
    strcpy((getTestLineHolder()), "write(x,\"line3\")");
    handleInput((getTestLineHolder()));
    strcpy((getTestLineHolder()), "write(x,\"line4\")");
    handleInput((getTestLineHolder()));
    strcpy((getTestLineHolder()), "write(x,\"final line, goodbye\")");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("close(x)");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "x=open(\"latria-test-file-for-functional-testing.txt\",\"r\")");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("line=read(x)");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("while(line!=null){");
    handleInput((getTestLineHolder()));
    strcpy((getTestLineHolder()), "print(\">\"+line)");
    handleInput((getTestLineHolder()));
    setTestLineHolder("line=read(x)");
    handleInput((getTestLineHolder()));
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("close(x)");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "remove(\"latria-test-file-for-functional-testing.txt\")");
    handleInput((getTestLineHolder()));
    
    
    rez = Read_Batched_Write();
    
    ctest_assert_char_equalto(">line1\n>null\n>line3\n>line4\n>final line, goodbye\n", rez, "Did not read expected file contents");
    
    /* Flush the queue */
    Flush_Batched_Write_NO_PRINT();
}
