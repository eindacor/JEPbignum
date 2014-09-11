----
#BigNum library README
----
###`bignum` Constructors

By default, failed or invalid constructors will throw a `bignum_Error` object.
Constructor|Description
:-----------------------------------------------	|:------------------------
`jep::bignum()`								|Creates a bignum of base 10 with the value of 0.
`jep::bignum(signed int n)`					|Creates a bignum of base 10 with the value of `n`.
`jep::bignum(string s)`						|Creates a bignum of base 10 with the value of `s`.
`jep::bignum(string s, int b)`					|Creates a bignum of base b with the value of `s`.
`jep::bignum(std::vector<int> n, int offset, int set_base, bool is_negativei)`			|Creates a `bignum` of base `b` from a `std::vector<int>`. The first element of the vector will be set as the right-most digit of the `bignum`, unless an offset is passed. Passing `true` as the `is_negative` value will make the `bignum` negative.



----
###Basic Mathematical `bignum` Member Functions                      
Function|Description
:-------------------------------------------------------------	|:------------------------
`jep::addNumbers(bignum bn1, bignum bn2)`			|Returns the sum of `bn1` and `bn2` as a `bignum`.
`jep::subtractNumbers(bignum bn1, bignum bn2)`		|Returns the difference of `bn1` and `bn2` as a `bignum`.
`jep::multiplyNumbers(bignum bn1, bignum bn2)`		|Returns the product of `bn1` and `bn2` as a `bignum`.
`jep::divideNumbers(bignum bn1, bignum bn2)`		|Returns the quotient of `bn1` and `bn2` as a `bignum`.
`jep::factorial(bignum bn)`							|Returns the factorial of `bn` as a `bignum`.
`jep::combinations(bignum bn1, bignum bn2)`			|Returns number of possible combinations when drawing `bn2` instances out of `bn1` instances total as a `bignum`.
`jep::exponent(bignum bn1, bignum bn2)`				|Returns `bn1` to the power of `bn2` as a `bignum`.

----
###Other `bignum` Member Functions 
Function|Description
:-------------------------------------------------------------	|:------------------------
`convertBase(int n)`									|Converts number to base `n`.
`getDigitCount()`										|Returns the number of digits, including all decimal places set by the `PRECISION` macro, as an `int`.
`getDecimalCount()`									|Returns the range of non-zero digits below the decimal point as an `int`. A `bignum` with the value 12.345 would return `3`.
`getDigitString(int n)`									|Returns the digit at location `n` as a `std::string`.
`getNumberString(bool include_commas, bool percent, int decimal_places)`	|Returns an `std::string` containing the value, with the precision of `decimal_places`. The string will contain commas only when `include_commas` is `true`. When `percent` is `true`, the string will include a % sign and be adjusted accordingly.
`timesTen(int n)`										|Multiplies the number by 10 in its native base by  shifting digits to the left `n` times.
`divideByTen(int n)`									|Divides the number by 10 in its native base by shifting digits to the right `n` times.
`absolute()`											|Returns the absolute value as a `bignum`.
`withoutDecimals()`									|Returns `bignum` with no decimal places. This function does not round or modify `*this`.
`adjustPrecision(int n)`								|Rounds the `bignum` to `n` decimal places.

----
###Acceptable Operators 
* `bignum + bignum`
* `bignum + int`
* `bignum - bignum`
* `bignum - int`
* `bignum * bignum`
* `bignum * int`
* `bignum / bignum`
* `bignum / int`
* `bignum < bignum`
* `bignum < int`
* `bignum <= bignum`
* `bignum <= int`
* `bignum > bignum`
* `bignum > int`
* `bignum >= bignum`
* `bignum >= int`
* `bignum == bignum`
* `bignum == int`
* `bignum != bignum`
* `bignum != int`   
* `bignum += bignum`  
* `bignum += int`	
* `bignum -= bignum`      
* `bignum -= int`
* `bignum *= bignum`
* `bignum *= int`
* `bignum /= bignum`
* `bignum /= int`
* `bignum --`
* `bignum ++`
* `bignum = bignum`

----
###Miscellaneous Utility Functions

Function|Description
:-------------------------------------------------------------	|:------------------------
`jep::fibonacci(bignum b)`							|Returns the fibonacci number at sequence location `b` as a `bignum`.
`jep::fibonacci(int n)`									|Returns the fibonacci number at sequence location `n` as a `bignum`.
`jep::golden(bignum b)`								|Returns the golden ratio as a `bignum`, based on fibonnaci numbers `b` and `b-1`.
`jep::golden(int n)`									|Returns the golden ratio as a `bignum`, based on fibonnaci numbers `n` and `n-1`.
`jep::randomNumberAddPrecision(bignum bn1, bignum bn2, int add_precision)`	|Returns a random number with the range of `bn1` to `bn2`, with an added resolution of `add_precision` decimal places, as a `bignum`.
`jep::randomNumberForcePrecision(bignum bn1, bignum bn2, int force_precision)`	|Returns a random number with the range of `bn1` to `bn2`, with a forced resolution of `force_precision`.
`jep::average(vector<bignum> numbers_passed)`		|Returns the average value of all values passed as a `bignum`.

----
###Building on Linux

To build this as a shared library from the source files, run the following 
commands with files BigNum.cpp and BigNum.h in your working directory:

	g++ -fpic -c JEPbignum.cpp
	g++ -shared -o libJEPbignum.so JEPbignum.o 

To use this library, source code must `#include` "JEPbignum.h", and the file 
JEPbignum.so must be placed in your shared library directory. When compiling 
your object/program, be sure to link the library with `-lJEPbignum`.

----
###Additional Information

JEPbignum uses the error report class bignum_Error. To retrieve error
information, use the member function `SAMPLE_ERROR.getErrorReport()`, which
returns a string describing where and how the error occurred.

By default this library can accommodate numbers with 1,000 digits, including 30 decimal places. These values can be modified by the `MAXDIGITS` and `PRECISION` macros in the header file JEPbignum.h, respectively. 

This library was created by Joseph E. Pollack in 2013.

jpollack320@gmail.com
