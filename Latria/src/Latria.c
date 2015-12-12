/*
The MIT License (MIT)

Copyright (c) 2015 Benjamin Wilson Friedman

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
 Created 12/9/14
 
 This 'language' is designed to SUPPLEMENT other languages, and is designed to be light on it's own. VERY light, lighter than lua, but won't contain nearly as much functionality. You can dump this in with C code and use it as if it were a supplemental library even, there is no need to use the actual language portion of it. Features like the 'string' math, quick tree storage (that will be changed, works for now), the ability to have batched GC or to run other processes underneath and capture their 'return' values in ONE line may be desired on their own. This is perfectly fine and desired! Latria should be a nice supplement and a time saver to any project you add it to. This is a hacky language by nature, Latria can be extended within itself by writing functions around system calls (this is so you may import your own programs, assuming they will output the proper data). it will be licensed under the MIT soon enough so feel free to change it up to your heart's content.
 
 
 //comment
 
 Variable declaration::
 x=2
 
 Addition::
 x+y
 x+10
 10+x
 10+20
 
 will execute the following file in place here (injected in a sense)
 result##lua filename.lua##>
 print(result) //prints the result

 //to declare a block utilize the following
 @aBlock(var1,var2) {
    x = 32
    y = 9 * 2
    q=x+y
    return q
 }
 
 q=aBlock() //calls it and stores the result in 'q'
 print(q) //prints 'q' out to the screen
 
 __purge() //flushes all contents in current GC batch
 
 __printMem() //prints out memory system is currently using
 
 __setGCRate(0.2) //sets the GC batching rate to a scale that you pass (2.0 for 2x, 0.0 for lowest possible are two things you could pass)
 
 sleep(1) //sleeps for a second
 
 */

#include "Latria.h"

#define LATRIA_VERSION_NUMBER "0.1.0"

/* recognized exit string (for dynamic interpreter) */
char exitVal[] = "exit";
char *sysTC = NULL;
FILE *runFile = NULL;
FILE *fileStack[10] = {NULL};
unsigned char fileStackIndex = 0;
#define LAT_INPUT_SIZE 1024

#warning we have an issue passing functions to other functions (rather than assigning to a var and passing that) does NOT work at the moment. Need to make sure functions can be properly recognized as inputs to other functions (most likely will end up in requiring extra registers)

int main( int argc, char* argv[]) {
    
    int argI;
    char input[LAT_INPUT_SIZE];
    char shouldCompile = 0;
    
    /* Set callback for exit */
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
            
            if(*arg == '-') {
                
                /* It's a command line arg, parse it */
                if(strlen(arg)) {
                    
                    switch(*(arg+1)) {
                            
                        #ifdef INCLUDECOMPILER
                        case 'c':
                            /* Compile option, we're compiling all files passed in */
                            shouldCompile = 1;
                            break;
                        #endif
                            
                        case 'o':
                            /* Option to run this vm in an optimized mode (considerably faster) */
                            setPrintCacheMode(1);
                            break;
                            
                        case 'v':
                            /* Print Version option */
                            printf("\nLatria v%s\n\n", LATRIA_VERSION_NUMBER);
                            break;
                            
                        case 'h':
                            /* Print help */
                            printf("\n:: Help ::\n\n");
                            printf("usage:\n");
                            printf("latria [-cohtv] test.lra\n");
                            printf("\n");
                            printf("Calling 'latria' by itself will invoke a dynamic interpreter\n");
                            printf("\n");
                            /* printf("-c    Only compiles the passed in files to .lrac files\n"); */
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
                        case 't':
                            /*Turn print caching on, required for testing */
                            setPrintCacheMode(1);
                            /* Run Test Cases (If compiled in) */
                            runAllTests();
                            break;
                        #endif
                            
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
    #if defined(INCLUDECOMPILER)
    else {
        
        /* No args passed, run interactive prompt */
        
        /* set to optimized mode (print all lines as they appear) */
        setPrintCacheMode(1);
        
        printf("\nLatria (Lang Atria) \nv%s\nCopyright © Benjamin Wilson Friedman - 2015\n\n\n", LATRIA_VERSION_NUMBER);
        
        openDynamicInterpreterFile();
        
        /* No file name passed, interactive prompt */
        while(true) {
            
            char *p = NULL;
            int rez = 0;
            
            printf("~~{");
            /* Using multiple scan f's (to compile on ARM specifically) */
            /* Specifically using an anti-class to take everything UP to \n */
            scanf(" %[^\n]", input);
            scanf("%*c");
            p = input;
            
            /* either no comment or all comment, nothing to run here though */
            if(!*input) {
                continue;
            }
            
            /* Handle the input with replaced opcodes */
            rez = handleInput(p);
            
            /* Always flush to output for prompt mode */
            Flush_Batched_Write();
            
            /* Check if we are done executing */
            if(rez == 1) {
                
                /* We have finished, break */
                break;
            }
        }
        
        closeDynamicInterpreterFile();
    }
    
    #else
    else {
        printf("\n\nNothing provided, try latria -h for help\n\n");
    }
    #endif
    
    /* Always flush at end, just in case */
    Flush_Batched_Write();
    
    freeRegexCache();
    
    /* Free only when NOT defined, as atexit() is set to call this automatically for Mac & Linux */
    #if !defined(MACOSX) && !defined(LINUXOS)
    freeLatria();
    #endif
    
    return 0;
}

/* Runs a passed in latria file, and if need be attempts to compile it beforehand */
void executeLatriaFile(char *fileName) {
    /* Get file name */
    char *newFileName = NULL;
    
    #ifdef INCLUDECOMPILER
    /* Check if this is a compiled or NON-compiled file */
    if(*(fileName + strlen(fileName)-1) != 'c') {
        /* File name is NOT compiled, compile it first */
        size_t len = strlen(fileName);
        
        compileLatria(fileName);
        
        /* Create our output file name */
        newFileName = malloc(len+2);
        
        /* Copy the file name we were given */
        strncpy( newFileName, fileName, len);
        
        /* Append 'c' to the end and \0 */
        newFileName[len] = 'c';
        newFileName[len+1] = '\0';
        
        fileName = newFileName;
    }
    #endif
    
    /* Check to push existing file on the stack */
    if(runFile != NULL) {
        /* Already have an open file, push it on the run stack and continue */
        fileStack[fileStackIndex] = runFile;
        fileStackIndex++;
        
    }
    
    /* Open file in Read mode */
    runFile = fopen( fileName, "r");
    
    if(newFileName != NULL) {
        /* Free our allocated new file name */
        free(newFileName);
    }
    
    /* Validate the file was opened */
    if(runFile == NULL) {
        /* Invalid filename */
        printf("\nThe file you provided, %s, could not be opened!\n\n", fileName);
        exit(1);
        
    }
    
    /* Calls the VM to run over the given instruction set */
    runInstructions();
    
    /* Close the file */
    fclose(runFile), runFile = NULL;
    
    /* Check to pop existing file from the stack */
    if(fileStackIndex > 0) {
        /* Pop existing file from the stack and continue */
        fileStackIndex--;
        runFile = fileStack[fileStackIndex];
        
    }
    
}

#if defined(INCLUDECOMPILER)

/* Opens up a file to work with dynamic interpretation */
void openDynamicInterpreterFile() {
    /* Check to push existing file on the stack */
    if(runFile != NULL) {
        /* Already have an open file, push it on the run stack and continue */
        fileStack[fileStackIndex] = runFile;
        fileStackIndex++;
        
    }
    
    //if(runFile != NULL) {
    //    fclose(runFile);
    //}
    
    runFile = tmpfile();
    //runFile = fopen("lastsession.lrac", "wb+");
}

/* Closes a file opened up for dynamic interpretation */
void closeDynamicInterpreterFile() {
    if(runFile != NULL) {
        fclose(runFile), runFile = NULL;
    }
    
    /* Check to pop existing file from the stack */
    if(fileStackIndex > 0) {
        /* Pop existing file from the stack and continue */
        fileStackIndex--;
        runFile = fileStack[fileStackIndex];
        
    }
}

long frozenInterpreterIndex = 0;

/* Handles a LINE of non-compiled latria code */
int handleInput(char *input) {
    
    unsigned char *returned,didPopJumpUpdates=0;
    short retSize;
    JumpUpdate *ju;
    /* Mark prior index to write */
    long priorFileIndex = ftell(runFile);
    
    /* Compile this line */
    compileLine(input);
    
    /* See if we read anything */
    if((retSize = getByteCodeCount()) > 0) {
        /* Read bytecodes */
        returned = readByteCodes();
        
        /* Write out */
        fwrite(returned, sizeof(unsigned char), retSize, runFile);
        
    }
    
    while((ju = popJumpUpdate()) != NULL) {
        char jumpCode[LAT_ADDRESS_SIZE+1] = {0};
        fseek( runFile, ju->bytecodeAddr, SEEK_SET);
        
        /* Extract new jump address into a hex */
        sprintf(jumpCode, LAT_ADDRESS_FORMAT_STRING, ju->jumpAddr);
        fwrite(jumpCode, sizeof(unsigned char), LAT_ADDRESS_SIZE, runFile);
        
        didPopJumpUpdates = 1;
        
    }
    
    if(didPopJumpUpdates == 1) {
        /* Reset back (protects else,elseif from dying) */
        fseek( runFile, 0, SEEK_END);
        
    }
    
    if(frozenInterpreterIndex == 0 && getJumpType() == NULL && getJumpStartTop() == NULL) {
        /* Reset to prior index for reading */
        fseek(runFile, priorFileIndex, SEEK_SET);
        
    }
    
    if(getJumpType() == NULL && getJumpStartTop() == NULL) {
        if(frozenInterpreterIndex != 0) {
            /* Reset file to frozen interpreter index and release it from stasis (aka catch up to the compiler's position) */
            fseek(runFile, frozenInterpreterIndex, SEEK_SET);
            /* reset frozen index (while the interpreter is in statis) */
            frozenInterpreterIndex = 0;
            
        }
        
        /* OK to run, no active control flows yet */
        runInstructions();
        
    } else if(frozenInterpreterIndex == 0) {
        /* Reset for freeze */
        fseek(runFile, priorFileIndex, SEEK_SET);
        /* Set the frozen instructions to our current index */
        frozenInterpreterIndex = ftell(runFile);
        /* And back to the end to continue working */
        fseek( runFile, 0, SEEK_END);
        
    }
    
    return 0;
}
#endif

/* Sets a new tail call */
void setTailCall(char *tc) {
    sysTC = setCharTablePointer(5, tc);
}

/* Returns the currently set tail call */
char *getTailCall() {
    return sysTC;
}

/* Clears any lingering tail calls */
void clearTailCall() {
    sysTC = NULL;
}

/*** FILE READING FUNCS ***/

/* Returns the current character */
int getCurrentChar() {
    return fgetc(runFile);
}

/* Returns a character relative to the current character */
int getCharByOffsetFromCurrent(int offset) {
    if(fseek(runFile, offset, SEEK_CUR) == 0) {
        return getCurrentChar();
        
    } else {
        printf("Invalid seek!\n");
        exit(4091);
        return EOF;
        
    }
}

/* Moves to the next character */
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
        /* Error */
        printf("Invalid seek to set new current index!\n");
        exit(4091);
        
    }
}

/* Frees the latria vm and associated objects */
void freeLatria() {
    /* free all labels */
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
