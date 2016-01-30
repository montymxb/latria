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
 * Latria_Regex.c
 * Latria
 *
 *  Created by Benjamin Friedman on 10/27/15.
 */

#include "Latria_Regex.h"

/* match */
#define CODE_MATCH                  '0'
/* wildcard . */
#define CODE_MATCH_ANY_CHAR         'd'
/* don't match */
#define CODE_NO_MATCH               '1'
/* Loop until we don't match */
#define CODE_MATCH_LOOP             '2'
/* Loop until we match */
#define CODE_NO_MATCH_LOOP          '3'

/* used for match_loop and no_match_loop codes, basically says go until we can't go anymore, but continually bridge off as we go
 This code goes in payload slot #2, right after where our char is usually dumped */
#define CODE_LOOP_ANY               '1'

/* Jump line of execution */
#define CODE_JUMP                   'e'
/* Spawn concurrent process from jump */
#define CODE_JUMP_SPLIT             '4'

/* pseudo 'switch', where default is fail */
#define CODE_ANY                    '5'
/* Loop until fail */
#define CODE_ANY_LOOP               '6'

/* (?: */
#define CODE_NO_CAPTURE_START       'c'
/* ( */
#define CODE_CAPTURE_START          '7'
/* ) */
#define CODE_CAPTURE_END            '8'

/* ^ */
#define CODE_START                  '9'
/* $ */
#define CODE_END                    'a'

/* finisher */
#define CODE_DONE                   'b'

/* empty code */
#define CODE_NONE                   '0'


/* length of instruction chunks */
#define LAT_REGEX_INSTRUCTION_SIZE 5

/*
 0 = Match
 
 • Must Matches
 
 1 = No-Match
 
 • Must not match
 
 2 = Match Loop
 
 • Stay until we don't match
 
 3 = No-Match Loop
 
 • Stay until we Do match
 
 4 = Jump
 
 • Jump to the next given instruction
 
 5 = Any
 
 • Matches any between this and another any instruction, once
 
 6 = Any Loop
 
 • Matches any between this and another any instruction until it fails. Every match creates another state to try and finish
 
 7 = Capture Start
 
 • Starts a new set of capturing parentheses
 
 8 = Capture End
 
 • Closes the most recent set of parentheses
 
 9 = ^
 
 • Match at start of string, no bump
 
 a = $
 
 • Match at end of string, no bump
 
 b = Done
 
 • Finished state
 
 c = No Capture Start
 
 • Starts a new set of non-capturing parentheses
 
 d = Match Any Char
 
 • Match, but for .
 
 */

#pragma message("introduce GREEDY quantifier behavior, for + * among others")

/***** START GROUPING STACK FUNCS *******/

typedef struct {
    char *lastGroupingIndex;
    unsigned int instructionsSinceLastGrouping;
}GroupingStack;

GroupingStack *groupingStack = NULL;
int groupingStackSize = 0;
int groupingStackIndex = -1;

void pushGroupingToGroupingStack(char *groupingIndex, unsigned int lastInstructs);
void popGroupingFromStack();
GroupingStack *getGroupingFromStack();

void incrementAllGroupingsOnStack(int incrementAmount);

void freeAllGroupingsAndStack();

/***** END GROUPING STACK FUNCS *******/


/* Regex struct */
typedef struct {
    
    /* String representation of regex */
    char *stringRep;
    
    /* Compiled regex / instruction set */
    char *regex;
    
    /* Regex forward counter */
    char *regexFwd;
    
    /* # of instruction/steps */
    unsigned int instructionCount;
    
    /* Size of individual instructions, standard is 6 */
    unsigned char instructionSize;
    
    /* Maximum amount of instructions (for compiling) */
    unsigned int maxInstructions;
    
} LATRegex;


LATRegex *compileRegex(char *regexString);
LATRegex *__compileRegex(LATRegex *regex, char *regexString);
unsigned char __runRegexOnString(LATRegex *regex, char *input);

void printRegexDebug(char *regex);

/* Successfuly capturing struct */
struct SuccessfulCapture {
    char *capture;
    struct SuccessfulCapture *next;
};

struct SuccessfulCapture *captureList = NULL;



/***** REGEX CACHE START ******/

#define LAT_REGEX_MAX_CACHE 20

short regexCacheCount = 0;
LATRegex *regexCache[LAT_REGEX_MAX_CACHE] = {NULL};

/***** REGEX CACHE END  ******/





/* Called to simulatenously compile and use a regex in an input string, with a string representation of a regex */
unsigned char regex(char *input, char *regexString) {
    
    LATRegex *regex = NULL;
    char didRecycleRegex = 0;
    int counter = 0;
    
    /* See if we have compiled this regex already */
    while(counter < regexCacheCount) {
        
        if(regexCache[counter] != NULL) {
            
            if(regexCache[counter]->stringRep != NULL) {
                
                if(strcmp(regexString, regexCache[counter]->stringRep) == 0) {
                    
                    /* We have a match! Use this regex */
                    regex = regexCache[counter];
                    didRecycleRegex = 1;
                    break;
                    
                }
            }
        } else {
            
            break;
        }
        counter++;
    }
    
    if(regex == NULL) {
        
        /* Compile a new regex */
        char *regexStringCPY = LATstrdup(regexString);
        regex = compileRegex(regexStringCPY);
        regex->stringRep = LATstrdup(regexString);
        LATDealloc(regexStringCPY);
        
    }
    
    if(regex) {
        
        unsigned char result;
        
        #ifdef LAT_REGEX_DEBUG
        printRegexDebug(regex->regex);
        #endif
        
        result = __runRegexOnString(regex, input);
        
        /* See if we should cache this regex */
        if(didRecycleRegex != 1) {
            
            /* Check to build our cache */
            /*
            if(regexCache == NULL) {
                
                // Allocate a brand new cache
                int cCount = 0;
                regexCache = LATAlloc(NULL, 0, sizeof(LATRegex) * LAT_REGEX_MAX_CACHE);
                
                // Clear values no newly allocated cache
                while(cCount < LAT_REGEX_MAX_CACHE) {
             
                    regexCache[cCount].stringRep = NULL;
                    regexCache[cCount++].regex = NULL;
                }
            }
            */
            
            /* Check if it can not fit on our stack */
            if(regexCacheCount >= LAT_REGEX_MAX_CACHE) {
                
                /* Pop the first element off the stack, shift all of them down and then put our new one on the top */
                LATRegex *r = regexCache[0];
                counter = 1;
                LATDealloc(r->regex);
                LATDealloc(r->stringRep);
                LATDealloc(r);
                regexCache[0] = NULL;
                
                /* Shift all cached regexes down 1 */
                while(counter < LAT_REGEX_MAX_CACHE) {
                    
                    regexCache[counter-1] = regexCache[counter];
                    counter++;
                }
                
                /* Add it to the end */
                regexCache[LAT_REGEX_MAX_CACHE-1] = regex;
                
            } else {
                
                /* Add It */
                regexCache[regexCacheCount] = regex;
                regexCacheCount++;
                
            }
            
        }
        
        /* return the result */
        return result;
    } else {
        
        printf("\n>>: Invalid regex supplied:\n%s\n\n",input);
        exit(1108);
    }
}


/* Compiles and returns a regex from a string representation */
LATRegex *compileRegex(char *regexString) {
    
    char *regexInstruct;
    int counter = 0;
    LATRegex *regex = (LATRegex *)LATAlloc(NULL, 0, sizeof(LATRegex));
    
    /* Allocate space for 100 instructions, and initialize vars */
    regexInstruct = LATAlloc(NULL, 0, LAT_REGEX_INSTRUCTION_SIZE*100*sizeof(char));
    
    /* Clear it out */
    while(counter < LAT_REGEX_INSTRUCTION_SIZE * 100) {
        
        regexInstruct[counter++] = '\0';
    }
    
    regex->regex = regexInstruct;
    regex->regexFwd = regexInstruct;
    regex->maxInstructions = 100;
    regex->instructionSize = LAT_REGEX_INSTRUCTION_SIZE; /* Includes terminator */
    regex->instructionCount = 0;
    
    regex = __compileRegex(regex, regexString);
    
    return regex;
}


struct JumpObject {
    /* Number of instructions to jump for this */
    unsigned int jumpSize;
    /* First instruction following this jump will go to (2 in case for classes & parens) */
    char firstInstruction[LAT_REGEX_INSTRUCTION_SIZE*2];
    /* Index of regex string to continue from */
    char *regexStringIndex;
    /* Index of actual regex instruction to start from (and then jump from) */
    char *regexIndex;
    /* Next JumpObject */
    struct JumpObject *next;
};


/* Prints the regex in a human readable format */
void printRegexDebug(char *regex) {
    char carrier[LAT_REGEX_INSTRUCTION_SIZE+1] = {0};
    char *cP = carrier;
    while(*regex) {
        
        int x = 0;
        carrier[x++] = *regex++;
        carrier[x++] = *regex++;
        carrier[x++] = *regex++;
        carrier[x++] = *regex++;
        carrier[x++] = *regex++;
        printf("%s\n",cP);
    }
}


/* Adds a grouping to the grouping stack */
void pushGroupingToGroupingStack(char *groupingIndex, unsigned int lastInstructs) {
    
    /* Check for a null stack */
    if(groupingStack == NULL) {
        
        /* malloc */
        groupingStack = LATAlloc(NULL, 0, sizeof(GroupingStack) * 5);
        groupingStackSize = 5;
        
    }
    
    /* Increment index */
    groupingStackIndex++;
    
    if(groupingStackIndex > groupingStackSize-1) {
        
        /* realloc */
        groupingStackSize+=5;
        groupingStack = LATAlloc(groupingStack, sizeof(GroupingStack) * (size_t)(groupingStackSize - 5), sizeof(GroupingStack) * (size_t)groupingStackSize);
        
    }
    
    /* Set */
    groupingStack[groupingStackIndex].instructionsSinceLastGrouping = lastInstructs;
    groupingStack[groupingStackIndex].lastGroupingIndex = groupingIndex;
    
    
}


/* Pops the last group from the stack */
void popGroupingFromStack() {
    
    if(groupingStackIndex > 0) {
        
        groupingStackIndex--;
    }
    
}


/* Fetches to top most grouping on the stack */
GroupingStack *getGroupingFromStack() {
    
    return &groupingStack[groupingStackIndex];
}


/* Increments all counters on all groupings */
void incrementAllGroupingsOnStack(int incrementAmount) {
    
    int counter = 0;
    while(counter < groupingStackSize) {
        
        groupingStack[counter++].instructionsSinceLastGrouping+=(unsigned int)incrementAmount;
    }
}


/* Frees all groupings and the current stack */
void freeAllGroupingsAndStack() {
    
    LATDealloc(groupingStack);
    groupingStackIndex = 0;
    groupingStackSize = 0;
}


/* Called actually build the regex itself */
LATRegex *__compileRegex(LATRegex *regex, char *regexString) {
    
    /* For debugging when compilation fails */
    char *debugRegexString = regexString;
    
    /* Used for tracking what is effected when we hit a | symbol */
    /*/////char *lastORGrouping = regexString;*/
    /* Additionllay used for tracking what is effected when we hit a | symbol, this increments as we go */
    /*////unsigned int instructionsSinceLastGrouping = 0;*/
    
    /* Prepare empty stack */
    struct JumpObject *jumpStack = NULL;
    
    /* Create holder for instructions, can hold 100 */
    int instructionThreshold = 100;
    char *instruction = LATAlloc(NULL, 0, (size_t)(LAT_REGEX_INSTRUCTION_SIZE * instructionThreshold+1));
    
    /* Push an initial grouping, for | reference */
    pushGroupingToGroupingStack(regexString, 0);
    
    for(;;) {
        
        unsigned char isCharEscaped = 0;
        unsigned long instructionsAdded = 0;
        char didReplace = '\0';
        char *replaceIndex = NULL;
        size_t instructionSize;
        
        while(*regexString) {
            
            unsigned long lastCountOfInstruction = instructionsAdded;
            
            
            if(*regexString == ' ') {
                
                /* Ignore whitespace */
                regexString++;
                continue;
                
                
            }
            
            /* Check for special characters to work with */
            if(*regexString == 's' && isCharEscaped == 1) {
                
                /* Whitespace character! */
                didReplace = 's';
                replaceIndex = regexString;
                *regexString = ' ';
                
            }
            
            if(*regexString == '\\' && isCharEscaped == 0) {
                
                /* ESCAPED, Continue, mark special for next character */
                isCharEscaped = 1;
                regexString++;
                continue;
                
                
            } else if(*regexString == '^' && isCharEscaped == 0) {
                
                /* (^) Starting sequence, write out a starting instruction */
                instruction[0] = CODE_START;
                instruction[1] = CODE_NONE;
                instruction[2] = CODE_NONE;
                instruction[3] = CODE_NONE;
                instruction[4] = CODE_NONE;
                instruction[5] = '\0';
                
                /* 1 instruction */
                instructionsAdded++;
                
                
            } else if(*regexString == '$' && isCharEscaped == 0) {
                
                /* ($) Ending sequence, write out an ending instruction (just match to \0) */
                instruction[0] = CODE_END;
                instruction[1] = CODE_NONE;
                instruction[2] = CODE_NONE;
                instruction[3] = CODE_NONE;
                instruction[4] = CODE_NONE;
                instruction[5] = '\0';
                
                /* 1 instruction */
                instructionsAdded++;
                
                
            } else if(*regexString == '.' && isCharEscaped == 0) {
                
                char check = -1;
                char loopType = CODE_MATCH_LOOP;
                
                
                if(*(regexString+1) && *(regexString+2)) {
                    
                    if(*(regexString+2) == '\\' && *(regexString+3)) {
                        
                        /* False loop to the next passable char */
                        loopType = CODE_NO_MATCH_LOOP;
                        check = *(regexString+3);
                        
                    } else if(*(regexString+1) != '$' &&
                              *(regexString+1) != '.' &&
                              *(regexString+1) != '[' &&
                              *(regexString+1) != '+' &&
                              *(regexString+1) != '*' &&
                              *(regexString+1) != '?' &&
                              *(regexString+1) != '(' &&
                              *(regexString+1) != ')' &&
                              *(regexString+1) != '|')
                    {
                        
                        /* Next char is NOT special, use it */
                        loopType = CODE_NO_MATCH_LOOP;
                        check = *(regexString+1);
                    }
                }
                
                /* Match any character */
                if(*(regexString+1)) {
                    
                    /* Check following characters */
                    if(*(regexString+1) == '*') {
                        
                        /* Make sure NOT followed by a special char */
                        if(*(regexString+2) &&
                           (*(regexString+2) != '$' &&
                            *(regexString+2) != '.' &&
                            *(regexString+2) != '[' &&
                            *(regexString+2) != '+' &&
                            *(regexString+2) != '*' &&
                            *(regexString+2) != '?' &&
                            *(regexString+2) != '(' &&
                            *(regexString+2) != ')' &&
                            *(regexString+2) != '|') &&
                           *(regexString+2) != '\\')
                        {
                            
                            /* Next char is NOT special, use it */
                            loopType = CODE_NO_MATCH_LOOP;
                            check = *(regexString+2);
                            
                        }
                        
                        /* Match the preceding element 0 or more times (push loop) */
                        instruction[0] = loopType;
                        instruction[1] = check == -1 ? CODE_NONE : check;
                        instruction[2] = check == -1 ? CODE_LOOP_ANY : CODE_NONE;
                        instruction[3] = CODE_NONE;
                        instruction[4] = CODE_NONE;
                        instruction[5] = '\0';
                        
                        /* Bump */
                        regexString++;
                        
                        /* 1 instruction */
                        instructionsAdded++;
                        
                    } else if(*(regexString+1) == '+') {
                        
                        /* Match the preceding element 1 or more times (push match) */
                        
                        if(*(regexString+2) &&
                           (*(regexString+2) != '$' &&
                            *(regexString+2) != '.' &&
                            *(regexString+2) != '[' &&
                            *(regexString+2) != '+' &&
                            *(regexString+2) != '*' &&
                            *(regexString+2) != '?' &&
                            *(regexString+2) != '(' &&
                            *(regexString+2) != ')' &&
                            *(regexString+2) != '|') &&
                           *(regexString+2) != '\\')
                        {
                            
                            /* Next char is NOT special, use it */
                            loopType = CODE_NO_MATCH_LOOP;
                            check = *(regexString+2);
                        }
                        
                        /* Push match */
                        instruction[0] = CODE_MATCH_ANY_CHAR;
                        instruction[1] = CODE_NONE;
                        instruction[2] = CODE_NONE;
                        instruction[3] = CODE_NONE;
                        instruction[4] = CODE_NONE;
                        
                        /* Push Loop */
                        instruction[5] = loopType;
                        instruction[6] = check == -1 ? CODE_NONE : check;
                        instruction[7] = check == -1 ? CODE_LOOP_ANY : CODE_NONE;
                        instruction[8] = CODE_NONE;
                        instruction[9] = CODE_NONE;
                        instruction[10] = '\0';
                        
                        /* Bump */
                        regexString++;
                        
                        /* 2 instructions */
                        instructionsAdded+=2;
                        
                    } else if(*(regexString+1) == '?') {
                        
                        /* Matches the preceding element 0 or one time (push a jump and a match) */
                        
                        /* JUMP */
                        struct JumpObject *jo = LATAlloc(NULL, 0, sizeof(struct JumpObject));
                        jo->jumpSize = 1;
                        jo->next = jumpStack;
                        jo->regexStringIndex = regexString+2;
                        jo->regexIndex = regex->regexFwd;
                        jo->firstInstruction[0] = '\0';
                        jumpStack = jo;
                        
                        /* push jump */
                        instruction[0] = CODE_JUMP_SPLIT;
                        instruction[1] = CODE_NONE;
                        instruction[2] = CODE_NONE;
                        instruction[3] = CODE_NONE;
                        instruction[4] = CODE_NONE;
                        
                        /* Push match */
                        instruction[5] = CODE_MATCH_ANY_CHAR;
                        instruction[6] = CODE_NONE;
                        instruction[7] = CODE_NONE;
                        instruction[8] = CODE_NONE;
                        instruction[9] = CODE_NONE;
                        instruction[10] = '\0';
                        
                        /* Bump */
                        regexString++;
                        
                        /* 2 instructions */
                        instructionsAdded++;
                        
                    } else {
                        
                        /* Normal check, just write it out */
                        instruction[0] = CODE_MATCH_ANY_CHAR;
                        instruction[1] = CODE_NONE;
                        instruction[2] = CODE_NONE;
                        instruction[3] = CODE_NONE;
                        instruction[4] = CODE_NONE;
                        instruction[5] = '\0';
                        
                        /* 1 instruction */
                        instructionsAdded++;
                    }
                } else {
                    
                    /* At the end, write it out */
                    instruction[0] = CODE_MATCH_ANY_CHAR;
                    instruction[1] = CODE_NONE;
                    instruction[2] = CODE_NONE;
                    instruction[3] = CODE_NONE;
                    instruction[4] = CODE_NONE;
                    instruction[5] = '\0';
                    
                    /* 1 instruction */
                    instructionsAdded++;
                    
                }
                
            } else if (*regexString == '[' && isCharEscaped == 0) {
                
                /* ( [ ] ) Character Class */
                
                /* list to hold all chars in this class */
                char list[256] = {0};
                char *origListP = list;
                char *listPointer = list;
                unsigned char classEscapedChar = 0;
                /* Class type, default MATCH */
                char classType = CODE_MATCH;
                char *adventurer;
                unsigned char classModifier = 0;
                
                if(*(regexString+1) == '^') {
                    /* NO Match for the following items */
                    classType = CODE_NO_MATCH;
                    regexString++;
                }
                
                /* Put all our characters into our list! */
                while(*regexString++) {
                    
                    if(*regexString == ' ') {
                        
                        /* Ignore whitespace */
                        continue;
                        
                    }
                    
                    /* Check for special characters to work with */
                    if(*regexString == 's' && isCharEscaped == 1) {
                        
                        /* Whitespace character! */
                        *regexString = ' ';
                        
                    }
                    
                    if(*regexString == '\\') {
                        
                        /* Escape the next character */
                        classEscapedChar = 1;
                        
                        
                    } else if(*regexString == ']' && classEscapedChar == 0) {
                        
                        /* Break out */
                        break;
                        
                        
                    } else {
                        /* Normal, add it if not a duplicate */
                        if(strchr(origListP, *regexString) == 0) {
                            
                            /* Not a duplicate, check for RANGE */
                            if(isalnum(*regexString) &&
                               *(regexString+1) &&
                               *(regexString+2) &&
                               *(regexString+1) == '-' &&
                               isalnum(*(regexString+2)))
                            {
                                
                                char start = *regexString;
                                char tgt = *(regexString+2);
                                
                                
                                /* Loop through and add all the values between THIS and what follows... */
                                for(;;) {
                                    
                                    /* avoid duplicates as we go */
                                    if(strchr(origListP, start) == 0) {
                                        
                                        /* add this item */
                                        *listPointer++ = start++;
                                        
                                    }
                                    
                                    if(start == tgt) {
                                        
                                        /* add last item */
                                        *listPointer++ = start;
                                        /* increment by 2 */
                                        regexString+=2;
                                        break;
                                        
                                        
                                    }
                                }
                                
                            } else if(*regexString != '[' || classEscapedChar == 1) {
                                
                                /* regular 1 item */
                                *listPointer++ = *regexString;
                                
                                
                            } else {
                                
                                printf("\n>>: Malformed regex at '%s', found unescaped opening [ inside of character class\n\n",debugRegexString);
                                exit(9100);
                                
                            }
                            
                            classEscapedChar = 0;
                        }
                    }
                }
                
                /* reset list pointer */
                listPointer = list;
                
                adventurer = regexString+1;
                
                while(*adventurer == ' ') {
                    
                    adventurer++;
                }
                
                if(*adventurer) {
                    
                    if(*adventurer == '*') {
                        
                        /* Creates one list of ANY LOOP containing all possible outcomes */
                        classModifier = '*';
                        
                    } else if(*adventurer == '+') {
                        
                        /* Modifies this to add a list of ANYs and ANY LOOPs for each set */
                        classModifier = '+';
                        
                    } else if(*adventurer == '?') {
                        
                        /* Creates a JUMP to a list of ANY, which need only match ONCE */
                        classModifier = '?';
                        
                    }
                    
                }
                
                /* Validate we are closed */
                if(*regexString == ']') {
                    
                    /* Validate we have anything inside this class */
                    if(*listPointer) {
                        
                        /* We have items to handle */
                        if(classModifier == '*') {
                            int x = 0;
                            
                            /* Create and push a list of ANY LOOP items */
                            
                            /* Open ANY loop */
                            instruction[x++] = CODE_ANY_LOOP;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instructionsAdded++;
                            
                            while(*listPointer) {
                                
                                /* Push our items in */
                                instruction[x++] = classType;
                                instruction[x++] = *listPointer;
                                instruction[x++] = CODE_NONE;
                                instruction[x++] = CODE_NONE;
                                instruction[x++] = CODE_NONE;
                                instructionsAdded++;
                                
                                if(x >= instructionThreshold-1) {
                                    
                                    /* Realloc our instruction list */
                                    instructionThreshold+=100;
                                    instruction = LATAlloc(instruction, (size_t)((instructionThreshold-100)*LAT_REGEX_INSTRUCTION_SIZE+1), (size_t)(LAT_REGEX_INSTRUCTION_SIZE * instructionThreshold+1));
                                }
                                
                                /* bump list */
                                listPointer++;
                            }
                            
                            /* Close ANY loop (same as open) */
                            instruction[x++] = CODE_ANY_LOOP;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x] = '\0';
                            instructionsAdded++;
                            
                        } else if(classModifier == '+') {
                            
                            /* Modifies this to add a list of ANYs and ANY LOOPs for each set */
                            
                            int x = 0;
                            
                            /*** Open ANY (no loop) ***/
                            instruction[x++] = CODE_ANY;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instructionsAdded++;
                            
                            while(*listPointer) {
                                /* Push our items in */
                                instruction[x++] = classType;
                                instruction[x++] = *listPointer;
                                instruction[x++] = CODE_NONE;
                                instruction[x++] = CODE_NONE;
                                instruction[x++] = CODE_NONE;
                                instructionsAdded++;
                                
                                if(x >= instructionThreshold-1) {
                                    
                                    /* Realloc our instruction list */
                                    instructionThreshold+=100;
                                    instruction = LATAlloc(instruction, (size_t)((instructionThreshold-100)*LAT_REGEX_INSTRUCTION_SIZE+1), (size_t)(LAT_REGEX_INSTRUCTION_SIZE * instructionThreshold+1));
                                }
                                
                                /* bump list */
                                listPointer++;
                            }
                            
                            /* reset list */
                            listPointer = list;
                            
                            /* Close ANY (same as open) */
                            instruction[x++] = CODE_ANY;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instructionsAdded++;
                            
                            
                            
                            /*** Open ANY loop ***/
                            instruction[x++] = CODE_ANY_LOOP;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instructionsAdded++;
                            
                            while(*listPointer) {
                                
                                /* Push our items in */
                                instruction[x++] = classType;
                                instruction[x++] = *listPointer;
                                instruction[x++] = CODE_NONE;
                                instruction[x++] = CODE_NONE;
                                instruction[x++] = CODE_NONE;
                                instructionsAdded++;
                                
                                if(x >= instructionThreshold-1) {
                                    
                                    /* Realloc our instruction list */
                                    instructionThreshold+=100;
                                    instruction = LATAlloc(instruction, (size_t)((instructionThreshold-100)*LAT_REGEX_INSTRUCTION_SIZE+1), (size_t)(LAT_REGEX_INSTRUCTION_SIZE * instructionThreshold+1));
                                }
                                
                                /* bump list */
                                listPointer++;
                            }
                            
                            /* Close ANY loop (same as open) */
                            instruction[x++] = CODE_ANY_LOOP;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x] = '\0';
                            instructionsAdded++;
                            
                        } else if(classModifier == '?') {
                            
                            /* Creates a JUMP to normal, and adds a list of ANY, which need only match ONCE */
                            
                            int x = 0;
                            
                            /*** JUMP ***/
                            
                            struct JumpObject *jo = LATAlloc(NULL, 0, sizeof(struct JumpObject));
                            jo->jumpSize = 1;
                            jo->next = jumpStack;
                            jo->regexStringIndex = regexString+2;
                            jo->regexIndex = regex->regexFwd;
                            /* No instruction */
                            jo->firstInstruction[0] = '\0';
                            /* set jump stack */
                            jumpStack = jo;
                            
                            if(x >= instructionThreshold-(LAT_REGEX_INSTRUCTION_SIZE*2)) {
                                
                                /* Realloc our instruction list */
                                instructionThreshold+=100;
                                instruction = LATAlloc(instruction, (size_t)((instructionThreshold-100)*LAT_REGEX_INSTRUCTION_SIZE+1), (size_t)(LAT_REGEX_INSTRUCTION_SIZE * instructionThreshold+1));
                            }
                            
                            /* push jump */
                            instruction[x++] = CODE_JUMP_SPLIT;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instructionsAdded++;
                            
                            /*** Open ANY (no loop) ***/
                            instruction[x++] = CODE_ANY;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instructionsAdded++;
                            
                            while(*listPointer) {
                                /* Push our items in */
                                instruction[x++] = classType;
                                instruction[x++] = *listPointer;
                                instruction[x++] = CODE_NONE;
                                instruction[x++] = CODE_NONE;
                                instruction[x++] = CODE_NONE;
                                instructionsAdded++;
                                
                                if(x >= instructionThreshold-1) {
                                    
                                    /* Realloc our instruction list */
                                    instructionThreshold+=100;
                                    instruction = LATAlloc(instruction, (size_t)((instructionThreshold-100)*LAT_REGEX_INSTRUCTION_SIZE+1), (size_t)(LAT_REGEX_INSTRUCTION_SIZE * instructionThreshold+1));
                                }
                                
                                /* bump list */
                                listPointer++;
                            }
                            
                            /* Close ANY (same as open) */
                            instruction[x++] = CODE_ANY;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x] = '\0';
                            instructionsAdded++;
                            
                        } else {
                            
                            /* Normal Behavior */
                            
                            int x = 0;
                            
                            /*** Open ANY (no loop) ***/
                            instruction[x++] = CODE_ANY;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instructionsAdded++;
                            
                            while(*listPointer) {
                                
                                /* Push our items in */
                                instruction[x++] = classType;
                                instruction[x++] = *listPointer;
                                instruction[x++] = CODE_NONE;
                                instruction[x++] = CODE_NONE;
                                instruction[x++] = CODE_NONE;
                                instructionsAdded++;
                                
                                if(x >= instructionThreshold-1) {
                                    
                                    /* Realloc our instruction list */
                                    instructionThreshold+=100;
                                    instruction = LATAlloc(instruction, (size_t)((instructionThreshold-100)*LAT_REGEX_INSTRUCTION_SIZE+1), (size_t)(LAT_REGEX_INSTRUCTION_SIZE * instructionThreshold+1));
                                }
                                
                                /* bump list */
                                listPointer++;
                            }
                            
                            /* Close ANY (same as open) */
                            instruction[x++] = CODE_ANY;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = CODE_NONE;
                            instruction[x++] = '\0';
                            instructionsAdded++;
                            
                        }
                    }
                    
                    /* depending if we ended up matching (+,?,* after [] we need to bump up one */
                    if(classModifier == '+' ||
                       classModifier == '?' ||
                       classModifier == '*')
                    {
                        
                        regexString+=(adventurer-regexString);
                    }
                    
                } else {
                    
                    /* This is a malformed regex! */
                    printf("\n>>: Malformed regex at '%s', missing closing ] for character class\n\n",debugRegexString);
                    exit(9100);
                }
                
            } else if(*regexString == '|' && isCharEscaped == 0) {
                
                /* OR */
                char *shiftCopy;
                char carryOver[5] = {0};
                char *carryOverPointer = carryOver;
                struct JumpObject *jo;
                
                /* Get current grouping */
                GroupingStack *lastGrouping = getGroupingFromStack();
                
                /* Check to realloc regex size to fit our injected jump */
                if(lastGrouping->instructionsSinceLastGrouping+1 >= regex->maxInstructions-1) {
                    
                    regex->maxInstructions+=100;
                    regex->regex = LATAlloc(regex->regex, LAT_REGEX_INSTRUCTION_SIZE*(regex->maxInstructions-100)*sizeof(char), LAT_REGEX_INSTRUCTION_SIZE*regex->maxInstructions*sizeof(char));
                    regex->regexFwd = regex->regex+strlen(regex->regex);
                    
                    
                }
                
                #pragma message("There is some wierd stuff going on here...Alternation doesn't work so well when we have a few or more alternating statements")
                
                /* Rewind LAT_REGEX_INSTRUCTION_SIZE * instructionsSinceLastGrouping */
                regex->regexFwd-=(LAT_REGEX_INSTRUCTION_SIZE*(lastGrouping->instructionsSinceLastGrouping));
                
                /* Create a shift copy */
                shiftCopy = LATAlloc(NULL, 0, strlen(regex->regexFwd)+LAT_REGEX_INSTRUCTION_SIZE+1);
                /* Copy everything LAT_REGEX_INSTRUCTION_SIZE up, to create space for an instruction */
                strncpy(shiftCopy+LAT_REGEX_INSTRUCTION_SIZE, regex->regexFwd, strlen(regex->regexFwd));
                shiftCopy[strlen(regex->regexFwd)+LAT_REGEX_INSTRUCTION_SIZE] = '\0';
                
                /* Build a jump instruction to go behind this last set, signaling to concurrently try this item as well */
                sprintf(carryOverPointer, "%04x", lastGrouping->instructionsSinceLastGrouping+2);

                shiftCopy[0] = CODE_JUMP_SPLIT;
                shiftCopy[1] = carryOverPointer[0];
                shiftCopy[2] = carryOverPointer[1];
                shiftCopy[3] = carryOverPointer[2];
                shiftCopy[4] = carryOverPointer[3];
                
                /* Copy our modified instruction (with injected jump) set back in  */
                strncpy(regex->regexFwd, shiftCopy, strlen(shiftCopy));
                
                /* Free shiftCopy */
                LATDealloc(shiftCopy);
                
                /* set regexFwd back forward */
                regex->regexFwd+=strlen(regex->regexFwd);
                
                /* Prepare a NEW jump instruction, telling us where to move on after we reach this point */
                jo = LATAlloc(NULL, 0, sizeof(struct JumpObject));
                
                /* Jump size should be 0 on this item (instead of 1)...not sure why however */
                jo->jumpSize = 0;
                jo->next = jumpStack;
                
                /* Try and find the next balancing closing paren */
                shiftCopy = getNextUnescapedClosingParen(regexString);
                
                if(shiftCopy == NULL) {
                    
                    /* Just set to end of regexString */
                    shiftCopy=(regexString+strlen(regexString));
                    
                    
                }
                
                jo->regexStringIndex = shiftCopy;
                jo->regexIndex = regex->regexFwd;
                jo->firstInstruction[0] = '\0';
                jumpStack = jo;
                
                /* Write out the actual linear jump instruction */
                instruction[0] = CODE_JUMP;
                instruction[1] = CODE_NONE;
                instruction[2] = CODE_NONE;
                instruction[3] = CODE_NONE;
                instruction[4] = CODE_NONE;
                instruction[5] = '\0';
                instructionsAdded++;
                
            } else if (*regexString == '(' && isCharEscaped == 0) {
                
                /* Containing parentheses, with capture if indicated */
                
                /* Push a paren capture instruction */
                if(*(regexString+1) && *(regexString+2) && *(regexString+1) == '?' && *(regexString+2) == ':') {
                    
                    /* Non-Capturing */
                    instruction[0] = CODE_NO_CAPTURE_START;
                    
                    /* bump up regex string to skip non-capture indicator */
                    regexString+=2;
                    
                } else {
                    
                    /* Capturing */
                    instruction[0] = CODE_CAPTURE_START;
                    
                }
                
                instruction[1] = CODE_NONE;
                instruction[2] = CODE_NONE;
                instruction[3] = CODE_NONE;
                instruction[4] = CODE_NONE;
                instruction[5] = '\0';
                
                /* Reset tracker vars */
                /*
                ///instructionsSinceLastGrouping = 0;
                ///lastORGrouping = regexString+1;
                 */
                
                /* Push new Grouping */
                pushGroupingToGroupingStack(regexString+1, 0);
                
            } else if(*regexString == ')' && isCharEscaped == 0) {
                
                /* Push a paren close capture instruction */
                instruction[0] = CODE_CAPTURE_END;
                instruction[1] = CODE_NONE;
                instruction[2] = CODE_NONE;
                instruction[3] = CODE_NONE;
                instruction[4] = CODE_NONE;
                instruction[5] = '\0';
                
                /* Pop grouping */
                popGroupingFromStack();
                
            } else {
                
                /* Everything else... */
                
                if(*(regexString+1)) {
                    
                    /* Check following characters */
                    if(*(regexString+1) == '*') {
                        
                        
                        /* Match the preceding element 0 or more times (push loop) */
                        instruction[0] = CODE_MATCH_LOOP;
                        instruction[1] = *regexString;
                        instruction[2] = CODE_NONE;
                        instruction[3] = CODE_NONE;
                        instruction[4] = CODE_NONE;
                        instruction[5] = '\0';
                        
                        /* Bump */
                        regexString++;
                        
                        /* 1 instruction */
                        instructionsAdded++;
                        
                    } else if(*(regexString+1) == '+') {
                        
                        /* Match the preceding element 1 or more times (push match) */
                        
                        /* Push match */
                        instruction[0] = CODE_MATCH;
                        instruction[1] = *regexString;
                        instruction[2] = CODE_NONE;
                        instruction[3] = CODE_NONE;
                        instruction[4] = CODE_NONE;
                        
                        /* Push Loop */
                        instruction[5] = CODE_MATCH_LOOP;
                        instruction[6] = *regexString;
                        instruction[7] = CODE_NONE;
                        instruction[8] = CODE_NONE;
                        instruction[9] = CODE_NONE;
                        instruction[10] = '\0';
                        
                        /* Bump */
                        regexString++;
                        
                        /* 2 instructions */
                        instructionsAdded+=2;
                        
                    } else if(*(regexString+1) == '?') {
                        
                        /* Matches the preceding element 0 or one time (push a jump and a match) */
                        
                        /* JUMP */
                        struct JumpObject *jo = LATAlloc(NULL, 0, sizeof(struct JumpObject));
                        jo->jumpSize = 1;
                        jo->next = jumpStack;
                        jo->regexStringIndex = regexString+2;
                        jo->regexIndex = regex->regexFwd;
                        jo->firstInstruction[0] = '\0';
                        jumpStack = jo;
                        
                        /* Push the Jump */
                        instruction[0] = CODE_JUMP_SPLIT;
                        instruction[1] = CODE_NONE;
                        instruction[2] = CODE_NONE;
                        instruction[3] = CODE_NONE;
                        instruction[4] = CODE_NONE;
                        
                        /* Push match */
                        instruction[5] = CODE_MATCH;
                        instruction[6] = *regexString;
                        instruction[7] = CODE_NONE;
                        instruction[8] = CODE_NONE;
                        instruction[9] = CODE_NONE;
                        instruction[10] = '\0';
                        
                        /* Bump past the ? */
                        regexString++;
                        
                        /* 2 instruction */
                        instructionsAdded+=2;
                        
                    } else {
                        
                        /* Normal check, just write it out */
                        instruction[0] = CODE_MATCH;
                        instruction[1] = *regexString;
                        instruction[2] = CODE_NONE;
                        instruction[3] = CODE_NONE;
                        instruction[4] = CODE_NONE;
                        instruction[5] = '\0';
                        
                        /* 1 instruction */
                        instructionsAdded++;
                    }
                } else {
                    
                    /* At the end, write it out */
                    instruction[0] = CODE_MATCH;
                    instruction[1] = *regexString;
                    instruction[2] = CODE_NONE;
                    instruction[3] = CODE_NONE;
                    instruction[4] = CODE_NONE;
                    instruction[5] = '\0';
                    
                    /* 1 instruction */
                    instructionsAdded++;
                    
                }
            }
            
            /* Check to realloc regex size */
            if(instructionsAdded >= regex->maxInstructions-1) {
                
                regex->maxInstructions+=100;
                regex->regex = LATAlloc(regex->regex, LAT_REGEX_INSTRUCTION_SIZE*(regex->maxInstructions-100)*sizeof(char), LAT_REGEX_INSTRUCTION_SIZE*regex->maxInstructions*sizeof(char));
                regex->regexFwd = regex->regex+strlen(regex->regex);
            }
            
            /* Write out the completed instruction */
            instructionSize = strlen(instruction);
            strncpy(regex->regexFwd, instruction, instructionSize);
            regex->regexFwd+=instructionSize;
            
            /* Resets any escaped characters */
            isCharEscaped = 0;
            
            /* Replace anything we escaped */
            if(didReplace != '\0') {
                
                *replaceIndex = didReplace;
                didReplace = '\0';
            }
            
            /* Check if we have a valid jump stack */
            if(jumpStack) {
                
                /* Increment jumpSize for each jump sequence */
                struct JumpObject *jo = jumpStack;
                
                while(jo) {
                    
                    jo->jumpSize+=(unsigned int)(instructionsAdded-lastCountOfInstruction);
                    jo = jo->next;
                }
            }
            
            /* Bump instructions added since last OR grouping (or opening parens grouping) */
            incrementAllGroupingsOnStack((int)(instructionsAdded-lastCountOfInstruction));
            
            /* Increment our regex string */
            regexString++;
        }
        
        /* Push DONE */
        instruction[0] = CODE_DONE;
        instruction[1] = CODE_NONE;
        instruction[2] = CODE_NONE;
        instruction[3] = CODE_NONE;
        instruction[4] = CODE_NONE;
        instruction[5] = '\0';
        instructionsAdded++;
        
        /* Check to realloc regex size */
        if(instructionsAdded >= regex->maxInstructions-1) {
            
            regex->maxInstructions+=100;
            regex->regex = LATAlloc(regex->regex, LAT_REGEX_INSTRUCTION_SIZE*(regex->maxInstructions-100)*sizeof(char), LAT_REGEX_INSTRUCTION_SIZE*regex->maxInstructions*sizeof(char));
            regex->regexFwd = regex->regex+strlen(regex->regex);
        }
        
        /* Write out the completed instruction */
        instructionSize = strlen(instruction);
        strncpy(regex->regexFwd, instruction, instructionSize);
        regex->regexFwd+=instructionSize;
        
        if(jumpStack != NULL && !*regexString) {
            
            char jumpCode[5] = {0};
            char *jumpCodePointer = jumpCode;
            unsigned int instructionCount = 0;
            /* Attend to this next jump sequence */
            struct JumpObject *tob;
            /* Reset our regex string */
            regexString = jumpStack->regexStringIndex;
            
            /* Modify our original jump instruction */
            
            /* Get our jump count in an array */
            sprintf(jumpCodePointer, "%04x", jumpStack->jumpSize);
            
            /* Copy our jump count from our array to our jump instruction in our regex, from earlier */
            jumpStack->regexIndex++;
            *jumpStack->regexIndex++ = jumpCode[0];
            *jumpStack->regexIndex++ = jumpCode[1];
            *jumpStack->regexIndex++ = jumpCode[2];
            *jumpStack->regexIndex++ = jumpCode[3];
            
            if(*(jumpStack->firstInstruction)) {
                
                /* Place in our first instruction */
                instructionCount += (unsigned int)(strlen(jumpStack->firstInstruction)/LAT_REGEX_INSTRUCTION_SIZE);
                instructionsAdded+=instructionCount;
                
                /* Check to realloc regex size */
                if(instructionsAdded >= regex->maxInstructions-1) {
                    
                    regex->maxInstructions+=100;
                    regex->regex = LATAlloc(regex->regex, LAT_REGEX_INSTRUCTION_SIZE*(regex->maxInstructions-100)*sizeof(char), LAT_REGEX_INSTRUCTION_SIZE*regex->maxInstructions*sizeof(char));
                    regex->regexFwd = regex->regex+strlen(regex->regex);
                }
                
                regex->regexFwd = strncpy(regex->regexFwd, jumpStack->firstInstruction, LAT_REGEX_INSTRUCTION_SIZE*2);
            }
            
            /* Carry over */
            tob = jumpStack;
            /* Pop */
            jumpStack = jumpStack->next;
            /* Dealloc */
            LATDealloc(tob);
            
            /* Update the other jump sequences we still have waiting with the distance this recent set of commands added added */
            if(instructionCount > 0) {
                
                tob = jumpStack;
                
                while(tob) {
                    
                    tob->jumpSize+=instructionCount;
                    tob = tob->next;
                }
            }
            
        } else {
            
            /* All done! */
            break;
        }
    }
    
    /* Free instructions */
    LATDealloc(instruction);
    
    return regex;
}


/* For capturing parens */
struct RegexCapture {
    
    unsigned char isComplete;
    char *capture;
    struct RegexCapture *next;
};


/* Used to set RegexState stateID below v*/
short stateIDNum = 0;

/* For the state machine, automata */
struct RegexState {
    
    short stateID;
    unsigned char isActive;
    unsigned char mustCaptureAtBeginning;
    unsigned char isCapturing;
    unsigned char overlyingState;
    unsigned short totalCaptures;
    char *regexIndex;
    struct RegexState *next;
    unsigned char currentState;
    struct RegexCapture *captureBlock;
};

typedef enum {
    
    JUMP_INSTRUCTION,
    JUMP_FORWARD_ONE,
    JUMP_INSTRUCTION_NO_SPLIT
}JumpType;

void disableState(struct RegexState *state);
void freeAllRegexStates();
void incrementStateMachine(struct RegexState *state);
void updateStateMachine(struct RegexState *state, unsigned char isAtBeginning, char *input);
void performJumpOnStateMatchine(struct RegexState *state, JumpType jumpType);

/* Linked list for holding machine states */
struct RegexState *rootStateList = NULL;

void deleteExistingCaptures();

void saveCapturesFromState(struct RegexState *state);

void printRegexStateDebugInfo(struct RegexState *regex);


/* Prints informative info about a regex state */
void printRegexStateDebugInfo(struct RegexState *regex) {
    
    struct RegexCapture *cap;
    int capture = 1;
    
    cap = regex->captureBlock;
    
    printf("#%d\n",regex->stateID);
    
    while(cap != NULL) {
        
        if(cap->isComplete == 1) {
            
            printf("#%d(%s)\n",capture,cap->capture);
            
            
        } else {
            
            printf("#%d~~%s\n",capture,cap->capture);
            
            
        }
        capture++;
        cap = cap->next;
    }
    
    printRegexDebug(regex->regexIndex);
    
    printf("\n");
    
}


/* Runs a given regex we previously generated, generating a result */
unsigned char __runRegexOnString(LATRegex *regex, char *input) {
    
    /* Get a regex state ready */
    unsigned long iterations = 0;
    unsigned char isAtBeginning = 1;
    struct RegexState *state;
    
    char *originalInputTracker = input;
    
    rootStateList = LATAlloc(NULL, 0, sizeof(struct RegexState));
    
    state = rootStateList;
    state->stateID = stateIDNum++;
    state->regexIndex = regex->regex;
    state->next = NULL;
    state->captureBlock = NULL;
    state->isCapturing = 0;
    state->overlyingState = 0;
    state->mustCaptureAtBeginning = 0;
    state->totalCaptures = 0;
    /* Mark active (on failure changes to 0) */
    state->isActive = 1;
    
    /* flush any existing captures */
    deleteExistingCaptures();
    
    /* Go over our input one char at a time */
    for(;;) {
        
        /* Bump the input (but not on first time, allowing us to handle ^ and other starters) */
        if(isAtBeginning != 0x01) {
            
            input++;
        }
        
        /* Update all available states */
        while(state != NULL) {
            
            iterations++;
            
            #ifdef LAT_REGEX_DEBUG
            printRegexStateDebugInfo(state);
            #endif
            
            /* Handles, jumps, Anys and Starts, basically anything that prepares the machine for the next input */
            updateStateMachine(state,isAtBeginning,input);
            
            #ifdef LAT_REGEX_DEBUG
            printRegexStateDebugInfo(state);
            #endif
            
            if(state->isActive == 0) {
                
                /* Skip inactive state */
                state = state->next;
                
                while(state != NULL) {
                    
                    state = state->next;
                    
                    if(state->isActive == 1) {
                        
                        break;
                    }
                }
                continue;
            }
            
            switch(state->currentState) {
                    
                case CODE_MATCH:
                    
                    if(state->overlyingState == CODE_ANY_LOOP) {
                        
                        /* Loop until we have success or failure */
                        char *startingRegexIndex = state->regexIndex;
                        unsigned char shouldContinue = 0;
                        
                        for(;;) {
                            
                            if(*(state->regexIndex+1) == *input) {
                                
                                /* Sucess! Increment state until we are AT CODE_ANY */
                                while(*state->regexIndex != CODE_ANY_LOOP) {
                                    
                                    incrementStateMachine(state);
                                    
                                }
                                
                                /* Perform a jump for this successful instance */
                                #pragma message("Possible this isn't safe to remove below")
                                /*///////performJumpOnStateMatchine(state, JUMP_FORWARD_ONE, input);*/
                                
                                /* Reset our state regex engine */
                                state->regexIndex = startingRegexIndex;
                                
                                break;
                                
                            } else {
                                
                                /* Try the next instruction */
                                incrementStateMachine(state);
                                
                                if(*state->regexIndex == CODE_ANY_LOOP) {
                                    shouldContinue = 1;
                                    /* Failed to match, exit and let the state machine attempt to match after */
                                    break;
                                }
                            }
                        }
                        
                        if(shouldContinue == 1) {
                            
                            continue;
                        }
                        
                    } else if(state->overlyingState == CODE_ANY) {
                        
                        /* Loop until we have success or failure */
                        for(;;) {
                            
                            if(*(state->regexIndex+1) == *input) {
                                
                                /* Sucess! Increment state until we are AT CODE_ANY, next iteration will shift us out */
                                while(*state->regexIndex != CODE_ANY) {
                                    
                                    incrementStateMachine(state);
                                    
                                }
                                break;
                                
                            } else {
                                
                                /* Try the next instruction */
                                incrementStateMachine(state);
                                
                                if(*state->regexIndex == CODE_ANY) {
                                    
                                    /* Failed to match, disable this state */
                                    disableState(state);
                                    break;
                                }
                            }
                        }
                    } else {
                        
                        /* default */
                        *(state->regexIndex+1) == *input ? incrementStateMachine(state) : disableState(state);
                        
                    }
                    
                    break;
                    
                case CODE_MATCH_ANY_CHAR:
                    
                    
                    /* Match or failure if not char found (\0) */
                    *input ? incrementStateMachine(state) : disableState(state);
                    
                    
                    break;
                    
                case CODE_NO_MATCH:
                    
                    if(state->overlyingState == CODE_ANY_LOOP) {
                        
                        /* Loop until we have success or failure */
                        char *startingRegexIndex = state->regexIndex;
                        unsigned char shouldContinue = 0;
                        
                        for(;;) {
                            
                            if(*(state->regexIndex+1) != *input) {
                                
                                /* Sucess! Increment state until we are AT CODE_ANY */
                                while(*state->regexIndex != CODE_ANY_LOOP) {
                                    
                                    incrementStateMachine(state);
                                    
                                }
                                
                                /* Perform a jump for this successful instance */
                                #pragma message("if this is safe to remove, we should remove it from our MATCH LOOP as well above")
                                /*///////performJumpOnStateMatchine(state, JUMP_FORWARD_ONE, input);*/
                                
                                /* Reset our state regex engine */
                                state->regexIndex = startingRegexIndex;
                                
                                break;
                                
                            } else {
                                
                                /* Try the next instruction */
                                incrementStateMachine(state);
                                
                                if(*state->regexIndex == CODE_ANY_LOOP) {
                                    
                                    shouldContinue = 1;
                                    /* Failed to match, exit and let the state machine attempt to match immediately afterwards */
                                    break;
                                }
                            }
                        }
                        
                        /* Execute next line now */
                        if(shouldContinue == 1) {
                            
                            continue;
                        }
                        
                    } else if(state->overlyingState == CODE_ANY) {
                        
                        /* Loop until we have success or failure */
                        for(;;) {
                            
                            if(*(state->regexIndex+1) != *input) {
                                
                                /* No Match (continue) */
                                
                                /* Try the next instruction */
                                incrementStateMachine(state);
                                
                                if(*state->regexIndex == CODE_ANY) {
                                    
                                    /* Failed to match, Success! */
                                    break;
                                }
                                
                            } else {
                                
                                /* Match (Failure) */
                                disableState(state);
                                break;
                                
                            }
                        }
                        
                    } else {
                        
                        /* default */
                        *(state->regexIndex+1) != *input ? incrementStateMachine(state) : disableState(state);
                        
                    }
                    
                    break;
                    
                case CODE_MATCH_LOOP:
                    
                    if(state->overlyingState == CODE_ANY_LOOP) {
                        
                        /* Loop until we have success or failure */
                        char *startingRegexIndex = state->regexIndex;
                        
                        for(;;) {
                            
                            if(*(state->regexIndex+1) == *input) {
                                
                                /* Sucess! Increment state until we are AT CODE_ANY */
                                while(*state->regexIndex != CODE_ANY_LOOP) {
                                    
                                    incrementStateMachine(state);
                                    
                                }
                                
                                /* Perform a jump for this successful instance */
                                performJumpOnStateMatchine(state, JUMP_FORWARD_ONE);
                                
                                /* Reset our state regex engine */
                                state->regexIndex = startingRegexIndex;
                                
                                break;
                                
                            } else {
                                
                                /* Try the next instruction */
                                incrementStateMachine(state);
                                
                                if(*state->regexIndex == CODE_ANY_LOOP) {
                                    
                                    /* Failed to match, exit and let the state machine attempt to match after */
                                    break;
                                }
                            }
                        }
                        
                    } else {
                        
                        if(*(state->regexIndex+2) == CODE_LOOP_ANY) {
                            
                            /* Match by default (.) , and SPLIT */
                            performJumpOnStateMatchine(state, JUMP_FORWARD_ONE);
                            
                        } else if(*(state->regexIndex+1) != *input) {
                            
                            /* default */
                            incrementStateMachine(state);
                            continue;
                            
                        }
                        
                    }
                    
                    break;
                    
                case CODE_NO_MATCH_LOOP:
                    
                    /* Do nothing until we DO match */
                    if(state->overlyingState == CODE_ANY_LOOP) {
                        
                        /* Loop until we have success or failure */
                        char *startingRegexIndex = state->regexIndex;
                        
                        for(;;) {
                            
                            if(*(state->regexIndex+1) != *input) {
                                
                                /* Sucess! Increment state until we are AT CODE_ANY */
                                while(*state->regexIndex != CODE_ANY_LOOP) {
                                    
                                    incrementStateMachine(state);
                                    
                                }
                                
                                /* Perform a jump for this successful instance */
                                performJumpOnStateMatchine(state, JUMP_FORWARD_ONE);
                                
                                /* Reset our state regex engine */
                                state->regexIndex = startingRegexIndex;
                                
                                break;
                                
                            } else {
                                
                                /* Try the next instruction */
                                incrementStateMachine(state);
                                
                                if(*state->regexIndex == CODE_ANY_LOOP) {
                                    
                                    /* Failed to not-match, exit and let the state machine attempt to match after */
                                    break;
                                }
                            }
                        }
                        
                    } else {
                        
                        if(*(state->regexIndex+1) == *input) {
                            
                            /* default, and run again */
                            incrementStateMachine(state);
                            continue;
                        }
                    }
                    
                    break;
                    
                case CODE_END:
                    
                    /* Checks if we've reached the end of our input yet */
                    if(!*input) {
                        
                        /* Increment state and continue execution */
                        incrementStateMachine(state);
                        continue;
                        
                    } else {
                        
                        /* Disable State */
                        disableState(state);
                        
                    }
                    
                    break;
                    
                case CODE_DONE:
                    
                    /* Stash any captures we successfully made */
                    saveCapturesFromState(state);
                    
                    disableState(state);
                    
                    /* Dealloc all regex associated states (doesn't touch captures however) */
                    freeAllRegexStates();
                    
                    /* See if we have a successful state, if so return 1 ELSE return 0 */
                    #ifdef LAT_REGEX_DEBUG
                    printf("iterated: %lu times\n",iterations);
                    #endif
                    
                    return captureList != NULL ? 1 : 0;
                    break;
            }
            
            /* Use the next active state */
            state = state->next;
            
            while(state != NULL) {
                
                if(state->isActive == 1) {
                    
                    break;
                }
                state = state->next;
            }
        }
        
        /* No more active states. Reset to first active state from root */
        state = rootStateList;
        while(state != NULL) {
            
            if(state->isActive == 1) {
                
                break;
                
            }
            state = state->next;
            
        }
        
        if(state == NULL) {
            
            if(rootStateList->mustCaptureAtBeginning == 1) {
                
                /* No way jose! We're done now running regexes, now check if we got a result */
                break;
                
            } else {
                
                /* No more states, turn on our original, reset and try again */
                state = rootStateList;
                state->regexIndex = regex->regex;
                state->isActive = 1;
                
                /* Check if we went ahead at all */
                if(strcmp(input, originalInputTracker) != 0) {
                    
                    /* reset our input to our original input tracker, incremented by 1 */
                    input = ++originalInputTracker;
                }
                
            }
        }
        
        /* No longer at the beginning */
        isAtBeginning = 0;
        
        /* Break out on end */
        if(!*input) {
            
            break;
        }
        
    }
    
    #pragma message("TEMPORARILY removed THIS, not really sure if this is what we want however...")
    /* Reset to first active state */
    /*
     state = rootStateList;
     
     while(state != NULL) {
     if(state->isActive == 1) {
     for(;;) {
     // Equivalent to checking state->currentState post update
     
     printf("::POST::\n");
     printRegexStateDebugInfo(state);
     
     if(*state->regexIndex == CODE_DONE) {
     
     // Save all captures from this state
     saveCapturesFromState(state);
     
     disableState(state);
     break;
     
     } else if(*state->regexIndex == CODE_CAPTURE_END) {
     
     incrementStateMachine(state);
     
     } else {
     
     // break out
     break;
     }
     
     }
     
     }
     
     state = state->next;
     }
     */
    
    /* Dealloc all regex associated states (doesn't touch captures however) */
    freeAllRegexStates();
    
    /* See if we have a successful state, if so return 1 ELSE return 0 */
    
    #ifdef LAT_REGEX_DEBUG
    printf("iterated: %lu times\n",iterations);
    #endif
    
    return captureList != NULL ? 1 : 0;
}


/* Takes a given state and bumps up the command list, as well as handles anything that needs to be handled */
void incrementStateMachine(struct RegexState *state) {
    
    state->regexIndex+=LAT_REGEX_INSTRUCTION_SIZE;
}


/* Handles Jumps, Any, Any Loop, Start, and captures, things that aren't directly compared to input */
void updateStateMachine(struct RegexState *state, unsigned char isAtBeginning, char *input) {
    
    for(;;) {
        
        /* Parse the first instruction to set the current state */
        state->currentState = (unsigned char)*state->regexIndex;
        
        if(state->currentState == CODE_JUMP_SPLIT) {
            
            /* Jump & Split into concurrent state */
            performJumpOnStateMatchine( state, JUMP_INSTRUCTION);
            
        } else if(state->currentState == CODE_JUMP) {
            
            /* Jump line of execution for this state */
            performJumpOnStateMatchine( state, JUMP_INSTRUCTION_NO_SPLIT);
            
        } else if(state->currentState == CODE_ANY) {
            
            /* toggle */
            state->overlyingState = state->overlyingState == CODE_ANY ? 0 : CODE_ANY;
            
        } else if(state->currentState == CODE_ANY_LOOP) {
            
            /* toggle */
            state->overlyingState = state->overlyingState == CODE_ANY_LOOP ? 0 : CODE_ANY_LOOP;
            
        } else if(state->currentState == CODE_CAPTURE_START) {
            
            int captureIndex = 1;
            
            /* Start capturing at current mark in input */
            struct RegexCapture *captureBlock = LATAlloc(NULL, 0, sizeof(struct RegexCapture));
            struct RegexCapture *stepCapture;
            captureBlock->capture = input;
            captureBlock->next = NULL;
            captureBlock->isComplete = 0;
            
            if(state->captureBlock == NULL) {
                
                /* New capture block */
                state->captureBlock = captureBlock;
                
                
            } else {
                
                /* Loop and append */
                stepCapture = state->captureBlock;
                
                while(stepCapture->next != NULL) {
                    
                    /* Step in */
                    stepCapture = stepCapture->next;
                    captureIndex++;
                    
                    
                }
                
                /* Add our new block */
                stepCapture->next = captureBlock;
                
            }
            
            /* Indicate we are now capturing, +1 */
            state->isCapturing = (unsigned char)(state->isCapturing+1);
            state->totalCaptures++;
            
        } else if(state->currentState == CODE_CAPTURE_END) {
            
            /* capture end */
            if(state->isCapturing >= 1) {
                
                /* We are capturing, find our lowest captureBlock, and copy the contents into a newly allocated space */
                struct RegexCapture *stepCapture;
                int captureLen;
                char *captureAllocHolder;
                stepCapture = state->captureBlock;
                
                while(stepCapture->next) {
                    
                    /* Step in */
                    stepCapture = stepCapture->next;
                }
                
                captureLen = (int)(input - stepCapture->capture);
                
                /* Alloc, and copy */
                captureAllocHolder = LATAlloc(NULL, 0, (size_t)(captureLen+1));
                strncpy(captureAllocHolder, stepCapture->capture, (size_t)captureLen);
                captureAllocHolder[captureLen] = '\0';
                
                /* Replace, mark complete */
                stepCapture->capture = captureAllocHolder;
                stepCapture->isComplete = 1;
                
                /* Stop Capturing */
                state->isCapturing = state->isCapturing>0?(unsigned char)(state->isCapturing-1):0;
            }
            
        } else if(state->currentState == CODE_START) {
            
            /* start ^ */
            state->mustCaptureAtBeginning = 1;
            
            if(isAtBeginning != 1) {
                
                disableState(state);
            }
            
        } else if(state->currentState == CODE_NO_CAPTURE_START) {
            
            /* no capture start */
            
        } else {
            
            /* Machine is ready */
            break;
        }
        
        
        incrementStateMachine(state);
    }
}


/* Generates a new state machine and sets it a given instruction, denoted indirectly JumpType */
void performJumpOnStateMatchine(struct RegexState *state, JumpType jumpType) {
    
    /* Create a new state */
    struct RegexState *newState = NULL;
    int jumpSize = 1;
    
    /* Look to use a recycled and existing state from root (only if NOT jumping normally) */
    if(jumpType != JUMP_INSTRUCTION_NO_SPLIT) {
        
        struct RegexState *stepState;
        newState = rootStateList;
        
        while(newState != NULL) {
            
            if(newState->isActive == 0) {
                
                /* Use this state instead */
                break;
            }
            newState = newState->next;
        }
        
        if(newState == NULL) {
            
            /* Alloc a new state instead */
            newState = LATAlloc(NULL, 0, sizeof(struct RegexState));
            newState->next = NULL;
            newState->stateID = stateIDNum++;
            newState->totalCaptures = 0;
            
            /* Add it on the end of our list as well */
            stepState = state;
            
            while(stepState->next != NULL) {
                
                stepState = stepState->next;
                
                
            }
            
            stepState->next = newState;
            
        }
    }
    
    /* Extract num (default jump size is 1 instruction) */
    
    if(jumpType == JUMP_INSTRUCTION ||  jumpType == JUMP_INSTRUCTION_NO_SPLIT) {
        
        char jumpCord[5] = {0};
        
        /* Jump by instruction */
        
        char *jumpMark = state->regexIndex+1;
        char tmpChar = *(jumpMark+(LAT_REGEX_INSTRUCTION_SIZE-1));
        /* Close */
        *(jumpMark+(LAT_REGEX_INSTRUCTION_SIZE-1)) = '\0';
        
        jumpCord[0] = *jumpMark;
        jumpCord[1] = *(jumpMark+1);
        jumpCord[2] = *(jumpMark+2);
        jumpCord[3] = *(jumpMark+3);
        
        /* Convert hex val to int */
        jumpSize = (int)strtol(jumpCord, NULL, 16);
        
        /* Replace */
        *(jumpMark+(LAT_REGEX_INSTRUCTION_SIZE-1)) = tmpChar;
    }
    
    if(jumpType == JUMP_INSTRUCTION_NO_SPLIT) {
        
        /* Jump line of execution for this existing state */
        state->regexIndex+=(LAT_REGEX_INSTRUCTION_SIZE*jumpSize);
        
    } else  {
        
        /* Create new state, setup and jump */
        
        newState->regexIndex = state->regexIndex+(LAT_REGEX_INSTRUCTION_SIZE*jumpSize);
        
        if(state->totalCaptures >= 1) {
            
            /* Copy capture blocks from our previous state into our new one */
            struct RegexCapture *srcCapture = state->captureBlock;
            struct RegexCapture *rootNewCapture = NULL;
            struct RegexCapture *lastCapture = NULL;
            struct RegexCapture *newCapture = NULL;
            
            while(srcCapture != NULL) {
                
                /* Allocate a new capture */
                newCapture = LATAlloc(NULL, 0, sizeof(struct RegexCapture));
                
                /* set values */
                newCapture->isComplete = srcCapture->isComplete;
                newCapture->next = NULL;
                
                /* Check if this is complete, if so, we need to copy the bytes, else we can just reference it */
                if(srcCapture->isComplete == 1) {
                    
                    /* Copy the bytes */
                    newCapture->capture = LATstrdup(srcCapture->capture);
                    
                } else {
                    /* Just reference the current in process capture */
                    newCapture->capture = srcCapture->capture;
                    
                }
                
                /* Set it to our root or to our last item */
                if(rootNewCapture == NULL) {
                    
                    /* Set the root */
                    rootNewCapture = newCapture;
                    lastCapture = rootNewCapture;
                    
                    
                } else {
                    
                    /* Chain this on */
                    lastCapture->next = newCapture;
                    lastCapture = newCapture;
                    
                }
                
                /* Move to our next capture */
                srcCapture = srcCapture->next;
                
            }
            
            /* Set our new state capture block list */
            newState->captureBlock = rootNewCapture;
            
            /* Set additional information */
            newState->isCapturing = state->isCapturing;
            newState->totalCaptures = state->totalCaptures;
            
        } else {
            
            /* no prior captures */
            newState->captureBlock = NULL;
            newState->isCapturing = 0;
            newState->totalCaptures = 0;
            
        }
        
        newState->overlyingState = 0;
        newState->mustCaptureAtBeginning = 0;
        /* Mark active (on failure changes to 0) */
        newState->isActive = 1;
        
        
    }
    
}


/* Disables a given state, and frees anything underneath it in terms of capture */
void disableState(struct RegexState *state) {
    
    /* Free up captures */
    struct RegexCapture *stepCapture = state->captureBlock;
    struct RegexCapture *tmpCapture;
    
    /* Reset vars */
    state->isActive = 0;
    
    while(stepCapture) {
        
        if(stepCapture->isComplete == 1) {
            
            LATDealloc(stepCapture->capture);
        }
        
        tmpCapture = stepCapture;
        stepCapture = stepCapture->next;
        
        LATDealloc(tmpCapture);
    }
    state->captureBlock = NULL;
}


/* Deallocs all states from given root */
void freeAllRegexStates() {
    
    struct RegexState *stepState = rootStateList, *tmpState;
    
    while(stepState != NULL) {
        
        disableState(stepState);
        
        tmpState = stepState;
        stepState = stepState->next;
        LATDealloc(tmpState);
    }
    
    freeAllGroupingsAndStack();
}


/* Returns the next non-escaped and outermost closing parenthese */
char *getNextUnescapedClosingParen(char *input) {
    
    /* Default to 1 (assuming already open paren was before this) */
    unsigned int pCount = 1;
    
    /* loop until we find a valid closing paren, or return NULL */
    while(*input) {
        
        if(*input == '(' && *(input-1) != '\\') {
            
            /* increment */
            pCount++;
            
        } else if(*input == ')' && *(input-1) != '\\') {
            
            /* decrement */
            pCount--;
            
            if(pCount == 0) {
                
                /* Return the index of this closing parenthese */
                return input;
            }
        }
        
        /* bump input */
        input++;
    }
    
    /* Failed, return NULL to indicate no valid closing paren was found */
    return NULL;
}


/* Saves captures from the passed in state to a linked list, while also removing them from the state */
void saveCapturesFromState(struct RegexState *state) {
    
    struct SuccessfulCapture *curCapture;
    struct RegexCapture *regexCapture;
    
    /* Flush any existing successful capture strings */
    deleteExistingCaptures();
    
    if(captureList == NULL) {
        
        /* list is NULL, allocate */
        captureList = LATAlloc(NULL, 0, sizeof(struct RegexCapture));
        captureList->next = NULL;
        captureList->capture = NULL;
    }
    
    /* reset current capture to our list */
    curCapture = captureList;
    /* set to our capture block of our state to save */
    regexCapture = state->captureBlock;
    
    /* loop over regex captures */
    while(regexCapture != NULL) {
        
        if(regexCapture->isComplete == 1) {
            /* Valid capture to use from this regex state  */
            
            if(curCapture->capture == NULL) {
                
                /* Just for first iteration (root), basically a skip */
                
                /* Set reference */
                curCapture->capture = regexCapture->capture;
                
            } else  {
                
                /* Allocate a new successful capture and set */
                struct SuccessfulCapture *newCapture = LATAlloc(NULL, 0, sizeof(struct RegexCapture));
                newCapture->next = NULL;
                newCapture->capture = regexCapture->capture;
                
                /* Chain this on the end */
                curCapture->next = newCapture;
                
                /* Move curCapture one down */
                curCapture = newCapture;
                
            }
            
            /* free original reference */
            regexCapture->capture = NULL;
        }
        
        regexCapture = regexCapture->next;
    }
}


/* Deletes any existing captures */
void deleteExistingCaptures() {
    
    struct SuccessfulCapture *curCapture = captureList;
    struct SuccessfulCapture *tmpCap;
    
    /* Flush any existing successful capture strings */
    while(curCapture != NULL) {
        
        if(curCapture->capture != NULL) {
            
            LATDealloc(curCapture->capture), curCapture->capture = NULL;
        }
        
        tmpCap = curCapture;
        curCapture = curCapture->next;
        LATDealloc(tmpCap);
    }
    
    captureList = NULL;
}


/* Returns the capture at the given index in our linked list or NULL */
char *getCaptureByIndex(unsigned char captureIndex) {
    
    struct SuccessfulCapture *curCapture = captureList;
    unsigned char counter = 0;
    
    while(curCapture != NULL) {
        
        if(curCapture->capture != NULL) {
            
            /* Match! */
            if(counter == captureIndex) {
                
                return curCapture->capture;
            } else {
                
                counter++;
            }
            
        }
        
        curCapture = curCapture->next;
        
    }
    
    /* No captures! */
    return NULL;
    
}


int getCaptureCount() {
    
    struct SuccessfulCapture *cc = captureList;
    int count = 0;
    
    while(cc != NULL) {
        
        count++;
        cc = cc->next;
    }
    
    return count;
}


/* Frees anything that is cached by our regex engine */
void freeRegexCache() {
    
    /* Free Captures */
    int counter = 0;
    struct SuccessfulCapture *nxtCapture = captureList;
    struct SuccessfulCapture *tmpCapture;
    
    while(nxtCapture != NULL) {
        
        LATDealloc(nxtCapture->capture);
        tmpCapture = nxtCapture;
        nxtCapture = nxtCapture->next;
        LATDealloc(tmpCapture);
    }
    
    /* Free precompiled Regexes */
    
    while(counter < regexCacheCount) {
        
        LATRegex *r = regexCache[counter];
        
        if(r != NULL) {
            
            LATDealloc(r->regex);
            LATDealloc(r->stringRep);
            LATDealloc(r);
        }
        
        counter++;
    }
}
