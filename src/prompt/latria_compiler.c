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
 *  latria_compiler.c
 *  Created by Benjamin Friedman on 11/3/15.
 */

#include "latria_compiler.h"

int main( int argc, char* argv[]) {
    int argI;
    
    /* Iterate over ARGS */
    if(argc > 1) {
        
        /* Potential args were passed, validate and run what we find */
        for(argI = 1; argI < argc; argI++) {
            
            /* Extract this arg */
            char *arg = argv[argI];
            
            if(*arg == '-') {
                
                /* It's a command line arg, parse it */
                if(strlen(arg) > 0) {
                    
                    switch(*(arg+1)) {
                            
                        case 'v':
                            /* Print Version option */
                            printf("\nLatria Compiler v%s\n\n", LATRIA_VERSION_NUMBER);
                            break;
                            
                        case 'h':
                            /* Print help */
                            printf("\n:: Help ::\n\n");
                            printf("usage:\n");
                            printf("latriac test.lra (creates a test.lrac file)\n");
                            printf("\n\n");
                            printf("-v    Prints out the version number\n");
                            printf("-h    Prints out the help (this)\n");
                            printf("\n\n");
                            break;
                            
                        default:
                            /* Unrecognized command line arg */
                            printf("\n:>> Unrecognized command line arg passed: %s\n\n",arg);
                            break;
                            
                    }
                } else {
                    
                    /* Empty command line arg passed */
                    printf("\nPassed an empty '-' with no parameter after it\n\n");
                    exit(1);
                    
                }
            } else {
                
                /* Attempt to compile passed in file */
                compileLatria(arg);
            }
        }
    } else {
        
        /* No args passed, prompt user to run with stuff passed in */
        printf("\n\nusage:\n");
        printf("latriac test.lra (creates a test.lrac file)\n\n");
    }
    
    return 0;
}
