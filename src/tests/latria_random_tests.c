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
 *  latria_random_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_random_tests.h"

void test_random();

/**
 * Run these tests
 */
void latria_random_tests() {
    test_random();
}

/**
 * Unit tests for random 
 */
void test_random() {
    int origValue;
    int rez;
    
    ctest_begin("random unit test");
    
    /* Test for 0 only */
    rez = Sys_Random(1);
    ctest_assert_int_equalto(0, rez, "Failed with random being greater than 1.");
    
    /* Test between 100 */
    rez = Sys_Random(100);
    ctest_assert_int_lessthanequalto(100, rez, "Failed with random being greater than 100.");
    
    /* Reseed */
    Sys_RandomSeed(12345);
    
    /* Test Value */
    rez = Sys_Random(100);
    /* store original value */
    origValue = rez;
    ctest_assert_int_lessthanequalto(100, rez, "Failed with random being greater than 100.");
    
    /* Reseed, again */
    Sys_RandomSeed(12345);
    
    /* Test Value to MATCH origValue */
    rez = Sys_Random(100);
    ctest_assert_int_equalto(origValue, rez, "Failed with seeding not generating same results.");
    
}
