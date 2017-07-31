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
 *  latria_assignment_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_assignment_tests.h"


void test_VarToVarAssign();
void test_BooleanAssign();
void test_RelationalAssign();

/**
 * Run these tests
 */
void latria_assignment_tests() {
    test_VarToVarAssign();
    test_BooleanAssign();
    /* Holding off on this one for now */
    /* test_RelationalAssign(); */
}

/**
 * Tests assigning a var to another var 
 */
void test_VarToVarAssign() {
    struct CoreObject *co;
    ctest_begin("var to var assign");
    setTestLineHolder("z=y2k");
    handleInput((getTestLineHolder()));
    co = getValue("z");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct data type");
    ctest_assert_false((strcmp(co->data._svalue, "__setGCRate(1)")!=0), "Not equal to '__setGCRate(1)' string");
}

/**
 * Tests assigning a bool to a var 
 */
void test_BooleanAssign() {
    struct CoreObject *co;
    ctest_begin("boolean assign");
    setTestLineHolder("h=true");
    handleInput((getTestLineHolder()));
    co = getValue("h");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(2, co->_state, "Not of correct data type");
    ctest_assert_true(co->data._bvalue, "Not equal to 'true' boolean value");
}

/**
 * Tests assigning a relation to a var 
 */
void test_RelationalAssign() {
    struct CoreObject *co;
    ctest_begin("relational assign");
    setTestLineHolder("z=(5<=5)");
    handleInput((getTestLineHolder()));
    co = getValue("z");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(2, co->_state, "Not of correct data type");
    ctest_assert_true(co->data._bvalue, "Not equal to 'true' boolean value");
}
