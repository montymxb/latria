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
 *  latria_print_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_print_tests.h"

/* Print tests */
void test_PrintString();
void test_PrintStringVar();
void test_PrintStringConcat();
void test_PrintMixedConcat();
void test_PrintVarConcat();
void test_PrintBool();
void test_PrintNumVar();
void test_PrintBoolNumStringMix();

/**
 * Run these tests
 */
void latria_print_tests() {
    test_PrintString();
    test_PrintStringVar();
    test_PrintStringConcat();
    test_PrintMixedConcat();
    test_PrintVarConcat();
    test_PrintBool();
    test_PrintNumVar();
    test_PrintBoolNumStringMix();
}

/** 
 * Tests printing of just a string 
 */
void test_PrintString() {
    char *rez;
    ctest_begin("print");
    strcpy((getTestLineHolder()), "print(\"some arbitary string to test\")");
    handleInput((getTestLineHolder()));
    rez = Read_Batched_Write();
    
    ctest_assert_notnull(rez, "Nothing written to the write buffer");
    /* Note the \n! Always appended by default */
    ctest_assert_char_equalto("some arbitary string to test\n", rez, "Not equal to what was expected to be printed out");
    
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/** 
 * Tests Printing of a string variable 
 */
void test_PrintStringVar() {
    char *rez;
    ctest_begin("print string variable");
    /* Declare a string var */
    strcpy((getTestLineHolder()), "s=\"a variable string value\"");
    handleInput((getTestLineHolder()));
    /* Print that string var */
    setTestLineHolder("print(s)");
    handleInput((getTestLineHolder()));
    rez = Read_Batched_Write();
    
    ctest_assert_notnull(rez, "Nothing written to the write buffer");
    /* Note the \n! Always appended by default */
    ctest_assert_char_equalto("a variable string value\n", rez, "Not equal to what was expected to be printed out");
    
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/**
 * Tests Printing of concatenated strings 
 */
void test_PrintStringConcat() {
    char *rez;
    ctest_begin("print strings concatenated");
    strcpy((getTestLineHolder()), "print(\"one set\"+\" and another\")");
    handleInput((getTestLineHolder()));
    rez = Read_Batched_Write();
    
    ctest_assert_notnull(rez, "Nothing written to the write buffer");
    /* Note the \n! Always appended by default */
    ctest_assert_char_equalto("one set and another\n", rez, "Not equal to what was expected to be printed out");
    
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/**
 * Tests Printing of strings & vars 
 */
void test_PrintMixedConcat() {
    char *rez;
    ctest_begin("print mixed concatenated");
    strcpy((getTestLineHolder()), "print(s+\" and another\")");
    handleInput((getTestLineHolder()));
    rez = Read_Batched_Write();
    
    ctest_assert_notnull(rez, "Nothing written to the write buffer");
    /* Note the \n! Always appended by default */
    ctest_assert_char_equalto("a variable string value and another\n", rez, "Not equal to what was expected to be printed out");
    
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/**
 * Tests Printing of concatenated vars 
 */
void test_PrintVarConcat() {
    char *rez;
    ctest_begin("print variables concatenated");
    /* Declare a string var */
    strcpy((getTestLineHolder()), "s2=\" and another variable string value\"");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("print(s+s2)");
    handleInput((getTestLineHolder()));
    rez = Read_Batched_Write();
    
    ctest_assert_notnull(rez, "Nothing written to the write buffer");
    /* Note the \n! Always appended by default */
    ctest_assert_char_equalto("a variable string value and another variable string value\n", rez, "Not equal to what was expected to be printed out");
    
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/**
 * Tests Printing a bool 
 */
void test_PrintBool() {
    char *rez;
    ctest_begin("print boolean variable");
    /* Declare a string var */
    setTestLineHolder("s3=true");
    handleInput((getTestLineHolder()));
    /* Print that string var */
    setTestLineHolder("print(s3)");
    handleInput((getTestLineHolder()));
    rez = Read_Batched_Write();
    
    ctest_assert_notnull(rez, "Nothing written to the write buffer");
    /* Note the \n! Always appended by default */
    ctest_assert_char_equalto("1\n", rez, "Not equal to what was expected to be printed out");
    
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/**
 * Tests printing a number variable 
 */
void test_PrintNumVar() {
    char *rez;
    ctest_begin("print number variable");
    /* Declare a string var */
    setTestLineHolder("s4=14.04");
    handleInput((getTestLineHolder()));
    /* Print that string var */
    setTestLineHolder("print(s4)");
    handleInput((getTestLineHolder()));
    rez = Read_Batched_Write();
    
    ctest_assert_notnull(rez, "Nothing written to the write buffer");
    /* Note the \n! Always appended by default */
    ctest_assert_char_equalto("14.04\n", rez, "Not equal to what was expected to be printed out");
    
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/**
 * Tests printing a boolean, number & string all in the same call 
 */
void test_PrintBoolNumStringMix() {
    char *rez;
    ctest_begin("print bool (interpreted as a num),num,string mix");
    /* Print that string var */
    setTestLineHolder("print(s3+s4+s2)");
    handleInput((getTestLineHolder()));
    rez = Read_Batched_Write();
    
    ctest_assert_notnull(rez, "Nothing written to the write buffer");
    /* Note the \n! Always appended by default */
    ctest_assert_char_equalto("15.04 and another variable string value\n", rez, "Not equal to what was expected to be printed out");
    
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}
