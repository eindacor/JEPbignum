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
#define PRECISION 100

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
	bignum exponent(const bignum &bn1, const bignum &bn2);

	//TOADD
	//bignum logarithm(const bignum &bn1, const bignum &bn2);

	bignum modulo(const bignum &bn1, const bignum &bn2);
	bool checkPrime(const bignum &bn);
	bool divisibleByThree(const bignum &bn);
	bool divisibleByFive(const bignum &bn);
	bool divisibleBySeven(const bignum &bn);
	void primeFactorization(const bignum &bn1, vector<bignum> &factors);
	bignum greatestCommonFactor(const bignum &bn1, const bignum &bn2);
	bignum lowestCommonMultiple(const bignum &bn1, const bignum &bn2);
	bignum root(const bignum &bn1, const bignum &bn2, int decimal_places);
	bignum fibonacci(const bignum &bn1);
	bignum fibonacci(int n);
	bignum golden(const bignum &bn1);
	bignum golden(int n);
	bignum randomNumberForcePrecision(const bignum &bn1, const bignum &bn2, int forceprecision);
	bignum randomNumberAddPrecision(const bignum &bn1, const bignum &bn2, int addprecision);
	bignum average(vector<bignum> numbers_passed);
	signed long int getInt(bignum bn);
	double getDouble(bignum bn);
	float getFloat(bignum bn);

    class bignum
    {
		public:
    		bignum(vector<int> n, int offset, int set_base, bool is_negative);
			bignum(vector<int> n, int set_base, bool is_negative);
    		bignum();
    		bignum(int n);
			bignum(float f);
			bignum(double d);
    		bignum(string s);
    		bignum(string s, int baseGiven);
			explicit bignum(const bignum &bn, int baseGiven) { *this = bn.getConverted(baseGiven); }
    		~bignum() {};    
    
			void setDigit(int n, int s) { digits[n] = s; updateDigits(); }
			void setNegative() { negative = true; updateDigits(); }
			void setPositive() { negative = false; }
			void setBase(int n);

			bignum getConverted(int n) const { bignum converted(*this); converted.convertBase(n); return converted; }
			int getDigit(int n) const { return digits[n]; }
			int getDigitCount() const { return digitCount; }
			int getDecimalCount() const { return decimalCount; }
    		int getBase() const {return base;}
			int getDigitRange() const { return digitRange; }
			bool getNegative() const { return negative; }		
    		string getDigitString (int n) const;
    		string getNumberString(bool include_commas, bool percent, int decimal_places) const;
			bool getZero() const { bignum zero; zero.setBase(base); return equals(*this, zero); }
    		
    		void updateDigits();
    		void convertBase(int n);
    		void convertBaseSimple(int n);
    		void timesTen (int n);
    		void divideByTen (int n);
			void adjustPrecision(int n);
			void decrement();

			const bignum absolute() const;
			const bignum noDecimal() const;
			const bignum withoutDecimals() const;

			const bignum operator + (const bignum &bn) const { return addNumbers(*this, bn); }
			const bignum operator - (const bignum &bn) const { return subtractNumbers(*this, bn); }
			const bignum operator * (const bignum &bn) const { return multiplyNumbers(*this, bn); }
			const bignum operator / (const bignum &bn) const { return divideNumbers(*this, bn); }
			const bignum operator % (const bignum &bn) const { return modulo(*this, bn); }

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
    		int digits[MAXDIGITS];
    		int digitCount;
    		int decimalCount;
    		int digitRange;
    		bool negative;
    		int base;

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


