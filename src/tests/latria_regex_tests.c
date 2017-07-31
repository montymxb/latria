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
 *  latria_regex_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_regex_tests.h"

void test_regex();

/**
 * Run these tests
 */
void latria_regex_tests() {
    test_regex();
}

/* Unit tests for regex */
void test_regex() {
    ctest_begin("regex unit test");
    
    ctest_assert_int_notequalto(0, regex("input", "input"), "Failed with basic regex full");
    ctest_assert_int_notequalto(0, regex("input", "in"), "Failed with basic regex at start");
    ctest_assert_int_notequalto(0, regex("input", "pu"), "Failed with basic regex in middle");
    ctest_assert_int_notequalto(0, regex("input", "put"), "Failed with basic regex at end");
    ctest_assert_int_notequalto(1, regex("input", "ins"), "Failed where mismatch should occur");
    ctest_assert_int_notequalto(1, regex("input", "input2"), "Matched where regex should not be completed yet");
    ctest_assert_int_notequalto(1, regex("input", "ps"), "Matched incorrect section in the middle");
    ctest_assert_int_notequalto(0, regex("input", ".*"), "Failed with .* to match all");
    ctest_assert_int_notequalto(0, regex("input", "n.u"), "Failed with . to match single wildcard char");
    ctest_assert_int_notequalto(0, regex("input", ".n.u."), "Failed with . to match multiple wildcard char");
    ctest_assert_int_notequalto(0, regex("input", "i.*t"), "Failed with wildcard surrounded by constant chars");
    ctest_assert_int_notequalto(0, regex("input", "^in"), "Failed matching at very beginning");
    ctest_assert_int_notequalto(1, regex("input", "$.n.u."), "Matched impossible match");
    ctest_assert_int_notequalto(0, regex("input", "^.n.u."), "Failed to match with multiple wildcards at start");
    ctest_assert_int_notequalto(1, regex("input", "^np"), "Matched at an incorrect spot at very beginning");
    ctest_assert_int_notequalto(0, regex("input", "put$"), "Failed matching with line anchor $");
    ctest_assert_int_notequalto(0, regex("input", "i*nput"), "Failed with indefinite i's");
    ctest_assert_int_notequalto(0, regex("input", "i.+t"), "Failed with + surrounded by constant chars");
    ctest_assert_int_notequalto(0, regex("input", "inps?ut"), "Failed matching with optional char");
    ctest_assert_int_notequalto(0, regex("input", "^input$"), "Failed matching with leading and trailing anchors");
    ctest_assert_int_notequalto(0, regex("input", "i[nnnie]pu"), "Failed matching character class at beginning of class");
    ctest_assert_int_notequalto(0, regex("input", "i[ine]pu"), "Failed matching character class at middle of class");
    ctest_assert_int_notequalto(0, regex("input", "i[ien]pu"), "Failed matching character class at end of class");
    ctest_assert_int_notequalto(0, regex("input", "i[ien][poi]u"), "Failed matching double back to back character classes");
    ctest_assert_int_notequalto(0, regex("input", "[ien][^p][pou]"), "Failed matching 3 back to back classes, one negative two positive");
    ctest_assert_int_notequalto(0, regex("input", "[ien][ato]?.[pou]"), "Failed matching optional class followed by a single wildcard");
    ctest_assert_int_notequalto(0, regex("input", "[ien][ato]?.[pou]"), "Failed matching optional class followed by a single wildcard");
    ctest_assert_int_notequalto(0, regex("input", "[abidnopsut]*"), "Failed matching entirely by * class");
    ctest_assert_int_notequalto(0, regex("input", "[abidnopsut]+"), "Failed matching entirely by + class");
    
    ctest_assert_int_notequalto(0, regex("input", "(.*)$"), "Failed matching all wildcard in parentheses");
    ctest_assert_false((getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "input") != 0), "Incorrect value from matching all wildcard in parentheses");
    
    ctest_assert_int_notequalto(0, regex("input", "(.*)t"), "Failed matching all wildcard in parentheses followed by constant");
    ctest_assert_false((getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "inpu") != 0), "Incorrect value from matching all wildcard in parentheses followed by constant");
    
    ctest_assert_int_notequalto(0, regex("input", "(.*t)"), "Failed matching all wildcard and constant in parentheses");
    ctest_assert_false((getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "input") != 0), "Incorrect value from matching all wildcard and constant in parentheses");
    
    ctest_assert_int_notequalto(0, regex("input", "([^t]*t)"), "Failed matching anti-class wildcard");
    ctest_assert_false((getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "input") != 0), "Incorrect value from matching anti-class wildcard in parentheses");
    
    ctest_assert_int_notequalto(0, regex("input", "([^t]+)t"), "Failed matching anti-class wildcard + in parentheses");
    ctest_assert_false((getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "inpu") != 0), "Incorrect value from matching anti-class wildcard + in parentheses");
    
    ctest_assert_int_notequalto(0, regex("input", "(in)put$"), "Failed matching partially in parentheses");
    ctest_assert_false((getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "in") != 0), "Incorrect value from partial match in parentheses");
    
    ctest_assert_int_notequalto(0, regex("input", "(input)$"), "Failed matching all with anchor in parentheses");
    ctest_assert_false((getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "input") != 0), "Incorrect value from all with anchor in parentheses");
    
    ctest_assert_int_notequalto(0, regex("input", "(?:input$)"), "Failed matching all with anchor in non-capturing parentheses");
    
    ctest_assert_int_notequalto(0, regex("input", "(p.*)$"), "Failed matching part with anchor in capturing parentheses");
    ctest_assert_false((getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "put") != 0), "Incorrect value captured from matching part with anchor in capturing parentheses");
    
    ctest_assert_int_notequalto(0, regex("input", "app|input"), "Failed matching alternation");
    ctest_assert_int_notequalto(0, regex("abc[xyz]", ".+\\[.+\\]"), "Failed to match with brackets");
    ctest_assert_int_notequalto(1, regex("abcasl]skdk", ".+\\[.+\\]"), "Matched incorrectly to one bracket");
    
    
    ctest_assert_int_notequalto(0, regex("input", "(appsareaverylongthing|input)"), "Failed matching alternation with parens");
    ctest_assert_false((getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "input") != 0), "Incorrect value from matching alternation with parens");
    
    ctest_assert_int_notequalto(0, regex("input", "i.*|inpug"), "Failed matching alternation with wildcard");
    ctest_assert_int_notequalto(1, regex("input", "i.*g|inpug"), "Incorrectly matched alternation with wildcard following by regular char");
    
}
