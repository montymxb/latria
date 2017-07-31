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
 *  latria_controlflow_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_controlflow_tests.h"

void test_BasicWhile();
void test_BasicFor();
void test_ChainedIfs();
void test_NestedIfs();
void test_NestedWhiles();
void test_NestedControlFlows();

/**
 * Run these tests
 */
void latria_controlflow_tests() {
    test_BasicWhile();
    test_BasicFor();
    test_ChainedIfs();
    test_NestedIfs();
    test_NestedWhiles();
    test_NestedControlFlows();
}

/**
 * Tests various nested control flows 
 */
void test_NestedControlFlows() {
    char *rez;
    char *checkAgainst = "f0:0:0\nf0:0:1\nf0:0:2\nresetting...\n0<\n1<\n2<\n3<\nf0:1:0\nf0:1:1\nf0:1:2\nresetting...\n0<\n1<\n2<\n3<\nf0:2:0\nf0:2:1\nf0:2:2\nresetting...\n0<\n1<\n2<\n3<\nf1:0:0\nf1:0:1\nf1:0:2\nresetting...\n0<\n1<\n2<\n3<\nf1:1:0\nf1:1:1\nf1:1:2\nresetting...\n0<\n1<\n2<\n3<\nf1:2:0\nf1:2:1\nf1:2:2\nresetting...\n0<\n1<\n2<\n3<\nf2:0:0\nx is now 2:2\nf2:0:1\nx is now 2:2\nf2:0:2\nresetting...\n0<\n1<\n2<\n3<\nf2:1:0\nx is now 2:2\nf2:1:1\nx is now 2:2\nf2:1:2\nresetting...\n0<\n1<\n2<\n3<\nf2:2:0\nx is now 2:2\nf2:2:1\nx is now 2:2\nf2:2:2\nresetting...\n0<\n1<\n2<\n3<\n";
    ctest_begin("nested control flows");
    
    setTestLineHolder("for(x=0,x<=2,x=x+1){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("for(y=0,y<=2,y=y+1){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("for(t=0,t<=2,t=t+1){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"f\"+x+\":\"+y+\":\"+t)");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("if(t==2){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"resetting...\")");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("m=0");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("while(m<4){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(m+\"<\")");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("m=m+1");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}elseif(x==2){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"x is now 2:\"+x)");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    rez = Read_Batched_Write();
    ctest_assert_char_equalto(checkAgainst, rez, "Did not execute nested control flows properly");
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}

/**
 * Tests Basic While Loops 
 */
void test_BasicWhile() {
    char *rez;
    ctest_begin("basic while");
    
    setTestLineHolder("x=0");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("while(x<10){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("x=x+1");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("print(x)");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    rez = Read_Batched_Write();
    ctest_assert_char_equalto("1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n", rez, "Basic while did not run properly");
    
    Flush_Batched_Write_NO_PRINT();
}

/**
 * Tests Basic For Loops 
 */
void test_BasicFor() {
    char *rez;
    ctest_begin("basic for");
    
    setTestLineHolder("for(k=0,k<=10,k=k+1){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"K:\"+k)");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "}print(\"done with k\")");
    handleInput((getTestLineHolder()));
    
    rez = Read_Batched_Write();
    
    ctest_assert_char_equalto("K:0\nK:1\nK:2\nK:3\nK:4\nK:5\nK:6\nK:7\nK:8\nK:9\nK:10\ndone with k\n", rez, "Basic for result did not match as expected");
    
    Flush_Batched_Write_NO_PRINT();
}

/**
 * Tests if, to elseif to else 
 */
void test_ChainedIfs() {
    char *rez;
    ctest_begin("chained ifs");
    
    setTestLineHolder("if(2!=2){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"invalid statement\")");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}elseif(19.2!=19.2){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("bn=5*2+4");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"2nd works!\"+bn)");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}else{");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"third & final\")");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    rez = Read_Batched_Write();
    ctest_assert_char_equalto("third & final\n", rez, "Did not execute chained ifs properly");
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}

/**
 * Tests nested if control flows 
 */
void test_NestedIfs() {
    char *rez;
    ctest_begin("nested ifs");
    
    setTestLineHolder("if(5==5){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("if(4!=3){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("if(3>2){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"nested ifs good\")");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}print('c')");
    handleInput((getTestLineHolder()));
    
    rez = Read_Batched_Write();
    ctest_assert_char_equalto("nested ifs good\nc\n", rez, "Did not execute nested ifs properly");
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}

/**
 * Tests nested while control flows 
 */
void test_NestedWhiles() {
    char *rez;
    char *checkAgainst = "w0:0:0\nw0:0:1\nw0:0:2\nw0:1:0\nw0:1:1\nw0:1:2\nw0:2:0\nw0:2:1\nw0:2:2\nw1:0:0\nw1:0:1\nw1:0:2\nw1:1:0\nw1:1:1\nw1:1:2\nw1:2:0\nw1:2:1\nw1:2:2\nw2:0:0\nw2:0:1\nw2:0:2\nw2:1:0\nw2:1:1\nw2:1:2\nw2:2:0\nw2:2:1\nw2:2:2\n";
    ctest_begin("nested while flows");
    
    setTestLineHolder("x=0");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("while(x<3){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("y=0");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("while(y<3){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("z=0");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("while(z<3){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"w\"+x+\":\"+y+\":\"+z)");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("z=z+1");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("y=y+1");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("x=x+1");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    rez = Read_Batched_Write();
    ctest_assert_char_equalto(checkAgainst, rez, "Did not execute nested whiles properly");
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}
