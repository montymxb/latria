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
 *  latria_core_object.h
 *
 *  Core object that all variables are an instance of in the latria vm
 *
 */

#ifndef latria_core_latria_core_object_h
#define latria_core_latria_core_object_h

/*Stores all core data types, numeric (float) & string for now*/
struct CoreObject {
    
    /* left object (when an array ONLY the left object is used) */
    /* NOTE, array type is a linked list in it's simplest form */
    struct CoreObject *lobj;
    
    /* right object (when an array this is always null) */
    struct CoreObject *robj;
    
    /* Shared memory for values */
    union {
        
        /* FILE * */
        FILE *_file;
        
        /* Number */
        double _fvalue;
        
        /* String */
        char *_svalue;
        
        /* Bool */
        LATBool _bvalue;
        
    } data;
    
    /* Key to fetch this value by */
    char *key;
    
    unsigned char _state; /*
                           0=double
                           1=string
                           2=bool
                           3=array
                           4=file
                           5=connection
                           */
};

#endif /* latria_core_latria_core_object_h */
