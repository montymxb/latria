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
 *  latria_ctest.h
 *  Created by Benjamin Friedman on 7/30/17.
 */

#ifndef latria_tests_latria_ctest_h
#define latria_tests_latria_ctest_h

#include <stdio.h>
#include <string.h>
#include "core/latria_string.h"

#if defined(MACOSX) || defined(LINUXOS)
/* Terminal Colors! */
/*#define COLOR_NRM  "\x1B[0m"*/
#define COLOR_RED  "\x1B[31m"
#define COLOR_GRN  "\x1B[32m"
#define COLOR_YEL  "\x1B[33m"
/*#define COLOR_BLU  "\x1B[34m"*/
#define COLOR_MAG  "\x1B[35m"
/*#define COLOR_CYN  "\x1B[36m"*/
/*#define COLOR_WHT  "\x1B[37m"*/
#define COLOR_RESET "\033[0m"
#else
/* No colors */
#define COLOR_RED ""
#define COLOR_GRN ""
#define COLOR_RESET ""
#define COLOR_YEL  ""
#define COLOR_MAG  ""

#endif

/**
 * Sets up for running tests
 */
void ctest_start_tests();

/**
 * Starts a new test, concluding any prior test
 */
void ctest_begin(char *name);

/**
 * Asserts that a pointer is null
 */
void ctest_assert_null(void *check, char *message);

/**
 * Asserts that a pointer is non-null
 */
void ctest_assert_notnull(void *check, char *message);

/**
 * Asserts that the value passed is 0|false
 */
void ctest_assert_false(int check, char *message);

/**
 * Assets that the value passed is 1|true
 */
void ctest_assert_true(int check, char *message);

/**
 * Asserts that an int equals another int
 */
void ctest_assert_int_equalto(int expected, int check, char *message);

/**
 * Asserts that an int does not equal another int
 */
void ctest_assert_int_notequalto(int expected, int check, char *message);

/**
 * Asserts that an int is greater than or equal to another int
 */
void ctest_assert_int_greaterthanequalto(int expected, int check, char *message);

/**
 * Asserts that an int is less than or equal to another int
 */
void ctest_assert_int_lessthanequalto(int expected, int check, char *message);

/**
 * Asserts that a float equals another float
 */
void ctest_assert_float_equalto(float expected, float check, char *message);

/**
 * Asserts that a float is less than another float
 */
void ctest_assert_float_lessthan(float expected, float check, char *message);

/**
 * Asserts that chars equal another chars
 */
void ctest_assert_char_equalto(char *expected, char *check, char *message);

/**
 * Stops testing and presents the results
 */
void ctest_conclude();

/**
 * Stops running tests and prints out final results
 */
void ctest_stop_tests();

#endif /* latria_tests_latria_ctest_h */
