#include "JEPbignum.h"

namespace jep
{
	//------------------
	//CLASS CONSTRUCTORS
	//------------------

	bignum::bignum()
	{
		for (int i = 0; i < MAXDIGITS; i++)
		{
			digits[i] = 0;
		}
		decimalCount = 0;
		base = 10;
		negative = false;
		updateDigits();
	}

	bignum::bignum(signed int n)
	{
		bool original_negative = (n < 0);

		if (n < 0)
			n *= -1;

		for (int i = 0; i < MAXDIGITS; i++)
			digits[i] = 0;

		for (int i = 0; i < 20; i++)
		{
			int modifier = (pow((double)10, i + 1));
			int reduced = (n % modifier);

			if (i == 0)
				digits[i + PRECISION] = reduced;

			else digits[i + PRECISION] = reduced / (pow((double)10, i));
		}

		decimalCount = 0;
		base = 10;
		updateDigits();
		negative = original_negative;
	}

	bignum::bignum(double target)
	{
		vector<int> exponent;
		vector<int> mantissa;
		bool sign = false;

		static int double_bits = 64, mantissa_delim = 52, exponent_delim = 63, exponent_bias = 1023;

		union float_converter
		{
			double d;
			unsigned long long int u;
		};

		float_converter converter;
		converter.d = target;

		unsigned long long int compare = 1;

		for (int i = 0; i < double_bits; i++)
		{
			if (i < mantissa_delim)
				mantissa.push_back(converter.u & compare == compare);

			else if (i < exponent_delim)
				exponent.push_back(converter.u & compare == compare);

			else sign = (converter.u & compare == compare);

			converter.u = converter.u >> 1;
		}

		std::reverse(mantissa.begin(), mantissa.end());
		std::reverse(exponent.begin(), exponent.end());

		bignum big_mantissa(mantissa, 2, false);
		big_mantissa.divideByTen(mantissa_delim);
		big_mantissa += 1;

		bignum big_exponent = bignum(exponent, 2, false) - bignum(exponent_bias);

		bignum temp = big_mantissa * jep::exponent(bignum(2), big_exponent);

		if (sign)
			temp.setNegative();

		temp.convertBase(10);

		*this = temp;
	}

	bignum::bignum(float target)
	{
		vector<int> exponent;
		vector<int> mantissa;
		bool sign = false;

		static int float_bits = 32, mantissa_delim = 23, exponent_delim = 31, exponent_bias = 127;

		union float_converter
		{
			float f;
			unsigned int u;
		};

		float_converter converter;
		converter.f = target;

		unsigned int compare = 1;

		for (int i = 0; i < float_bits; i++)
		{
			if (i < mantissa_delim)
				mantissa.push_back(converter.u & compare == compare);

			else if (i < exponent_delim)
				exponent.push_back(converter.u & compare == compare);

			else sign = (converter.u & compare == compare);

			converter.u = converter.u >> 1;
		}

		std::reverse(mantissa.begin(), mantissa.end());
		std::reverse(exponent.begin(), exponent.end());

		bignum big_mantissa(mantissa, 2, false);
		big_mantissa.divideByTen(mantissa_delim);
		big_mantissa += 1;

		bignum big_exponent = bignum(exponent, 2, false) - bignum(exponent_bias);
		
		bignum temp = big_mantissa * jep::exponent(bignum(2), big_exponent);

		if (sign)
			temp.setNegative();

		temp.convertBase(10);

		*this = temp;
	}

	bignum::bignum(vector<int> n, int set_base, bool is_negative)
	{
		base = set_base;
		for (int i = 0; i < MAXDIGITS; i++)
			digits[i] = 0;

		int count = (PRECISION - 1) + n.size();
		for (vector<int>::iterator i = n.begin(); i != n.end(); i++)
		{
			if (count >= MAXDIGITS || count < 0)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			if (*i >= base)
				throw error_handler(__FILE__, __LINE__, "One of the values passed is beyond the given base");

			digits[count] = (*i);
			count--;
		}

		updateDigits();
		negative = is_negative;
	}

	bignum::bignum(vector<int> n, int offset, int set_base, bool is_negative)
	{
		base = set_base;
		for (int i = 0; i < MAXDIGITS; i++)
			digits[i] = 0;

		int count = (PRECISION - 1) + n.size();
		count += offset;
		for (vector<int>::iterator i = n.begin(); i != n.end(); i++)
		{
			if (count >= MAXDIGITS || count < 0)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			if (*i >= base)
				throw error_handler(__FILE__, __LINE__, "One of the values passed is beyond the given base");

			digits[count] = (*i);
			count--;
		}

		updateDigits();
		negative = is_negative;
	}

	bignum::bignum(string s)
	{
		for (int i = 0; i < MAXDIGITS; i++)
			digits[i] = 0;

		negative = false;
		base = 10;

		vector <int> numbersToAdd;
		int numbersAdded = 0;
		int decimalNumbers = 0;
		bool decimal = false;
		int commaNumbers = 0;
		bool comma = false;

		for (int i = 0; i < s.length(); i++)
		{
			switch (s[i])
			{
			case ',':
				if (decimal == true)
					throw error_handler(__FILE__, __LINE__, "constructor failed, comma included after decimal point");

				else if (comma == true && commaNumbers != 3)
					throw error_handler(__FILE__, __LINE__, "constructor failed, improper use of commas");

				else if (comma == false && numbersAdded > 3)
					throw error_handler(__FILE__, __LINE__, "constructor failed, improper use of commas");

				else if (numbersAdded == 0)
					throw error_handler(__FILE__, __LINE__, "constructor failed, improper use of commas");

				comma = true;
				commaNumbers = 0;

				break;

			case '.':
				if (decimal == true)
					throw error_handler(__FILE__, __LINE__, "constructor failed, number contains multiple decimal points");

				else decimal = true;

				break;

			case '-':
				if (decimal == true)
					throw error_handler(__FILE__, __LINE__, "constructor failed, negative symbol included after a decimal point");

				if (numbersAdded > 0)
					throw error_handler(__FILE__, __LINE__, "constructor failed, negative symbol included after a number");

				if (comma == true)
					throw error_handler(__FILE__, __LINE__, "constructor failed, negative sign included after a comma");

				if (negative == true)
					throw error_handler(__FILE__, __LINE__, "constructor failed, number contains multiple negative symbols");

				else negative = true;
				break;

			default:
				char zero = '0';

				if (s[i] >= zero && s[i] <= zero + 9)
				{
					int digitToAdd = s[i] - zero;
					if (digitToAdd >= base)
						throw error_handler(__FILE__, __LINE__, "constructor failed, digit exceeds base desired");

					if (decimal == true)
						decimalNumbers++;

					if (comma == true)
						commaNumbers++;

					numbersAdded++;
					numbersToAdd.push_back(digitToAdd);
					break;
				}

				else throw error_handler(__FILE__, __LINE__, "constructor failed, invalid character(s) included");
				break;
			}
		}

		if (decimalNumbers > PRECISION)
			throw error_handler(__FILE__, __LINE__, "constructor failed, number has too many decimal places");

		int startingPoint = PRECISION + (numbersAdded - decimalNumbers) - 1;
		for (int i = 0; i < numbersToAdd.size(); i++)
		{
			int digitToAdd = numbersToAdd.at(i);
			int locationToSet = startingPoint - i;
			digits[locationToSet] = digitToAdd;
		}

		updateDigits();
	}

	bignum::bignum(string s, int baseGiven)
	{
		for (int i = 0; i < MAXDIGITS; i++)
			digits[i] = 0;

		negative = false;
		base = baseGiven;

		vector <int> numbersToAdd;
		int numbersAdded = 0;
		int decimalNumbers = 0;

		for (int i = 0; i < s.length(); i++)
		{
			bool decimal = false;

			switch (s[i])
			{
			case ',': break;

			case '.':
				if (decimal == true)
					throw error_handler(__FILE__, __LINE__, "constructor failed, number contains multiple decimal points");

				else decimal = true;
				break;

			case '-':
				if (negative == true)
					throw error_handler(__FILE__, __LINE__, "constructor failed, number contains multiple negative symbols");

				else negative = true;
				break;

			default:
				char zero = '0';
				char letter = 'A';

				if (s[i] >= zero && s[i] <= zero + 9)
				{
					int digitToAdd = s[i] - zero;
					if (digitToAdd >= base)
						throw error_handler(__FILE__, __LINE__, "constructor failed, digit exceeds base desired");

					if (decimal == true)
						decimalNumbers++;

					numbersAdded++;
					numbersToAdd.push_back(digitToAdd);
					break;
				}

				else if (s[i] >= letter && s[i] <= letter + 27)
				{
					int digitToAdd = s[i] - letter + 10;
					if (digitToAdd >= base)
						throw error_handler(__FILE__, __LINE__, "constructor failed, digit exceeds base desired");

					if (decimal == true)
						decimalNumbers++;

					numbersAdded++;
					numbersToAdd.push_back(digitToAdd);
					break;
				}

				else throw error_handler(__FILE__, __LINE__, "constructor failed, invalid character(s) included");
				break;
			}
		}

		if (decimalNumbers > PRECISION)
			throw error_handler(__FILE__, __LINE__, "constructor failed, number has too many decimal places");

		int startingPoint = PRECISION + (numbersAdded - decimalNumbers) - 1;
		for (int i = 0; i < numbersToAdd.size(); i++)
		{
			int digitToAdd = numbersToAdd.at(i);
			int locationToSet = startingPoint - i;
			digits[locationToSet] = digitToAdd;
		}

		updateDigits();
	}

	//----------------------
	//BASIC BIGNUM FUNCTIONS
	//----------------------

	bool equals(const bignum &bn1, const bignum &bn2)
	{
		if (bn1.getBase() != bn2.getBase())
			return equals(bn1, bn2.getConverted(bn1.getBase()));

		if (bn1.getNegative() != bn2.getNegative())
			return false;

		if (bn1.getDigitCount() != bn2.getDigitCount())
			return false;

		if (bn1.getDecimalCount() != bn2.getDecimalCount())
			return false;

		for (int i = bn1.getDigitCount(); i > 0; i--)
		{
			if (bn1.getDigit(i - 1) != bn2.getDigit(i - 1))
				return false;
		}
		return true;
	}

	bool lessThan(const bignum &bn1, const bignum &bn2)
	{
		//if bases are different, convert the second and re-evaluate
		if (bn1.getBase() != bn2.getBase())
			return lessThan(bn1, bn2.getConverted(bn1.getBase()));

		if (equals(bn1, bn2))
			return false;

		if (bn1.getNegative() == true && bn2.getNegative() == false)
			return true;

		if (bn1.getNegative() == false && bn2.getNegative() == true)
			return false;

		if (bn1.getNegative() == true && bn2.getNegative() == true)
			return (greaterThan(bn1.absolute(), bn2.absolute()));

		if (bn1.getDigitCount() < bn2.getDigitCount())
			return true;

		if (bn1.getDigitCount() > bn2.getDigitCount())
			return false;

		for (int i = bn1.getDigitCount() - 1; i >= 0; i--)
		{
			if (bn1.getDigit(i) < bn2.getDigit(i))
				return true;

			if (bn1.getDigit(i) > bn2.getDigit(i))
				return false;
		}

		return false;
	}

	bool greaterThan(const bignum &bn1, const bignum &bn2)
	{
		//if bases are different, convert the second and re-evaluate
		if (bn1.getBase() != bn2.getBase())
			return greaterThan(bn1, bn2.getConverted(bn1.getBase()));

		if (equals(bn1, bn2))
			return false;

		if (bn1.getNegative() == true && bn2.getNegative() == false)
			return false;

		if (bn1.getNegative() == false && bn2.getNegative() == true)
			return true;

		if (bn1.getNegative() == true && bn2.getNegative() == true)
			return (lessThan(bn1.absolute(), bn2.absolute()));

		if (bn1.getDigitCount() > bn2.getDigitCount())
			return true;

		if (bn1.getDigitCount() < bn2.getDigitCount())
			return false;

		for (int i = bn1.getDigitCount() - 1; i >= 0; i--)
		{
			if (bn1.getDigit(i) > bn2.getDigit(i))
				return true;

			if (bn1.getDigit(i) < bn2.getDigit(i))
				return false;
		}

		return false;
	}

	bignum addNumbers(const bignum &bn1, const bignum &bn2)
	{
		if (bn1.getBase() != bn2.getBase())
			return addNumbers(bn1, bn2.getConverted(bn1.getBase()));

		//evaluate the numbers being equal
		if (equals(bn1.absolute(), bn2.absolute()))
		{
			//	-12 + 12 or 12 + -12 ---> 0
			if (bn1.getNegative() != bn2.getNegative())
			{
				bignum temp;
				temp.setBase(bn1.getBase());
				return temp;
			}

			// -12 + -12 ---> -(12 + 12)
			if (bn1.getNegative() == true && bn2.getNegative() == true)
			{
				bignum temp(addNumbers(bn1.absolute(), bn2.absolute()));
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}
		}

		//evaluate the numbers if absolute first is larger than absolute second
		if (greaterThan(bn1.absolute(), bn2.absolute()))
		{
			//	-12 + 8 ---> -(12 - 8)
			if (bn1.getNegative() == true && bn2.getNegative() == false)
			{
				bignum temp = subtractNumbers(bn1.absolute(), bn2.absolute());
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}

			//	12 + -8 ---> 12 - 8
			if (bn1.getNegative() == false && bn2.getNegative() == true)
				return subtractNumbers(bn1.absolute(), bn2.absolute());

			//	-12 + -8 ---> -(12 + 8)
			if (bn1.getNegative() == true && bn2.getNegative() == true)
			{
				bignum temp = addNumbers(bn1.absolute(), bn2.absolute());
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}
		}

		//evaluate the numbers if absolute first is smaller than absolute second
		if (lessThan(bn1.absolute(), bn2.absolute()))
		{
			//	-8 + 12 ---> 12 - 8
			if (bn1.getNegative() == true && bn2.getNegative() == false)
			{
				bignum temp = subtractNumbers(bn2.absolute(), bn1.absolute());
				temp.updateDigits();
				return temp;
			}

			//	8 + -12 ---> 8 - 12
			if (bn1.getNegative() == false && bn2.getNegative() == true)
				return subtractNumbers(bn1.absolute(), bn2.absolute());

			// -8 + -12 ---> -(8 + 12)
			if (bn1.getNegative() == true && bn2.getNegative() == true)
			{
				bignum temp = addNumbers(bn1.absolute(), bn2.absolute());
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}
		}

		vector<int> temp;
		int carry = 0;
		int digits = 0;
		int decimal = 0;

		//sets decimal and digit values to the highest of each number
		decimal = (bn1.getDecimalCount() > bn2.getDecimalCount() ? bn1.getDecimalCount() : bn2.getDecimalCount());
		digits = (bn1.getDigitCount() > bn2.getDigitCount() ? bn1.getDigitCount() + 1 : bn2.getDigitCount() + 1);

		bignum sum;
		int base = bn1.getBase();

		for (int i = (PRECISION - decimal); i < digits + 1; i++)
		{
			//verify function isn't checking beyond bounds of the stored array
			if (i >= MAXDIGITS || i < 0)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			int tempNumber = bn1.getDigit(i) + bn2.getDigit(i);

			tempNumber += carry;

			if (tempNumber>(base - 1))
			{
				tempNumber -= base;
				carry = 1;
			}

			else carry = 0;

			sum.setDigit(i, tempNumber);
		}

		sum.updateDigits();
		sum.setBase(base);
		return sum;
	}

	bignum subtractNumbers(const bignum &bn1, const bignum &bn2)
	{
		if (bn1.getBase() != bn2.getBase())
			return subtractNumbers(bn1, bn2.getConverted(bn1.getBase()));

		int base = bn1.getBase();
		bignum difference;
		difference.setBase(base);

		//evaluate the numbers being of equal absolute value
		if (equals(bn1.absolute(), bn2.absolute()))
		{
			//	-12 - 12 ---> -(12 + 12)
			if (bn1.getNegative() == true && bn2.getNegative() == false)
			{
				bignum temp = addNumbers(bn1.absolute(), bn2.absolute());
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}

			//	12 - -12 ---> 12 + 12
			if (bn1.getNegative() == false && bn2.getNegative() == true)
				return addNumbers(bn1.absolute(), bn2.absolute());

			//	-12 - -12 ---> 0
			if (bn1.getNegative() == true && bn2.getNegative() == true)
			{
				bignum temp(0);
				return temp;
			}
		}

		//evaluate the numbers if absolute first is larger than absolute second
		if (greaterThan(bn1.absolute(), bn2.absolute()))
		{
			//	-12 - 8 ---> -(12 + 8)
			if (bn1.getNegative() == true && bn2.getNegative() == false)
			{
				bignum temp = addNumbers(bn1.absolute(), bn2.absolute());
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}

			//	12 - -8 ---> 12 + 8
			if (bn1.getNegative() == false && bn2.getNegative() == true)
				return addNumbers(bn1.absolute(), bn2.absolute());

			//	-12 - -8 ---> -(12 - 8)
			if (bn1.getNegative() == true && bn2.getNegative() == true)
			{
				bignum temp = subtractNumbers(bn1.absolute(), bn2.absolute());
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}
		}

		//evaluate the numbers if absolute first is smaller than absolute second
		if (lessThan(bn1.absolute(), bn2.absolute()))
		{
			//	8 - 12 ---> -(12 - 8)
			if (bn1.getNegative() == false && bn2.getNegative() == false)
			{
				bignum temp = subtractNumbers(bn2.absolute(), bn1.absolute());
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}

			//	-8 - 12 ---> -(8 + 12)
			if (bn1.getNegative() == true && bn2.getNegative() == false)
			{
				bignum temp = addNumbers(bn1.absolute(), bn2.absolute());
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}

			//	8 - -12 ---> 8 + 12
			if (bn1.getNegative() == false && bn2.getNegative() == true)
				return addNumbers(bn1.absolute(), bn2.absolute());

			//	-8 - -12 ---> (12 - 8)
			if (bn1.getNegative() == true && bn2.getNegative() == true)
				return subtractNumbers(bn2.absolute(), bn1.absolute());
		}

		vector<int> temp;
		int carry = 0;
		int digits = 0;
		int decimal = 0;
		//bool carry_negative = false;

		//sets decimal and digit values to the highest of each number
		decimal = (bn1.getDecimalCount() > bn2.getDecimalCount() ? bn1.getDecimalCount() : bn2.getDecimalCount());
		digits = (bn1.getDigitCount() > bn2.getDigitCount() ? bn1.getDigitCount() + 1 : bn2.getDigitCount() + 1);

		for (int i = (PRECISION - decimal); i < digits + 1; i++)
		{
			//verify function isn't checking beyond bounds of the stored array
			if (i >= MAXDIGITS || i < 0)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			int tempNumber = bn1.getDigit(i) - bn2.getDigit(i);

			tempNumber -= carry;

			if (tempNumber < 0)
			{
				tempNumber += base;
				carry = 1;
			}

			else carry = 0;

			difference.setDigit(i, tempNumber);
		}

		difference.updateDigits();
		difference.setBase(base);
		return difference;
	}

	bignum multiplyNumbersSimple(const bignum &bn1, int n)
	{
		if (n == 0)
		{
			bignum zero;
			zero.setBase(bn1.getBase());
			return zero;
		}

		bignum temp(bn1);

		//if both numbers are negative, make the result positive
		if (bn1.getNegative() == n < 0)
			temp.setPositive();

		//add the first number to itself n times
		for (int i = 0; i < (n - 1); i++)
			temp += bn1;

		temp.updateDigits();
		return temp;
	}

	bignum multiplyNumbers(const bignum &bn1, const bignum &bn2)
	{
		if (bn1.getBase() != bn2.getBase())
			return multiplyNumbers(bn1, bn2.getConverted(bn1.getBase()));

		bignum temp(0);
		temp.setBase(bn1.getBase());

		//if either number is 0, return 0
		if (bn1.getZero() || bn2.getZero())
			return temp;

		//multiply bn1 by each digit of bn2 independently, then add the values together
		int counter = bn2.getDigitRange();
		for (int i = 0; i < counter; i++)
		{
			int toMultiply = (PRECISION - bn2.getDecimalCount()) + i;

			//verify function isn't checking beyond bounds of the stored array
			if (toMultiply >= MAXDIGITS || toMultiply < 0)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			bignum toAdd = multiplyNumbersSimple(bn1.absolute(), bn2.getDigit(toMultiply));

			//verify toAdd would not overstep bounds
			if (toAdd.getDigitCount() == MAXDIGITS && i > 0)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			toAdd.timesTen(i);
			temp += toAdd;
		}

		if (bn1.getNegative() != bn2.getNegative())
			temp.setNegative();

		//adjust for added decimal places during multiplication
		temp.divideByTen(bn2.getDecimalCount());

		temp.updateDigits();
		return temp;
	}

	bignum divideNumbersSimple(const bignum &bn1, const bignum &bn2, bool &remainder)
	{
		bignum temp(bn1);
		bignum counter;
		counter.setBase(bn1.getBase());

		while (!lessThan(temp, bn2))
		{
			temp -= bn2;
			counter++;
		}

		//adjusts bool passed for remainder
		if (equals(counter * bn2, temp))
			remainder = false;

		else remainder = true;

		return counter;
	}

	bignum divideNumbers(const bignum &bn1, const bignum &bn2)
	{
		//throws an exception if the program is attempting to divide by zero
		if (bn2.getZero())
			throw error_handler(__FILE__, __LINE__, "Cannot divide a number by zero");

		bignum temp;
		bool negative_result = (bn1.getNegative() != bn2.getNegative());

		if (bn1.getBase() != bn2.getBase())
			return divideNumbers(bn1, bn2.getConverted(bn1.getBase()));

		//set base of the return value to match that of the passed values
		int baseSet = bn1.getBase();
		temp.setBase(baseSet);

		bool remainder = false;
		bool end = false;
		int marker = bn1.getDigitCount() - 1;

		//starting with the left-most digit, create a bignumber of that digit that matches the set base
		bignum number_to_compare(bn1.getDigit(marker));
		number_to_compare.setBase(baseSet);

		//ignore decimal places when comparing dividend to digits of the divisor
		bignum nextNumber = divideNumbersSimple(number_to_compare, bn2.absolute().noDecimal(), remainder);
		bignum number_to_subtract;
		number_to_subtract.setBase(baseSet);

		while (end != true)
		{
			if (remainder == false && marker < PRECISION - bn1.getDecimalCount())
				end = true;

			if (marker >= MAXDIGITS || marker < 0)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			temp.setDigit(marker, nextNumber.getDigit(PRECISION));
			marker--;

			number_to_subtract = bn2.absolute().noDecimal() * nextNumber;
			number_to_subtract.updateDigits();
			number_to_compare -= number_to_subtract;
			number_to_compare.timesTen(1);

			if (marker >= MAXDIGITS)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			if (marker >= 0)
				number_to_compare += bn1.getDigit(marker);

			nextNumber = divideNumbersSimple(number_to_compare, bn2.absolute().noDecimal(), remainder);

			if (marker < 0)
				end = true;
		}

		if (negative_result && temp != 0)
			temp.setNegative();

		temp.timesTen(bn2.getDecimalCount());
		return temp;
	}

	bignum factorial(const bignum &bn)
	{
		if (!lessThan(bn, bignum(500)))
			throw error_handler(__FILE__, __LINE__, "The desired calculation is too large");

		bignum temp(bn);

		for (bignum counter(bn); greaterThan(counter, (int)1); counter--)
		{
			if (greaterThan(bn, counter))
				temp *= counter;
		}

		temp.updateDigits();
		return temp;
	}

	bignum combinations(const bignum &bn1, const bignum &bn2)
	{
		if (bn1.getBase() != bn2.getBase())
			return combinations(bn1, bn2.getConverted(bn1.getBase()));

		bignum first = factorial(bn1);
		bignum second = bn1 - bn2;
		bignum third = factorial(second);
		bignum fourth = factorial(bn2);
		bignum fifth = third * fourth;

		bignum temp = divideNumbers(first, fifth);

		return temp;
	}

	bignum exponent(const bignum &bn1, const bignum &bn2)
	{
		if (bn1.getBase() != bn2.getBase())
			return exponent(bn1, bn2.getConverted(bn1.getBase()));

		bignum one(1);
		one.setBase(bn1.getBase());
		one.setPositive();

		if (bn2.getDecimalCount() > 0)
			throw error_handler(__FILE__, __LINE__, "Cannot use decimals as exponential powers");

		bignum counter = bn2.absolute();
		bignum temp(bn1);

		//n^0 always returns 1
		if (bn2.getZero())
			return one;

		while (greaterThan(counter, one))
		{
			temp *= bn1;
			counter--;
		}

		//if the power is negative, return 1/solution
		if (bn2.getNegative() == true)
			temp = divideNumbers(one, temp);

		return temp;
	}

	//----------------
	//BIGNUM OPERATORS
	//----------------

	bignum& bignum::operator = (const bignum& b)
	{
		if (this == &b)
			return *this;

		base = b.getBase();

		int highestDigits = 0;
		int decimal = 0;

		highestDigits = (digitCount < b.getDigitCount() ? b.getDigitCount() : digitCount);

		decimal = (decimalCount < b.getDecimalCount() ? b.getDecimalCount() : decimalCount);

		for (int i = (PRECISION - decimal); i < highestDigits; i++)
		{
			if (i >= MAXDIGITS)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			digits[i] = b.getDigit(i);
		}

		negative = b.getNegative();

		updateDigits();

		return *this;
	}

	//-----------------
	//GENERAL UTILITIES
	//-----------------

	//FUNCTION FOR CHANGING THE BASE OF A BIGNUM MANUALLY
	void bignum::setBase(int n)
	{
		int marker = PRECISION - getDecimalCount();

		for (int i = 0; i < digitRange; i++)
		{
			if (digits[marker + i] >= n)
				throw error_handler(__FILE__, __LINE__, "the specified base is smaller than one or more of the bignum digits, could not set base manually");
		}

		base = n;
	}

	//FUNCTION FOR CONVERTING BASES BY COUNTING UP/DOWN IN PARALLEL
	void bignum::convertBaseSimple(int n)
	{
		if (base != n)
		{
			bool original_negative = negative;

			bignum zero;
			zero.setBase(base);
			bignum counter(absolute());
			bignum converted;
			converted.setBase(n);

			while (greaterThan(counter, zero))
			{
				converted++;
				counter--;

				//cout << "line " << __LINE__ << ": " << converted.getNumberString(false, false, 0) << endl;
				//cout << "line " << __LINE__ << ": " << counter.getNumberString(false, false, 0) << endl;
			}

			*this = converted;

			negative = original_negative;
		}

		updateDigits();
	}

	void bignum::convertBase(int n)
	{
		bool original_negative = negative;

		bignum toAdd;
		bignum temp(0);
		temp.setBase(n);

		if (base != n)
		{
			for (int i = 0; i < digitRange ; i++)
			{
				//start marker at the left-most digit and continue through all digits
				int marker(left_most - i);

				if (marker >= MAXDIGITS || marker < 0)
					throw error_handler(__FILE__, __LINE__, "void bignum::convertBase(int n): The program has attempted to calculate a value outside of its limits");

				//convert individual digit to a different base
				bignum converted_digit(getDigit(marker));
				converted_digit.convertBaseSimple(n);

				//create a multiplier based on the position of the digit (Nth power)
				bignum ten(10);
				ten.setBase(base);
				ten.convertBaseSimple(n);
				bignum nth(marker - PRECISION);
				nth.convertBaseSimple(n);

				bignum multiplier = exponent(ten, nth);

				//add value to the temporary return value
				toAdd = multiplyNumbers(converted_digit, multiplier);

				temp += toAdd;
			}
			*this = temp;
		}
		negative = original_negative;
		updateDigits();
	}

	//returns a string char from an int passed
	string bignum::getDigitString(int n) const
	{
		string temp;

		if (n < 10)
		{
			char toAdd = '0';
			toAdd += n;
			temp += toAdd;
			return temp;
		}

		else
		{
			char toAdd = 'A';
			toAdd += (n - 10);
			temp += toAdd;
			return temp;
		}
	}

	//returns a string representing stored value
	string bignum::getNumberString(bool include_commas, bool percent, int decimal_places) const
	{
		bignum temp(*this);
		string tempString;

		if (decimal_places > PRECISION)
			decimal_places = PRECISION;

		temp.updateDigits();

		bignum zero(0);
		zero.setBase(base);
		if (temp == zero)
		{
			tempString += "0";

			for (int i = 0; i < decimal_places; i++)
				tempString += (i == 0 ? ".0" : "0");

			if (percent == true)
				tempString += "%";

			return tempString;
		}

		if (percent)
		{
			bignum hundred(100);
			hundred.setBase(base);
			temp *= hundred;
		}


		int comma = (temp.getDigitCount() - PRECISION) % 3;

		if (negative == true)
			tempString += "-";

		temp.adjustPrecision(decimal_places);

		int counter = temp.getDigitRange();
		for (int i = 0; i < counter; i++)
		{
			int target = temp.getDigitCount() - i - 1;

			if (target == (PRECISION - 1))
				tempString += ".";

			else if (comma == 0 && (temp.getDigitCount() - i) >= PRECISION)
			{
				if (i>0 && include_commas == true)
					tempString += ",";

				comma = 3;
			}

			tempString += temp.getDigitString(temp.getDigit(target));

			comma--;
		}

		if (percent == true)
			tempString += "%";

		return tempString;
	}

	void bignum::operator -- (int n)
	{
		bignum one(1);
		one.setBase(base);
		*this = subtractNumbers(*this, one);
	}

	void bignum::operator ++ (int n)
	{
		bignum one(1);
		one.setBase(base);
		*this = addNumbers(*this, one);
	}

	//multiplies the number by 10 in its native base by literally shifting digit
	void bignum::timesTen(int n)
	{
		if (digitCount >= MAXDIGITS - 1)
			throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

		for (int i = 0; i < n; i++)
		{
			for (int c = 0; c < digitRange; c++)
				digits[digitCount - c] = digits[left_most - c];

			digits[right_most] = 0;
			updateDigits();
		}
		updateDigits();
	}

	//divides the number by 10 in its native base by literally shifting digit
	void bignum::divideByTen(int n)
	{
		for (int i = n; i > 0; i--)
		{
			for (int c = right_most; c <= digitCount; c++)
			{
				if (c == 0)
					continue;

				digits[c - 1] = digits[c];
			}
			updateDigits();
		}
		updateDigits();
	}

	//refreshes digitCount and decimalCount
	void bignum::updateDigits()
	{
		digitCount = 0;
		decimalCount = 0;

		for (int i = MAXDIGITS - 1; i > 0; i--)
		{
			if (i == PRECISION)
			{
				digitCount = PRECISION + 1;
				left_most = i;
				break;
			}

			if (digits[i] > 0)
			{
				digitCount = (i + 1);
				left_most = i;
				break;
			}
		}

		for (int i = 0; i < PRECISION; i++)
		{
			if (digits[i]>0)
			{
				decimalCount = PRECISION - i;
				right_most = i;
				break;
			}

			if (i == PRECISION - 1)
				right_most = PRECISION;
		}

		if (decimalCount == 0 && digitCount == (PRECISION + 1))
		{
			if (digits[PRECISION] == 0)
				setPositive();

			right_most = PRECISION;
		}

		//sets number of operative digits
		//	12.077 ---> digitRange = 5
		digitRange = left_most - right_most + 1;
	}

	//returns absolute value of bignum
	const bignum bignum::absolute() const
	{
		bignum temp = *this;
		temp.updateDigits();
		temp.setPositive();
		return temp;
	}

	//used in calculation for long division
	const bignum bignum::noDecimal() const
	{
		bignum temp(*this);
		temp.timesTen(getDecimalCount());
		return temp;
	}

	//returns bignum with no decimal places
	const bignum bignum::withoutDecimals() const
	{
		bignum temp(*this);

		for (int i = 0; i < PRECISION; i++)
			temp.setDigit(i, 0);

		return temp;
	}

	//rounds the number to a specific number of decimal places
	void bignum::adjustPrecision(int n)
	{
		int marker = PRECISION - n;

		if (digits[marker - 1] >= (base / 2))
		{
			bignum increase;
			increase.setBase(base);
			increase.setDigit(marker, 1);

			(*this) += increase;
			updateDigits();
		}

		for (int i = 0; i < marker; i++)
			digits[i] = 0;

		updateDigits();
	}

	bignum golden(const bignum &b)
	{
		bignum temp1 = bignum(fibonacci(b));
		bignum temp2 = bignum(fibonacci(b - 1));

		return (temp2 == 0 ? 0 : temp1 / temp2);
	}

	bignum golden(int n)
	{
		bignum temp(n);
		return golden(temp);
	}

	//return fibonacci number at location b of the sequence
	bignum fibonacci(const bignum &b)
	{
		if (b.getDecimalCount() > 0)
			throw error_handler(__FILE__, __LINE__, "fibonacci sequence values may only be derived from integers");

		bignum counter;
		bignum high;
		bignum low;
		bignum temp;
		bignum zero;

		counter.setBase(b.getBase());
		high.setBase(b.getBase());
		low.setBase(b.getBase());

		if (b > bignum(bignum(), b.getBase()))
		{
			high++;

			while (lessThan(counter, b))
			{
				temp = low;
				low = high;
				high += temp;
				counter++;
			}

			return low;
		}

		else if (b < bignum(bignum(), b.getBase()))
		{
			low++;

			while (lessThan(counter, b.absolute()))
			{
				temp = high;
				high = low;
				low = (temp - low);
				counter++;
			}

			return high;
		}

		return temp;
	}

	bignum fibonacci(int n)
	{
		bignum temp(n);
		return fibonacci(temp);
	}

	//returns random number within range with added resolution
	bignum randomNumberAddPrecision(const bignum &bn1, const bignum &bn2, int add_precision)
	{
		if (add_precision > PRECISION)
			throw error_handler(__FILE__, __LINE__, "specified precision was too fine");

		if (bn1.getBase() != bn2.getBase())
			throw error_handler(__FILE__, __LINE__, "random numbers can only be generated between numbers of the same base");

		if (bn1 == bn2)
			return bn1;

		bignum temp;
		bignum difference;
		difference.setBase(bn1.getBase());
		temp.setBase(bn1.getBase());

		difference = (bn1 > bn2 ? bn1 - bn2 : bn2 - bn1);

		int start = difference.getDigitCount() - 1;
		int tempdigit;

		int counter = (add_precision > 0 ? difference.getDigitRange() + add_precision : difference.getDigitRange());

		for (int i = 0; i < counter; i++)
		{
			int marker = (start - i);
			tempdigit = (rand() % difference.getBase());
			temp.setDigit(marker, tempdigit);
		}

		bignum increment;
		increment.setBase(difference.getBase());
		increment.setDigit((difference.getDigitCount() - counter), 1);

		difference += increment;

		while (temp >= difference && temp.getNegative() == false)
			temp -= difference;

		return (bn1 > bn2 ? bn2 + temp : bn1 + temp);
	}

	//returns random number within range with a specified resolution
	bignum randomNumberForcePrecision(const bignum &bn1, const bignum &bn2, int force_precision)
	{
		if (force_precision > PRECISION)
			throw error_handler(__FILE__, __LINE__, "specified precision was too fine");

		if (bn1.getBase() != bn2.getBase())
			throw error_handler(__FILE__, __LINE__, "random numbers can only be generated between numbers of the same base");

		if (bn1 == bn2)
			return bn1;

		bignum temp;
		bignum difference;
		difference.setBase(bn1.getBase());
		temp.setBase(bn1.getBase());

		bignum bn1_adjusted(bn1);
		bn1_adjusted.adjustPrecision(force_precision);

		bignum bn2_adjusted(bn1);
		bn2_adjusted.adjustPrecision(force_precision);

		difference = (bn1_adjusted > bn2_adjusted ? bn1_adjusted - bn2_adjusted : bn2_adjusted - bn1_adjusted);

		int start = difference.getDigitCount() - 1;
		int tempdigit;

		int counter = (difference.getDigitCount() - (PRECISION - force_precision));

		for (int i = 0; i < counter; i++)
		{
			int marker = (start - i);
			tempdigit = (rand() % difference.getBase());
			temp.setDigit(marker, tempdigit);
		}

		bignum increment;
		increment.setBase(difference.getBase());
		increment.setDigit((difference.getDigitCount() - counter), 1);

		difference += increment;

		while (!lessThan(temp, difference))
			temp -= difference;

		return (bn1_adjusted > bn2_adjusted ? bn2_adjusted + temp : bn1_adjusted + temp);
	}

	signed long int getInt(bignum bn)
	{
		bignum min(numeric_limits<int>::min());
		bignum max(numeric_limits<int>::max());

		signed long int temp = 0;
		bn.convertBase(10);

		if (bn > max || bn < min)
			throw error_handler(__FILE__, __LINE__, "the targeted bignum is too large to convert to an integer");

		for (int i = PRECISION; i < bn.getDigitCount(); i++)
		{
			int power = (pow((double)10, i - PRECISION));
			int toAdd = bn.getDigit(i) * power;
			temp += toAdd;
		}

		return (bn.getNegative() == true ? temp * -1 : temp);
	}

	/*
	long double getDouble(bignum bn)
	{
		bignum min(numeric_limits<double>::min());
		bignum max(numeric_limits<double>::max());

		double temp = 0;
		bn.convertBase(10);

		if (bn > max || bn < min)
			throw error_handler(__FILE__, __LINE__, "the targeted bignum is too large to convert to a double");

		for (int i = PRECISION; i < bn.getDigitCount(); i++)
		{
			int power = (pow((double)10, i - PRECISION));
			int toAdd = bn.getDigit(i) * power;
			temp += toAdd;
		}

		return (bn.getNegative() == true ? temp * -1 : temp);
	}

	long float getFloat(bignum bn)
	{
		bignum min(numeric_limits<float>::min());
		bignum max(numeric_limits<float>::max());

		float temp = 0;
		bn.convertBase(10);

		if (bn > max || bn < min)
			throw error_handler(__FILE__, __LINE__, "the targeted bignum is too large to convert to a double");

		for (int i = PRECISION; i < bn.getDigitCount(); i++)
		{
			int power = (pow((double)10, i - PRECISION));
			int toAdd = bn.getDigit(i) * power;
			temp += toAdd;
		}

		return (bn.getNegative() == true ? temp * -1 : temp);
	}
	*/

	bignum average(vector<bignum> numbers_passed)
	{
		bignum counter((int)numbers_passed.size());
		bignum total;

		for (int i = 0; i < numbers_passed.size(); i++)
			total += numbers_passed.at(i);

		if (total == bignum(0) || counter == bignum(0))
			return bignum(0);

		return divideNumbers(total, counter);
	}

	string error_handler::getErrorReport()
	{
		string temp;

		temp += filename;
		temp += " (line ";

		bignum line_number(line);

		temp += line_number.getNumberString(true, false, 0);

		temp += "): ";
		temp += message;

		return temp;
	}

} //END OF NAMESPACE JEP
