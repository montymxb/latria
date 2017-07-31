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
 *  latria_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_tests.h"

/* END FUNCTIONAL TESTS */

/* Runs ALL Tests */
void runAllTests() {
    
    /* Start tests */
    ctest_start_tests();
    
    openDynamicInterpreterFile();
    
    /* Alter GC to zero, tends to bring out memory related bugs */
    runLatriaCode("__setGCRate(0)");
    
    /* Test Groups */
    latria_whitespace_tests();
    latria_random_tests();
    latria_regex_tests();
    latria_integer_tests();
    latria_float_tests();
    latria_string_tests();
    latria_assignment_tests();
    latria_print_tests();
    latria_math_tests();
    latria_array_tests();
    latria_fileio_tests();
    latria_relational_tests();
    latria_controlflow_tests();
    latria_xlang_tests();
    latria_function_tests();
    
    closeDynamicInterpreterFile();
    
    /* free compiler related items */
    freeByteCodes();
    freeLexicalAllocations();
    deallocStackStates();
    
    #if !defined(MACOSX) && !defined(LINUXOS)
    freeLatria();
    #endif
    
    /* Stop tests */
    ctest_stop_tests();
}
