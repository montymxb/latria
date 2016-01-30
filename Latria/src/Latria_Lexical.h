/*
The MIT License (MIT)

Copyright (c) 2016 Benjamin Wilson Friedman

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
 *  Latria (Atria Lang)
 */

#ifndef __Latria_Lexical__
#define __Latria_Lexical__

#include "Latria_Math.h"

char * executeLang(char *xlangSource);
char * fetchXDeclaration(char *input);
LATBool checkForReturnValue(char *input);
void stripOutUnwantedSymbols(char *input);
void setVarFromRegister(char *varName, unsigned char registerNum);
void setArrayVarFromRegister(char *varName, char *key, unsigned char registerNum);

/*** NEW VM FUNCTIONS ****/

void runInstructions();

/** Forwardly declared from Latria.h **/
void executeLatriaFile(char *fileName);

/* Functions for reading and writing from the file as we read */
int getCurrentChar();
int getCharByOffsetFromCurrent(int offset);
void moveToNextChar();
void resetCurrentFileIndex(long int newIndex);
long int getCurrentFileIndex();

#ifdef INCLUDECOMPILER
void openDynamicInterpreterFile();
void closeDynamicInterpreterFile();
int handleInput(char *input);
#endif

#endif /* defined(__Latria_Lexical__) */
