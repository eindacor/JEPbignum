----
#BigNum library README
----


To build this as a shared library from the source files, run the following 
commands with files BigNum.cpp and BigNum.h in your working directory:

g++ -fpic -c JEPbignum.cpp
g++ -shared -o libJEPbignum.so JEPbignum.o 

To use this library, source code must #include "JEPbignum.h", and the file 
JEPbignum.so must be placed in your shared library directory. When compiling 
your object/program, be sure to link the library with -lBigNum.

----
###bignum Constructors

Function|Description
:-----------------------------------------------	|:------------------------
`bignum()`								|Creates a bignum of base 10 with the value of 0
`bignum(signed int n)`					|Creates a bignum of base 10 with the value of n
`bignum(string s)`						|Creates a bignum of base 10 with the value of s. If invalid characters are detected in string s, this constructor will throw a bignum_Error class.
`bignum(string s, int b)`					|Creates a bignum of base b with the value of s. If invalid characters are detected in string s, this constructor will throw a bignum_Error class.
`bignum(vector<int> n, int offset, int set_base, bool is_negativei)`			|Creates a bignum of base b from a vector of ints. Offset




Function List                       
--------------------------------------------------------------------------------    

jep::addNumbers(bignum bn1, bignum bn2)

    Returns the sum of bn1 and bn2 as a bignum
    
    
   
jep::subtractNumbers(bignum bn1, bignum bn2)

    Returns the difference of bn1 and bn2 as a bignum
    
    
    
jep::multiplyNumbers(bignum bn1, bignum bn2)

    Returns the product of bn1 and bn2 as a bignum
    
    
    
jep::divideNumbers(bignum bn1, bignum bn2)

    Returns the quotient of bn1 and bn2 as a bignum



jep::factorial(bignum bn)

    Returns the factorial of bn as a bignum



jep::combinations(bignum bn1, bignum bn2)

    Returns number of possible combinations when drawing bn2 instances out of bn1
    instances total
    


jep::exponent(bignum bn1, bignum bn2)

    Returns bn1 to the power of bn2 as a bignum




bignum Member Functions
--------------------------------------------------------------------------------

SAMPLE.convertBase(int n)

    Converts base of SAMPLE to n, returns void
    
    
    
SAMPLE.getNumberString(bool c, int d)

    Returns string containing the value of SAMPLE, with the precision of d
    decimal places. The string will contain commas only when c is true.



SAMPLE.absolute()

    Returns the absolute value of SAMPLE as a bignum
    
    
    
    
Accepted bignum Operators                   Return value
--------------------------------------------------------------------------------

bignum < bignum                             bool
bignum < int                                bool

bignum <= bignum                            bool
bignum <= int                               bool    

bignum > bignum                             bool
bignum > int                                bool

bignum >= bignum                            bool
bignum >= int                               bool

bignum == bignum                            bool
bignum == int                               bool

bignum != bignum                            bool
bignum != int                               bool
    
bignum += bignum                            void
bignum += int				    void
bignum -= bignum                            void
bignum -= int                               void
bignum *= bignum                            void
bignum *= int                               void
bignum /= bignum                            void
bignum /= int                               void
bignum --                                   void
bignum ++                                   void
bignum = bignum                             void
    
bignum * bignum                             bignum
bignum * int                                bignum

bignum / bignum                             bignum
bignum / int                                bignum

bignum + bignum                             bignum
bignum + int                                bignum

bignum - bignum                             bignum
bignum - int                                bignum



Misc. Information
--------------------------------------------------------------------------------

BigNumLib uses the error report class bignum_Error. To retrieve error
information, use the member function SAMPLE_ERROR.getErrorReport(), which
returns a string describing where and how the error occurred.

By default this library uses a precision of 20 decimal places. To increase the 
precision of the library to 30, 40, or 50 decimal places, you must define the 
precision when compiling the library using -D P30, -D P40, or -D P50
respectively.

This library was created by Joseph E. Pollack in 2013.
