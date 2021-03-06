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
 *  latria.h
 *  Created 12/9/14
 */

#ifndef latria_prompt_latria_h
#define latria_prompt_latria_h

/* Standard IO Layer of Latria */
#include "core/latria_core.h"
#include "core/latria_referencestack.h"
#include "core/latria_opcodes.h"
#include "core/latria_sys.h"
#include "core/latria_jumps.h"

#include "memory/latria_gc.h"

#include "regex/latria_regex.h"

#if defined(INCLUDECOMPILER) || defined(COMPILER_ONLY)
/* Built in compiler interface */
#include "compiler/latria_compiler_interface.h"

/* Only usable if the compiler is included */
void openDynamicInterpreterFile();
void closeDynamicInterpreterFile();
int handleInput(char *input);

#endif

#ifdef LAT_TESTS
/* Built in test module */
#include "tests/latria_tests.h"
#endif

void freeLatria();

void executeLatriaFile(char *fileName);

#endif /* latria_prompt_latria_h */
