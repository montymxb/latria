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

void testFailedWithError(char *name,char *er);
void utestFailedWithError(char *name,char *er);
void testPassed(char *testname);

void runFunctionalTests();
void runUnitTests();

/* START UNIT TESTS */

void utest_getRelationalOperatorResult();
void utest_stripWhitespace();
void utest_random();
void utest_regex();

/* END UNIT TESTS */


/* START FUNCTIONAL TESTS */

/* Integer tests */
void ftest_IntegerAssign();
void ftest_IntegerReassign();

/* Float tests */
void ftest_FloatAssign();
void ftest_FloatReassign();

/* String tests */
void ftest_StringAssign();
void ftest_NullStringAssign();
void ftest_StringReassign();
void ftest_StringConcat();
void ftest_StringEmbeddedOP();
void ftest_SingleQuoteInDoubleQuotes();
void ftest_SingleQuotedString();
void ftest_DoubleQuoteInSingleQuotes();

/* Other Assignment Tests */
void ftest_VarToVarAssign();
void ftest_BooleanAssign();
void ftest_RelationalAssign();

/* Print tests */
void ftest_PrintString();
void ftest_PrintStringVar();
void ftest_PrintStringConcat();
void ftest_PrintMixedConcat();
void ftest_PrintVarConcat();
void ftest_PrintBool();
void ftest_PrintNumVar();
void ftest_PrintBoolNumStringMix();

/* Arithmetic tests */
void ftest_Arithmetic();

/* String functions */
void ftest_StringReplace();
void ftest_StringReplaceAll();
void ftest_StringSubFull();
void ftest_StringSubShort();
void ftest_StringFind();

/* Array Tests */
void ftest_OneArrayElement();
void ftest_TwoArrayElements();
void ftest_ThreeArrayElements();
void ftest_ChangeArrayElement();
void ftest_ChangeAllArrayElements();

/* Relational Tests */
void ftest_NestedRelationals();
void ftest_CompoundRelationals();
void ftest_SingleRelational();

/* Control Flow Tests */
void ftest_BasicWhile();
void ftest_BasicFor();
void ftest_ChainedIfs();
void ftest_NestedIfs();
void ftest_NestedWhiles();
void ftest_NestedControlFlows();

/* Sys execution & var interpolation */
void ftest_ExecSimpleEcho();
void ftest_ExecInterpolation();
void ftest_ExecMultiLine();

/* Function Tests */
void ftest_basicFunction();
void ftest_returnNumberFunction();
void ftest_returnBooleanFunction();
void ftest_returnStringFunction();
void ftest_returnStringsConcatenated();
void ftest_returnVarFunction();
void ftest_returnArrayFunction();
void ftest_returnFunctionFromFunction();
void ftest_returnEarlyFromFunction();
void ftest_recursiveFunction();
void ftest_concatInputFunction();
void ftest_chainedFunctions();

/* File IO tests */
void ftest_FileIO();

/* END FUNCTIONAL TESTS */

int failCount = 0;

char testLineHolder[512];
char testName[256];

/* Runs ALL Tests */
void runAllTests() {
    
    openDynamicInterpreterFile();
    
    failCount = 0;
    
    printf("\nRunning all tests\n\n");
    
    /* Alter GC to zero, brings out all the bugs when they're around */
    strcpy(testLineHolder, "__setGCRate(0)");
    handleInput(testLineHolder);
    
    runUnitTests();
    runFunctionalTests();
    
    closeDynamicInterpreterFile();
    
    /* free compiler related items */
    freeByteCodes();
    freeLexicalAllocations();
    deallocStackStates();
    
    #if !defined(MACOSX) && !defined(LINUXOS)
    freeLatria();
    #endif
    
    if(failCount == 0) {
        printf("\nAll Tests Passed\n");
        exit(0);
    } else {
        printf("\n%d Tests Failed\n\n", failCount);
        exit(1);
    }
}

/* Runs unit tests */
void runUnitTests() {
    printf("================\nUNIT TESTS\n================\n");
    
    /* 
     Former design was a truly interpretive scripting approach where the below were useful. No longer the case
     
     utest_getRelationalOperatorResult();
     utest_fetchArg();
     */
    utest_stripWhitespace();
    utest_random();
    utest_regex();
}

/* Runs functional tests */
void runFunctionalTests() {
    printf("================\nFUNCTIONAL TESTS\n================\n");
    
    ftest_IntegerAssign();
    ftest_IntegerReassign();
    
    ftest_FloatAssign();
    ftest_FloatReassign();
    
    ftest_StringAssign();
    ftest_NullStringAssign();
    ftest_StringReassign();
    ftest_StringConcat();
    ftest_StringEmbeddedOP();
    ftest_SingleQuoteInDoubleQuotes();
    ftest_SingleQuotedString();
    ftest_DoubleQuoteInSingleQuotes();
    
    ftest_VarToVarAssign();
    ftest_BooleanAssign();
    /* TODO holding off on relational assign for now */
    /* ftest_RelationalAssign(); */
    
    ftest_PrintString();
    ftest_PrintStringVar();
    ftest_PrintStringConcat();
    ftest_PrintMixedConcat();
    ftest_PrintVarConcat();
    ftest_PrintBool();
    ftest_PrintNumVar();
    ftest_PrintBoolNumStringMix();
    
    ftest_Arithmetic();
     
    ftest_StringReplace();
    ftest_StringReplaceAll();
    ftest_StringSubFull();
    ftest_StringSubShort();
    ftest_StringFind();
    
    ftest_OneArrayElement();
    ftest_TwoArrayElements();
    ftest_ThreeArrayElements();
    ftest_ChangeArrayElement();
    ftest_ChangeAllArrayElements();
    
    ftest_FileIO();
    ftest_NestedRelationals();
    ftest_CompoundRelationals();
    ftest_SingleRelational();
    
    ftest_BasicWhile();
    ftest_BasicFor();
    ftest_ChainedIfs();
    ftest_NestedIfs();
    ftest_NestedWhiles();
    ftest_NestedControlFlows();
    
    ftest_ExecSimpleEcho();
    ftest_ExecInterpolation();
    ftest_ExecMultiLine();
    
    ftest_basicFunction();
    ftest_returnNumberFunction();
    ftest_returnBooleanFunction();
    ftest_returnStringFunction();
    ftest_returnStringsConcatenated();
    ftest_returnVarFunction();
    ftest_returnArrayFunction();
    ftest_returnFunctionFromFunction();
    ftest_returnEarlyFromFunction();
    ftest_recursiveFunction();
    ftest_concatInputFunction();
    ftest_chainedFunctions();
}

/* START UNIT TESTS */

/* Unit test for noMathOperatorsPresent function */
/* DEPRECATED TEST : no longer relevant in the current version of latria
void utest_noMathOpsPresent() {
    LATBool failed = false;
    strcpy(testName, "noMathOperatorsPresent unit test");
    if(!noMathOperatorsPresent("some arbitary string")) {
        failed = true;
        utestFailedWithError(testName, "Failed on string with NO math operators");
    }
    
    if(noMathOperatorsPresent("some% arbitary string + -")) {
        failed = true;
        utestFailedWithError(testName, "Failed on string with math operators");
    }
    
    if(!noMathOperatorsPresent("\"encapsulated* / -+\"")) {
        failed = true;
        utestFailedWithError(testName, "Failed on string with math operators in a string");
    }
    
    if(noMathOperatorsPresent("\"string one\"+\"string 2\"")) {
        failed = true;
        utestFailedWithError(testName, "Failed on two strings being added");
    }
    
    if(noMathOperatorsPresent("f(x+y+\"dunigan\")")) {
        failed = true;
        utestFailedWithError(testName, "Failed on function call with math operators");
    }
    
    if(!noMathOperatorsPresent("\"latria-test-file-for-functional-testing.txt\"")) {
        failed = true;
        utestFailedWithError(testName, "Failed on file name with multiple dashes");
    }
    
    if(!failed) {
        testPassed(testName);
    }
}
 */


/* Unit test for findUnquotedCharacter function */
/*
void utest_findUnquotedCharacter() {
    LATBool passed = true;
    strcpy(testName, "findUnquotedCharacter unit test");
    if(findUnquotedCharacter('a', "a")==-1) {
        utestFailedWithError(testName, "Failed to find unquoted character");
        passed = false;
    }
    
    if(findUnquotedCharacter('a', "\"apples are delicious\" jgmdjks")!=-1) {
        utestFailedWithError(testName, "Found a quoted character incorrectly");
        passed = false;
    }
    
    if(findUnquotedCharacter('+', "\"baby back \"+\" ribs\"")==-1) {
        utestFailedWithError(testName, "Failed to find unquoted character");
        passed = false;
    }
    
    if(findUnquotedCharacter('+', "\"baby back +++ ribs\"")!=-1) {
        utestFailedWithError(testName, "Found a quoted character incorrectly");
        passed = false;
    }
    
    if(findUnquotedCharacter('a', "a")==-1) {
        utestFailedWithError(testName, "Failed to find unquoted character");
        passed = false;
    }
    
    if(passed) {
        testPassed(testName);
    }
}
 */

/* Unit test for findUncontainedCharacter function */
/*
void utest_findUncontainedCharacter() {
    LATBool passed = true;
    strcpy(testName, "findUncontainedCharacter unit test");
    if(findUncontainedChar('a', "a")==-1) {
        utestFailedWithError(testName, "Failed to find uncontained character");
        passed = false;
    }
    
    if(findUncontainedChar('a', "\"apples are delicious\" jgmdjks")!=-1) {
        utestFailedWithError(testName, "Found a contained character incorrectly");
        passed = false;
    }
    
    if(findUncontainedChar('+', "\"baby back \"+\" ribs\"")==-1) {
        utestFailedWithError(testName, "Failed to find uncontained character");
        passed = false;
    }
    
    if(findUncontainedChar('+', "\"baby back +++ ribs\"")!=-1) {
        utestFailedWithError(testName, "Found a contained character incorrectly");
        passed = false;
    }
    
    if(findUncontainedChar('a', "a")==-1) {
        utestFailedWithError(testName, "Failed to find uncontained character");
        passed = false;
    }
    
    if(findUncontainedChar('+', "f(x+y)")!=-1) {
        utestFailedWithError(testName, "Found a contained character incorrectly");
        passed = false;
    }
    
    if(findUncontainedChar('+', "f(\"x+y\")")!=-1) {
        utestFailedWithError(testName, "Found a contained character incorrectly");
        passed = false;
    }
    
    if(findUncontainedChar('+', "f(x+y)+z")==-1) {
        utestFailedWithError(testName, "Failed to find uncontained character");
        passed = false;
    }
    
    if(findUncontainedChar('+', "f(\"x+y\")-\"+\"-(+)/(\"+\")")!=-1) {
        utestFailedWithError(testName, "Found a contained character incorrectly");
        passed = false;
    }
    
    if(findUncontainedChar('+', "f(\"x+y\")-\"+\"-(+)+/(\"+\")")==-1) {
        utestFailedWithError(testName, "Failed to find uncontained character");
        passed = false;
    }
    
    if(passed) {
        testPassed(testName);
    }
}
 */

/* Unit tests for getRelationalOperatorResult */
void utest_getRelationalOperatorResult() {
    /*
    int localFailCount = failCount;
    strcpy(testName, "getRelationalOperatorResult unit test");
     */
    
    /*
    strcpy(testLineHolder, "1==1");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed basic 1==1 relational op");
    
    strcpy(testLineHolder, "1!=1");
    if(getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed basic 1!=1 relational op");
    
    strcpy(testLineHolder, "15.2==15.2");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed 15.2==15.2 relational op");
    
    strcpy(testLineHolder, "15.2!=19.2");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed 15.2==15.2 relational op");
    
    strcpy(testLineHolder, "\"apple\"==\"apple\"");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed string to string relational op");
    
    strcpy(testLineHolder, "x=89.1");
    handleInput(testLineHolder);
    strcpy(testLineHolder, "x==89.1");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed x==89.1 relational op");
    
    strcpy(testLineHolder, "x!=89.1");
    if(getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed x!=89.1 relational op");
    
    strcpy(testLineHolder, "81!=x");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed 81!=x relational op");
    
    strcpy(testLineHolder, "x=\"quadruple\"");
    handleInput(testLineHolder);
    strcpy(testLineHolder, "x==\"quadruple\"");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed variable to string relational op");
    
    strcpy(testLineHolder, "y=\"quadruple\"");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "x==y");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed variable to variable (string) relational op");
    
    strcpy(testLineHolder, "x=24");
    handleInput(testLineHolder);
    strcpy(testLineHolder, "y=24");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "x==y");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed true variable to variable relational op");
    
    strcpy(testLineHolder, "y=21");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "y==x");
    if(getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed false variable to variable relational op");
    
    strcpy(testLineHolder, "2>1");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed basic greater than relational op");
    
    strcpy(testLineHolder, "1<2");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed basic less than relational op");
    
    strcpy(testLineHolder, "2>=2");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed basic greater than or equal to relational op");
    
    strcpy(testLineHolder, "2<=2");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed basic less than or equal to relational op");
    
    strcpy(testLineHolder, "y<=x");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed variable less than or equal to relational op");
    
    strcpy(testLineHolder, "x>=y");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed variable less than or equal to relational op");
    
    strcpy(testLineHolder, "x>=y&&2!=1");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed compound relational op");
    
    strcpy(testLineHolder, "x>=y?50==50");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed or relational op");
    
    strcpy(testLineHolder, "2>100?(1==1&&2==2)");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed 'or nested and' relational op");
    
    strcpy(testLineHolder, "50>40&&(1>5?5>1)");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed 'and nested or' relational op");
    
    strcpy(testLineHolder, "(1==1&&2==2)?2>100");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed inverse 'or nested and' relational op");
    
    strcpy(testLineHolder, "(1>5?5>1)&&50>40");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed inverse 'and nested or' relational op");
    
    strcpy(testLineHolder, "\"null\"==\"null\"");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed null to null string relational op");
    
    strcpy(testLineHolder, "null==null");
    if(!getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed null to null relational op");
    
    strcpy(testLineHolder, "null==\"null\"");
    if(getRelationalOperatorResult(testLineHolder))
        utestFailedWithError(testName, "Failed null to \"null\" relational op");
     
     */
    
    /*
    if(localFailCount == failCount) {
        testPassed(testName);
    }
     */
}

/* Unit test for stripping unwanted symbols from input (\n, \r, \t, etc) */
/*
void utest_stripUnwantedSymbols() {
    
    int localFailCount = failCount;
    strcpy(testName, "strip unwanted symbols unit test");
    
    strcpy(testLineHolder, "do not modify this input");
    stripOutUnwantedSymbols(testLineHolder);
    if(LAT_STRCMP(testLineHolder, "do not modify this input") != 0) {
        utestFailedWithError(testName, "Failed with non modifiable input");
    }
    
    strcpy(testLineHolder, "strip of the end of this one\r");
    stripOutUnwantedSymbols(testLineHolder);
    if(LAT_STRCMP(testLineHolder, "strip of the end of this one") != 0) {
        utestFailedWithError(testName, "Failed with simple input");
    }
    
    strcpy(testLineHolder, "strip both\t\r");
    stripOutUnwantedSymbols(testLineHolder);
    if(LAT_STRCMP(testLineHolder, "strip both") != 0) {
        utestFailedWithError(testName, "Failed with moderate input");
    }
    
    strcpy(testLineHolder, "\rboth sides\t\r");
    stripOutUnwantedSymbols(testLineHolder);
    if(LAT_STRCMP(testLineHolder, "both sides") != 0) {
        utestFailedWithError(testName, "Failed with doubled complex input");
    }
    
    strcpy(testLineHolder, "\r\tall \t around\t\r");
    stripOutUnwantedSymbols(testLineHolder);
    if(LAT_STRCMP(testLineHolder, "all  around") != 0) {
        utestFailedWithError(testName, "Failed with embedded complex input");
    }
    
    strcpy(testLineHolder, "\r\tall\" \t \"around\t\r");
    stripOutUnwantedSymbols(testLineHolder);
    if(LAT_STRCMP(testLineHolder, "all\" \t \"around") != 0) {
        utestFailedWithError(testName, "Failed with double quoted input");
    }
    
    strcpy(testLineHolder, "\r\tall' \t 'around\t\r");
    stripOutUnwantedSymbols(testLineHolder);
    if(LAT_STRCMP(testLineHolder, "all' \t 'around") != 0) {
        utestFailedWithError(testName, "Failed with single quoted input");
    }
    
    strcpy(testLineHolder, "\r\"\tall\"+\"around\r\"\t");
    stripOutUnwantedSymbols(testLineHolder);
    if(LAT_STRCMP(testLineHolder, "\"\tall\"+\"around\r\"") != 0) {
        utestFailedWithError(testName, "Failed with multiple double quoted input");
    }
    
    if(localFailCount == failCount) {
        testPassed(testName);
    }
}
 */

/* Unit tests for stripping whitespace, while avoiding altering quoted material */
void utest_stripWhitespace() {
    
    int localFailCount = failCount;
    char *arg;
    strcpy(testName, "strip whitespace unit test");
    
    strcpy(testLineHolder, "compress this");
    arg = stripWhitespace(testLineHolder);
    if(LAT_STRCMP(arg, "compressthis") != 0) {
        utestFailedWithError(testName, "Failed with simple input");
    }
    
    strcpy(testLineHolder, "compress this and this");
    arg = stripWhitespace(testLineHolder);
    if(LAT_STRCMP(arg, "compressthisandthis") != 0) {
        utestFailedWithError(testName, "Failed with moderate input");
    }
    
    strcpy(testLineHolder, "'do no compress this'");
    arg = stripWhitespace(testLineHolder);
    if(LAT_STRCMP(arg, "'do no compress this'") != 0) {
        utestFailedWithError(testName, "Failed with single quoted input");
    }
    
    strcpy(testLineHolder, "\"or this\"");
    arg = stripWhitespace(testLineHolder);
    if(LAT_STRCMP(arg, "\"or this\"") != 0) {
        utestFailedWithError(testName, "Failed with double quoted input");
    }
    
    strcpy(testLineHolder, " \"trim around\" ");
    arg = stripWhitespace(testLineHolder);
    if(LAT_STRCMP(arg, "\"trim around\"") != 0) {
        utestFailedWithError(testName, "Failed with trimming around simple quoted input");
    }
    
    strcpy(testLineHolder, " \"trim around\" + \" all this \" ");
    arg = stripWhitespace(testLineHolder);
    if(LAT_STRCMP(arg, "\"trim around\"+\" all this \"") != 0) {
        utestFailedWithError(testName, "Failed with trimming around multiple quoted inputs");
    }
    
    strcpy(testLineHolder, " \" trim around \" + ' mixed stuff ' ");
    arg = stripWhitespace(testLineHolder);
    if(LAT_STRCMP(arg, "\" trim around \"+' mixed stuff '") != 0) {
        utestFailedWithError(testName, "Failed with trimming around mixed type multiple quoted inputs");
    }
    
    if(localFailCount == failCount) {
        testPassed(testName);
    }
}

/* Unit tests for random */
void utest_random() {
    int origValue;
    int localFailCount = failCount;
    int rez;
    strcpy(testName, "random unit test");
    
    /* Test for 0 only */
    rez = Sys_Random(1);
    if(rez >= 1) {
        utestFailedWithError(testName, "Failed with random being greater than 1.");
    }
    
    /* Test between 100 */
    rez = Sys_Random(100);
    if(rez >= 100) {
        utestFailedWithError(testName, "Failed with random being greater than 100.");
    }
    
    /* Reseed */
    Sys_RandomSeed(12345);
    
    /* Test Value */
    rez = Sys_Random(100);
    if((origValue = rez) >= 100) {
        utestFailedWithError(testName, "Failed with random being greater than 100.");
    }
    
    /* Reseed, again */
    Sys_RandomSeed(12345);
    
    /* Test Value to MATCH origValue */
    rez = Sys_Random(100);
    if(rez != origValue) {
        utestFailedWithError(testName, "Failed with seeding not generating same results.");
    }
    
    if(localFailCount == failCount) {
        testPassed(testName);
    }
}

/* Unit tests for regex */
void utest_regex() {
    int localFailCount = failCount;
    strcpy(testName, "regex unit test");
    
    if(regex("input", "input") == 0) {
        utestFailedWithError(testName, "Failed with basic regex full");
    }
    
    if(regex("input", "in") == 0) {
        utestFailedWithError(testName, "Failed with basic regex at start");
    }
    
    if(regex("input", "pu") == 0) {
        utestFailedWithError(testName, "Failed with basic regex in middle");
    }
    
    if(regex("input", "put") == 0) {
        utestFailedWithError(testName, "Failed with basic regex at end");
    }
    
    if(regex("input", "ins") == 1) {
        utestFailedWithError(testName, "Failed where mismatch should occur");
    }
    
    if(regex("input", "input2") == 1) {
        utestFailedWithError(testName, "Matched where regex should not be completed yet");
    }
    
    if(regex("input", "ps") == 1) {
        utestFailedWithError(testName, "Matched incorrect section in the middle");
    }
    
    if(regex("input", ".*") == 0) {
        utestFailedWithError(testName, "Failed with .* to match all");
    }
    
    if(regex("input", "n.u") == 0) {
        utestFailedWithError(testName, "Failed with . to match single wildcard char");
    }
    
    if(regex("input", ".n.u.") == 0) {
        utestFailedWithError(testName, "Failed with . to match multiple wildcard char");
    }
    
    if(regex("input", "i.*t") == 0) {
        utestFailedWithError(testName, "Failed with wildcard surrounded by constant chars");
    }
    
    if(regex("input", "^in") == 0) {
        utestFailedWithError(testName, "Failed matching at very beginning");
    }
    
    if(regex("input", "$.n.u.") == 1) {
        utestFailedWithError(testName, "Matched impossible match");
    }
    
    if(regex("input", "^.n.u.") == 0) {
        utestFailedWithError(testName, "Failed to match with multiple wildcards at start");
    }
    
    if(regex("input", "^np") == 1) {
        utestFailedWithError(testName, "Matched at an incorrect spot at very beginning");
    }
    
    if(regex("input", "put$") == 0) {
        utestFailedWithError(testName, "Failed matching with line anchor $");
    }
    
    if(regex("input", "i*nput") == 0) {
        utestFailedWithError(testName, "Failed with indefinite i's ");
    }
    
    if(regex("input", "i.+t") == 0) {
        utestFailedWithError(testName, "Failed with + surrounded by constant chars");
    }
    
    if(regex("input", "inps?ut") == 0) {
        utestFailedWithError(testName, "Failed matching with optional char");
    }
    
    if(regex("input", "^input$") == 0) {
        utestFailedWithError(testName, "Failed matching with leading and trailing anchors");
    }
    
    /* Tests for stripping out duplicates as well */
    if(regex("input", "i[nnnie]pu") == 0) {
        utestFailedWithError(testName, "Failed matching character class at beginning of class");
    }
    
    if(regex("input", "i[ine]pu") == 0) {
        utestFailedWithError(testName, "Failed matching character class at middle of class");
    }
    
    if(regex("input", "i[ien]pu") == 0) {
        utestFailedWithError(testName, "Failed matching character class at end of class");
    }
    
    if(regex("input", "i[ien][poi]u") == 0) {
        utestFailedWithError(testName, "Failed matching double back to back character classes");
    }
    
    if(regex("input", "[ien][^p][pou]") == 0) {
        utestFailedWithError(testName, "Failed matching 3 back to back classes, one negative two positive");
    }
    
    if(regex("input", "[ien][ato]?.[pou]") == 0) {
        utestFailedWithError(testName, "Failed matching optional class followed by a single wildcard");
    }
    
    if(regex("input", "[abidnopsut]*") == 0) {
        utestFailedWithError(testName, "Failed matching entirely by * class");
    }
    
    if(regex("input", "[abidnopsut]+") == 0) {
        utestFailedWithError(testName, "Failed matching entirely by + class");
    }
    
    
    if(regex("input", "(.*)$") == 0) {
        utestFailedWithError(testName, "Failed matching all wildcard in parentheses");
    }
    
    if(getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "input") != 0) {
        utestFailedWithError(testName, "Incorrect value from matching all wildcard in parentheses");
        printf("%s\n\n", getCaptureByIndex(0));
    }
    
    
    if(regex("input", "(.*)t") == 0) {
        utestFailedWithError(testName, "Failed matching all wildcard in parentheses followed by constant");
    }
    
    if(getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "inpu") != 0) {
        utestFailedWithError(testName, "Incorrect value from matching all wildcard in parentheses followed by constant");
        printf("%s\n\n", getCaptureByIndex(0));
    }
    
    if(regex("input", "(.*t)") == 0) {
        utestFailedWithError(testName, "Failed matching all wildcard and constant in parentheses");
    }
    
    if(getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "input") != 0) {
        utestFailedWithError(testName, "Incorrect value from matching all wildcard and constant in parentheses");
        printf("%s\n\n", getCaptureByIndex(0));
    }
    
    if(regex("input", "([^t]*t)") == 0) {
        utestFailedWithError(testName, "Failed matching anti-class wildcard");
    }
    
    if(getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "input") != 0) {
        utestFailedWithError(testName, "Incorrect value from matching anti-class wildcard in parentheses");
        printf("%s\n\n", getCaptureByIndex(0));
    }
    
    if(regex("input", "([^t]+)t") == 0) {
        utestFailedWithError(testName, "Failed matching anti-class wildcard + in parentheses");
    }
    
    if(getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "inpu") != 0) {
        utestFailedWithError(testName, "Incorrect value from matching anti-class wildcard + in parentheses");
        printf("%s\n\n", getCaptureByIndex(0));
    }
    
    if(regex("input", "(in)put$") == 0) {
        utestFailedWithError(testName, "Failed matching partially in parentheses");
    }
    
    if(getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "in") != 0) {
        utestFailedWithError(testName, "Incorrect value from partial match in parentheses");
        printf("%s\n\n", getCaptureByIndex(0));
    }
    
    if(regex("input", "(input)$") == 0) {
        utestFailedWithError(testName, "Failed matching all with anchor in parentheses");
    }
    
    if(getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "input") != 0) {
        utestFailedWithError(testName, "Incorrect value from all with anchor in parentheses");
        printf("%s\n\n", getCaptureByIndex(0));
    }
    
    if(regex("input", "(?:input$)") == 0) {
        utestFailedWithError(testName, "Failed matching all with anchor in non-capturing parentheses");
    }
    
    if(regex("input", "(p.*)$") == 0) {
        utestFailedWithError(testName, "Failed matching part with anchor in capturing parentheses");
    }
    
    if(getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "put") != 0) {
        utestFailedWithError(testName, "Incorrect value captured from matching part with anchor in capturing parentheses");
        printf("%s\n\n", getCaptureByIndex(0));
    }
    
    if(regex("input", "app|input") == 0) {
        utestFailedWithError(testName, "Failed matching alternation");
    }
    
    if(regex("abc[xyz]", ".+\\[.+\\]") == 0) {
        utestFailedWithError(testName, "Failed to match with brackets");
    }
    
    if(regex("abcasl]skdk", ".+\\[.+\\]") == 1) {
        utestFailedWithError(testName, "Matched incorrectly to one bracket");
    }
    
    if(regex("input", "(appsareaverylongthing|input)") == 0) {
        utestFailedWithError(testName, "Failed matching alternation with parens");
    }
    
    if(getCaptureByIndex(0) == NULL || strcmp(getCaptureByIndex(0), "input") != 0) {
        utestFailedWithError(testName, "Incorrect value from matching alternation with parens");
        printf("%s\n\n", getCaptureByIndex(0));
    }
    
    if(regex("input", "i.*|inpug") == 0) {
        utestFailedWithError(testName, "Failed matching alternation with wildcard");
    }
    
    if(regex("input", "i.*g|inpug") == 1) {
        utestFailedWithError(testName, "Incorrectly matched alternation with wildcard following by regular char");
    }
    
    if(localFailCount == failCount) {
        testPassed(testName);
    }
}

/* END UNIT TESTS */


/* START FUNCTIONAL TESTS */

/* Test Assigning an integer value */
void ftest_IntegerAssign() {
    struct CoreObject *co;
    strcpy(testName, "integer assign test");
    strcpy(testLineHolder, "x=2");
    handleInput(testLineHolder);
    co = getValue("x");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 0) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(co->data._fvalue != 2) {
        testFailedWithError(testName,"Not equal to 2");
    } else {
        testPassed(testName);
    }
}

/* Test reassigning an integer value */
void ftest_IntegerReassign() {
    struct CoreObject *co;
    strcpy(testName, "integer reassign");
    strcpy(testLineHolder, "x=24");
    handleInput(testLineHolder);
    co = getValue("x");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 0) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(co->data._fvalue != 24) {
        testFailedWithError(testName,"Not equal to 24");
    } else {
        testPassed(testName);
    }
}

/* Test assigning a float */
void ftest_FloatAssign() {
    struct CoreObject *co;
    strcpy(testName, "float assign");
    strcpy(testLineHolder, "y=16.02");
    handleInput(testLineHolder);
    co = getValue("y");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 0) {
        testFailedWithError(testName,"Not of correct data type");
    } else if((fabs(co->data._fvalue - 16.02)) >= 0.000001) {
        testFailedWithError(testName,"Not equal to 16.02");
        printf("%f\n\n", co->data._fvalue);
    } else {
        testPassed(testName);
    }
}

/* Test reassigning a float */
void ftest_FloatReassign() {
    struct CoreObject *co;
    strcpy(testName, "float reassign");
    strcpy(testLineHolder, "y=11349.452156789");
    handleInput(testLineHolder);
    co = getValue("y");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 0) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(co->data._fvalue != 11349.452156789) {
        testFailedWithError(testName,"Not equal to 11349.452156789");
        printf("%.9f\n\n", co->data._fvalue);
    } else {
        testPassed(testName);
    }
}

/* Test assigning a string value */
void ftest_StringAssign() {
    struct CoreObject *co;
    strcpy(testName, "string assign");
    strcpy(testLineHolder, "z=\"apples and oranges\"");
    handleInput(testLineHolder);
    co = getValue("z");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "apples and oranges")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Test assigning a string of value "null" */
void ftest_NullStringAssign() {
    struct CoreObject *co;
    strcpy(testName, "string assign");
    strcpy(testLineHolder, "z=\"apples and oranges\"");
    handleInput(testLineHolder);
    co = getValue("z");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "apples and oranges")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Test reassigning a string value */
void ftest_StringReassign() {
    struct CoreObject *co;
    strcpy(testName, "string null assign");
    strcpy(testLineHolder, "z=\"null\"");
    handleInput(testLineHolder);
    co = getValue("z");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "null")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests adding strings together */
void ftest_StringConcat() {
    struct CoreObject *co;
    strcpy(testName, "string concat");
    strcpy(testLineHolder, "b=z+\" and some extra content\"");
    handleInput(testLineHolder);
    co = getValue("b");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "null and some extra content")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests creating a string with an embedded Sys OP inside */
void ftest_StringEmbeddedOP() {
    struct CoreObject *co;
    strcpy(testName, "string embedded op");
    strcpy(testLineHolder, "y2k=\"__setGCRate(1)\"");
    handleInput(testLineHolder);
    co = getValue("y2k");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "__setGCRate(1)")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests a single quote embedded (and unescaped) inside a double quoted string is not interpreted as anything other than just another character */
void ftest_SingleQuoteInDoubleQuotes() {
    struct CoreObject *co;
    strcpy(testName, "string embedded single quote");
    strcpy(testLineHolder, "y3k=\"a string's string\"");
    handleInput(testLineHolder);
    co = getValue("y3k");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "a string's string")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests a regular single quoted string */
void ftest_SingleQuotedString() {
    struct CoreObject *co;
    strcpy(testName, "string single quoted");
    strcpy(testLineHolder, "y3k='a single quoted string'");
    handleInput(testLineHolder);
    co = getValue("y3k");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "a single quoted string")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests a double quote embedded in a single quoted string */
void ftest_DoubleQuoteInSingleQuotes() {
    struct CoreObject *co;
    strcpy(testName, "string single quote w/ embedded double quote");
    strcpy(testLineHolder, "y3k='a single quoted \" string'");
    handleInput(testLineHolder);
    co = getValue("y3k");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "a single quoted \" string")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests assigning a var to another var */
void ftest_VarToVarAssign() {
    struct CoreObject *co;
    strcpy(testName, "var to var assign");
    strcpy(testLineHolder, "z=y2k");
    handleInput(testLineHolder);
    co = getValue("z");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "__setGCRate(1)")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests assigning a bool to a var */
void ftest_BooleanAssign() {
    struct CoreObject *co;
    strcpy(testName, "boolean assign");
    strcpy(testLineHolder, "h=true");
    handleInput(testLineHolder);
    co = getValue("h");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 2) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(!co->data._bvalue) {
        testFailedWithError(testName,"Not equal to target boolean value");
        printf("%d\n\n", co->data._bvalue);
    } else {
        testPassed(testName);
    }
}

/* Tests assigning a relation to a var */
void ftest_RelationalAssign() {
    struct CoreObject *co;
    strcpy(testName, "relational assign");
    strcpy(testLineHolder, "z=(5<=5)");
    handleInput(testLineHolder);
    co = getValue("z");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 2) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(!co->data._bvalue) {
        testFailedWithError(testName,"Not equal to target boolean value");
        printf("%d\n\n", co->data._bvalue);
    } else {
        testPassed(testName);
    }
}

/* Tests printing of just a string */
void ftest_PrintString() {
    char *rez;
    strcpy(testName, "print");
    strcpy(testLineHolder, "print(\"some arbitary string to test\")");
    handleInput(testLineHolder);
    rez = Read_Batched_Write();
    if(rez == NULL) {
        testFailedWithError(testName, "Nothing written to the write buffer");
    } else if(strcmp(rez, "some arbitary string to test\n")!=0) { /* Note the \n! Always appended by default */
        testFailedWithError(testName, "Not equal to what was expected to be printed out");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests Printing of a string variable */
void ftest_PrintStringVar() {
    char *rez;
    strcpy(testName, "print string variable");
    /* Declare a string var */
    strcpy(testLineHolder, "s=\"a variable string value\"");
    handleInput(testLineHolder);
    /* Print that string var */
    strcpy(testLineHolder, "print(s)");
    handleInput(testLineHolder);
    rez = Read_Batched_Write();
    if(rez == NULL) {
        testFailedWithError(testName, "Nothing written to the write buffer");
    } else if(strcmp(rez, "a variable string value\n")!=0) { /* Note the \n! Always appended by default */
        testFailedWithError(testName, "Not equal to what was expected to be printed out");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests Printing of concatenated strings */
void ftest_PrintStringConcat() {
    char *rez;
    strcpy(testName, "print strings concatenated");
    strcpy(testLineHolder, "print(\"one set\"+\" and another\")");
    handleInput(testLineHolder);
    rez = Read_Batched_Write();
    if(rez == NULL) {
        testFailedWithError(testName, "Nothing written to the write buffer");
    } else if(strcmp(rez, "one set and another\n")!=0) { /* Note the \n! Always appended by default */
        testFailedWithError(testName, "Not equal to what was expected to be printed out");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests Printing of strings & vars */
void ftest_PrintMixedConcat() {
    char *rez;
    strcpy(testName, "print mixed concatenated");
    strcpy(testLineHolder, "print(s+\" and another\")");
    handleInput(testLineHolder);
    rez = Read_Batched_Write();
    if(rez == NULL) {
        testFailedWithError(testName, "Nothing written to the write buffer");
    } else if(strcmp(rez, "a variable string value and another\n")!=0) { /* Note the \n! Always appended by default */
        testFailedWithError(testName, "Not equal to what was expected to be printed out");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests Printing of concatenated vars */
void ftest_PrintVarConcat() {
    char *rez;
    strcpy(testName, "print variables concatenated");
    /* Declare a string var */
    strcpy(testLineHolder, "s2=\" and another variable string value\"");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(s+s2)");
    handleInput(testLineHolder);
    rez = Read_Batched_Write();
    if(rez == NULL) {
        testFailedWithError(testName, "Nothing written to the write buffer");
    } else if(strcmp(rez, "a variable string value and another variable string value\n")!=0) { /* Note the \n! Always appended by default */
        testFailedWithError(testName, "Not equal to what was expected to be printed out");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests Printing a bool */
void ftest_PrintBool() {
    char *rez;
    strcpy(testName, "print boolean variable");
    /* Declare a string var */
    strcpy(testLineHolder, "s3=true");
    handleInput(testLineHolder);
    /* Print that string var */
    strcpy(testLineHolder, "print(s3)");
    handleInput(testLineHolder);
    rez = Read_Batched_Write();
    if(rez == NULL) {
        testFailedWithError(testName, "Nothing written to the write buffer");
    } else if(strcmp(rez, "1\n")!=0) { /* Note the \n! Always appended by default */
        testFailedWithError(testName, "Not equal to what was expected to be printed out");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests printing a number variable */
void ftest_PrintNumVar() {
    char *rez;
    strcpy(testName, "print number variable");
    /* Declare a string var */
    strcpy(testLineHolder, "s4=14.04");
    handleInput(testLineHolder);
    /* Print that string var */
    strcpy(testLineHolder, "print(s4)");
    handleInput(testLineHolder);
    rez = Read_Batched_Write();
    if(rez == NULL) {
        testFailedWithError(testName, "Nothing written to the write buffer");
    } else if(strcmp(rez, "14.04\n")!=0) { /* Note the \n! Always appended by default */
        testFailedWithError(testName, "Not equal to what was expected to be printed out");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests printing a boolean, number & string all in the same call */
void ftest_PrintBoolNumStringMix() {
    char *rez;
    strcpy(testName, "print bool (interpreted as a num),num,string mix");
    /* Print that string var */
    strcpy(testLineHolder, "print(s3+s4+s2)");
    handleInput(testLineHolder);
    rez = Read_Batched_Write();
    if(rez == NULL) {
        testFailedWithError(testName, "Nothing written to the write buffer");
    } else if(strcmp(rez, "15.04 and another variable string value\n")!=0) { /* Note the \n! Always appended by default */
        testFailedWithError(testName, "Not equal to what was expected to be printed");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    /* Flush the print queue */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests for validity of various arithmetic operations */
void ftest_Arithmetic() {
    struct CoreObject *co;
    int startingFailures = failCount;
    strcpy(testName, "arithmetic tests");
    
    /* #1 */
    strcpy(testLineHolder, "a=5*(1+2)*(1+2)");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 45) {
        testFailedWithError(testName, "#1 Incorrect value where 45 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #2 */
    strcpy(testLineHolder, "a=5*(1+2)");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 15) {
        testFailedWithError(testName, "#2 Incorrect value where 15 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #3 */
    strcpy(testLineHolder, "a=5*(1+2)*3");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 45) {
        testFailedWithError(testName, "#3 Incorrect value where 45 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #4 */
    strcpy(testLineHolder, "a=(1+2) * (1-2)");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != -3) {
        testFailedWithError(testName, "#4 Incorrect value where -3 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #5 */
    strcpy(testLineHolder, "a=(1-2)*(1+2)");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != -3) {
        testFailedWithError(testName, "#5 Incorrect value where -3 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #6 */
    strcpy(testLineHolder, "a=+1 * (1+2)");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 3) {
        testFailedWithError(testName, "#6 Incorrect value where 3 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #7 */
    strcpy(testLineHolder, "a=-1*(1+2)");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != -3) {
        testFailedWithError(testName, "#7  Incorrect value where -3 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #8 */
    strcpy(testLineHolder, "a=-1--1--1");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 1) {
        testFailedWithError(testName, "#8 Incorrect value where 1 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #9 */
    strcpy(testLineHolder, "a=+1--1++1");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 3) {
        testFailedWithError(testName, "#9 Incorrect value where 3 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #10 */
    strcpy(testLineHolder, "a=+1++1++1");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 3) {
        testFailedWithError(testName, "#10 Incorrect value where 3 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #11 */
    strcpy(testLineHolder, "a=-1--1-+1");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != -1) {
        testFailedWithError(testName, "#11 Incorrect value where -1 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #12 */
    strcpy(testLineHolder, "a=((1+2)*(1+2)) * 3");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 27) {
        testFailedWithError(testName, "#12 Incorrect value where 27 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #13 */
    strcpy(testLineHolder, "a=(1+2) * (1+2) * (1+2) * (1+2)");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 81) {
        testFailedWithError(testName, "#13 Incorrect value where 81 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #14 */
    strcpy(testLineHolder, "a=(1+2) * (1+2) * (1+2)");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 27) {
        testFailedWithError(testName, "#14 Incorrect value where 27 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #15 */
    strcpy(testLineHolder, "a=(1+2) * (1+2) ");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 9) {
        testFailedWithError(testName, "#15 Incorrect value where 9 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #16 */
    strcpy(testLineHolder, "a=2 * -1");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != -2) {
        testFailedWithError(testName, "#16 Incorrect value where -2 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #17 */
    strcpy(testLineHolder, "a=(1 * 3) * -5");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != -15) {
        testFailedWithError(testName, "#17 Incorrect value where -15 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #18 */
    strcpy(testLineHolder, "a=(-1 * -3) * 5");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 15) {
        testFailedWithError(testName, "#18 Incorrect value where 15 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #19 */
    strcpy(testLineHolder, "a=(-1 * -3) * -5");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != -15) {
        testFailedWithError(testName, "#19 Incorrect value where -15 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #20 */
    strcpy(testLineHolder, "a=2 * -1 * -2");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 4) {
        testFailedWithError(testName, "#20 Incorrect value where 4 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #21 */
    strcpy(testLineHolder, "a=2 * 2 * 2");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 8) {
        testFailedWithError(testName, "#21 Incorrect value where 8 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #22 */
    strcpy(testLineHolder, "a=4 * 5 + 1 ");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 21) {
        testFailedWithError(testName, "#22 Incorrect value where 21 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #23 */
    strcpy(testLineHolder, "a=2 * 2 * 2 * 2");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 16) {
        testFailedWithError(testName, "#23 Incorrect value where 16 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #24 */
    strcpy(testLineHolder, "a=2 + 2 + 2 + 2");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 8) {
        testFailedWithError(testName, "#24 Incorrect value where 8 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #25 */
    strcpy(testLineHolder, "a=1 + 1  + 1");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 3) {
        testFailedWithError(testName, "#25 Incorrect value where 3 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #26 */
    strcpy(testLineHolder, "a=(1+2)*5");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 15) {
        testFailedWithError(testName, "#26 Incorrect value where 15 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #27 */
    strcpy(testLineHolder, "a=1+(2+3*4)+5");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 20) {
        testFailedWithError(testName, "#27 Incorrect value where 20 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #28 */
    strcpy(testLineHolder, "a=(1+2)");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 3) {
        testFailedWithError(testName, "#28 Incorrect value where 3 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #29 */
    strcpy(testLineHolder, "a=1+(2+3)");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 6) {
        testFailedWithError(testName, "#29 Incorrect value where 6 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #30 */
    strcpy(testLineHolder, "a=(1+2)*(3+4)*(5+6)");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 231) {
        testFailedWithError(testName, "#30 Incorrect value where 231 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #31 */
    strcpy(testLineHolder, "a=1+2*3^(1+2+3*1)+8");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 1467) {
        testFailedWithError(testName, "#31 Incorrect value where 1467 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    /* #32 */
    strcpy(testLineHolder, "trisquit = 1");
    handleInput(testLineHolder);
    strcpy(testLineHolder, "bacon = 1");
    handleInput(testLineHolder);
    strcpy(testLineHolder, "a= 5 + (2 * (bacon + 4 ^ (trisquit + 6)))");
    handleInput(testLineHolder);
    co = getValue("a");
    if(co == NULL) {
        testFailedWithError(testName, "No variable created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Not of correct object type");
    } else if(co->data._fvalue != 32775) {
        testFailedWithError(testName, "#32 Incorrect value where 32775 was expected");
        printf("%f\n\n", co->data._fvalue);
    }
    
    if(startingFailures == failCount) {
        testPassed(testName);
    }
}

/* Tests String Replace, single */
void ftest_StringReplace() {
    struct CoreObject *co;
    strcpy(testName, "string replace");
    strcpy(testLineHolder, "r=replace(\"some junky string, not junky!\",\"junky\",\"fantastical\",0)");
    handleInput(testLineHolder);
    co = getValue("r");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "some fantastical string, not junky!")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests String Replace, all */
void ftest_StringReplaceAll() {
    struct CoreObject *co;
    strcpy(testName, "string replace all");
    strcpy(testLineHolder, "r=replace(\"some junky string, very junky!\",\"junky\",\"fantastical\",1)");
    handleInput(testLineHolder);
    co = getValue("r");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "some fantastical string, very fantastical!")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests String Substitution */
void ftest_StringSubFull() {
    struct CoreObject *co;
    strcpy(testName, "string substitution full");
    strcpy(testLineHolder, "sub=substr(\"get this section inside here\",4,16)");
    handleInput(testLineHolder);
    co = getValue("sub");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "this section")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests String Substitution */
void ftest_StringSubShort() {
    struct CoreObject *co;
    strcpy(testName, "string substitution short");
    strcpy(testLineHolder, "sub=substr(\"get this section inside here\",4)");
    handleInput(testLineHolder);
    co = getValue("sub");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "this section inside here")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests finding a string in a string */
void ftest_StringFind() {
    struct CoreObject *co;
    strcpy(testName, "string find");
    strcpy(testLineHolder, "gh=find(\"hello world\",\"world\")");
    handleInput(testLineHolder);
    co = getValue("gh");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 0) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(co->data._fvalue != 6) {
        testFailedWithError(testName,"Not equal expected value of 6");
        printf("%g\n\n", co->data._fvalue);
    } else {
        testPassed(testName);
    }
}

/* Tests single item array initialilization */
void ftest_OneArrayElement() {
    struct CoreObject *co;
    strcpy(testName, "array create single element");
    strcpy(testLineHolder, "sub={\"item1\":\"value1\"}");
    handleInput(testLineHolder);
    co = getArrayValue("sub", "item1");
    if(co == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(strcmp(co->data._svalue, "value1")!=0) {
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests creating an array with 2 elements */
void ftest_TwoArrayElements() {
    struct CoreObject *co1,*co2;
    LATBool passed = true;
    strcpy(testName, "array create double elements");
    
    strcpy(testLineHolder, "sub={\"item1\":\"value1\",\"item2\":29}");
    handleInput(testLineHolder);
    
    co1 = getArrayValue("sub", "item1");
    co2 = getArrayValue("sub", "item2");
    
    if(co1 == NULL) {
        testFailedWithError(testName,"No CoreObject created");
        passed = false;
    } else if(co1->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
        passed = false;
    } else if(strcmp(co1->data._svalue, "value1")!=0) {
        passed = false;
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co1->data._svalue);
    }
    
    if(co2 == NULL) {
        testFailedWithError(testName,"No CoreObject created");
        passed = false;
    } else if(co2->_state != 0) {
        testFailedWithError(testName,"Not of correct data type");
        passed = false;
    } else if(co2->data._fvalue != 29) {
        passed = false;
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%g\n\n", co2->data._fvalue);
    }
    
    if(passed) {
        testPassed(testName);
    }
    
}

/* Tests creating an array with 3 elements */
void ftest_ThreeArrayElements() {
    struct CoreObject *co1,*co2,*co3;
    LATBool passed = true;
    strcpy(testName, "array create three elements");
    
    strcpy(testLineHolder, "sub={\"item1\":154,\"item2\":\"bacon-lady\",\"item3\":\"xmasdogs!-''\"}");
    handleInput(testLineHolder);
    
    co1 = getArrayValue("sub", "item1");
    co2 = getArrayValue("sub", "item2");
    co3 = getArrayValue("sub", "item3");
    
    if(co1 == NULL) {
        testFailedWithError(testName,"No CoreObject created");
        passed = false;
    } else if(co1->_state != 0) {
        testFailedWithError(testName,"Not of correct data type");
        passed = false;
    } else if(co1->data._fvalue != 154) {
        passed = false;
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%g\n\n", co1->data._fvalue);
    }
    
    if(co2 == NULL) {
        testFailedWithError(testName,"No CoreObject created");
        passed = false;
    } else if(co2->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
        passed = false;
    } else if(strcmp(co2->data._svalue, "bacon-lady") != 0) {
        passed = false;
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co2->data._svalue);
    }
    
    if(co3 == NULL) {
        testFailedWithError(testName,"No CoreObject created");
        passed = false;
    } else if(co3->_state != 1) {
        testFailedWithError(testName,"Not of correct data type");
        passed = false;
    } else if(strcmp(co3->data._svalue, "xmasdogs!-''")!=0) {
        passed = false;
        testFailedWithError(testName,"Not equal to tgt string");
        printf("%s\n\n", co3->data._svalue);
    }
    
    if(passed) {
        testPassed(testName);
    }
}

/* Tests changing one element of an array */
void ftest_ChangeArrayElement() {
    struct CoreObject *co1;
    strcpy(testName, "array modify 1 element");
    
    strcpy(testLineHolder, "sub[\"item1\"]=95");
    handleInput(testLineHolder);
    
    co1 = getArrayValue("sub", "item1");
    
    if(co1 == NULL) {
        testFailedWithError(testName,"No CoreObject created");
    } else if(co1->_state != 0) {
        testFailedWithError(testName,"Not of correct data type");
    } else if(co1->data._fvalue != 95) {
        testFailedWithError(testName, "Not equal to expected value of 95");
        printf("%g\n\n", co1->data._fvalue);
    }
}

/* Tests altering all pre-existing elements of an array */
void ftest_ChangeAllArrayElements() {
    struct CoreObject *co1,*co2,*co3;
    LATBool passed = true;
    strcpy(testName, "array modify all elements");
    
    strcpy(testLineHolder, "sub[\"item1\"]=208");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "sub[\"item2\"]=218");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "sub[\"item3\"]=398");
    handleInput(testLineHolder);
    
    co1 = getArrayValue("sub", "item1");
    co2 = getArrayValue("sub", "item2");
    co3 = getArrayValue("sub", "item3");
    
    if(co1 == NULL) {
        passed = false;
        testFailedWithError(testName,"No CoreObject created");
    } else if(co1->_state != 0) {
        passed = false;
        testFailedWithError(testName,"Not of correct data type");
    } else if(co1->data._fvalue != 208) {
        passed = false;
        testFailedWithError(testName, "Not equal to expected value of 95");
        printf("%g\n\n", co1->data._fvalue);
    }
    
    if(co2 == NULL) {
        passed = false;
        testFailedWithError(testName,"No CoreObject created");
    } else if(co2->_state != 0) {
        passed = false;
        testFailedWithError(testName,"Not of correct data type");
    } else if(co2->data._fvalue != 218) {
        passed = false;
        testFailedWithError(testName, "Not equal to expected value of 218");
        printf("%g\n\n", co2->data._fvalue);
    }
    
    if(co3 == NULL) {
        passed = false;
        testFailedWithError(testName,"No CoreObject created");
    } else if(co3->_state != 0) {
        passed = false;
        testFailedWithError(testName,"Not of correct data type");
    } else if(co3->data._fvalue != 398) {
        passed = false;
        testFailedWithError(testName, "Not equal to expected value of 398");
        printf("%g\n\n", co3->data._fvalue);
    }
    
    if(passed) {
        testPassed(testName);
    }
}

/* Tests File IO */
void ftest_FileIO() {
    char *rez;
    int lastCount = failCount;
    strcpy(testName, "file io");
    
    strcpy(testLineHolder, "x=open(\"latria-test-file-for-functional-testing.txt\",\"w\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "write(x,\"line1\")");
    handleInput(testLineHolder);
    strcpy(testLineHolder, "write(x,\"null\")");
    handleInput(testLineHolder);
    strcpy(testLineHolder, "write(x,\"line3\")");
    handleInput(testLineHolder);
    strcpy(testLineHolder, "write(x,\"line4\")");
    handleInput(testLineHolder);
    strcpy(testLineHolder, "write(x,\"final line, goodbye\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "close(x)");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "x=open(\"latria-test-file-for-functional-testing.txt\",\"r\")");
    handleInput(testLineHolder);

    strcpy(testLineHolder, "line=read(x)");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "while(line!=null){");
    handleInput(testLineHolder);
    strcpy(testLineHolder, "print(\">\"+line)");
    handleInput(testLineHolder);
    strcpy(testLineHolder, "line=read(x)");
    handleInput(testLineHolder);
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "close(x)");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "remove(\"latria-test-file-for-functional-testing.txt\")");
    handleInput(testLineHolder);
    
    
    rez = Read_Batched_Write();
    
    if(LAT_STRCMP(rez, ">line1\n>null\n>line3\n>line4\n>final line, goodbye\n") != 0) {
        testFailedWithError(testName, "Did not read expected file contents");
        printf("%s\n", rez);
        
    }
    
    /* Flush the queue */
    Flush_Batched_Write_NO_PRINT();
    
    if(lastCount == failCount) {
        testPassed(testName);
    }
}

/* Tests nested relational ops */
void ftest_NestedRelationals() {
    char *rez;
    strcpy(testName, "nested relationals");
    
    strcpy(testLineHolder, "if(1==1&&(2==2&&3==3)){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"nested success\")");
    handleInput(testLineHolder);
    
    /* print 'c' forces our compiler to realize there is no extension of elseif,else & to compute NOW */
    strcpy(testLineHolder, "}print('c')");
    handleInput(testLineHolder);
    
    rez = Read_Batched_Write();
    if(LAT_STRCMP(rez, "nested success\nc\n") != 0) {
        testFailedWithError(testName, "Did not execute nested relational op code properly");
    } else {
        testPassed(testName);
    }
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests Compound relational ops */
void ftest_CompoundRelationals() {
    char *rez;
    strcpy(testName, "compound relationals");
    
    strcpy(testLineHolder, "if(1==2&&2==0||4!=3){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"compound success\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}print('c')");
    handleInput(testLineHolder);
    
    rez = Read_Batched_Write();
    if(LAT_STRCMP(rez, "compound success\nc\n") != 0) {
        testFailedWithError(testName, "Did not execute compound relational op code properly");
    } else {
        testPassed(testName);
    }
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests a SINGLE passed item as a relational call (boolean) */
void ftest_SingleRelational() {
    char *rez;
    strcpy(testName, "single true relational");
    
    strcpy(testLineHolder, "x=true");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "if(x == true){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"single true success\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}print('c')");
    handleInput(testLineHolder);
    
    rez = Read_Batched_Write();
    if(LAT_STRCMP(rez, "single true success\nc\n") != 0) {
        testFailedWithError(testName, "Did not execute single true relational properly");
    } else {
        testPassed(testName);
    }
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
    
    strcpy(testName, "single false relational");
    
    strcpy(testLineHolder, "x=false");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "if(!x){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"single false success\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}print('c')");
    handleInput(testLineHolder);
    
    rez = Read_Batched_Write();
    if(LAT_STRCMP(rez, "single false success\nc\n") != 0) {
        testFailedWithError(testName, "Did not execute single false relational properly");
    } else {
        testPassed(testName);
    }
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests various nested control flows */
void ftest_NestedControlFlows() {
    char *rez;
    char *checkAgainst = "f0:0:0\nf0:0:1\nf0:0:2\nresetting...\n0<\n1<\n2<\n3<\nf0:1:0\nf0:1:1\nf0:1:2\nresetting...\n0<\n1<\n2<\n3<\nf0:2:0\nf0:2:1\nf0:2:2\nresetting...\n0<\n1<\n2<\n3<\nf1:0:0\nf1:0:1\nf1:0:2\nresetting...\n0<\n1<\n2<\n3<\nf1:1:0\nf1:1:1\nf1:1:2\nresetting...\n0<\n1<\n2<\n3<\nf1:2:0\nf1:2:1\nf1:2:2\nresetting...\n0<\n1<\n2<\n3<\nf2:0:0\nx is now 2:2\nf2:0:1\nx is now 2:2\nf2:0:2\nresetting...\n0<\n1<\n2<\n3<\nf2:1:0\nx is now 2:2\nf2:1:1\nx is now 2:2\nf2:1:2\nresetting...\n0<\n1<\n2<\n3<\nf2:2:0\nx is now 2:2\nf2:2:1\nx is now 2:2\nf2:2:2\nresetting...\n0<\n1<\n2<\n3<\n";
    strcpy(testName, "nested control flows");
    
    strcpy(testLineHolder, "for(x=0,x<=2,x=x+1){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "for(y=0,y<=2,y=y+1){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "for(t=0,t<=2,t=t+1){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"f\"+x+\":\"+y+\":\"+t)");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "if(t==2){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"resetting...\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "m=0");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "while(m<4){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(m+\"<\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "m=m+1");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}elseif(x==2){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"x is now 2:\"+x)");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    rez = Read_Batched_Write();
    if(strcmp(rez, checkAgainst) != 0) {
        testFailedWithError(testName, "Did not execute nested control flows properly");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests Basic While Loops */
void ftest_BasicWhile() {
    char *rez;
    strcpy(testName, "basic while");
    
    strcpy(testLineHolder, "x=0");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "while(x<10){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "x=x+1");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(x)");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    rez = Read_Batched_Write();
    
    if(LAT_STRCMP(rez, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n") != 0) {
        testFailedWithError(testName, "Basic while did not run properly");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    
    Flush_Batched_Write_NO_PRINT();
}

/* Tests Basic For Loops */
void ftest_BasicFor() {
    char *rez;
    strcpy(testName, "basic for");
    
    strcpy(testLineHolder, "for(k=0,k<=10,k=k+1){");
    handleInput(testLineHolder);

    strcpy(testLineHolder, "print(\"K:\"+k)");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}print(\"done with k\")");
    handleInput(testLineHolder);
    
    rez = Read_Batched_Write();
    
    if(LAT_STRCMP(rez, "K:0\nK:1\nK:2\nK:3\nK:4\nK:5\nK:6\nK:7\nK:8\nK:9\nK:10\ndone with k\n") != 0) {
        testFailedWithError(testName, "Output did not match as expected");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    
    Flush_Batched_Write_NO_PRINT();
}

/* Tests if, to elseif to else */
void ftest_ChainedIfs() {
    char *rez;
    strcpy(testName, "chained ifs");
    
    strcpy(testLineHolder, "if(2!=2){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"invalid statement\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}elseif(19.2!=19.2){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "bn=5*2+4");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"2nd works!\"+bn)");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}else{");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"third & final\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    rez = Read_Batched_Write();
    if(LAT_STRCMP(rez, "third & final\n") != 0) {
        testFailedWithError(testName, "Did not execute chained ifs properly");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests nested if control flows */
void ftest_NestedIfs() {
    char *rez;
    strcpy(testName, "nested ifs");
    
    strcpy(testLineHolder, "if(5==5){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "if(4!=3){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "if(3>2){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"nested ifs good\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}print('c')");
    handleInput(testLineHolder);
    
    rez = Read_Batched_Write();
    if(LAT_STRCMP(rez, "nested ifs good\nc\n") != 0) {
        testFailedWithError(testName, "Did not execute nested ifs properly");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests nested while control flows */
void ftest_NestedWhiles() {
    char *rez;
    char *checkAgainst = "w0:0:0\nw0:0:1\nw0:0:2\nw0:1:0\nw0:1:1\nw0:1:2\nw0:2:0\nw0:2:1\nw0:2:2\nw1:0:0\nw1:0:1\nw1:0:2\nw1:1:0\nw1:1:1\nw1:1:2\nw1:2:0\nw1:2:1\nw1:2:2\nw2:0:0\nw2:0:1\nw2:0:2\nw2:1:0\nw2:1:1\nw2:1:2\nw2:2:0\nw2:2:1\nw2:2:2\n";
    strcpy(testName, "nested while flows");
    
    strcpy(testLineHolder, "x=0");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "while(x<3){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "y=0");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "while(y<3){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "z=0");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "while(z<3){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"w\"+x+\":\"+y+\":\"+z)");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "z=z+1");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "y=y+1");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "x=x+1");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);

    rez = Read_Batched_Write();
    if(strcmp(rez, checkAgainst) != 0) {
        testFailedWithError(testName, "Did not execute nested whiles properly");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    
    /* Flush */
    Flush_Batched_Write_NO_PRINT();
}

/* Tests simple OS Execution with echo */
void ftest_ExecSimpleEcho() {
    struct CoreObject *co;
    strcpy(testName, "exec simple");
    
    strcpy(testLineHolder, "    v ## ");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "    echo echoed this");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "    #>");
    handleInput(testLineHolder);
    
    co = getValue("v");
    if(co == NULL) {
        testFailedWithError(testName, "No Core Object created or found");
    } else if(co->_state != 1) {
        testFailedWithError(testName, "Wrong data type");
    } else if(LAT_STRCMP(co->data._svalue, "echoed this\n")) {
        testFailedWithError(testName, "Incorrect result value");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests OS Execution with interpolation of values */
void ftest_ExecInterpolation() {
    struct CoreObject *co;
    strcpy(testName, "exec interpolation");
    
    strcpy(testLineHolder, "g##");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "echo we last echoed:IN{v}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "#>");
    handleInput(testLineHolder);
    
    co = getValue("g");
    if(co == NULL) {
        testFailedWithError(testName, "No Core Object created or found");
    } else if(co->_state != 1) {
        testFailedWithError(testName, "Wrong data type");
    } else if(LAT_STRCMP(co->data._svalue, "we last echoed:echoed this\n")) {
        testFailedWithError(testName, "Incorrect result value");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests OS Execution across multiple lines with interpolation of values */
void ftest_ExecMultiLine() {
    struct CoreObject *co;
    strcpy(testName, "multi-line exec with interpolation");
    
    strcpy(testLineHolder, "g##");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "echo tesla nikola1 some very long");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "echo tesla nikola2 also very long");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "echo tesla nikola3 additionally long");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "echo tesla nikola4 quite long");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "echo IN{v}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "echo eggs");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "#>");
    handleInput(testLineHolder);
    
    co = getValue("g");
    if(co == NULL) {
        testFailedWithError(testName, "No Core Object created or found");
    } else if(co->_state != 1) {
        testFailedWithError(testName, "Wrong data type");
        printf("%d\n\n", co->_state);
    } else if(LAT_STRCMP(co->data._svalue, "tesla nikola1 some very long\ntesla nikola2 also very long\ntesla nikola3 additionally long\ntesla nikola4 quite long\nechoed this\neggs\n")) {
        testFailedWithError(testName, "Incorrect result value");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests basic function declaration */
void ftest_basicFunction() {
    char *rez;
    strcpy(testName, "basic function");
    
    strcpy(testLineHolder, "@arrowPrint(x){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\">\"+x)");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "arrowPrint(\"man -oh+ man\")");
    handleInput(testLineHolder);
    
    rez = Read_Batched_Write();
    
    if(LAT_STRCMP(rez, ">man -oh+ man\n")!=0) {
        testFailedWithError(testName, "Incorrect result value");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    
    Flush_Batched_Write_NO_PRINT();
}

/* Tests a function that concatenates it's args and returns them */
void ftest_concatInputFunction() {
    struct CoreObject *co;
    strcpy(testName, "concat function");
    
    strcpy(testLineHolder, "@concat(x,y){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return x+y");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "g=concat(\"jail \",\"beans\")");
    handleInput(testLineHolder);
    
    co = getValue("g");
    if(co == NULL) {
        testFailedWithError(testName, "No Core Object created");
    } else if(co->_state != 1) {
        testFailedWithError(testName, "Wrong data type");
    } else if(LAT_STRCMP(co->data._svalue,"jail beans") != 0) {
        testFailedWithError(testName, "Did not match expected output");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests functions calling functions in functions */
void ftest_chainedFunctions() {
    char *rez;
    strcpy(testName, "chained,nested & one-liner functions");
    
    strcpy(testLineHolder, "@test(){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "g=bacon(\"()\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(g)");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return finalFunc()");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "@bacon(i){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return \"bacon, what did you expect?\"");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "@finalFunc(){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "@innerFunc(){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"inner function walkie talkie!\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "innerFunc()");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "@oneliner(){print(\"one liners are pretty valid\")}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "oneliner()");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "@hmm(){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(\"hmm, is there something ahead of me?\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}print(\"Yes! I'm right 'ahead' of you, after the closing }\")");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "hmm()");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return \"this is final\"");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "b=test()");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print(b)");
    handleInput(testLineHolder);
    
    rez = Read_Batched_Write();
    
    if(LAT_STRCMP(rez, "bacon, what did you expect?\ninner function walkie talkie!\none liners are pretty valid\nYes! I'm right 'ahead' of you, after the closing }\nhmm, is there something ahead of me?\nthis is final\n") != 0) {
        testFailedWithError(testName, "chained functions did not execute as expected");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    
    Flush_Batched_Write_NO_PRINT();
}

/* Tests returning a number from a function */
void ftest_returnNumberFunction() {
    struct CoreObject *co;
    strcpy(testName, "square number function");
    
    strcpy(testLineHolder, "@square(x){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return x*x");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "h=square(5)");
    handleInput(testLineHolder);
    
    co=getValue("h");
    
    if(co == NULL) {
        testFailedWithError(testName, "No Core Object created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Incorrect data type");
    } else if(co->data._fvalue != 25) {
        testFailedWithError(testName, "Wrong value returned");
        printf("%g\n\n", co->data._fvalue);
    } else {
        testPassed(testName);
    }
}

/* Tests returning a boolean from a function */
void ftest_returnBooleanFunction() {
    struct CoreObject *co;
    strcpy(testName, "true boolean function");
    
    strcpy(testLineHolder, "@getTrue(){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return true");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "h=getTrue()");
    handleInput(testLineHolder);
    
    co=getValue("h");
    
    if(co == NULL) {
        testFailedWithError(testName, "No Core Object created");
    } else if(co->_state != 2) {
        testFailedWithError(testName, "Incorrect data type");
        printf("%d\n\n", co->_state);
    } else if(!co->data._bvalue) {
        testFailedWithError(testName, "Wrong value returned");
        printf("%d\n\n", co->data._bvalue);
    } else {
        testPassed(testName);
    }
    
    strcpy(testName, "false boolean function");
    
    strcpy(testLineHolder, "@getFalse(){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return false");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "h=getFalse()");
    handleInput(testLineHolder);
    
    co=getValue("h");
    
    if(co == NULL) {
        testFailedWithError(testName, "No Core Object created");
    } else if(co->_state != 2) {
        testFailedWithError(testName, "Incorrect data type");
        printf("%d\n\n", co->_state);
    } else if(co->data._bvalue) {
        testFailedWithError(testName, "Wrong value returned");
        printf("%d\n\n", co->data._bvalue);
    } else {
        testPassed(testName);
    }
}

/* Tests returning an array from a function */
void ftest_returnArrayFunction() {
    struct CoreObject *co;
    strcpy(testName, "array function");
    
    strcpy(testLineHolder, "@tinyarray(){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "x={\"data1\":\"value1\"}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return x");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "h=tinyarray()");
    handleInput(testLineHolder);
    
    co=getArrayValue("h", "data1");
    
    if(co == NULL) {
        testFailedWithError(testName, "No Core Object created");
    } else if(co->_state != 1) {
        testFailedWithError(testName, "Incorrect data type");
        printf("%d\n\n", co->_state);
    } else if(LAT_STRCMP(co->data._svalue, "value1") != 0) {
        testFailedWithError(testName, "Wrong value returned");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests returning a string from a function */
void ftest_returnStringFunction() {
    struct CoreObject *co;
    strcpy(testName, "string function");
    
    strcpy(testLineHolder, "@getstring(){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return \"a latria string\"");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "h=getstring()");
    handleInput(testLineHolder);
    
    co = getValue("h");
    
    if(co == NULL) {
        testFailedWithError(testName, "No Core Object created");
    } else if(co->_state != 1) {
        testFailedWithError(testName, "Incorrect data type");
    } else if(LAT_STRCMP(co->data._svalue, "a latria string") != 0) {
        testFailedWithError(testName, "Wrong value returned");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests returning concatenated strings from a function (with an embedded single quote) */
void ftest_returnStringsConcatenated() {
    struct CoreObject *co;
    strcpy(testName, "string concat function");
    
    strcpy(testLineHolder, "@concatstrings(){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return \"bean's\" + \" and tortillas\"");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "q=concatstrings()");
    handleInput(testLineHolder);
    
    co = getValue("q");
    
    if(co == NULL) {
        testFailedWithError(testName, "No Core Object created");
    } else if(co->_state != 1) {
        testFailedWithError(testName, "Incorrect data type");
    } else if(LAT_STRCMP(co->data._svalue, "bean's and tortillas") != 0) {
        testFailedWithError(testName, "Wrong value returned");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests returning a var from a function */
void ftest_returnVarFunction() {
    struct CoreObject *co;
    strcpy(testName, "variable returning function");
    
    strcpy(testLineHolder, "@getstring(){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "x=\"another latria string\"");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return x");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "h=getstring()");
    handleInput(testLineHolder);
    
    co = getValue("h");
    
    if(co == NULL) {
        testFailedWithError(testName, "No Core Object created");
    } else if(co->_state != 1) {
        testFailedWithError(testName, "Incorrect data type");
    } else if(LAT_STRCMP(co->data._svalue, "another latria string") != 0) {
        testFailedWithError(testName, "Wrong value returned");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

/* Tests returning a function from a function (the value of course) */
void ftest_returnFunctionFromFunction() {
    struct CoreObject *co;
    strcpy(testName, "function returning function");
    
    strcpy(testLineHolder, "@funcfunc(){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return funcinfunc()");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "@funcinfunc(){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return \"a value from inside the madness!\"");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "h=funcfunc()");
    handleInput(testLineHolder);
    
    /* I fixed up this function already, but it's important to note that 'return func()' where func() is defined in the enclosing function will FAIL, as the enclosing scope (with func) will have been removed BEFORE it is called */
    
    co = getValue("h");
    
    if(co == NULL) {
        testFailedWithError(testName, "No Core Object created");
    } else if(co->_state != 1) {
        testFailedWithError(testName, "Incorrect data type");
    } else if(LAT_STRCMP(co->data._svalue, "a value from inside the madness!") != 0) {
        testFailedWithError(testName, "Wrong value returned");
        printf("%s\n\n", co->data._svalue);
    } else {
        testPassed(testName);
    }
}

void ftest_returnEarlyFromFunction() {
    char *rez;
    strcpy(testName, "function returning early");
    
    strcpy(testLineHolder, "@early(){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print('first')");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "print('second')");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "early()");
    handleInput(testLineHolder);
    
    rez = Read_Batched_Write();
    
    if(LAT_STRCMP(rez, "first\n") != 0) {
        testFailedWithError(testName, "function returning early did not run as expected");
        printf("%s\n\n", rez);
    } else {
        testPassed(testName);
    }
    
    Flush_Batched_Write_NO_PRINT();
}

/* Tests recursive functions */
void ftest_recursiveFunction() {
    struct CoreObject *co;
    strcpy(testName, "recursive function");
    
    strcpy(testLineHolder, "@recursive(x){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "if(x<10){");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return recursive(x+1)");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "return x");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "}");
    handleInput(testLineHolder);
    
    strcpy(testLineHolder, "h=recursive(0)");
    handleInput(testLineHolder);

    co = getValue("h");
    
    if(co == NULL) {
        testFailedWithError(testName, "No Core Object created");
    } else if(co->_state != 0) {
        testFailedWithError(testName, "Incorrect data type");
        printf("%d\n\n", co->_state);
    } else if(co->data._fvalue != 10) {
        testFailedWithError(testName, "Wrong value returned");
        printf("%g\n\n", co->data._fvalue);
    } else {
        testPassed(testName);
    }
}

/* END FUNCTIONAL TESTS */

/* Functional test failure */
void testFailedWithError(char *name,char *er) {
#ifdef _WIN32
    printf(KRED "\nX%s %s Failed with Error: %s\n", RESET, name, er);
#else
    printf(KRED "\n✘%s %s Failed with Error: %s\n", RESET, name, er);
#endif
    failCount+=1;
}

/* Unit test failure */
void utestFailedWithError(char *name,char *er) {
#ifdef _WIN32
    printf(KRED "\nX%s %s Failed with Error: %s\n\n", RESET, name, er);
#else
    printf(KRED "\n✘%s %s Failed with Error: %s\n\n", RESET, name, er);
#endif
    failCount+=1;
}

/* Generic test pass */
void testPassed(char *testname) {
#ifdef _WIN32
    printf(KGRN "-%s %s passed\n", RESET, testname);
#else
    printf(KGRN "✔%s %s passed\n", RESET, testname);
#endif
}
