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
 *  latria_string_tests.c
 *  Created by Benjamin Friedman on 9/29/15.
 */

#include "latria_string_tests.h"

/* String tests */
void test_StringAssign();
void test_NullStringAssign();
void test_StringReassign();
void test_StringConcat();
void test_StringEmbeddedOP();
void test_SingleQuoteInDoubleQuotes();
void test_SingleQuotedString();
void test_DoubleQuoteInSingleQuotes();

void test_StringReplace();
void test_StringReplaceAll();
void test_StringSubFull();
void test_StringSubShort();
void test_StringFind();

/**
 * Run these tests
 */
void latria_string_tests() {
    test_StringAssign();
    test_NullStringAssign();
    test_StringReassign();
    test_StringConcat();
    test_StringEmbeddedOP();
    test_SingleQuoteInDoubleQuotes();
    test_SingleQuotedString();
    test_DoubleQuoteInSingleQuotes();
    
    test_StringReplace();
    test_StringReplaceAll();
    test_StringSubFull();
    test_StringSubShort();
    test_StringFind();
}


/* Test assigning a string value */
void test_StringAssign() {
    struct CoreObject *co;
    ctest_begin("string assign");
    strcpy((getTestLineHolder()), "z=\"apples and oranges\"");
    handleInput((getTestLineHolder()));
    co = getValue("z");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct data type");
    ctest_assert_false((strcmp(co->data._svalue, "apples and oranges")!=0), "Not equal to 'apples and oranges' string");
}

/* Test assigning a string of value "null" */
void test_NullStringAssign() {
    struct CoreObject *co;
    ctest_begin("string null assign");
    strcpy((getTestLineHolder()), "z=\"null\"");
    handleInput((getTestLineHolder()));
    co = getValue("z");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct data type");
    ctest_assert_false((strcmp(co->data._svalue, "null")!=0), "Not equal to 'null' string");
}

/* Test reassigning a string value */
void test_StringReassign() {
    struct CoreObject *co;
    ctest_begin("string null reassign");
    strcpy((getTestLineHolder()), "z=\"null\"");
    handleInput((getTestLineHolder()));
    co = getValue("z");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct data type");
    ctest_assert_false((strcmp(co->data._svalue, "null")!=0), "Not equal to 'null' string");
}

/* Tests adding strings together */
void test_StringConcat() {
    struct CoreObject *co;
    ctest_begin("string concat");
    strcpy((getTestLineHolder()), "b=z+\" and some extra content\"");
    handleInput((getTestLineHolder()));
    co = getValue("b");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct data type");
    ctest_assert_false((strcmp(co->data._svalue, "null and some extra content")!=0), "Not equal to 'null and some extra content' string");
}

/* Tests creating a string with an embedded Sys OP inside */
void test_StringEmbeddedOP() {
    struct CoreObject *co;
    ctest_begin("string embedded op");
    strcpy((getTestLineHolder()), "y2k=\"__setGCRate(1)\"");
    handleInput((getTestLineHolder()));
    co = getValue("y2k");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct data type");
    ctest_assert_false((strcmp(co->data._svalue, "__setGCRate(1)")!=0), "Not equal to '__setGCRate(1)' string");
}

/* Tests a single quote embedded (and unescaped) inside a double quoted string is not interpreted as anything other than just another character */
void test_SingleQuoteInDoubleQuotes() {
    struct CoreObject *co;
    ctest_begin("string embedded single quote");
    strcpy((getTestLineHolder()), "y3k=\"a string's string\"");
    handleInput((getTestLineHolder()));
    co = getValue("y3k");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct data type");
    ctest_assert_false((strcmp(co->data._svalue, "a string's string")!=0), "Not equal to 'a string's string' string");
}

/* Tests a regular single quoted string */
void test_SingleQuotedString() {
    struct CoreObject *co;
    ctest_begin("string single quoted");
    setTestLineHolder("y3k='a single quoted string'");
    handleInput((getTestLineHolder()));
    co = getValue("y3k");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct data type");
    ctest_assert_false((strcmp(co->data._svalue, "a single quoted string")!=0), "Not equal to 'a single quoted string' string");
}

/* Tests a double quote embedded in a single quoted string */
void test_DoubleQuoteInSingleQuotes() {
    struct CoreObject *co;
    ctest_begin("string single quote w/ embedded double quote");
    strcpy((getTestLineHolder()), "y3k='a single quoted \" string'");
    handleInput((getTestLineHolder()));
    co = getValue("y3k");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct data type");
    ctest_assert_false((strcmp(co->data._svalue, "a single quoted \" string")!=0), "Not equal to 'a single quoted \" string' string");
}

/* Tests String Replace, single */
void test_StringReplace() {
    struct CoreObject *co;
    ctest_begin("string replace");
    strcpy((getTestLineHolder()), "r=replace(\"some junky string, not junky!\",\"junky\",\"fantastical\",0)");
    handleInput((getTestLineHolder()));
    co = getValue("r");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct object type");
    ctest_assert_char_equalto("some fantastical string, not junky!", co->data._svalue, "Not equal to tgt string");
}

/* Tests String Replace, all */
void test_StringReplaceAll() {
    struct CoreObject *co;
    ctest_begin("string replace all");
    strcpy((getTestLineHolder()), "r=replace(\"some junky string, very junky!\",\"junky\",\"fantastical\",1)");
    handleInput((getTestLineHolder()));
    co = getValue("r");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct object type");
    ctest_assert_char_equalto("some fantastical string, very fantastical!", co->data._svalue, "Not equal to tgt string");
}

/* Tests String Substitution */
void test_StringSubFull() {
    struct CoreObject *co;
    ctest_begin("string substitution full");
    strcpy((getTestLineHolder()), "sub=substr(\"get this section inside here\",4,16)");
    handleInput((getTestLineHolder()));
    co = getValue("sub");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct object type");
    ctest_assert_char_equalto("this section", co->data._svalue, "Not equal to tgt string");
}

/* Tests String Substitution */
void test_StringSubShort() {
    struct CoreObject *co;
    ctest_begin("string substitution short");
    strcpy((getTestLineHolder()), "sub=substr(\"get this section inside here\",4)");
    handleInput((getTestLineHolder()));
    co = getValue("sub");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(1, co->_state, "Not of correct object type");
    ctest_assert_char_equalto("this section inside here", co->data._svalue, "Not equal to tgt string");
}

/* Tests finding a string in a string */
void test_StringFind() {
    struct CoreObject *co;
    ctest_begin("string find");
    strcpy((getTestLineHolder()), "gh=find(\"hello world\",\"world\")");
    handleInput((getTestLineHolder()));
    co = getValue("gh");
    
    ctest_assert_notnull(co, "No CoreObject created");
    ctest_assert_int_equalto(0, co->_state, "Not of correct object type");
    ctest_assert_int_equalto(6, co->data._fvalue, NULL);
}
