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
 */
#include "Latria_Libs.h"

/*
 *Concatenates two char pointers together and returns a pointer to the end of it
 *char bigString[1000];
 *char *p = bigString;
 *bigString[0] = '\0';
 *p = LATstrcat(p,"John, ");
 *p = LATstrcat(p,"Paul, ");
 */

/*
 * You must free the result if result is non-NULL.
 * Returns a pointer to a new string with the injected section
 */
char *str_replace(char *orig, char *rep, char *with, char removeAll) {
    char *result = NULL, *ins, *tmp; /* the return string && the next insert point && varies*/
    unsigned long len_with;  /* length of rep && len of with*/
    long len_front; /* distance between rep and end of last rep*/
    int count;    /* number of replacements*/
    size_t resultLen;
    
    /* Get length of our replacement string */
    long len_rep = (long)strlen(rep);
    
    /* Skip if original is null */
    if (!orig)
        return NULL;
    
    /* If no target string provided default to empty */
    if (!rep)
        rep = "";
    
    /* If no replacement string provided default to empty */
    if (!with)
        with = "";
    
    /* Get length of replacement string */
    len_with = strlen(with);
    
    /* Temporary pointer set to start of 'orig' char * */
    ins = orig;
    
    /* Move 'ins' to the end of a replacement match, increment count & loop until no more matches */
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }
    
    /* Alloc and set tmp & result to char * with size of our string plus the difference in size of the tgt & replacement items throughout the string */
    resultLen = strlen(orig) + (size_t)((long)len_with - len_rep) * (size_t)count + 1;
    /* strange valgrind check...less than 6 causes valgrind to freak the f**k out, from what I understand this may be due to a system optimization going on behind the scenes that's resulting in memory blocks that are about 6 apiece (possible it may be a bug or something specific on the latest (10.10.5) OSX version, have yet to test explicitly for this on linux or windows yet) */
    if(resultLen < 6) {
        resultLen = 6;
    }
    tmp = result = LATAlloc(result, 0,  resultLen * sizeof(char));
    
    /* If for some reason our alloc fails throw NULL */
    if (!result)
        return NULL;
    
    if(removeAll == 1) {
        /* Remove all instances based on count before */
        while(count--) {
            /* Set 'ins' pointer to next match in 'orig' */
            ins = strstr(orig, rep);
            /* Get the length from the front of the 'orig' string */
            len_front = ins - orig;
            /* Copy data from 'orig' to 'tmp' up to our front length, then fast forward that front length */
            tmp = strncpy(tmp, orig, (size_t)len_front) + len_front;
            /* Copy our replacement string into 'tmp', and then fast forward the width of that replacement string */
            tmp = strncpy(tmp, with, len_with) + len_with;
            /* Fast forward our 'orig' pointer past this match */
            orig += len_front + len_rep;
        }
    } else {
        /* Set 'ins' pointer to next match in 'orig' */
        ins = strstr(orig, rep);
        /* Get the length from the front of the 'orig' string */
        len_front = ins - orig;
        /* Copy data from 'orig' to 'tmp' up to our front length, then fast forward that front length */
        tmp = strncpy(tmp, orig, (size_t)len_front) + len_front;
        /* Copy our replacement string into 'tmp', and then fast forward the width of that replacement string */
        tmp = strncpy(tmp, with, len_with) + len_with;
        /* Fast forward our 'orig' pointer past this match */
        orig += len_front + len_rep;
    }
    
    /* Copy our new 'replaced' string into tmp from 'orig' */
    resultLen = strlen(orig);
    strncpy(tmp, orig, resultLen);
    tmp[resultLen] = '\0';
    
    /* Return the now modified original, basically tmp */
    return result;
}

/* Variation with str replace where NO memory allocations are made, can cause undesired effects if 'with' is LARGER than 'rep' */
char *str_replaceNCPY(char *orig, char*rep, char*with) {
    char *result = NULL, *ins, *tmp, *truOrig; /* the return string && the next insert point && varies*/
    unsigned long len_with;  /* length of rep && len of with*/
    long len_front; /* distance between rep and end of last rep*/
    
    /* Get length of our replacement string */
    unsigned long len_rep = strlen(rep);
    
    /* Skip if original is null */
    if (!orig)
        return NULL;
    
    /* If no target string provided default to empty */
    if (!rep)
        rep = "";
    
    /* If no replacement string provided default to empty */
    if (!with)
        with = "";
    
    /* Get length of replacement string */
    len_with = strlen(with);
    
    /* Temporary pointer set to start of 'orig' char * */
    truOrig = orig;
    
    /* Alloc and set tmp & result to char * with size of our string plus the difference in size of the tgt & replacement items throughout the string */
    tmp = result = LATAlloc(result, 0, (strlen(orig) + (len_with - len_rep) + 1) * sizeof(char));
    
    /* If for some reason our alloc fails throw NULL */
    if (!result)
        return NULL;
    
    /* ONLY Remove FIRST INSTANCE, we'll come back for others as we find valid ones */
    
    /* Set 'ins' pointer to next match in 'orig' */
    ins = strstr(orig, rep);
    /* Get the length from the front of the 'orig' string */
    len_front = ins - orig;
    /* Copy data from 'orig' to 'tmp' up to our front length, then fast forward that front length */
    tmp = strncpy(tmp, orig, (size_t)len_front) + len_front;
    /* Copy our replacement string into 'tmp', and then fast forward the width of that replacement string */
    tmp = strncpy(tmp, with, len_with) + len_with;
    /* Fast forward our 'orig' pointer past this match */
    orig += len_front + (long)len_rep;
    
    /* Copy our new 'replaced' string into tmp from 'orig' */
    len_front = (long)strlen(orig);
    strncpy(tmp, orig, (size_t)len_front);
    tmp[len_front] = '\0';
    
    /* Reset orig back to the beginning */
    orig = truOrig;
    
    /* Copy our result into the original string */
    len_front = (long)strlen(result);
    strncpy(orig, result, (size_t)len_front);
    orig[len_front] = '\0';
    
    /* free result */
    LATDealloc(result);
    
    /* Return the now modified original, basically tmp */
    return orig;
}

/* Checks to see if a string is compatible for math operations */
LATBool isNumeric (char * s) {
    if (s == NULL || *s == '\0')
        return false;
    if(*s == '-' || *s == '+')
        s++; /* advance if negative/positive start*/
    while(*s) {
        if(!isdigit(*s) && *s != '.' && !isspace(*s)) return false;
        s++;
    }
    return true;
}

/* Strips whitespace from the input (avoiding that which is in quotes) */
char * stripWhitespace(char *input) {
    char *i = input, *j = input;
    if(*j == '"') {
        /* Fast forward to NEXT double quote, if applicable */
        char *nDQ = strchr(j+1, '"');
        if(nDQ != NULL) {
            /* Validate */
            if(*(nDQ-1) != '\\') {
                /* Fast forward */
                int z = 0;
                int inLen = (int)(nDQ-j)+1;
                for(;z<inLen;z++) {
                    *i++ = *j++;
                }
            }
        }
    } else if(*j == '\'') {
        /* Fast forward to NEXT single quote, if applicable */
        char *nDQ = strchr(j+1, '\'');
        if(nDQ != NULL) {
            /* Validate */
            if(*(nDQ-1) != '\\') {
                /* Fast forward */
                int z = 0;
                int inLen = (int)(nDQ-j)+1;
                for(;z<inLen;z++) {
                    *i++ = *j++;
                }
            }
        }
    }
    
    /* Loop over 'j' */
    while(*j) {
        
        if(*j == '"') {
            
            /* Fast forward to NEXT double quote, if applicable */
            char *nDQ = strchr(j+1, '"');
            
            if(nDQ != NULL) {
                
                /* Validate */
                if(*(nDQ-1) != '\\') {
                    
                    /* Fast forward */
                    int z = 0;
                    int inLen = (int)(nDQ-j);
                    for(;z<inLen;z++) {
                        *i++ = *j++;
                    }
                }
            }
        } else if(*j == '\'') {
            
            /* Fast forward to NEXT single quote, if applicable */
            char *nDQ = strchr(j+1, '\'');
            
            if(nDQ != NULL) {
                
                /* Validate */
                if(*(nDQ-1) != '\\') {
                    
                    /* Fast forward */
                    int z = 0;
                    int inLen = (int)(nDQ-j);
                    for(;z<inLen;z++) {
                        *i++ = *j++;
                    }
                }
            }
        }
        
        *i = *j++;
        
        if(!isspace(*i)) {
            /* This character is NOT a space, so don't move along as expected, otherwise this will get overriden in the next pass */
            i++;
        }
        
    }
    /* Cap the end, as it will just be shorter if whitespace was present */
    *i = '\0';
    return input;
}

/* Latria String Duplicate */
char *LATstrdup(char *s) {
    char *retVal;
    size_t len = strlen(s);
    retVal = LATAlloc(NULL, 0, len+1);
    strncpy( retVal, s, len);
    retVal[len] = '\0';
    return retVal;
}

/* Latria String concatenate */
char *LATstrcat(char *d, char *s) {
    size_t slen = strlen(s);
    char *nv = strncpy( d+strlen(d), s, slen) + slen;
    *nv = '\0';
    return nv;
}
