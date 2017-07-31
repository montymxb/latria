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
 *  latria_relational_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_relational_tests.h"

void test_NestedRelationals();
void test_CompoundRelationals();
void test_SingleRelational();

/**
 * Run these tests
 */
void latria_relational_tests() {
    test_NestedRelationals();
    test_CompoundRelationals();
    test_SingleRelational();
}

/* Tests nested relational ops */
void test_NestedRelationals() {
    char *rez;
    ctest_begin("nested relationals");
    
    setTestLineHolder("if(1==1&&(2==2&&3==3)){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"nested success\")");
    handleInput((getTestLineHolder()));
    
    /* print 'c' forces our compiler to realize there is no extension of elseif,else & to compute NOW */
    setTestLineHolder("}print('c')");
    handleInput((getTestLineHolder()));
    
    rez = Read_Batched_Write();
    ctest_assert_char_equalto("nested success\nc\n", rez, "Did not execute nested relational op code properly");
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests Compound relational ops */
void test_CompoundRelationals() {
    char *rez;
    ctest_begin("compound relationals");
    
    setTestLineHolder("if(1==2&&2==0||4!=3){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"compound success\")");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}print('c')");
    handleInput((getTestLineHolder()));
    
    rez = Read_Batched_Write();
    ctest_assert_char_equalto("compound success\nc\n", rez, "Did not execute compound relational op code properly");
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests a SINGLE passed item as a relational call (boolean) */
void test_SingleRelational() {
    char *rez;
    ctest_begin("single true relational");
    
    setTestLineHolder("x=true");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("if(x == true){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"single true success\")");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}print('c')");
    handleInput((getTestLineHolder()));
    
    rez = Read_Batched_Write();
    ctest_assert_char_equalto("single true success\nc\n", rez, "Did not execute single true relational properly");
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
    
    /* SEPERATE OUT INTO A SEPARATE TEST */
    ctest_begin("single false relational");
    
    setTestLineHolder("x=false");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("if(!x){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"single false success\")");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}print('c')");
    handleInput((getTestLineHolder()));
    
    rez = Read_Batched_Write();
    ctest_assert_char_equalto("single false success\nc\n", rez, "Did not execute single false relational properly");
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}
