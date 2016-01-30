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
 *  Created 12/9/14
 *
 */

#include "Latria.h"

/* Current latria version */
#define LATRIA_VERSION_NUMBER "0.1.1"

/* Fixed lenght of data we can read at a time in */
#define LAT_INPUT_SIZE 1024

/* exit char array (for dynamic interpreter) */
char exitVal[]      = "exit";

/* The current file we are executing */
FILE *runFile       = NULL;

/* The stack of files we are executing (each call to load("file.lrac") pushes an additional file onto this stack) */
FILE *fileStack[20] = {NULL};

/* The current index of the file we are reading from the file stack above */
unsigned char fileStackIndex = 0;

#pragma message("we have an issue passing functions to other functions (rather than assigning to a var and passing that) does NOT work at the moment. Need to make sure functions can be properly recognized as inputs to other functions (most likely will end up in requiring extra registers)")

/* If LATRIA_EMBEDDED is defined main will be omitted, for embedded implementations */
#ifndef LATRIA_EMBEDDED
int main( int argc, char* argv[]) {
    
    int argI;
    char input[LAT_INPUT_SIZE];
    char shouldCompile = 0;
    
    /* Set free callback on exit for mac & linux */
    #if defined(MACOSX) || defined(LINUXOS)
    atexit(freeLatria);
    #endif
    
    /* Builds LATRIA VM */
    createLatriaVM();
    
    /* Prepares global namespace */
    stack_pushStack();
    
    /* Iterate over ARGS */
    if(argc > 1) {
        
        /* Potential args were passed, validate and run what we find */
        for(argI = 1; argI < argc; argI++) {
            
            /* Extract this arg */
            char *arg = argv[argI];
            
            /* Check for the type of this arg */
            if(*arg == '-') {
                
                /* A potential command line arg, check to see if there is more to it */
                if(strlen(arg+1) > 0) {
                    
                    /* iterate over the argument character */
                    switch(*(arg+1)) {
                            
                        #ifdef INCLUDECOMPILER
                        /* Compile only option */
                        case 'c':
                            
                            /* Verify we have additional input to handle */
                            if(argI == argc-1) {
                                
                                /* Nothing passed, this must be an error, exit */
                                printf("\n\nNothing passed to compile!\n\n");
                                exit(1);
                                
                            }
                            
                            /* indicate we should compile only */
                            shouldCompile = 1;
                            break;
                        #endif
                            
                        /* Run in Optimized Mode */
                        case 'o':
                            
                            /* Indicates that latria should batch return output, which can significantly improve performance */
                            setPrintCacheMode(1);
                            break;
                            
                        /* Print Version */
                        case 'v':
                            
                            printf("\nLatria v%s\n\n", LATRIA_VERSION_NUMBER);
                            break;
                            
                        /* Print help */
                        case 'h':
                            
                            printf("\n:: Help ::\n\n");
                            printf("usage:\n");
                            printf("latria [-cohtv] test.lra\n");
                            printf("\n");
                            printf("Calling 'latria' by itself will invoke a dynamic interpreter\n");
                            printf("\n");
                            #ifdef INCLUDECOMPILER
                            printf("-c    Only compiles the passed in files\n");
                            #endif
                            printf("-o    Runs the VM in an optimized mode, printing is cached and so output may not show up until the queue is filled or the program has finished\n");
                            printf("-v    Prints out the version number\n");
                            printf("-h    Prints out the help (this)\n");
                            printf("-t    Runs functional and unit tests");
                            #if defined(LAT_TESTS)
                            printf(" (%scurrently compiled in debug mode and available%s)\n",KGRN,RESET);
                            #else
                            printf(" (%snot available%s, recompile latria in debug mode to have access to tests)\n",KRED,RESET);
                            #endif
                            printf("\n\n");
                            break;
                            
                        #if defined(LAT_TESTS)
                        /* Runs internal unit/functional tests to verify integrity of build */
                        case 't':
                            
                            /*Turn print caching on, required for testing */
                            setPrintCacheMode(1);
                            /* Run all Test Cases */
                            runAllTests();
                            break;
                        #endif
                            
                        /* Unrecognized command argument */
                        default:
                            
                            printf("\n:>> Unrecognized command line arg passed: %s\n\n",arg);
                            break;
                            
                    }
                } else {
                    
                    /* Empty command line arg passed */
                    printf("\nPassed an empty '-' with no parameter after it\n\n");
                    exit(1);
                    
                }
            } else {
                
                /* Potential File Name passed */
                if(shouldCompile == 1) {
                    
                    #ifdef INCLUDECOMPILER
                    /* Just compile */
                    compileLatria(arg);
                    #else
                    printf("\n\nCannot compile this file! Recompile latria with the compiler embedded to be able to do this.\n\n");
                    #endif
                    
                } else {
                    
                    /* Run the file (compiles if capable and necessary as well) */
                    executeLatriaFile(arg);
                    
                }
            }
        }
    }
    #ifdef INCLUDECOMPILER
    else {
        
        /* No args passed, run interactive prompt */
        
        /* Set to optimized mode to batch output, we will be reading it manually */
        setPrintCacheMode(1);
        
        /* Copyright symbol not available in the windows command prompt, check to define as empty */
        #ifndef _WIN32
        #define COPYRIGHT_SYMBOL "©"
        #else
        #define COPYRIGHT_SYMBOL ""
        #endif
        
        /* Print copyright statement */
        printf("\nLatria (Lang Atria) \nv%s\nCopyright %s Benjamin Wilson Friedman - 2016\n\n\n", LATRIA_VERSION_NUMBER, COPYRIGHT_SYMBOL);
        
        /* Open up a temporary file for handling our interpreter input */
        openDynamicInterpreterFile();
        
        /* Loop over user input */
        while(true) {
            
            char *p = NULL;
            int rez = 0;
            
            printf("~~{");
            
            /* Using multiple scan f's (to compile on ARM specifically) */
            /* Specifically using a negating character to take everything UP to \n into our 'input' */
            rez = scanf(" %[^\n]", input);
            
            /* Check for EOF */
            if(rez == EOF) {
                
                /* Break our loop */
                break;
            }
            
            /* Eat up any leftovers */
            rez = scanf("%*c");
            
            /* Check for EOF */
            if(rez == EOF) {
                
                /* break */
                break;
            }
            
            /* Assign our array to our pointer */
            p = input;
            
            /* Check if we have an empty input, skip if we do */
            if(!*input) {
                continue;
            }
            
            /* Compile and run our input */
            rez = handleInput(p);
            
            /* Flush to output for prompt mode */
            Flush_Batched_Write();
            
            /* Check if we got a signal to finish executing */
            if(rez == 1) {
                
                /* We have finished, break out */
                break;
            }
        }
        
        /* Close our dynamic interpreter file now that we are done */
        closeDynamicInterpreterFile();
    }
    
    #else
    else {
        
        /* Dynamic interpreter not available without the compiler, print a generic message */
        printf("\n\nNothing provided, try latria -h for help\n\n");
    }
    #endif
    
    /* Always flush output at end */
    Flush_Batched_Write();
    
    /* Free only when NOT defined, as atexit() is set to call this automatically for Mac & Linux */
    #if !defined(MACOSX) && !defined(LINUXOS)
    freeLatria();
    #else
    /* free our regex cache preemptively */
    freeRegexCache();
    #endif
    
    return 0;
}
#endif


/* Runs a passed in latria file, and if need be attempts to compile it beforehand */
void executeLatriaFile(char *fileName) {
    
    /* Get file name */
    char *newFileName = NULL;
    
    #ifdef INCLUDECOMPILER
    /* Check if this is a compiled or non-compiled file */
    if(*(fileName + strlen(fileName)-1) != 'c') {
        
        /* File name is NOT compiled, compile it first */
        size_t len = strlen(fileName);
        
        /* Compile the provided file */
        compileLatria(fileName);
        
        /* Create our output file name */
        newFileName = malloc(len+2);
        
        /* Copy the file name we were given */
        strncpy( newFileName, fileName, len);
        
        /* Append 'c' to the end and \0 (compiled files end in .lrac) */
        newFileName[len]    = 'c';
        newFileName[len+1]  = '\0';
        
        /* Swap our fileName with our newly compiled file name */
        fileName = newFileName;
    }
    #endif
    
    /* Check if we are already executing a file */
    if(runFile != NULL) {
        
        /* Already have an open file, push this one on the run stack and continue */
        fileStack[fileStackIndex] = runFile;
        
        /* Increment our file stack location */
        fileStackIndex++;
        
    }
    
    /* Open file in Read mode */
    runFile = fopen( fileName, "r");
    
    /* Check if we created a compiled file */
    if(newFileName != NULL) {
        
        /* We did, free the name we generated */
        free(newFileName);
    }
    
    /* Validate the file was opened */
    if(runFile == NULL) {
        
        /* Invalid filename, error out */
        printf("\nThe file you provided, %s, could not be opened!\n\n", fileName);
        exit(1);
        
    }
    
    /* Call the VM to run over the given instruction set */
    runInstructions();
    
    /* Close the file */
    fclose(runFile), runFile = NULL;
    
    /* Check to pop existing file from the stack */
    if(fileStackIndex > 0) {
        
        /* Pop current file from the stack and continue */
        fileStackIndex--;
        runFile = fileStack[fileStackIndex];
        
    }
    
}


#ifdef INCLUDECOMPILER


/* Opens up a file to work with dynamic interpretation */
void openDynamicInterpreterFile() {
    
    /* Check if we are already executing a file */
    if(runFile != NULL) {
        
        /* Already have an open file, push this on the run stack and continue */
        fileStack[fileStackIndex] = runFile;
        fileStackIndex++;
        
    }
    
    /* Open up a temporary file as our run file */
    runFile = tmpfile();
}


/* Closes a file opened up for dynamic interpretation */
void closeDynamicInterpreterFile() {
    
    /* Check if we have an open file */
    if(runFile != NULL) {
        
        /* Close our file */
        fclose(runFile), runFile = NULL;
    }
    
    /* Check to pop an existing file from the stack */
    if(fileStackIndex > 0) {
        
        /* Pop existing file from the stack and continue */
        fileStackIndex--;
        runFile = fileStack[fileStackIndex];
        
    }
}

/* Locked reference to where we last executed bytecodes in the current file */
long frozenInterpreterIndex = 0;


/* Handles a line of non-compiled latria code */
int handleInput(char *input) {
    
    unsigned char didPopJumpUpdates=0;
    short retSize;
    JumpUpdate *ju;
    
    /* Mark our current position in the current file, before we start advancing */
    long priorFileIndex = ftell(runFile);
    
    /* Compile this line */
    compileLine(input);
    
    /* Check if we read anything */
    if((retSize = getByteCodeCount()) > 0) {
        
        /* We have bytecodes, read them out and into our current runFile */
        fwrite(readByteCodes(), sizeof(unsigned char), (size_t)retSize, runFile);
        
    }
    
    /* Iterate over all pending jump updates */
    while((ju = popJumpUpdate()) != NULL) {
        
        /* Create an array to hold our new jump instruction address */
        char jumpCode[LAT_ADDRESS_SIZE+1] = {0};
        
        /* Seek to the indicated address to update */
        fseek( runFile, ju->bytecodeAddr, SEEK_SET);
        
        /* Convert our new jump address into a hexcode */
        sprintf(jumpCode, LAT_ADDRESS_FORMAT_STRING, ju->jumpAddr);
        
        /* Update the address we are currently at with our new hex one */
        fwrite(jumpCode, sizeof(unsigned char), LAT_ADDRESS_SIZE, runFile);
        
        /* Indicate we have handled at least 1 jump update */
        didPopJumpUpdates = 1;
        
    }
    
    /* Check if we handled any jump updates */
    if(didPopJumpUpdates == 1) {
        
        /* We have recently updated jumps, reset to the end of this file (protects else,elseif from dying) */
        fseek( runFile, 0, SEEK_END);
        
    }
    
    /* Check if have not set our frozenIndex and no jump updates */
    if(frozenInterpreterIndex == 0 && getJumpType() == NULL && getJumpStartTop() == NULL) {
        
        /* Reset to prior index for reading our */
        fseek(runFile, priorFileIndex, SEEK_SET);
        
    }
    
    /* Check if we have no jump updates */
    if(getJumpType() == NULL && getJumpStartTop() == NULL) {
        
        /* Check if our frozen index is set */
        if(frozenInterpreterIndex != 0) {
            
            /* Reset file to frozen interpreter index, this allows us to execute all the code that was recently modified and to catch up to the compiler */
            fseek(runFile, frozenInterpreterIndex, SEEK_SET);
            
            /* reset frozen index to inactive */
            frozenInterpreterIndex = 0;
            
        }
        
        /* Run our bytecodes */
        runInstructions();
        
    /* Check if our frozen index is not set */
    } else if(frozenInterpreterIndex == 0) {
        
        /* Reset to our priorFileIndex from earlier */
        fseek(runFile, priorFileIndex, SEEK_SET);
        
        /* Set the frozen instructions to our current index in this file */
        frozenInterpreterIndex = ftell(runFile);
        
        /* Seek back to the end so we may continue adding bytecodes, but our interpreter is now frozen/waiting to be unlocked again */
        fseek( runFile, 0, SEEK_END);
        
    }
    
    /* indicate no errors occurred */
    return 0;
}
#endif


/* Returns the current character in our runFile */
int getCurrentChar() {
    
    return fgetc(runFile);
}


/* Returns a character relative to the current character in our runFile */
int getCharByOffsetFromCurrent(int offset) {
    
    if(fseek(runFile, offset, SEEK_CUR) == 0) {
        return getCurrentChar();
        
    } else {
        /* Something went wrong, exit out */
        printf("Invalid seek!\n");
        exit(4091);
        return EOF;
        
    }
}


/* Moves to the next character in our file */
void moveToNextChar() {
    
    fgetc(runFile);
}


/* Returns the index of where we are in the current file */
long int getCurrentFileIndex() {
    
    return ftell(runFile);
}


/* Resets the file index to the provided index */
void resetCurrentFileIndex(long int newIndex) {
    
    if(fseek(runFile, newIndex, SEEK_SET) != 0) {
        /* Error out */
        printf("Invalid seek to set new current index!\n");
        exit(4091);
        
    }
}


/* Frees the latria vm and associated objects */
void freeLatria() {
    
    /* free all labels (jumps) */
    freeAllLabels();
    
    /* Free regex cache */
    freeRegexCache();
    
    /* Free all objects in memory */
    freeObjects();
    
    /* Free heap and stack for memory */
    stack_freeHeapAndStack();
    
    /* Deconstruct the virtual machine */
    deconstructLATVM();
    
}
