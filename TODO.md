:: Latria TODO ::

• Accessing of an undefined variable creates an OOM error, this should be more specific...

• Split up GC to create separate VM from memory allocation/management

• Setup VM?? This is compiler only or are we talking memory as well?

• We have an issue passing functions to other functions (rather than assigning to a var and passing that) does NOT work at the moment. Need to make sure functions can be properly recognized as inputs to other functions (most likely will end up in requiring extra registers)

• infinitely recursive functions leak and crash badly still

• returning a function value when there is nothing to be assigned causes a crash/leak/bug (needs to know to discard it's return value when there's nothing to tack it onto)

• Consider reorganizing memory under an AVL, instead of a binary tree

• Unable to function on Android Properly (can't build due to some missing dependencies. 
    - Difficult to debug due to GDB server issues.

• Null comparisons are not consistent
    - Such as for data from a server, are not consistent, and frequently give an error of mismatched types being compared
    - Upon this issue being fixed test cases need to be written to prevent this from happening in the future

• Latria binary is blocked on windows in most cases.

• Bug with passing function calls (closures) as arguments to other function calls

• Set up coroutines in latria (from latria_sys)

• Files must be closed manually (they are not reclaimed) or they will be leaked out. This is a courtesy notice.
    - from TODO in latria_sys.c

• BACKBURNER For Tail call recursion we need to meet a few conditions. 1. We must be in a function (i.e we have a trace we can jump 'back' to that is not null). 2. We must have just read a 'call' and it's following name. 3. If the 2 following characters we are reading are a POP_STACK and a FUNC_END command. If these criteria are met we should pop our current stack and make the jump to our function. Do not push up another returning mark to get back from our function, the existing one will work just fine

• By doing this we're leaving an invalid reference in our memory tree, to a non-existant file, may or may not be an issue
    - TODO in latria_lexical.c

• BUG from TODO in latria_gc.h, this is a rather nasty bug. When this is called the program is dead, it will inevitably segfault due to the realloc not taking place, to some degree or another. As to 'why', I have a clue, but it continues to alude me. Reduce the constant stack size and fix this when i'm ready

• REGEX Fix. Introduce GREEDY quantifier behavior, for + * among others

• REGEX Bug from TODO in latria_regex.h. 
    - There is some wierd stuff going on here...Alternation doesn't work so well when we have a few or more alternating statements

• Additional compilers we could use in .travis.yml
    - gcc
    - clang
