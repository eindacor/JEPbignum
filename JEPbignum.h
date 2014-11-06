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

#define MAXDIGITS 1024
#define ONES_PLACE 100
#define DEFAULT_FLOAT_PRECISION 10

namespace jep
{
	class bignum;

	bool lessThan(const bignum &bn1, const bignum &bn2);
	bool greaterThan(const bignum &bn1, const bignum &bn2);
	bool equals(const bignum &bn1, const bignum &bn2);
	bignum addNumbers(const bignum &bn1, const bignum &bn2);
	bignum subtractNumbers(const bignum &bn1, const bignum &bn2);
	bignum multiplyNumbersSimple(const bignum &bn1, int n);
	bignum multiplyNumbers(const bignum &bn1, const bignum &bn2);
	bignum divideNumbersSimple(const bignum &bn1, const bignum &bn2, bool &remainder);
	bignum divideNumbers(const bignum &bn1, const bignum &bn2);
	bignum factorial(const bignum &bn);
	bignum combinations(const bignum &bn1, const bignum &bn2);
	bignum exponent(const bignum &base_value, const bignum &power);

	//TOADD
	//bignum logarithm(const bignum &bn1, const bignum &bn2);

	bignum modulo(const bignum &bn1, const bignum &bn2);
	void primeFactorization(const bignum &bn1, vector<bignum> &factors);
	bignum greatestCommonFactor(const bignum &bn1, const bignum &bn2);
	bignum lowestCommonMultiple(const bignum &bn1, const bignum &bn2);
	bignum root(const bignum &nth_root, const bignum &base, int precision);
	bignum fibonacci(const bignum &bn1);
	bignum golden(const bignum &bn1);
	bignum randomNumberForcePrecision(const bignum &bn1, const bignum &bn2, int forceprecision);
	bignum randomNumberAddPrecision(const bignum &bn1, const bignum &bn2, int addprecision);
	bignum average(vector<bignum> numbers_passed);

    class bignum
    {
		public:
    		bignum(vector<int> n, int offset, int set_base, bool is_negative);
			bignum(vector<int> n, int set_base, bool is_negative);
    		bignum();
    		bignum(int n);
			bignum(float f);
			bignum(float f, int decimal_places);
			bignum(double d);
			bignum(double d, int decimal_places);
    		bignum(string s);
    		bignum(string s, int baseGiven);
			explicit bignum(const bignum &bn, int desired_base) { *this = bn.getConverted(desired_base); }
    		~bignum() {};    
    
			void setDigit(int n, int s) { digits[n] = s; updateDigits(); }
			void setNegative() { negative = true; updateDigits(); }
			void setPositive() { negative = false; }
			void setBase(int n);

			int getDigit(int n) const { return digits[n]; }
			int getDigitCount() const { return digitCount; }
			int getDecimalCount() const { return decimalCount; }
    		int getBase() const {return base;}
			int getDigitRange() const { return digitRange; }	
    		string getDigitString (int n) const;
    		string getNumberString(bool include_commas, bool percent, int decimal_places) const;

			bool isZero() const {return is_zero; }
			bool isPrime() const;
			bool isNegative() const { return negative; }
    		
    		void updateDigits();
    		void convertBase(int n);
    		void convertBaseSimple(int n);
    		void leftShift (int places);
    		void rightShift (int places);

			void roundToIndex(int index);
			void roundDownToIndex(int index);
			void roundUpToIndex(int index);
			void roundAllDigitsToIndex(int index);

			const bignum getConverted(int n) const { bignum converted(*this); converted.convertBase(n); return converted; }
			const bignum getRounded(int index) const { bignum temp(*this); temp.roundToIndex(index); return temp; }
			const bignum getRoundedUp(int index) const { bignum temp(*this); temp.roundUpToIndex(index); return temp; }
			const bignum getRoundedDown(int index) const { bignum temp(*this); temp.roundDownToIndex(index); return temp; }
			const bignum getRoundedAllDigits(int index) const { bignum temp(*this); temp.roundAllDigitsToIndex(index); return temp; }

			const bignum absolute() const;
			const bignum noDecimal() const;

			const bignum operator + (const bignum &bn) const { return addNumbers(*this, bn); }
			const bignum operator - (const bignum &bn) const { return subtractNumbers(*this, bn); }
			const bignum operator * (const bignum &bn) const { return multiplyNumbers(*this, bn); }
			const bignum operator / (const bignum &bn) const { return divideNumbers(*this, bn); }
			const bignum operator % (const bignum &bn) const { return modulo(*this, bn); }

			explicit operator int() const;
			//explicit operator float() const;
			//explicit operator double() const;
			explicit operator string() const { return getNumberString(false, false, decimalCount); }

			bignum& operator += (const bignum& bn) { *this = addNumbers(*this, bn); return *this; }
			bignum& operator -= (const bignum& bn) { *this = subtractNumbers(*this, bn); return *this; }
			bignum& operator *= (const bignum& bn) { *this = multiplyNumbers(*this, bn); return *this; }
			bignum& operator /= (const bignum& bn) { *this = divideNumbers(*this, bn); return *this; }

			bool operator < (const bignum &bn) const { return lessThan(*this, bn); }
			bool operator > (const bignum &bn) const { return greaterThan(*this, bn); }

			bool operator <= (const bignum &bn) const { return !greaterThan(*this, bn); }
			bool operator >= (const bignum &bn) const { return !lessThan(*this, bn); }

			bool operator == (const bignum &bn) const { return equals(*this, bn); }
			bool operator != (const bignum &bn) const { return !equals(*this, bn); }

			void operator -- (int);
			void operator ++ (int);
			bignum& operator = (const bignum& b);
    
    	private:
			bool isDivisibleByThree() const;
			bool isDivisibleByFive() const;
			bool isDivisibleBySeven() const;

    		int digits[MAXDIGITS];
    		int digitCount;
    		int decimalCount;
    		int digitRange;
    		bool negative;
    		int base;
			bool is_zero;

			int left_most;
			int right_most;
    };

    class error_handler
    {
        public:
            error_handler(string file, int n, string s) {filename = file; line = n; message = s;}
            ~error_handler(){};
            
            string getErrorReport();
            
        private:
            string filename;
            int line;
            string message;
    };
    
}//END OF NAMESPACE JEP

#endif


