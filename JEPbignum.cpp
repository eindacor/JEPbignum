#include "JEPbignum.h"

namespace jep
{
	//------------------
	//CLASS CONSTRUCTORS
	//------------------

	bignum::bignum(int n)
	{
		bool original_negative = (n < 0);

		if (n < 0)
			n *= -1;

		initializeBignum();

		for (int i = 0; i < 20; i++)
		{
			int modifier = (pow((double)10, i + 1));
			int reduced = (n % modifier);

			if (i == 0)
				digits[i + ONES_PLACE] = reduced;

			else digits[i + ONES_PLACE] = reduced / (pow((double)10, i));
		}

		updateDigits();
		negative = original_negative;
	}

	bignum::bignum(double d, int decimal_places)
	{
		initializeBignum();

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
		converter.d = d;

		unsigned int compare = 1;

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
		big_mantissa.rightShift(mantissa_delim);

		bignum big_exponent(exponent, 2, false);
		bignum big_exponent_bias(exponent_bias);
		big_exponent_bias.convertBase(2);
		big_exponent -= big_exponent_bias;

		//generates multiplier of the floating format
		bignum mantissa_multiplier(10);
		mantissa_multiplier.setBase(2);
		mantissa_multiplier = jep::exponent(mantissa_multiplier, big_exponent);
		mantissa_multiplier.convertBase(2);
		big_mantissa += 1;

		bignum temp = big_mantissa * mantissa_multiplier;

		if (sign)
			temp.setNegative();

		temp.convertBase(10);

		*this = temp;

		roundToIndex(ONES_PLACE - decimal_places);
	}

	bignum::bignum(float f, int decimal_places)
	{
		initializeBignum();

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
		converter.f = f;

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
		big_mantissa.rightShift(mantissa_delim);

		bignum big_exponent(exponent, 2, false);
		bignum big_exponent_bias(exponent_bias);
		big_exponent_bias.convertBase(2);
		big_exponent -= big_exponent_bias;
		
		//generates multiplier of the floating format
		bignum mantissa_multiplier(10);
		mantissa_multiplier.setBase(2);
		mantissa_multiplier = jep::exponent(mantissa_multiplier, big_exponent);
		mantissa_multiplier.convertBase(2);
		big_mantissa += 1;

		bignum temp = big_mantissa * mantissa_multiplier;

		if (sign)
			temp.setNegative();

		temp.convertBase(10);

		*this = temp;

		roundToIndex(ONES_PLACE - decimal_places);
	}

	bignum::bignum(vector<int> n, int set_base, bool is_negative)
	{
		initializeBignum();
		base = set_base;

		int count = (ONES_PLACE - 1) + n.size();
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
		initializeBignum();
		base = set_base;

		int count = (ONES_PLACE - 1) + n.size();
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
		initializeBignum();

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

		if (decimalNumbers > ONES_PLACE)
			throw error_handler(__FILE__, __LINE__, "constructor failed, number has too many decimal places");

		int startingPoint = ONES_PLACE + (numbersAdded - decimalNumbers) - 1;
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
		initializeBignum();
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

		if (decimalNumbers > ONES_PLACE)
			throw error_handler(__FILE__, __LINE__, "constructor failed, number has too many decimal places");

		int startingPoint = ONES_PLACE + (numbersAdded - decimalNumbers) - 1;
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

	void bignum::initializeBignum()
	{
		for (int i = 0; i < MAXDIGITS; i++)
			digits[i] = 0;

		base = 10;
		updateDigits();
	}

	bool equals(const bignum &bn1, const bignum &bn2)
	{
		if (bn1.getBase() != bn2.getBase())
			return equals(bn1, bn2.getConverted(bn1.getBase()));

		if (bn1.isNegative() != bn2.isNegative())
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
		if (bn1.getBase() != bn2.getBase())
			return lessThan(bn1, bn2.getConverted(bn1.getBase()));

		if (bn1 == bn2)
			return false;

		if (bn1.isNegative() && !bn2.isNegative())
			return true;

		if (!bn1.isNegative() && bn2.isNegative())
			return false;

		if (bn1.isNegative() && bn2.isNegative())
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

		if (bn1.isNegative() && !bn2.isNegative())
			return false;

		if (!bn1.isNegative() && bn2.isNegative())
			return true;

		if (bn1.isNegative() && bn2.isNegative())
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
			if (bn1.isNegative() != bn2.isNegative())
			{
				bignum temp;
				temp.setBase(bn1.getBase());
				return temp;
			}

			// -12 + -12 ---> -(12 + 12)
			if (bn1.isNegative() && bn2.isNegative())
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
			if (bn1.isNegative() && !bn2.isNegative())
			{
				bignum temp = subtractNumbers(bn1.absolute(), bn2.absolute());
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}

			//	12 + -8 ---> 12 - 8
			if (!bn1.isNegative() && bn2.isNegative())
				return subtractNumbers(bn1.absolute(), bn2.absolute());

			//	-12 + -8 ---> -(12 + 8)
			if (bn1.isNegative()&& bn2.isNegative())
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
			if (bn1.isNegative() && !bn2.isNegative())
			{
				bignum temp = subtractNumbers(bn2.absolute(), bn1.absolute());
				temp.updateDigits();
				return temp;
			}

			//	8 + -12 ---> 8 - 12
			if (!bn1.isNegative() && bn2.isNegative())
				return subtractNumbers(bn1.absolute(), bn2.absolute());

			// -8 + -12 ---> -(8 + 12)
			if (bn1.isNegative() && bn2.isNegative())
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

		for (int i = (ONES_PLACE - decimal); i < digits + 1; i++)
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
			if (bn1.isNegative() && !bn2.isNegative())
			{
				bignum temp = addNumbers(bn1.absolute(), bn2.absolute());
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}

			//	12 - -12 ---> 12 + 12
			if (!bn1.isNegative() && bn2.isNegative())
				return addNumbers(bn1.absolute(), bn2.absolute());

			//	-12 - -12 ---> 0
			if (bn1.isNegative() && bn2.isNegative())
			{
				bignum temp;
				temp.setBase(base);
				return temp;
			}
		}

		//evaluate the numbers if absolute first is larger than absolute second
		if (bn1.absolute() > bn2.absolute())
		{
			//	-12 - 8 ---> -(12 + 8)
			if (bn1.isNegative() && !bn2.isNegative())
			{
				bignum temp = addNumbers(bn1.absolute(), bn2.absolute());
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}

			//	12 - -8 ---> 12 + 8
			if (!bn1.isNegative()&& bn2.isNegative())
				return addNumbers(bn1.absolute(), bn2.absolute());

			//	-12 - -8 ---> -(12 - 8)
			if (bn1.isNegative() && bn2.isNegative())
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
			if (!bn1.isNegative()&& !bn2.isNegative())
			{
				bignum temp = subtractNumbers(bn2.absolute(), bn1.absolute());
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}

			//	-8 - 12 ---> -(8 + 12)
			if (bn1.isNegative() && !bn2.isNegative())
			{
				bignum temp = addNumbers(bn1.absolute(), bn2.absolute());
				temp.setNegative();
				temp.updateDigits();
				return temp;
			}

			//	8 - -12 ---> 8 + 12
			if (!bn1.isNegative() && bn2.isNegative())
				return addNumbers(bn1.absolute(), bn2.absolute());

			//	-8 - -12 ---> (12 - 8)
			if (bn1.isNegative() && bn2.isNegative())
				return subtractNumbers(bn2.absolute(), bn1.absolute());
		}

		int carry = 0;
		int digits = 0;
		int decimal = 0;
		//bool carry_negative = false;

		//sets decimal and digit values to the highest of each number
		decimal = (bn1.getDecimalCount() > bn2.getDecimalCount() ? bn1.getDecimalCount() : bn2.getDecimalCount());
		digits = (bn1.getDigitCount() > bn2.getDigitCount() ? bn1.getDigitCount() + 1 : bn2.getDigitCount() + 1);

		for (int i = (ONES_PLACE - decimal); i < digits + 1; i++)
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
		if (bn1.isNegative() == n < 0)
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

		if (bn1.isZero() || bn2.isZero())
			return temp;

		//multiply bn1 by each digit of bn2 independently, then add the values together
		int counter = bn2.getDigitRange();
		for (int i = 0; i < counter; i++)
		{
			int toMultiply = (ONES_PLACE - bn2.getDecimalCount()) + i;

			//verify function isn't checking beyond bounds of the stored array
			if (toMultiply >= MAXDIGITS || toMultiply < 0)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			bignum toAdd = multiplyNumbersSimple(bn1.absolute(), bn2.getDigit(toMultiply));

			//verify toAdd would not overstep bounds
			if (toAdd.getDigitCount() == MAXDIGITS && i > 0)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			toAdd.leftShift(i);
			temp += toAdd;
		}

		if (bn1.isNegative() != bn2.isNegative())
			temp.setNegative();

		//adjust for added decimal places during multiplication
		temp.rightShift(bn2.getDecimalCount());

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
		if (bn2.isZero())
			throw error_handler(__FILE__, __LINE__, "Cannot divide a number by zero");

		bignum temp;
		bool negative_result = (bn1.isNegative() != bn2.isNegative());

		if (bn1.getBase() != bn2.getBase())
			return divideNumbers(bn1, bn2.getConverted(bn1.getBase()));

		//set base of the return value to match that of the passed values
		int baseSet = bn1.getBase();
		temp.setBase(baseSet);

		bool remainder = false;
		bool end = false;
		int index = bn1.getDigitCount() - 1;

		//starting with the left-most digit, create a bignumber of that digit that matches the set base
		bignum number_to_compare(bn1.getDigit(index));
		number_to_compare.setBase(baseSet);

		//ignore decimal places when comparing dividend to digits of the divisor
		bignum nextNumber = divideNumbersSimple(number_to_compare, bn2.absolute().noDecimal(), remainder);
		bignum number_to_subtract;
		number_to_subtract.setBase(baseSet);

		while (end != true)
		{
			if (remainder == false && index < ONES_PLACE - bn1.getDecimalCount())
				end = true;

			if (index >= MAXDIGITS || index < 0)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			temp.setDigit(index, nextNumber.getDigit(ONES_PLACE));
			index--;

			number_to_subtract = bn2.absolute().noDecimal() * nextNumber;
			number_to_subtract.updateDigits();
			number_to_compare -= number_to_subtract;
			number_to_compare.leftShift(1);

			if (index >= MAXDIGITS)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			if (index >= 0)
			{
				bignum digit(bn1.getDigit(index));
				digit.convertBaseSimple(number_to_compare.getBase());
				number_to_compare += digit;
			}

			nextNumber = divideNumbersSimple(number_to_compare, bn2.absolute().noDecimal(), remainder);

			if (index < 0)
				end = true;
		}

		if (negative_result && temp != 0)
			temp.setNegative();

		temp.leftShift(bn2.getDecimalCount());
		return temp;
	}

	bignum factorial(const bignum &bn)
	{
		if (bn > bignum(1000))
			throw error_handler(__FILE__, __LINE__, "The desired calculation is too large");

		bignum temp(bn);

		for (bignum counter(bn); counter > 1; counter--)
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
		bignum actual_quotient = bn1.absolute() / bn2.absolute();

		if (bn1.isPositive() && bn2.isPositive())
		{
			bignum product_to_compare = actual_quotient.getRoundedDown(ONES_PLACE) * bn2;
			return bn1 - product_to_compare;
		}

		if (bn1.isPositive() && bn2.isNegative())
		{
			bignum product_to_compare = actual_quotient.getRoundedUp(ONES_PLACE) * bn2.absolute();
			return bn1 - product_to_compare;
		}

		if (bn1.isNegative() && bn2.isPositive())
		{
			bignum product_to_compare = actual_quotient.getRoundedUp(ONES_PLACE) * bn2;
			return bn1 + product_to_compare;
		}

		if (bn1.isNegative() && bn2.isNegative())
		{
			bignum product_to_compare = actual_quotient.getRoundedDown(ONES_PLACE) * bn2.absolute();
			return bn1 + product_to_compare;
		}

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

			if (bn.absolute().isPrime())
			{
				factors.push_back(bn.absolute());
				return;
			}

			else return primeFactorization(bn.absolute(), factors);
		}

		if (bn.isPrime())
		{
			factors.push_back(1);
			factors.push_back(bn);
			return;
		}

		bignum temp(2);
		while (bn % temp != 0)
			temp++;

		if (!(bn / temp).isPrime())
			primeFactorization(bn / temp, factors);

		else factors.push_back(bn / temp);

		if (!temp.isPrime())
			primeFactorization(temp, factors);

		else factors.push_back(temp);
	}

	bignum greatestCommonFactor(const bignum &bn1, const bignum &bn2)
	{
		if (bn1.isNegative() != bn2.isNegative())
			return greatestCommonFactor(bn1.absolute(), bn2.absolute());

		if (bn1.getDecimalCount() > 0 || bn2.getDecimalCount() > 0)
			throw error_handler(__FILE__, __LINE__, "Greatest common factor can only be found with two integers");

		if (bn1 == bn2)
			return bn1;

		bignum lowest = bn1 < bn2 ? bn1 : bn2;
		bignum highest = bn1 > bn2 ? bn1 : bn2;

		int converted_lowest = (int)lowest;
		int converted_highest = (int)highest;

		if (highest % lowest == 0)
			return lowest;

		for (int i = converted_lowest; i > 1; i--)
		{
			if (converted_lowest % i == 0 && converted_highest % i == 0)
			{
				bignum temp(i);
				temp.convertBase(bn1.getBase());
				return temp;
			}
		}

		bignum one(1);
		one.setBase(bn1.getBase());
		return one;
	}

	bignum lowestCommonMultiple(const bignum &bn1, const bignum &bn2)
	{
		if (bn1.isNegative() != bn2.isNegative())
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
	
	bignum root(const bignum &nth_root, const bignum &base_number)
	{
		return root(nth_root, base_number, ROOT_ACCURACY_TOLERANCE);
	}

	bignum root(const bignum &nth_root, const bignum &base_number, int decimal_places)
	{	
		if (nth_root.getBase() != base_number.getBase())
			return root(nth_root, base_number.getConverted(nth_root.getBase()), decimal_places);

		//TODO: verify that the 2.5th root of -2 is irrational, 
		if (base_number.isNegative())
		{
			if (nth_root.getDecimalCount() > 0 || nth_root % 2 == 0)
				throw error_handler(__FILE__, __LINE__, "The program attempted to compute an irrational value");

			else return root(nth_root, base_number.absolute(), decimal_places) * -1;
		}	

		if (nth_root.isNegative())
		{
			bignum one(1);
			one.setBase(base_number.getBase());
			return root(nth_root.absolute(), one / base_number, decimal_places);
		}

		if (base_number.isZero() || base_number == 1)
			return base_number;

		//cross-checks the root being tested to the precision threshold specified
		bignum precision_check(1);
		precision_check.setBase(nth_root.getBase());
		precision_check.rightShift(decimal_places);
		bignum range_low = base_number - precision_check;
		bignum range_high = base_number + precision_check;

		bignum root_test;
		root_test = base_number < 1 ? base_number * nth_root : base_number / nth_root;

		//TODO: refine increment function to scale depending on the size of the number being tested
		bignum increment(1);
		increment.setBase(nth_root.getBase());
		if (base_number < 1)
			increment.rightShift(1);

		bignum answer_check;
		answer_check.setBase(nth_root.getBase());

		//sets once function finds general region of the answer
		bool approximate = false;

		for (;;)
		{
			//adjusts number precision to check for nearest round roots
			//prevents function from returning 3.99999 instead of 4
			if (!approximate)
				root_test.roundToIndex(ONES_PLACE - increment.getDecimalCount());

			answer_check = exponent(root_test, nth_root);

			if (answer_check > range_low && answer_check < range_high)
				return root_test;

			if (answer_check > base_number)
			{
				if (approximate)
				{
					root_test -= increment;
					increment.rightShift(1);
					root_test += increment;
				}

				else root_test /= nth_root;
			}
				
			else if (answer_check < base_number)
			{
				if (!approximate)
					approximate = true;

				root_test += increment;
			}
		}
	}

	//TODO
	bignum logarithm(const bignum &base_value, const bignum &resultant)
	{
		bignum temp;
		return temp;
	}

	bignum exponent(const bignum &base_value, const bignum &power)
	{
		return exponent(base_value, power, EXPONENTIAL_ACCURACY_TOLERANCE);
	}

	bignum exponent(const bignum &base_value, const bignum &power, int precision)
	{
		if (base_value.getBase() != power.getBase())
			return exponent(base_value, power.getConverted(power.getBase()));

		bignum one(1);
		one.setBase(base_value.getBase());
		one.setPositive();

		if (power.isZero())
			return one;

		//if the power is negative, return 1/solution
		if (power.isNegative())
			return one / exponent(base_value, power.absolute());

		if (power.getDecimalCount() > 0)
		{
			if (power < 1)
			{
				bignum modified_power(power);
				modified_power.leftShift(power.getDecimalCount());
				bignum divisor(1);
				divisor.setBase(power.getBase());
				divisor.leftShift(power.getDecimalCount());

				bignum gcf(greatestCommonFactor(modified_power, divisor));
				modified_power /= gcf;
				divisor /= gcf;

				bignum divisor_root_of_base = jep::root(divisor, base_value, precision);
				return exponent(divisor_root_of_base, modified_power).getRoundedAllDigits(ONES_PLACE - precision);
			}

			else
			{
				bignum power_decimal = power % 1;
				bignum power_int = power.getRoundedDown(ONES_PLACE);

				return exponent(base_value, power_int, precision) * exponent(base_value, power_decimal, precision);
			}
		}

		bignum counter = power.absolute();
		bignum temp(base_value);

		//n^0 always returns 1
		if (power.isZero())
			return one;

		while (counter > one)
		{
			temp *= base_value;
			counter--;
		}

		return temp;
	}

	//----------------
	//BIGNUM OPERATORS
	//----------------

	bignum::operator int() const
	{
		bignum temp(*this);
		temp.roundToIndex(ONES_PLACE);
		temp.convertBase(10);
		bignum int_max(numeric_limits<int>::max());
		bignum int_min(numeric_limits<int>::min());

		if (temp > int_max || temp < int_min)
			throw error_handler(__FILE__, __LINE__, "attempted conversion failed: bignum is too large to be converted to an int");

		int return_int = 0;
		for (int i = 0; i < digitRange; i++)
		{
			int power = (pow((double)10, i));
			return_int += (power * digits[ONES_PLACE + i]);
		}
		
		return return_int;
	}

	bignum& bignum::operator = (const bignum& b)
	{
		if (this == &b)
			return *this;

		base = b.getBase();

		int highestDigits = 0;
		int decimal = 0;

		highestDigits = (digitCount < b.getDigitCount() ? b.getDigitCount() : digitCount);

		decimal = (decimalCount < b.getDecimalCount() ? b.getDecimalCount() : decimalCount);

		for (int i = (ONES_PLACE - decimal); i < highestDigits; i++)
		{
			if (i >= MAXDIGITS)
				throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			digits[i] = b.getDigit(i);
		}

		negative = b.isNegative();

		updateDigits();

		return *this;
	}

	//-----------------
	//GENERAL UTILITIES
	//-----------------

	bool bignum::isPrime() const
	{
		if (*this == 1 || *this == 2 || *this == 3 || *this == 5 || *this == 7)
			return true;

		if (negative || decimalCount > 0)
			return false;

		if (*this % 2 == 0)
			return false;

		if (isDivisibleBySeven())
			return false;

		if (isDivisibleByFive())
			return false;

		if (isDivisibleByThree())
			return false;

		bignum check(1);
		while (check < *this / 2)
		{
			if (*this % check == 0 && check != 1)
				return false;

			check += 2;
		}

		return true;
	}

	bool bignum::isDivisibleByThree() const
	{
		if (decimalCount > 0)
			return false;

		bignum temp(getConverted(10));

		int sumdigits = 0;
		for (int i = 0; i < temp.getDigitRange(); i++)
			sumdigits += temp.getDigit(ONES_PLACE + i);

		if (sumdigits % 3 == 0)
			return true;

		else return false;
	}

	bool bignum::isDivisibleByFive() const
	{
		if (decimalCount > 0)
			return false;

		bignum temp(getConverted(10));

		if (temp.getDigit(ONES_PLACE) == 5 || temp.getDigit(ONES_PLACE) == 0)
			return true;

		else return false;
	}

	bool bignum::isDivisibleBySeven() const
	{
		if (decimalCount > 0)
			return false;

		bignum temp(getConverted(10).absolute());

		if (temp == 7)
			return true;

		if (temp < 10)
			return false;

		bignum last_digit(temp.getDigit(ONES_PLACE));

		bignum rest_of_digits = temp - last_digit;
		rest_of_digits.rightShift(1);

		bignum new_test(rest_of_digits - (last_digit * 2));

		return new_test.isDivisibleBySeven();
	}

	void bignum::setBase(int n)
	{
		int index = ONES_PLACE - getDecimalCount();

		for (int i = 0; i < digitRange; i++)
		{
			if (digits[index + i] >= n)
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
		if (isZero())
		{
			base = n;
			return;
		}

		bool original_negative = negative;

		bignum toAdd;
		bignum temp(0);
		temp.setBase(n);

		if (base != n)
		{
			for (int i = 0; i < digitRange; i++)
			{
				int index(left_most - i);

				if (index >= MAXDIGITS || index < 0)
					throw error_handler(__FILE__, __LINE__, "void bignum::convertBase(int n): The program has attempted to calculate a value outside of its limits");

				//each digit of the number is evaluated evaluated X * 10^n format
				bignum original_digit(getDigit(index));
				original_digit.convertBaseSimple(base);
				bignum original_ten(10);
				original_ten.setBase(base);
				bignum original_nth(index - ONES_PLACE);
				original_nth.convertBaseSimple(base);

				//each of the above format is converted simply
				bignum converted_digit(original_digit);
				converted_digit.convertBaseSimple(n);
				bignum converted_ten(original_ten);
				converted_ten.convertBaseSimple(n);
				bignum converted_nth(original_nth);
				converted_nth.convertBaseSimple(n);
				
				//add X * 10^n to the solution
				bignum multiplier(exponent(converted_ten, converted_nth));
				bignum toAdd = converted_digit * multiplier;

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

		if (decimal_places > ONES_PLACE)
			decimal_places = ONES_PLACE;

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


		int comma = (temp.getDigitCount() - ONES_PLACE) % 3;

		if (negative == true)
			tempString += "-";

		temp.roundToIndex(ONES_PLACE - decimal_places);

		int counter = temp.getDigitRange();
		for (int i = 0; i < counter; i++)
		{
			int index = temp.getDigitCount() - i - 1;

			if (index == (ONES_PLACE - 1))
				tempString += ".";

			else if (comma == 0 && (temp.getDigitCount() - i) >= ONES_PLACE)
			{
				if (i>0 && include_commas == true)
					tempString += ",";

				comma = 3;
			}

			tempString += temp.getDigitString(temp.getDigit(index));

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
	void bignum::leftShift(int places)
	{
		if (digitCount >= MAXDIGITS - 1)
			throw error_handler(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

		for (int i = 0; i < places; i++)
		{
			for (int c = 0; c < digitRange; c++)
				digits[digitCount - c] = digits[left_most - c];

			digits[right_most] = 0;
			updateDigits();
		}
		updateDigits();
	}

	//divides the number by 10 in its native base by literally shifting digit
	void bignum::rightShift(int places)
	{
		for (int i = places; i > 0; i--)
		{
			for (int n = right_most; n <= digitCount; n++)
			{
				if (n == 0)
					continue;

				digits[n - 1] = digits[n];
			}
			updateDigits();
		}
		updateDigits();
	}

	//refreshes digitCount and decimalCount
	void bignum::updateDigits()
	{
		is_zero = false;

		digitCount = 0;
		decimalCount = 0;

		for (int i = MAXDIGITS - 1; i > 0; i--)
		{
			if (i == ONES_PLACE)
			{
				digitCount = ONES_PLACE + 1;
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

		for (int i = 0; i < ONES_PLACE; i++)
		{
			if (digits[i]>0)
			{
				decimalCount = ONES_PLACE - i;
				right_most = i;
				break;
			}

			if (i == ONES_PLACE - 1)
				right_most = ONES_PLACE;
		}

		if (decimalCount == 0 && digitCount == (ONES_PLACE + 1))
		{
			if (digits[ONES_PLACE] == 0)
			{
				setPositive();
				is_zero = true;
			}

			right_most = ONES_PLACE;
		}

		//sets number of operative digits
		//	12.077 ---> digitRange = 5
		digitRange = left_most - right_most + 1;
	}

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
		temp.leftShift(getDecimalCount());
		return temp;
	}

	//rounds the number to a speecific position, rounding each previous digit first
	void bignum::roundAllDigitsToIndex(int index)
	{
		if (index == 0)
			return;

		if (index < 1 || index > MAXDIGITS)
			throw error_handler(__FILE__, __LINE__, "program attempted to round outside the limits of the bignum");

		for (int i = 1; i <= index; i++)
		{
			if (digits[i - 1] >= (base / 2))
			{
				bignum increment;
				increment.setBase(base);
				increment.setDigit(i, 1);

				isNegative() ? *this -= increment : *this += increment;
			}
		}

		for (int i = 0; i < index; i++)
			digits[i] = 0;

		updateDigits();
	}

	//rounds the number to a specific position of digits[index]
	void bignum::roundToIndex(int index)
	{
		if (index == 0)
			return;

		if (index < 1 || index > MAXDIGITS)
			throw error_handler(__FILE__, __LINE__, "program attempted to round outside the limits of the bignum");

		//checks to see if number is already rounded without using modulo to prevent stack overflow
		for (int i = 0; i < index; i++)
		{
			if (digits[i] != 0)
				break;

			else if (i == index - 1)
				return;
		}

		if (digits[index - 1] >= (base / 2))
		{
			bignum increment;
			increment.setBase(base);
			increment.setDigit(index, 1);

			isNegative() ? *this -= increment : *this += increment;
		}

		for (int i = 0; i < index; i++)
			digits[i] = 0;

		updateDigits();
	}

	void bignum::roundDownToIndex(int index)
	{
		//first checks to see if the number is already rounded
		bignum round_test;
		round_test.setBase(base);
		round_test.setDigit(index, 1);

		//checks to see if number is already rounded without using modulo to prevent stack overflow
		for (int i = 0; i < index; i++)
		{
			if (digits[i] != 0)
				break;

			else if (i == index - 1)
				return;
		}
		
		for (int i = 0; i < index; i++)
			setDigit(i, 0);

		if (isNegative())
		{
			bignum decrement;
			decrement.setDigit(index, 1);
			decrement.setBase(base);
			*this -= decrement;
		}

		updateDigits();
	}

	void bignum::roundUpToIndex(int index)
	{
		//first checks to see if the number is already rounded
		bignum round_test;
		round_test.setBase(base);
		round_test.setDigit(index, 1);

		//checks to see if number is already rounded without using modulo to prevent stack overflow
		for (int i = 0; i < index; i++)
		{
			if (digits[i] != 0)
				break;

			else if (i == index - 1)
				return;
		}

		for (int i = 0; i < index; i++)
			setDigit(i, 0);

		if (isPositive())
		{
			bignum increment;
			increment.setDigit(index, 1);
			increment.setBase(base);
			*this += increment;
		}

		updateDigits();
	}

	bignum golden(const bignum &b)
	{
		bignum temp1 = bignum(fibonacci(b));
		bignum temp2 = bignum(fibonacci(b - 1));

		return (temp2 == 0 ? 0 : temp1 / temp2);
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

	//returns random number within range with added resolution
	bignum randomNumberAddPrecision(const bignum &bn1, const bignum &bn2, int add_precision)
	{
		if (add_precision > ONES_PLACE)
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
			int index = (start - i);
			tempdigit = (rand() % difference.getBase());
			temp.setDigit(index, tempdigit);
		}

		bignum increment;
		increment.setBase(difference.getBase());
		increment.setDigit((difference.getDigitCount() - counter), 1);

		difference += increment;

		while (temp >= difference && !temp.isNegative())
			temp -= difference;

		return (bn1 > bn2 ? bn2 + temp : bn1 + temp);
	}

	//returns random number within range with a specified resolution
	bignum randomNumberForcePrecision(const bignum &bn1, const bignum &bn2, int force_precision)
	{
		if (force_precision > ONES_PLACE)
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
		bn1_adjusted.roundToIndex(ONES_PLACE - force_precision);

		bignum bn2_adjusted(bn1);
		bn2_adjusted.roundToIndex(ONES_PLACE - force_precision);

		difference = (bn1_adjusted > bn2_adjusted ? bn1_adjusted - bn2_adjusted : bn2_adjusted - bn1_adjusted);

		int start = difference.getDigitCount() - 1;
		int tempdigit;

		int counter = (difference.getDigitCount() - (ONES_PLACE - force_precision));

		for (int i = 0; i < counter; i++)
		{
			int index = (start - i);
			tempdigit = (rand() % difference.getBase());
			temp.setDigit(index, tempdigit);
		}

		bignum increment;
		increment.setBase(difference.getBase());
		increment.setDigit((difference.getDigitCount() - counter), 1);

		difference += increment;

		while (temp >= difference)
			temp -= difference;

		return (bn1_adjusted > bn2_adjusted ? bn2_adjusted + temp : bn1_adjusted + temp);
	}

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
