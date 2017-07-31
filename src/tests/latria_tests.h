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
 *  latria_tests.h
 *  Created by Benjamin Friedman on 9/29/15.
 */

#ifndef latria_tests_latria_tests_h
#define latria_tests_latria_tests_h

#include "latria_ctest.h"
#include "latria_testframework.h"
#include "prompt/latria.h"
#include "core/latria_core.h"
#include "core/latria_opcodes.h"
#include "core/latria_sys.h"

/* Test Groups */
#include "latria_whitespace_tests.h"
#include "latria_random_tests.h"
#include "latria_regex_tests.h"
#include "latria_integer_tests.h"
#include "latria_float_tests.h"
#include "latria_string_tests.h"
#include "latria_assignment_tests.h"
#include "latria_print_tests.h"
#include "latria_math_tests.h"
#include "latria_array_tests.h"
#include "latria_fileio_tests.h"
#include "latria_relational_tests.h"
#include "latria_controlflow_tests.h"
#include "latria_xlang_tests.h"
#include "latria_function_tests.h"

void runAllTests();

#endif /* latria_tests_latria_tests_h */
