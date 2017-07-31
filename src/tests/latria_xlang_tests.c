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
 *  latria_xlang_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_xlang_tests.h"

void test_ExecSimpleEcho();
void test_ExecInterpolation();
void test_ExecMultiLine();

/**
 * Run these tests
 */
void latria_xlang_tests() {
    test_ExecSimpleEcho();
    test_ExecInterpolation();
    test_ExecMultiLine();
}

/**
 * Tests simple OS Execution with echo 
 */
void test_ExecSimpleEcho() {
    struct CoreObject *co;
    ctest_begin("exec simple");
    
    setTestLineHolder("    v ## ");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("    echo echoed this");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("    #>");
    handleInput((getTestLineHolder()));
    
    co = getValue("v");
    
    ctest_assert_notnull(co, "No CoreObject created or found");
    ctest_assert_int_equalto(1, co->_state, "Wrong data type");
    ctest_assert_char_equalto("echoed this\n", co->data._svalue, NULL);
}

/**
 * Tests OS Execution with interpolation of values 
 */
void test_ExecInterpolation() {
    struct CoreObject *co;
    ctest_begin("exec interpolation");
    
    setTestLineHolder("g##");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("echo we last echoed:IN{v}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("#>");
    handleInput((getTestLineHolder()));
    
    co = getValue("g");
    
    ctest_assert_notnull(co, "No CoreObject created or found");
    ctest_assert_int_equalto(1, co->_state, "Wrong data type");
    ctest_assert_char_equalto("we last echoed:echoed this\n", co->data._svalue, NULL);
}

/**
 * Tests OS Execution across multiple lines with interpolation of values 
 */
void test_ExecMultiLine() {
    struct CoreObject *co;
    ctest_begin("multi-line exec with interpolation");
    
    setTestLineHolder("g##");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("echo tesla nikola1 some very long");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("echo tesla nikola2 also very long");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("echo tesla nikola3 additionally long");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("echo tesla nikola4 quite long");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("echo IN{v}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("echo eggs");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("#>");
    handleInput((getTestLineHolder()));
    
    co = getValue("g");
    ctest_assert_notnull(co, "No CoreObject created or found");
    ctest_assert_int_equalto(1, co->_state, "Wrong data type");
    ctest_assert_char_equalto("tesla nikola1 some very long\ntesla nikola2 also very long\ntesla nikola3 additionally long\ntesla nikola4 quite long\nechoed this\neggs\n", co->data._svalue, NULL);
}
