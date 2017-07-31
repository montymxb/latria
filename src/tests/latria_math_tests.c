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
 *  latria_math_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_math_tests.h"

void test_Arithmetic();

/**
 * Run these tests
 */
void latria_math_tests() {
    test_Arithmetic();
}

/* Tests for validity of various arithmetic operations */
void test_Arithmetic() {
    struct CoreObject *co;
    ctest_begin("arithmetic tests");
    
    /* #1 */
    setTestLineHolder("a=5*(1+2)*(1+2)");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(45, co->data._fvalue, "Incorrect value where 45 was expected");
    
    /* #2 */
    setTestLineHolder("a=5*(1+2)");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(15, co->data._fvalue, "Incorrect value where 15 was expected");
    
    /* #3 */
    setTestLineHolder("a=5*(1+2)*3");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(45, co->data._fvalue, "Incorrect value where 45 was expected");
    
    /* #4 */
    setTestLineHolder("a=(1+2) * (1-2)");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(-3, co->data._fvalue, "Incorrect value where -3 was expected");
    
    /* #5 */
    setTestLineHolder("a=(1-2)*(1+2)");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(-3, co->data._fvalue, "Incorrect value where -3 was expected");
    
    /* #6 */
    setTestLineHolder("a=+1 * (1+2)");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(3, co->data._fvalue, "Incorrect value where 3 was expected");
    
    /* #7 */
    setTestLineHolder("a=-1*(1+2)");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(-3, co->data._fvalue, "Incorrect value where -3 was expected");
    
    /* #8 */
    setTestLineHolder("a=-1--1--1");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(1, co->data._fvalue, "Incorrect value where 1 was expected");
    
    /* #9 */
    setTestLineHolder("a=+1--1++1");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(3, co->data._fvalue, "Incorrect value where 3 was expected");
    
    /* #10 */
    setTestLineHolder("a=+1++1++1");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(3, co->data._fvalue, "Incorrect value where 3 was expected");
    
    /* #11 */
    setTestLineHolder("a=-1--1-+1");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(-1, co->data._fvalue, "Incorrect value where -1 was expected");
    
    /* #12 */
    setTestLineHolder("a=((1+2)*(1+2)) * 3");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(27, co->data._fvalue, "Incorrect value where 27 was expected");
    
    /* #13 */
    setTestLineHolder("a=(1+2) * (1+2) * (1+2) * (1+2)");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(81, co->data._fvalue, "Incorrect value where 81 was expected");
    
    /* #14 */
    setTestLineHolder("a=(1+2) * (1+2) * (1+2)");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(27, co->data._fvalue, "Incorrect value where 27 was expected");
    
    /* #15 */
    setTestLineHolder("a=(1+2) * (1+2) ");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(9, co->data._fvalue, "Incorrect value where 9 was expected");
    
    /* #16 */
    setTestLineHolder("a=2 * -1");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(-2, co->data._fvalue, "Incorrect value where -2 was expected");
    
    /* #17 */
    setTestLineHolder("a=(1 * 3) * -5");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(-15, co->data._fvalue, "Incorrect value where -15 was expected");
    
    /* #18 */
    setTestLineHolder("a=(-1 * -3) * 5");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(15, co->data._fvalue, "Incorrect value where 15 was expected");
    
    /* #19 */
    setTestLineHolder("a=(-1 * -3) * -5");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(-15, co->data._fvalue, "Incorrect value where -15 was expected");
    
    /* #20 */
    setTestLineHolder("a=2 * -1 * -2");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(4, co->data._fvalue, "Incorrect value where 4 was expected");
    
    /* #21 */
    setTestLineHolder("a=2 * 2 * 2");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(8, co->data._fvalue, "Incorrect value where 8 was expected");
    
    /* #22 */
    setTestLineHolder("a=4 * 5 + 1 ");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(21, co->data._fvalue, "Incorrect value where 21 was expected");
    
    /* #23 */
    setTestLineHolder("a=2 * 2 * 2 * 2");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(16, co->data._fvalue, "Incorrect value where 16 was expected");
    
    /* #24 */
    setTestLineHolder("a=2 + 2 + 2 + 2");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(8, co->data._fvalue, "Incorrect value where 8 was expected");
    
    /* #25 */
    setTestLineHolder("a=1 + 1  + 1");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(3, co->data._fvalue, "Incorrect value where 3 was expected");
    
    /* #26 */
    setTestLineHolder("a=(1+2)*5");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(15, co->data._fvalue, "Incorrect value where 15 was expected");
    
    /* #27 */
    setTestLineHolder("a=1+(2+3*4)+5");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(20, co->data._fvalue, "Incorrect value where 20 was expected");
    
    /* #28 */
    setTestLineHolder("a=(1+2)");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(3, co->data._fvalue, "Incorrect value");
    
    /* #29 */
    setTestLineHolder("a=1+(2+3)");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(6, co->data._fvalue, "Incorrect value, expected 6");
    
    /* #30 */
    setTestLineHolder("a=(1+2)*(3+4)*(5+6)");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(231, co->data._fvalue, "Incorrect value");
    
    /* #31 */
    setTestLineHolder("a=1+2*3^(1+2+3*1)+8");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(1467, co->data._fvalue, "Incorrect value");
    
    /* #32 */
    setTestLineHolder("trisquit = 1");
    handleInput((getTestLineHolder()));
    setTestLineHolder("bacon = 1");
    handleInput((getTestLineHolder()));
    setTestLineHolder("a= 5 + (2 * (bacon + 4 ^ (trisquit + 6)))");
    handleInput((getTestLineHolder()));
    co = getValue("a");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(32775, co->data._fvalue, "Incorrect value");
    
    /* #33 */
    setTestLineHolder("x = ((5 ^ 2) + (10 / 2)) / 5");
    handleInput((getTestLineHolder()));
    co = getValue("x");
    
    ctest_assert_notnull(co, "No variable created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(6, co->data._fvalue, "Incorrect value, expected 6");
}

