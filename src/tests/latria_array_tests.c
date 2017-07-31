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
 *  latria_array_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_array_tests.h"

void test_OneArrayElement();
void test_TwoArrayElements();
void test_ThreeArrayElements();
void test_ChangeArrayElement();
void test_ChangeAllArrayElements();

/**
 * Run these tests
 */
void latria_array_tests() {
    test_OneArrayElement();
    test_TwoArrayElements();
    test_ThreeArrayElements();
    test_ChangeArrayElement();
    test_ChangeAllArrayElements();
}

/**
 * Tests single item array initialilization 
 */
void test_OneArrayElement() {
    struct CoreObject *co;
    ctest_begin("array create single element");
    strcpy((getTestLineHolder()), "sub={\"item1\":\"value1\"}");
    handleInput((getTestLineHolder()));
    co = getArrayValue("sub", "item1");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct object type");
    ctest_assert_char_equalto("value1", co->data._svalue, "Not equal to tgt string");
}

/**
 * Tests creating an array with 2 elements 
 */
void test_TwoArrayElements() {
    struct CoreObject *co1,*co2;
    LATBool passed = true;
    ctest_begin("array create double elements");
    
    strcpy((getTestLineHolder()), "sub={\"item1\":\"value1\",\"item2\":29}");
    handleInput((getTestLineHolder()));
    
    co1 = getArrayValue("sub", "item1");
    co2 = getArrayValue("sub", "item2");
    
    ctest_assert_notnull(co1, "No CoreObject created");
    ctest_assert_int_equalto(1, co1->_state, "Not of correct object type");
    ctest_assert_char_equalto("value1", co1->data._svalue, NULL);
    
    ctest_assert_notnull(co2, "No CoreObject created");
    ctest_assert_int_equalto(0, co2->_state, "Not of correct object type");
    ctest_assert_int_equalto(29, co2->data._fvalue, NULL);
    
}

/**
 * Tests creating an array with 3 elements 
 */
void test_ThreeArrayElements() {
    struct CoreObject *co1,*co2,*co3;
    LATBool passed = true;
    ctest_begin("array create three elements");
    
    strcpy((getTestLineHolder()), "sub={\"item1\":154,\"item2\":\"bacon-lady\",\"item3\":\"xmasdogs!-''\"}");
    handleInput((getTestLineHolder()));
    
    co1 = getArrayValue("sub", "item1");
    co2 = getArrayValue("sub", "item2");
    co3 = getArrayValue("sub", "item3");
    
    ctest_assert_notnull(co1, "No CoreObject created");
    ctest_assert_int_equalto(0, co1->_state, "Not of correct object type");
    ctest_assert_int_equalto(154, co1->data._fvalue, NULL);
    
    ctest_assert_notnull(co2, "No CoreObject created");
    ctest_assert_int_equalto(1, co2->_state, "Not of correct object type");
    ctest_assert_char_equalto("bacon-lady", co2->data._svalue, NULL);
    
    ctest_assert_notnull(co3, "No CoreObject created");
    ctest_assert_int_equalto(1, co3->_state, "Not of correct object type");
    ctest_assert_char_equalto("xmasdogs!-''", co3->data._svalue, NULL);
}

/**
 * Tests changing one element of an array 
 */
void test_ChangeArrayElement() {
    struct CoreObject *co1;
    ctest_begin("array modify 1 element");
    
    strcpy((getTestLineHolder()), "sub[\"item1\"]=95");
    handleInput((getTestLineHolder()));
    
    co1 = getArrayValue("sub", "item1");
    
    ctest_assert_notnull(co1, "No CoreObject created");
    ctest_assert_int_equalto(0, co1->_state, "Not of correct object type");
    ctest_assert_int_equalto(95, co1->data._fvalue, NULL);
}

/**
 * Tests altering all pre-existing elements of an array 
 */
void test_ChangeAllArrayElements() {
    struct CoreObject *co1,*co2,*co3;
    LATBool passed = true;
    ctest_begin("array modify all elements");
    
    strcpy((getTestLineHolder()), "sub[\"item1\"]=208");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "sub[\"item2\"]=218");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "sub[\"item3\"]=398");
    handleInput((getTestLineHolder()));
    
    co1 = getArrayValue("sub", "item1");
    co2 = getArrayValue("sub", "item2");
    co3 = getArrayValue("sub", "item3");
    
    ctest_assert_notnull(co1, "No CoreObject created");
    ctest_assert_int_equalto(0, co1->_state, "Not of correct object type");
    ctest_assert_int_equalto(208, co1->data._fvalue, NULL);
    
    ctest_assert_notnull(co2, "No CoreObject created");
    ctest_assert_int_equalto(0, co2->_state, "Not of correct object type");
    ctest_assert_int_equalto(218, co2->data._fvalue, NULL);
    
    ctest_assert_notnull(co3, "No CoreObject created");
    ctest_assert_int_equalto(0, co3->_state, "Not of correct object type");
    ctest_assert_int_equalto(398, co3->data._fvalue, NULL);
}
