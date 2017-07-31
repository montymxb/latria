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
 *  latria_function_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_function_tests.h"

void test_basicFunction();
void test_returnNumberFunction();
void test_returnBooleanFunction();
void test_returnStringFunction();
void test_returnStringsConcatenated();
void test_returnVarFunction();
void test_returnArrayFunction();
void test_returnFunctionFromFunction();
void test_returnEarlyFromFunction();
void test_recursiveFunction();
void test_concatInputFunction();
void test_chainedFunctions();

/**
 * Run these tests
 */
void latria_function_tests() {
    test_basicFunction();
    test_returnNumberFunction();
    test_returnBooleanFunction();
    test_returnStringFunction();
    test_returnStringsConcatenated();
    test_returnVarFunction();
    test_returnArrayFunction();
    test_returnFunctionFromFunction();
    test_returnEarlyFromFunction();
    test_recursiveFunction();
    test_concatInputFunction();
    test_chainedFunctions();
}

/* Tests basic function declaration */
void test_basicFunction() {
    char *rez;
    ctest_begin("basic function");
    
    setTestLineHolder("@arrowPrint(x){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\">\"+x)");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "arrowPrint(\"man -oh+ man\")");
    handleInput((getTestLineHolder()));
    
    rez = Read_Batched_Write();
    ctest_assert_char_equalto(">man -oh+ man\n", rez, NULL);
    
    Flush_Batched_Write_NO_PRINT();
}

/* Tests a function that concatenates it's args and returns them */
void test_concatInputFunction() {
    struct CoreObject *co;
    ctest_begin("concat function");
    
    setTestLineHolder("@concat(x,y){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("return x+y");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "g=concat(\"foo \",\"time\")");
    handleInput((getTestLineHolder()));
    
    co = getValue("g");
    ctest_assert_notnull(co, "No CoreObject created or found");
    ctest_assert_int_equalto(1, co->_state, "Wrong data type");
    ctest_assert_char_equalto("foo time", co->data._svalue, NULL);
}

/* Tests functions calling functions in functions */
void test_chainedFunctions() {
    char *rez;
    ctest_begin("chained,nested & one-liner functions");
    
    setTestLineHolder("@test(){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "g=bacon(\"()\")");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("print(g)");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("return finalFunc()");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("@bacon(i){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "return \"bacon, what did you expect?\"");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("@finalFunc(){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("@innerFunc(){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"inner function walkie talkie!\")");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("innerFunc()");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "@oneliner(){print(\"one liners are pretty valid\")}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("oneliner()");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("@hmm(){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "print(\"hmm, is there something ahead of me?\")");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "}print(\"Yes! I'm right 'ahead' of you, after the closing }\")");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("hmm()");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "return \"this is final\"");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("b=test()");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("print(b)");
    handleInput((getTestLineHolder()));
    
    rez = Read_Batched_Write();
    ctest_assert_char_equalto("bacon, what did you expect?\ninner function walkie talkie!\none liners are pretty valid\nYes! I'm right 'ahead' of you, after the closing }\nhmm, is there something ahead of me?\nthis is final\n", rez, NULL);
    
    Flush_Batched_Write_NO_PRINT();
}

/* Tests returning a number from a function */
void test_returnNumberFunction() {
    struct CoreObject *co;
    ctest_begin("square number function");
    
    setTestLineHolder("@square(x){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("return x*x");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("h=square(5)");
    handleInput((getTestLineHolder()));
    
    co=getValue("h");
    ctest_assert_notnull(co, "No CoreObject created or found");
    ctest_assert_int_equalto(0, co->_state, "Wrong data type");
    ctest_assert_int_equalto(25, co->data._fvalue, NULL);
}

/* Tests returning a boolean from a function */
void test_returnBooleanFunction() {
    struct CoreObject *co;
    ctest_begin("true boolean function");
    
    setTestLineHolder("@getTrue(){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("return true");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("h=getTrue()");
    handleInput((getTestLineHolder()));
    
    co=getValue("h");
    ctest_assert_notnull(co, "No CoreObject created or found");
    ctest_assert_int_equalto(2, co->_state, "Wrong data type");
    ctest_assert_true(co->data._bvalue, NULL);
    
    ctest_begin("false boolean function");
    
    setTestLineHolder("@getFalse(){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("return false");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("h=getFalse()");
    handleInput((getTestLineHolder()));
    
    co=getValue("h");
    ctest_assert_notnull(co, "No CoreObject created or found");
    ctest_assert_int_equalto(2, co->_state, "Wrong data type");
    ctest_assert_false(co->data._bvalue, NULL);
}

/* Tests returning an array from a function */
void test_returnArrayFunction() {
    struct CoreObject *co;
    ctest_begin("array function");
    
    setTestLineHolder("@tinyarray(){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "x={\"data1\":\"value1\"}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("return x");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("h=tinyarray()");
    handleInput((getTestLineHolder()));
    
    co=getArrayValue("h", "data1");
    ctest_assert_notnull(co, "No CoreObject created or found");
    ctest_assert_int_equalto(1, co->_state, "Wrong data type");
    ctest_assert_char_equalto("value1", co->data._svalue, NULL);
}

/* Tests returning a string from a function */
void test_returnStringFunction() {
    struct CoreObject *co;
    ctest_begin("string function");
    
    setTestLineHolder("@getstring(){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "return \"a latria string\"");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("h=getstring()");
    handleInput((getTestLineHolder()));
    
    co = getValue("h");
    ctest_assert_notnull(co, "No CoreObject created or found");
    ctest_assert_int_equalto(1, co->_state, "Wrong data type");
    ctest_assert_char_equalto("a latria string", co->data._svalue, NULL);
}

/* Tests returning concatenated strings from a function (with an embedded single quote) */
void test_returnStringsConcatenated() {
    struct CoreObject *co;
    ctest_begin("string concat function");
    
    setTestLineHolder("@concatstrings(){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "return \"bill's\" + \" tortillas\"");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("q=concatstrings()");
    handleInput((getTestLineHolder()));
    
    co = getValue("q");
    ctest_assert_notnull(co, "No CoreObject created or found");
    ctest_assert_int_equalto(1, co->_state, "Wrong data type");
    ctest_assert_char_equalto("bill's tortillas", co->data._svalue, NULL);
}

/* Tests returning a var from a function */
void test_returnVarFunction() {
    struct CoreObject *co;
    ctest_begin("variable returning function");
    
    setTestLineHolder("@getstring(){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "x=\"another latria string\"");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("return x");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("h=getstring()");
    handleInput((getTestLineHolder()));
    
    co = getValue("h");
    ctest_assert_notnull(co, "No CoreObject created or found");
    ctest_assert_int_equalto(1, co->_state, "Wrong data type");
    ctest_assert_char_equalto("another latria string", co->data._svalue, NULL);
}

/* Tests returning a function from a function (the value of course) */
void test_returnFunctionFromFunction() {
    struct CoreObject *co;
    ctest_begin("function returning function");
    
    setTestLineHolder("@funcfunc(){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("return funcinfunc()");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("@funcinfunc(){");
    handleInput((getTestLineHolder()));
    
    strcpy((getTestLineHolder()), "return \"a value from inside the madness!\"");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("h=funcfunc()");
    handleInput((getTestLineHolder()));
    
    /* I fixed up this function already, but it's important to note that 'return func()' where func() is defined in the enclosing function will FAIL, as the enclosing scope (with func) will have been removed BEFORE it is called */
    
    co = getValue("h");
    ctest_assert_notnull(co, "No CoreObject created or found");
    ctest_assert_int_equalto(1, co->_state, "Wrong data type");
    ctest_assert_char_equalto("a value from inside the madness!", co->data._svalue, NULL);
}

void test_returnEarlyFromFunction() {
    char *rez;
    ctest_begin("function returning early");
    
    setTestLineHolder("@early(){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("print('first')");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("return");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("print('second')");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("early()");
    handleInput((getTestLineHolder()));
    
    rez = Read_Batched_Write();
    
    ctest_assert_char_equalto("first\n", rez, "Function returning early did not run as expected");
    
    Flush_Batched_Write_NO_PRINT();
}

/* Tests recursive functions */
void test_recursiveFunction() {
    struct CoreObject *co;
    ctest_begin("recursive function");
    
    setTestLineHolder("@recursive(x){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("if(x<10){");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("return recursive(x+1)");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("return x");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("}");
    handleInput((getTestLineHolder()));
    
    setTestLineHolder("h=recursive(0)");
    handleInput((getTestLineHolder()));
    
    co = getValue("h");
    ctest_assert_notnull(co, "No CoreObject created or found");
    ctest_assert_int_equalto(0, co->_state, "Wrong data type");
    ctest_assert_int_equalto(10, co->data._fvalue, NULL);
}
