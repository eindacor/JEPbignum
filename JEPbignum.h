#ifndef JEPBIGNUM_H
#define JEPBIGNUM_H

#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <limits>
#include <time.h>

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::list;
using std::string;
using std::numeric_limits;

#define MAXDIGITS 1000
#define PRECISION 30
#define PI "3141592653589793238462643383279"
#define THETA "1618033988749894848204586834365"

namespace jep
{

    class bignum
    {
		public:
    		bignum(vector<int> n, int offset, int set_base, bool is_negative);
    		bignum();
    		bignum(int n);
			bignum(float f);
			bignum(double d);
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
    		string getNumberString(bool include_commas, bool percent, int decimal_places);
    		
    		bool getNegative() {return negative;}
    		void updateDigits();
    		void convertBase(int n);
    		void convertBaseSimple(int n);
    
    		void timesTen (int n);
    		void divideByTen (int n);
    		
    		void setNegative() {negative = true;}
    		void setPositive() {negative = false;}
			void setBase(int n);
    		void adjustPrecision(int n);

			template <class T>
			bool operator < (T passed)
			{
				bignum b(passed);
				return lessThan(*this, b);
			}

			template <class T>
			bool operator <= (T passed)
			{
				bignum b(passed);
				return !greaterThan(*this, b);
			}
    
			template <class T>
			bool operator > (T passed)
			{
				bignum b(passed);
				return greaterThan(*this, b);
			}

			template <class T>
			bool operator >= (T passed)
			{
				bignum b(passed);
				return !lessThan(*this, b);
			}
    
			template <class T>
			bool operator == (T passed)
			{
				bignum b(passed);
				return equals(*this, b);
			}

			template <class T>
			bool operator != (T passed)
			{
				bignum b(passed);
				return !(equals(*this, b));
			}
			
    		void operator -- (int n);
    		void operator ++ (int n);
    		void operator = (bignum b);
    		void operator = (string s);
    		void operator = (int n);
    
			template <class T>
			bignum operator * (T passed)
			{
				bignum b(passed);
				return multiplyNumbers(*this, b);
			}

			template <class T>
			bignum operator / (T passed)
			{
				bignum b(passed);
				return divideNumbers(*this, b);
			}

			template <class T>
			bignum operator + (T passed)
			{
				bignum b(passed);
				return addNumbers(*this, b);
			}

			template <class T>
			bignum operator - (T passed)
			{
				bignum b(passed);
				return subtractNumbers(*this, b);
			}

			template <class T>
			void operator += (T passed)
			{
				bignum b(passed);
				*this = *this + b;
			}

			template <class T>
			void operator -= (T passed)
			{
				bignum b(passed);
				*this = *this - b;
			}

			template <class T>
			void operator *= (T passed)
			{
				bignum b(passed);
				*this = *this * b;
			}

			template <class T>
			void operator /= (T passed)
			{
				bignum b(passed);
				*this = *this / b;
			}

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
    
	bool lessThan(bignum bn1, bignum bn2);
	bool greaterThan(bignum bn1, bignum bn2);
	bool equals(bignum bn1, bignum bn2);
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
	signed long int getInt(bignum bn);
	long double getDouble(bignum bn);
	long float getFloat(bignum bn);

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


