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
 *  latria_ctest.c
 *  Created by Benjamin Friedman on 7/30/17.
 *
 *  C Testing Framework embedded in latria
 *
 */

#include "latria_ctest.h"

/* # of tests we've failed */
int LAT_TEST_FAIL_COUNT;

/* # of tests we've passed */
int LAT_TEST_PASS_COUNT;

/* small 256 character array for holding the current test name */
char LAT_TEST_NAME[256];

/**
 * Total # of assertions made over all tests
 */
int LAT_TEST_TOTAL_ASSERTIONS;

/**
 * # of assertions passed in a test
 */
int LAT_TEST_ASSERTIONS_PASSED;

/**
 * # of assertions failed in a test
 */
int LAT_TEST_ASSERTIONS_FAILED;

/**
 * Starting time of a section of tests
 */
long LAT_TEST_START_TIME;

/**
 * Indicates an assertion passed
 */
void ctest_assertion_passed();

/**
 * Indicates an assertion failed
 */
void ctest_assertion_failed();

/**
 * Sets up for running tests
 */
void ctest_start_tests() {
    
    /* Set start time */
    LAT_TEST_START_TIME = time(0);
    
    /* Clear vars */
    LAT_TEST_FAIL_COUNT = 0;
    LAT_TEST_PASS_COUNT = 0;
    LAT_TEST_ASSERTIONS_PASSED = 0;
    LAT_TEST_ASSERTIONS_FAILED = 0;
    LAT_TEST_TOTAL_ASSERTIONS  = 0;
    
    printf("\n===Starting CTests===\n");
}

/**
 * Starts a new test, concluding any prior test
 */
void ctest_begin(char *name) {
    /* conclude any prior test */
    ctest_conclude();
    
    strcpy(LAT_TEST_NAME, name);
    
}

/**
 * Asserts that a pointer is null
 */
void ctest_assert_null(void *check, char *message) {
    if(LAT_TEST_ASSERTIONS_FAILED > 0) {
        /* do not check further */
        return;
    }
    if(check == NULL) {
        ctest_assertion_passed();
    } else {
        if(message != NULL) {
            printf("    %s-%s Failed asserting value was NULL: %s", COLOR_RED, COLOR_RESET, message);
        } else {
            printf("    %s-%s Failed asserting value was NULL", COLOR_RED, COLOR_RESET);
        }
        ctest_assertion_failed();
    }
}

/**
 * Asserts that a pointer is non-null
 */
void ctest_assert_notnull(void *check, char *message) {
    if(LAT_TEST_ASSERTIONS_FAILED > 0) {
        /* do not check further */
        return;
    }
    if(check != NULL) {
        ctest_assertion_passed();
    } else {
        if(message != NULL) {
            printf("    %s-%s Failed asserting value was not NULL: %s", COLOR_RED, COLOR_RESET, message);
        } else {
            printf("    %s-%s Failed asserting value was not NULL", COLOR_RED, COLOR_RESET);
        }
        ctest_assertion_failed();
    }
}

/**
 * Asserts that the value passed is 0|false
 */
void ctest_assert_false(int check, char *message) {
    if(LAT_TEST_ASSERTIONS_FAILED > 0) {
        /* do not check further */
        return;
    }
    if(!check || check == 0) {
        ctest_assertion_passed();
    } else {
        if(message != NULL) {
            printf("    %s-%s Failed asserting %d was false: %s", COLOR_RED, COLOR_RESET, check, message);
        } else {
            printf("    %s-%s Failed asserting %d was false", COLOR_RED, COLOR_RESET, check);
        }
        ctest_assertion_failed();
    }
}

/**
 * Assets that the value passed is 1|true
 */
void ctest_assert_true(int check, char *message) {
    if(LAT_TEST_ASSERTIONS_FAILED > 0) {
        /* do not check further */
        return;
    }
    if(!check || check == 1) {
        ctest_assertion_passed();
    } else {
        if(message != NULL) {
            printf("    %s-%s Failed asserting %d was true: %s", COLOR_RED, COLOR_RESET, check, message);
        } else {
            printf("    %s-%s Failed asserting %d was true", COLOR_RED, COLOR_RESET, check);
        }
        ctest_assertion_failed();
    }
}

/**
 * Asserts that an int equals another int
 */
void ctest_assert_int_equalto(int expected, int check, char *message) {
    if(LAT_TEST_ASSERTIONS_FAILED > 0) {
        /* do not check further */
        return;
    }
    if(expected == check) {
        ctest_assertion_passed();
    } else {
        if(message != NULL) {
            printf("    %s-%s Int failed asserting %d was equal to %d: %s", COLOR_RED, COLOR_RESET, check, expected, message);
        } else {
            printf("    %s-%s Int failed asserting %d was equal to %d", COLOR_RED, COLOR_RESET, check, expected);
        }
        ctest_assertion_failed();
    }
}

/**
 * Asserts that an int does not equal another int
 */
void ctest_assert_int_notequalto(int expected, int check, char *message) {
    if(LAT_TEST_ASSERTIONS_FAILED > 0) {
        /* do not check further */
        return;
    }
    if(expected != check) {
        ctest_assertion_passed();
    } else {
        if(message != NULL) {
            printf("    %s-%s Int failed asserting %d was not equal to %d: %s", COLOR_RED, COLOR_RESET, check, expected, message);
        } else {
            printf("    %s-%s Int failed asserting %d was not equal to %d", COLOR_RED, COLOR_RESET, check, expected);
        }
        ctest_assertion_failed();
    }
}

/**
 * Asserts that an int is greater than or equal to another int
 */
void ctest_assert_int_greaterthanequalto(int expected, int check, char *message) {
    if(LAT_TEST_ASSERTIONS_FAILED > 0) {
        /* do not check further */
        return;
    }
    if(expected <= check) {
        ctest_assertion_passed();
    } else {
        if(message != NULL) {
            printf("    %s-%s Int failed asserting %d was greater than or equal to %d: %s", COLOR_RED, COLOR_RESET, check, expected, message);
        } else {
            printf("    %s-%s Int failed asserting %d was greater than or equal to %d", COLOR_RED, COLOR_RESET, check, expected);
        }
        ctest_assertion_failed();
    }
}

/**
 * Asserts that an int is less than or equal to another int
 */
void ctest_assert_int_lessthanequalto(int expected, int check, char *message) {
    if(LAT_TEST_ASSERTIONS_FAILED > 0) {
        /* do not check further */
        return;
    }
    if(expected >= check) {
        ctest_assertion_passed();
    } else {
        if(message != NULL) {
            printf("    %s-%s Int failed asserting %d was less than or equal to %d: %s", COLOR_RED, COLOR_RESET, check, expected, message);
        } else {
            printf("    %s-%s Int failed asserting %d was less than or equal to %d", COLOR_RED, COLOR_RESET, check, expected);
        }
        ctest_assertion_failed();
    }
}

/**
 * Asserts that a float equals another float
 */
void ctest_assert_float_equalto(float expected, float check, char *message) {
    if(LAT_TEST_ASSERTIONS_FAILED > 0) {
        /* do not check further */
        return;
    }
    if(expected == check) {
        ctest_assertion_passed();
    } else {
        if(message != NULL) {
            printf("    %s-%s Float failed asserting %f was equal to %f: %s", COLOR_RED, COLOR_RESET, check, expected, message);
        } else {
            printf("    %s-%s Chars Float failed asserting %f was equal to %f", COLOR_RED, COLOR_RESET, check, expected);
        }
        ctest_assertion_failed();
    }
}

/**
 * Asserts that a float is less than another float
 */
void ctest_assert_float_lessthan(float expected, float check, char *message) {
    if(LAT_TEST_ASSERTIONS_FAILED > 0) {
        /* do not check further */
        return;
    }
    if(expected > check) {
        ctest_assertion_passed();
    } else {
        if(message != NULL) {
            printf("    %s-%s Float failed asserting %f was less than %f: %s", COLOR_RED, COLOR_RESET, check, expected, message);
        } else {
            printf("    %s-%s Chars Float failed asserting %f was less than %f", COLOR_RED, COLOR_RESET, check, expected);
        }
        ctest_assertion_failed();
    }
}

/**
 * Asserts that chars equal another chars
 */
void ctest_assert_char_equalto(char *expected, char *check, char *message) {
    if(LAT_TEST_ASSERTIONS_FAILED > 0) {
        /* do not check further */
        return;
    }
    if(LAT_STRCMP(expected, check) == 0) {
        ctest_assertion_passed();
    } else {
        if(message != NULL) {
            printf("    %s-%s Chars failed asserting %s was equal to %s: %s", COLOR_RED, COLOR_RESET, check, expected, message);
        } else {
            printf("    %s-%s Chars failed asserting %s was equal to %s", COLOR_RED, COLOR_RESET, check, expected);
        }
        ctest_assertion_failed();
    }
}

/**
 * Indicates an assertion passed
 */
void ctest_assertion_passed() {
    LAT_TEST_ASSERTIONS_PASSED+=1;
    LAT_TEST_TOTAL_ASSERTIONS+=1;
}

/**
 * Indicates an assertion failed
 */
void ctest_assertion_failed() {
    LAT_TEST_ASSERTIONS_FAILED+=1;
    LAT_TEST_TOTAL_ASSERTIONS+=1;
}

/**
 * Stops testing and presents the results
 */
void ctest_conclude() {
    if(LAT_TEST_ASSERTIONS_PASSED == 0 && LAT_TEST_ASSERTIONS_FAILED == 0) {
        /* no tests to conclude... */
        return;
    }
    
    if(LAT_TEST_ASSERTIONS_FAILED == 0) {
        /* passed */
        #ifdef _WIN32
        printf(COLOR_GRN "-%s %s passed (%d assertions)\n", COLOR_RESET, LAT_TEST_NAME, LAT_TEST_ASSERTIONS_PASSED);
        #else
        printf(COLOR_GRN "✔%s %s passed (%d assertions)\n", COLOR_RESET, LAT_TEST_NAME, LAT_TEST_ASSERTIONS_PASSED);
        #endif
        LAT_TEST_PASS_COUNT+=1;
    } else {
        /* failed */
        #ifdef _WIN32
        printf(COLOR_RED "\nX%s %s Failed\n", COLOR_RESET, LAT_TEST_NAME);
        #else
        printf(COLOR_RED "\n✘%s %s Failed\n", COLOR_RESET, LAT_TEST_NAME);
        #endif
        LAT_TEST_FAIL_COUNT+=1;
    }
    
    /* reset assertion counts */
    LAT_TEST_ASSERTIONS_PASSED = 0;
    LAT_TEST_ASSERTIONS_FAILED = 0;
}

/**
 * Stops running tests and prints out final results
 */
void ctest_stop_tests() {
    /* conclude any prior test */
    ctest_conclude();
    
    printf("\nDone, ran in %d seconds.", time(0) - LAT_TEST_START_TIME);
    
    /* Check failure Count */
    if(LAT_TEST_FAIL_COUNT == 0) {
        /* Passed */
        printf(
               "\n%sAll Tests Passed%s\nRan %d Tests, %d Assertions\n\n%s+%s %d Passed\n\n",
               COLOR_GRN,
               COLOR_RESET,
               LAT_TEST_FAIL_COUNT+LAT_TEST_PASS_COUNT,
               LAT_TEST_TOTAL_ASSERTIONS,
               COLOR_GRN,
               COLOR_RESET,
               LAT_TEST_PASS_COUNT);
        exit(0);
    } else {
        /* Failed */
        printf(
               "\n%sTests Failed%s\nRan %d Tests, %d Assertions\n\n%s+%s %d Passed\n%s-%s %d Failed\n\n",
               COLOR_RED,
               COLOR_RESET,
               LAT_TEST_FAIL_COUNT+LAT_TEST_PASS_COUNT,
               LAT_TEST_TOTAL_ASSERTIONS,
               COLOR_GRN,
               COLOR_RESET,
               LAT_TEST_PASS_COUNT,
               COLOR_RED,
               COLOR_RESET,
               LAT_TEST_FAIL_COUNT);
        exit(1);
    }
}
