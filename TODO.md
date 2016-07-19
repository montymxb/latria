:: Latria TODO ::
    
• Consider reorganizing memory under an AVL, instead of a binary tree

• Organize Tests, document as well
    - Organize in relation to the files each test is running
    - May need to incorporate additional files as well, to split it up and make it reasonably understandable
    - Consider moving tests into a TESTS folder

• Unable to function on Android Propely (can't build due to some missing dependencies. Difficult to debug due to GDB server issues.

• Include ability to build by the configure, make, make install approach (Automake/Autoconf)

• Null comparisons are not consistent
    - Such as for data from a server, are not consistent, and frequently give an error of mismatched types being compared
    - Upon this issue being fixed test cases need to be written to prevent this from happening in the future

• Debug compiler and interpreter builds are not passing the tests

• Latria binary is blocked on windows in most cases.

• Bug with passing function calls (closures) as arguments to other function calls