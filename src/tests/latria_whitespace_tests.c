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
 *  latria_whitespace_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_whitespace_tests.h"

void test_stripWhitespace();

/**
 * Run these tests
 */
void latria_whitespace_tests() {
    test_stripWhitespace();
}


/**
 * Unit tests for stripping whitespace, while avoiding altering quoted material 
 */
void test_stripWhitespace() {
    
    char *arg;
    
    ctest_begin("strip whitespace unit test");
    
    setTestLineHolder("compress this");
    arg = stripWhitespace((getTestLineHolder()));
    ctest_assert_char_equalto("compressthis", arg, "Failed with simple input");
    
    setTestLineHolder("compress this and this");
    arg = stripWhitespace((getTestLineHolder()));
    ctest_assert_char_equalto("compressthisandthis", arg, "Failed with moderate input");
    
    setTestLineHolder("'do not compress this'");
    arg = stripWhitespace((getTestLineHolder()));
    ctest_assert_char_equalto("'do not compress this'", arg, "Failed with single quoted input");
    
    strcpy((getTestLineHolder()), "\"or this\"");
    arg = stripWhitespace((getTestLineHolder()));
    ctest_assert_char_equalto("\"or this\"", arg, "Failed with double quoted input");
    
    strcpy((getTestLineHolder()), " \"trim around\" ");
    arg = stripWhitespace((getTestLineHolder()));
    ctest_assert_char_equalto("\"trim around\"", arg, "Failed with trimming around simple quoted input");
    
    strcpy((getTestLineHolder()), " \"trim around\" + \" all this \" ");
    arg = stripWhitespace((getTestLineHolder()));
    ctest_assert_char_equalto("\"trim around\"+\" all this \"", arg, "Failed with trimming around multiple quoted inputs");
    
    strcpy((getTestLineHolder()), " \" trim around \" + ' mixed stuff ' ");
    arg = stripWhitespace((getTestLineHolder()));
    ctest_assert_char_equalto("\" trim around \"+' mixed stuff '", arg, "Failed with trimming around mixed type multiple quoted inputs");
    
}
