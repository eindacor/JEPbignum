#ifndef JEPBIGNUM_H
#define JEPBIGNUM_H

#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <time.h>

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::list;
using std::string;

#define MAXDIGITS 1000
#define PRECISION 30
#define PI "3141592653589793238462643383279"
#define THETA "1618033988749894848204586834365"

namespace jep
{

    class bignum
    {
    	public:
    		bignum(vector<int> n);
    		bignum();
    		bignum(int n);
    		bignum(string s);
    		bignum(string s, int baseGiven);
    		~bignum() {};    
    
    		int getDigit(int n) {return digits[n];}
    		void setDigit(int n, int s) {digits[n] = s; updateDigits();}
    		int getDigitCount();
    		int getDecimalCount();
    		int getBase() {return base;}
    		int getDigitRange() {return digitRange;}
    		
    		string getDigitString(int n);
    		string getNumberString(bool includeCommas, bool percent, int decimalPlaces);
    		
    		bool getNegative() {return negative;}
    		void updateDigits();
    		void convertBase(int n);
    		void convertBaseSimple(int n);
    
    		void timesTen (int n);
    		void divideByTen (int n);
    		
    		void setNegative() {negative = true;}
    		void setPositive() {negative = false;}
    		void setBase(int n) {base = n;}
    		void adjustPrecision(int n);
    		
    		bool operator < (bignum b); 
    		bool operator <= (bignum b); 
    		bool operator < (int n);
    		bool operator <= (int n);
    
    		bool operator > (bignum b);
    		bool operator >= (bignum b);
    		bool operator > (int n);
    		bool operator >= (int n);
    
    		bool operator == (bignum b); 
    		bool operator == (int n);   
    		bool operator != (bignum b);
    		bool operator != (int n); 
    
    		void operator += (bignum b);  
			void operator += (int n);
    		void operator -= (bignum b);
			void operator -= (int n);
    		void operator *= (bignum b);
			void operator *= (int n);
    		void operator /= (bignum b);
			void operator /= (int n);
    		void operator -- (int n);
    		void operator ++ (int n);
    		void operator = (bignum b);
    		void operator = (string s);
    		void operator = (int n);
    
    		bignum operator * (bignum b);
    		bignum operator * (int n);
    		bignum operator / (bignum b);
    		bignum operator / (int n);
    		bignum operator + (bignum b);
    		bignum operator + (int n);
    		bignum operator - (bignum b);
    		bignum operator - (int n);
    		bignum absolute();
    		bignum noDecimal();
    		bignum withoutDecimals();
    
    		void decrement();
    
    	private:
    		int digits[MAXDIGITS];
    		int digitCount;
    		int decimalCount;
    		int digitRange;
    		bool negative;
    		int base;
    };
    
	bignum addNumbers(bignum bn1, bignum bn2);
    bignum subtractNumbers(bignum bn1, bignum bn2);
    bignum multiplyNumbersSimple(bignum bn1, int n);
    bignum multiplyNumbers(bignum bn1, bignum bn2);
    bignum divideNumbersSimple (bignum bn1, bignum bn2, bool &r);
    bignum divideNumbers(bignum bn1, bignum bn2);
    bignum factorial(bignum bn);
    bignum combinations(bignum bn1, bignum bn2);
    bignum exponent(bignum bn1, bignum bn2);
    bignum fibonacci(bignum bn1);
    bignum fibonacci(int n);
    bignum golden(bignum bn1);
    bignum golden(int n);
    bignum randomNumberForcePrecision(bignum bn1, bignum bn2, int forceprecision);
    bignum randomNumberAddPrecision(bignum bn1, bignum bn2, int addprecision);
    bignum average(vector<bignum> numbers_passed);

    class bignum_Error
    {
        public:
            bignum_Error(string file, int n, string s) {filename = file; line = n; message = s;}
            ~bignum_Error(){};
            
            string getErrorReport();
            
        private:
            string filename;
            int line;
            string message;
    };
    
}//END OF NAMESPACE JEP

#endif


