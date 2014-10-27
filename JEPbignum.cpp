#include "JEPbignum.h"

namespace jep
{
	//------------------
	//CLASS CONSTRUCTORS
	//------------------

	bignum::bignum()
	{
		for (int i = 0; i < MAXDIGITS; i++)
			digits[i] = 0;

		decimalCount = 0;
		base = 10;
		negative = false;
		updateDigits();
	}

	bignum::bignum(int n)
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
		for (int i = 0; i < MAXDIGITS; i++)
			digits[i] = 0;

		updateDigits();

		vector<int> exponent;
		vector<int> mantissa;
		bool sign = false;

		static int double_bits = 64, mantissa_delim = 52, exponent_delim = 63, exponent_bias = 1023;

		union double_converter
		{
			double d;
			unsigned long long int u;
		};

		double_converter converter;
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
		for (int i = 0; i < MAXDIGITS; i++)
			digits[i] = 0;

		updateDigits();

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

		bignum big_exponent = bignum(exponent, 2, false) - bignum(exponent_bias);
		bignum mantissa_multiplier = jep::exponent(bignum(2), big_exponent);
		mantissa_multiplier.convertBase(2);
		big_mantissa += 1;

		bignum temp = big_mantissa * mantissa_multiplier;

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

				else
				{
					decimal = true;
				}

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

				else
				{
					negative = true;
				}
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

				else
				{
					throw error_handler(__FILE__, __LINE__, "constructor failed, invalid character(s) included");
				}
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

		if (bn1 == bn2)
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

		if (bn1 == bn2)
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

		if (bn1.absolute() == bn2.absolute())
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

		if (bn1.absolute() > bn2.absolute())
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

		if (bn1.absolute() < bn2.absolute())
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
		if (bn1.absolute() == bn2.absolute())
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
		if (bn1.absolute() > bn2.absolute())
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
		if (bn1.absolute() < bn2.absolute())
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

		while (temp >= bn2)
		{
			temp -= bn2;
			counter++;
		}

		//adjusts bool passed for remainder
		if ((counter * bn2) == temp)
			remainder = false;

		else remainder = true;

		return counter;
	}

	bignum divideNumbers(const bignum &bn1, const bignum &bn2)
	{
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
			{
				bignum digit(bn1.getDigit(marker));
				digit.convertBaseSimple(number_to_compare.getBase());
				number_to_compare += digit;
			}

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
		if (bn >= bignum(500))
			throw error_handler(__FILE__, __LINE__, "The desired calculation is too large");

		bignum temp(bn);

		for (bignum counter(bn); counter > (int)1; counter--)
		{
			if (bn > counter)
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

	bignum modulo(const bignum &bn1, const bignum &bn2)
	{
		if (bn1.getNegative() != bn2.getNegative())
			return modulo(bn1.absolute(), bn2.absolute()) * -1;

		bignum temp(bn1);

		while (temp >= bn2)
			temp -= bn2;

		return temp;
	}

	bool divisibleByThree(const bignum &bn)
	{
		if (bn.getDecimalCount() > 0)
			return false;

		bignum temp(bn);
		temp.convertBase(10);

		int sumdigits = 0;
		for (int i = 0; i < temp.getDigitRange(); i++)
			sumdigits += temp.getDigit(PRECISION + i);

		if (sumdigits % 3 == 0)
			return true;

		else return false;
	}

	bool divisibleByFive(const bignum &bn)
	{
		if (bn.getDecimalCount() > 0)
			return false;

		bignum temp(bn);
		temp.convertBase(10);

		if (temp.getDigit(PRECISION) == 5 || temp.getDigit(PRECISION) == 0)
			return true;

		else return false;
	}

	bool divisibleBySeven(const bignum &bn)
	{
		if (bn.getDecimalCount() > 0)
			return false;

		bignum temp(bn.absolute());
		temp.convertBase(10);

		if (temp == 7)
			return true;

		if (temp < 10)
			return false;

		bignum last_digit(temp.getDigit(PRECISION));

		bignum rest_of_digits = temp - last_digit;
		rest_of_digits.divideByTen(1);

		return divisibleBySeven(rest_of_digits - (last_digit * 2));
	}

	bool checkPrime(const bignum &bn)
	{
		if (bn == 1 || bn == 2 || bn == 3 || bn == 5 || bn == 7)
			return true;

		if (bn.getNegative() || bn.getDecimalCount() > 0)
			return false;

		if (bn % 2 == 0)
			return false;

		if (divisibleBySeven(bn))
			return false;

		if (divisibleByFive(bn))
			return false;

		if (divisibleByThree(bn))
			return false;

		bignum check(1);
		while (check < bn / 2)
		{
			if (bn % check == 0 && check != 1)
				return false;

			check += 2;
		}

		return true;
	}

	void primeFactorization(const bignum &bn, vector<bignum> &factors)
	{
		if (bn.getDecimalCount() > 0)
			throw error_handler(__FILE__, __LINE__, "Cannot find the prime factorization of a decimal");

		//converts to base 10 first for faster prime checks, then converts base of all prime factors in the list
		if (bn.getBase() != 10)
		{
			bignum converted(bn);
			converted.convertBase(10);

			primeFactorization(converted, factors);

			for (vector<bignum>::iterator i = factors.begin(); i != factors.end(); i++)
				i->convertBase(bn.getBase());

			return;
		}

		if (bn < 0)
		{
			factors.push_back(-1);

			if (checkPrime(bn.absolute()))
			{
				factors.push_back(bn.absolute());
				return;
			}

			else return primeFactorization(bn.absolute(), factors);
		}

		if (checkPrime(bn))
		{
			factors.push_back(1);
			factors.push_back(bn);
			return;
		}

		bignum temp(2);
		while (bn % temp != 0)
			temp++;

		if (!checkPrime(bn / temp))
			primeFactorization(bn / temp, factors);

		else factors.push_back(bn / temp);

		if (!checkPrime(temp))
			primeFactorization(temp, factors);

		else factors.push_back(temp);
	}

	bignum greatestCommonFactor(const bignum &bn1, const bignum &bn2)
	{
		if (bn1.getNegative() != bn2.getNegative())
			return greatestCommonFactor(bn1.absolute(), bn2.absolute());

		if (bn1.getDecimalCount() > 0 || bn2.getDecimalCount() > 0)
			throw error_handler(__FILE__, __LINE__, "Greatest common factor can only be found with two integers");

		if (bn1 == bn2)
			return bn1;

		bignum lowest = bn1 < bn2 ? bn1 : bn2;
		bignum highest = bn1 > bn2 ? bn1 : bn2;

		if (highest % lowest == 0)
			return lowest;

		for (int i = 2; lowest > i; i++)
		{
			if (lowest % i == 0 && highest % i == 0)
				return bignum(i, bn1.getBase());
		}

		return bignum(1, bn1.getBase());
	}

	bignum lowestCommonMultiple(const bignum &bn1, const bignum &bn2)
	{
		if (bn1.getNegative() != bn2.getNegative())
			return lowestCommonMultiple(bn1.absolute(), bn2.absolute());

		if (bn1.getDecimalCount() > 0 || bn2.getDecimalCount() > 0)
			throw error_handler(__FILE__, __LINE__, "Lowest common multiple can only be found with two integers");

		if (bn1 == bn2)
			return bn1;

		bignum lowest = bn1 < bn2 ? bn1 : bn2;
		bignum highest = bn1 > bn2 ? bn1 : bn2;

		if (highest % lowest == 0)
			return highest;

		for (int i = 1; lowest >= i; i++)
		{
			bignum product_check(highest * i);
			if (product_check % lowest == 0)
				return product_check;
		}

		throw error_handler(__FILE__, __LINE__, "An error has occurred");
	}
	
	bignum root(const bignum &bn1, const bignum &bn2, int decimal_places)
	{	
		if (bn1.getBase() != bn2.getBase())
			return root(bn1, bn2.getConverted(bn1.getBase()), decimal_places);

		if (bn2.getNegative())
		{
			if (bn1 % 2 == 0)
				throw error_handler(__FILE__, __LINE__, "The program attempted to compute an irrational value");

			else return root(bn1, bn2.absolute(), decimal_places) * -1;
		}	

		if (bn2.getZero() || bn2 == 1)
			return bn2;

		//cross-checks the root being tested to the precision threshold specified
		bignum precision_check(1);
		precision_check.setBase(bn1.getBase());
		precision_check.divideByTen(decimal_places);
		bignum range_low = bn2 - precision_check;
		bignum range_high = bn2 + precision_check;

		bignum root_test;
		root_test = bn2 < 1 ? bn2*bn1 : bn2 / bn1;

		//TODO: refine increment function to scale depending on the size of the number being tested
		bignum increment(1);
		increment.setBase(bn1.getBase());
		if (bn2 < 1)
			increment.divideByTen(1);

		bignum answer_check;
		answer_check.setBase(bn1.getBase());

		//sets once function finds general region of the answer
		bool approximate = false;

		for (;;)
		{
			//adjusts number precision to check for nearest round roots
			//prevents function from returning 3.99999 instead of 4
			if (!approximate)
				root_test.adjustPrecision(increment.getDecimalCount());

			 answer_check = exponent(root_test, bn1);

			if (answer_check > range_low && answer_check < range_high)
				return root_test;

			if (answer_check > bn2)
			{
				if (approximate)
				{
					root_test -= increment;
					increment.divideByTen(1);
					root_test += increment;
				}

				else root_test /= bn1;
			}
				
			else if (answer_check < bn2)
			{
				if (!approximate)
					approximate = true;

				root_test += increment;
			}
		}
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

		while (counter > one)
		{
			temp *= bn1;
			counter--;
		}

		//if the power is negative, return 1/solution
		if (bn2.getNegative())
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

			while (counter > zero)
			{
				converted++;
				counter--;
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
			for (int i = 0; i < digitRange; i++)
			{
				int marker(left_most - i);

				if (marker >= MAXDIGITS || marker < 0)
					throw error_handler(__FILE__, __LINE__, "void bignum::convertBase(int n): The program has attempted to calculate a value outside of its limits");

				//each digit of the number is evaluated evaluated X * 10^n format
				bignum original_digit(getDigit(marker));
				original_digit.convertBaseSimple(base);
				bignum original_ten(10);
				original_ten.setBase(base);
				bignum original_nth(marker - PRECISION);
				original_nth.convertBaseSimple(base);

				//each of the above format is converted simply
				bignum converted_digit(original_digit);
				converted_digit.convertBaseSimple(n);
				bignum converted_ten(original_ten);
				converted_ten.convertBaseSimple(n);
				bignum converted_nth(original_nth);
				converted_nth.convertBaseSimple(n);

				//add X * 10^n to the solution
				bignum toAdd = converted_digit * exponent(converted_ten, converted_nth);

				temp += toAdd;
			}
			*this = temp;
		}
		negative = original_negative;
		updateDigits();
	}

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

			while (counter < b)
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

			while (counter > b.absolute())
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

		while (temp >= difference)
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

	//returns average of all values passed
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
