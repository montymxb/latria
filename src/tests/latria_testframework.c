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
 *  latria_testframework.c
 *  Created by Benjamin Friedman on 2/23/17.
 *
 *  Framework for writing and running tests in Latria on Latria
 *
 */

#include "latria_testframework.h"

/* small 512 character array for copying and running code via */
char LAT_TEST_LINE_HOLDER[512];

/**
 * Sets the test line holder
 */
void setTestLineHolder(char *line) {
    strcpy(LAT_TEST_LINE_HOLDER, line);
}

/**
 * Runs the provided line as latria code
 */
void runLatriaCode(char *line) {
    setTestLineHolder(line);
    handleInput(LAT_TEST_LINE_HOLDER);
}

/**
 * Gets the current test line holder
 */
char* getTestLineHolder() {
    return LAT_TEST_LINE_HOLDER;
}
