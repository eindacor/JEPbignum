#include "JEPbignum.h"

namespace jep
{
	//------------------
	//CLASS CONSTRUCTORS
	//------------------

	bignum::bignum()
	{
		for (int i = 0; i<MAXDIGITS; i++)
		{
			digits[i] = 0;
		}
		decimalCount = 0;
		base = 10;
		updateDigits();
	}

	bignum::bignum(signed int n)
	{
		if (n<0)
		{
			negative = true;
			n *= -1;
		}

		else negative = false;

		for (int i = 0; i<MAXDIGITS; i++)
		{
			digits[i] = 0;
		}

		for (int i = 0; i<20; i++)
		{
			int modifier = (pow((double)10, i + 1));
			int reduced = (n % modifier);

			if (i == 0)
			{
				digits[i + PRECISION] = reduced;
			}

			else
			{
				digits[i + PRECISION] = reduced / (pow((double)10, i));
			}
		}

		decimalCount = 0;
		base = 10;
		updateDigits();
	}

	bignum::bignum(float f)
	{
		if (f<0)
		{
			negative = true;
			f *= -1;
		}

		else negative = false;

		decimalCount = 0;
		base = 10;
		updateDigits();

		int i = (int)f;
		int decimal_places = 0;

		while (f != i)
		{
			f *= 10.0f;
			i = f;
			decimal_places++;
		}

		bignum bn_float((int)f);
		bn_float.divideByTen(decimal_places);
		
		(*this) = bn_float;

		/*
		base = b.getBase();
        
    	int highestDigits=0;
    	int decimal=0;
    
		highestDigits = (digitCount < b.getDigitCount() ? b.getDigitCount() : digitCount);
    
		decimal = (decimalCount < b.getDecimalCount() ? b.getDecimalCount() : decimalCount);
        
    	for (int i=(PRECISION-decimal); i<highestDigits; i++)
    	{
			if (i >= MAXDIGITS)
				throw bignum_Error(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			digits[i] = b.getDigit(i);
    	}
        
        negative = b.getNegative();
        
    	updateDigits();
		*/
	}

	bignum::bignum(double d)
	{
		if (d<0)
		{
			negative = true;
			d *= -1;
		}

		else negative = false;

		decimalCount = 0;
		base = 10;
		updateDigits();

		int i = (int)d;
		int decimal_places = 0;

		while (d != i)
		{
			d *= 10.0f;
			i = d;
			decimal_places++;
		}

		bignum bn_float((int)d);
		bn_float.divideByTen(decimal_places);

		(*this) = bn_float;
	}

	bignum::bignum(vector<int> n, int offset, int set_base, bool is_negative)
	{
		base = set_base;
		if (offset < 0)
			throw bignum_Error(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

		for (int i = 0; i<MAXDIGITS; i++)
		{
			digits[i] = 0;
		}

		int count = offset;
		for (vector<int>::iterator i = n.begin(); i != n.end(); i++)
		{	
			if (count >= MAXDIGITS || count < 0)
				throw bignum_Error(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			if (*i >= base)
				throw bignum_Error(__FILE__, __LINE__, "One of the values passed is beyond the given base");

			digits[count] = (*i);
			count++;
		}

		updateDigits();
		negative = is_negative;
	}

	bignum::bignum(string s)
	{
		for (int i = 0; i<MAXDIGITS; i++)
		{
			digits[i] = 0;
		}

		negative = false;
		base = 10;

		vector <int> numbersToAdd;
		int numbersAdded = 0;
		int decimalNumbers = 0;
		bool decimal = false;
		int commaNumbers = 0;
		bool comma = false;

		for (int i = 0; i<s.length(); i++)
		{
			switch (s[i])
			{
			case ',':
				if (decimal == true)
					throw bignum_Error(__FILE__, __LINE__, "constructor failed, comma included after decimal point");

				else if (comma == true && commaNumbers != 3)
					throw bignum_Error(__FILE__, __LINE__, "constructor failed, improper use of commas");

				else if (comma == false && numbersAdded > 3)
					throw bignum_Error(__FILE__, __LINE__, "constructor failed, improper use of commas");

				else if (numbersAdded == 0)
					throw bignum_Error(__FILE__, __LINE__, "constructor failed, improper use of commas");

				comma = true;
				commaNumbers = 0;

				break;

			case '.':
				if (decimal == true)
					throw bignum_Error(__FILE__, __LINE__, "constructor failed, number contains multiple decimal points");

				else
				{
					decimal = true;
				}

				break;

			case '-':
				if (decimal == true)
					throw bignum_Error(__FILE__, __LINE__, "constructor failed, negative symbol included after a decimal point");

				if (numbersAdded>0)
					throw bignum_Error(__FILE__, __LINE__, "constructor failed, negative symbol included after a number");

				if (comma == true)
					throw bignum_Error(__FILE__, __LINE__, "constructor failed, negative sign included after a comma");

				if (negative == true)
					throw bignum_Error(__FILE__, __LINE__, "constructor failed, number contains multiple negative symbols");

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
						throw bignum_Error(__FILE__, __LINE__, "constructor failed, digit exceeds base desired");

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
					throw bignum_Error(__FILE__, __LINE__, "constructor failed, invalid character(s) included");
				}
				break;
			}
		}

		if (decimalNumbers>PRECISION)
			throw bignum_Error(__FILE__, __LINE__, "constructor failed, number has too many decimal places");

		int startingPoint = PRECISION + (numbersAdded - decimalNumbers) - 1;
		for (int i = 0; i<numbersToAdd.size(); i++)
		{
			int digitToAdd = numbersToAdd.at(i);
			int locationToSet = startingPoint - i;
			digits[locationToSet] = digitToAdd;
		}

		updateDigits();
	}

	bignum::bignum(string s, int baseGiven)
	{
		for (int i = 0; i<MAXDIGITS; i++)
		{
			digits[i] = 0;
		}

		negative = false;
		base = baseGiven;

		vector <int> numbersToAdd;
		int numbersAdded = 0;
		int decimalNumbers = 0;

		for (int i = 0; i<s.length(); i++)
		{
			bool decimal = false;

			switch (s[i])
			{
			case ',': break;

			case '.':
				if (decimal == true)
					throw bignum_Error(__FILE__, __LINE__, "constructor failed, number contains multiple decimal points");

				else decimal = true;
				break;

			case '-':
				if (negative == true)
					throw bignum_Error(__FILE__, __LINE__, "constructor failed, number contains multiple negative symbols");

				else negative = true;
				break;

			default:
				char zero = '0';
				char letter = 'A';

				if (s[i] >= zero && s[i] <= zero + 9)
				{
					int digitToAdd = s[i] - zero;
					if (digitToAdd >= base)
						throw bignum_Error(__FILE__, __LINE__, "constructor failed, digit exceeds base desired");

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
						throw bignum_Error(__FILE__, __LINE__, "constructor failed, digit exceeds base desired");

					if (decimal == true)
						decimalNumbers++;

					numbersAdded++;
					numbersToAdd.push_back(digitToAdd);
					break;
				}

				else throw bignum_Error(__FILE__, __LINE__, "constructor failed, invalid character(s) included");
				break;
			}
		}

		if (decimalNumbers>PRECISION)
			throw bignum_Error(__FILE__, __LINE__, "constructor failed, number has too many decimal places");

		int startingPoint = PRECISION + (numbersAdded - decimalNumbers) - 1;
		for (int i = 0; i<numbersToAdd.size(); i++)
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

	bool equals(bignum bn1, bignum bn2)
	{
		if (bn1.getNegative() != bn2.getNegative())
			return false;

		if (bn1.getDigitCount() != bn2.getDigitCount())
			return false;

		if (bn1.getBase() != bn2.getBase())
			return false;

		if (bn1.getDecimalCount() != bn2.getDecimalCount())
			return false;

		for (int i = bn1.getDigitCount(); i>0; i--)
		{
			if (bn1.getDigit(i - 1) != bn2.getDigit(i - 1))
				return false;
		}
		return true;
	}

	bool lessThan(bignum bn1, bignum bn2)
	{
		//if bases are different, convert the second and re-evaluate
		if (bn1.getBase() != bn2.getBase())
			bn2.convertBase(bn1.getBase());

		bn1.updateDigits();
		bn2.updateDigits();

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

	bool greaterThan(bignum bn1, bignum bn2)
	{
		//if bases are different, convert the second and re-evaluate
		if (bn1.getBase() != bn2.getBase())
			bn2.convertBase(bn1.getBase());

		bn1.updateDigits();
		bn2.updateDigits();

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


    //FUNCTION FOR ADDING NUMBERS OF DIFFERENT BASES
    bignum addNumbers(bignum bn1, bignum bn2)
    {
		if (bn1.getBase() != bn2.getBase())
			bn2.convertBase(bn1.getBase());

        int base = bn1.getBase();
        
    	bignum sum;

		//evaluate the numbers being equal
        if (equals(bn1.absolute(), bn2.absolute()))
        {
			//	-12 + 12 or 12 + -12 ---> 0
            if (bn1.getNegative() != bn2.getNegative())
            {
                bignum temp(0);
                return temp;
            }
            
			// -12 + -12 ---> -(12 + 12)
            if (bn1.getNegative()==true && bn2.getNegative()==true)
            {
                bignum temp = addNumbers(bn1.absolute(), bn2.absolute());
                temp.setNegative();
    			temp.updateDigits();
                return temp;
            }
        }
        
		//evaluate the numbers if absolute first is larger than absolute second
        if (greaterThan(bn1.absolute(), bn2.absolute()))
        {
			//	-12 + 8 ---> -(12 - 8)
            if (bn1.getNegative()==true && bn2.getNegative()==false)
            {
                bignum temp = subtractNumbers(bn1.absolute(), bn2.absolute());
                temp.setNegative();
    				temp.updateDigits();
                return temp;
            }
            
			//	12 + -8 ---> 12 - 8
            if (bn1.getNegative()==false && bn2.getNegative()==true)
            {
                return subtractNumbers(bn1.absolute(), bn2.absolute());
            }
            
			//	-12 + -8 ---> -(12 + 8)
            if (bn1.getNegative()==true && bn2.getNegative()==true)
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
            if (bn1.getNegative()==true && bn2.getNegative()==false)
            {
                bignum temp = subtractNumbers(bn2.absolute(), bn1.absolute());
    			temp.updateDigits();
                return temp;
            }
            
			//	8 + -12 ---> 8 - 12
            if (bn1.getNegative()==false && bn2.getNegative()==true)
            {
                return subtractNumbers(bn1.absolute(), bn2.absolute());
            }
            
			// -8 + -12 ---> -(8 + 12)
            if (bn1.getNegative()==true && bn2.getNegative()==true)
            {
                bignum temp = addNumbers(bn1.absolute(), bn2.absolute());
                temp.setNegative();
    			temp.updateDigits();
                return temp;
            }
        }
        
    	vector<int> temp;
    	int carry=0;
    	int digits=0;
    	int decimal=0;
    
		//sets decimal and digit values to the highest of each number
		decimal = (bn1.getDecimalCount() > bn2.getDecimalCount() ? bn1.getDecimalCount() : bn2.getDecimalCount());   
		digits = (bn1.getDigitCount() > bn2.getDigitCount() ? bn1.getDigitCount() + 1 : bn2.getDigitCount() + 1);

    	for (int i=(PRECISION-decimal); i<digits+1; i++)
    	{
			//verify function isn't checking beyond bounds of the stored array
			if (i >= MAXDIGITS || i < 0)
				throw bignum_Error(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

    		int tempNumber = bn1.getDigit(i) + bn2.getDigit(i);
    		
    		tempNumber += carry;
    
    		if (tempNumber>(base-1))
    		{
    			tempNumber-=base;
    			carry = 1;
    		}
    
    		else 
    		{
    			carry = 0;
    		}
    
    		sum.setDigit(i, tempNumber);
    	}
        
    	sum.updateDigits();
    	sum.setBase(base);
    	return sum;
    }
    
    //FUNCTION FOR SUBTRACTING NUMBERS
    bignum subtractNumbers(bignum bn1, bignum bn2)
    {
		if (bn1.getBase() != bn2.getBase())
			bn2.convertBase(bn1.getBase());

        int base = bn1.getBase();
        bignum difference;
    	difference.setBase(base);
    
		//evaluate the numbers being of equal absolute value
        if (equals(bn1.absolute(), bn2.absolute()))
        {
			//	-12 - 12 ---> -(12 + 12)
            if (bn1.getNegative()==true && bn2.getNegative()==false)
            {
                bignum temp = addNumbers(bn1.absolute(), bn2.absolute());
                temp.setNegative();
    			temp.updateDigits();
                return temp;
            }
            
			//	12 - -12 ---> 12 + 12
            if (bn1.getNegative()==false && bn2.getNegative()==true)
            {
                return addNumbers(bn1.absolute(), bn2.absolute());
            }
            
			//	-12 - -12 ---> 0
            if (bn1.getNegative()==true && bn2.getNegative()==true)
            {
				bignum temp(0);
                return temp;
            }
        }
        
		//evaluate the numbers if absolute first is larger than absolute second
        if (greaterThan(bn1.absolute(), bn2.absolute()))
        {
			//	-12 - 8 ---> -(12 + 8)
            if (bn1.getNegative()==true && bn2.getNegative()==false)
            {
                bignum temp = addNumbers(bn1.absolute(), bn2.absolute());
                temp.setNegative();
    			temp.updateDigits();
                return temp;
            }
            
			//	12 - -8 ---> 12 + 8
            if (bn1.getNegative()==false && bn2.getNegative()==true)
            {
                return addNumbers(bn1.absolute(), bn2.absolute());
            }
            
			//	-12 - -8 ---> -(12 - 8)
            if (bn1.getNegative()==true && bn2.getNegative()==true)
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
            if (bn1.getNegative()==false && bn2.getNegative()==false)
            {
                bignum temp = subtractNumbers(bn2.absolute(), bn1.absolute());
                temp.setNegative();
    			temp.updateDigits();
                return temp;
            }
            
			//	-8 - 12 ---> -(8 + 12)
            if (bn1.getNegative()==true && bn2.getNegative()==false)
            {
                bignum temp = addNumbers(bn1.absolute(), bn2.absolute());
                temp.setNegative();
    			temp.updateDigits();
                return temp;
            }
            
			//	8 - -12 ---> 8 + 12
            if (bn1.getNegative()==false && bn2.getNegative()==true)
            {
                return addNumbers(bn1.absolute(), bn2.absolute());
            }
            
			//	-8 - -12 ---> (12 - 8)
            if (bn1.getNegative()==true && bn2.getNegative()==true)
            {
                return subtractNumbers(bn2.absolute(), bn1.absolute());
            }
        }
    	  
      	vector<int> temp;
    	int carry=0;
    	int digits=0;
    	int decimal=0;
		//bool carry_negative = false;

		//sets decimal and digit values to the highest of each number
		decimal = (bn1.getDecimalCount() > bn2.getDecimalCount() ? bn1.getDecimalCount() : bn2.getDecimalCount());
		digits = (bn1.getDigitCount() > bn2.getDigitCount() ? bn1.getDigitCount() + 1 : bn2.getDigitCount() + 1);
    
    	for (int i=(PRECISION-decimal); i<digits+1; i++)
    	{
			//verify function isn't checking beyond bounds of the stored array
			if (i >= MAXDIGITS || i < 0)
				throw bignum_Error(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

    		int tempNumber = bn1.getDigit(i) - bn2.getDigit(i);
    		
    		tempNumber -= carry;
    
    		if (tempNumber<0)
    		{
    			tempNumber+=base;
    			carry = 1;
    		}
    
    		else 
    		{
    			carry = 0;
    		}
    
    		difference.setDigit(i, tempNumber);
    	}
    
        difference.updateDigits();
        difference.setBase(base);
    	return difference;
    }
    
	//FUNCTION FOR MULTIPLYING BIGNUMS BY INTS
    bignum multiplyNumbersSimple(bignum bn1, int n)
    {  
        if (n==0)
        {
        	bignum zero;
        	zero.setBase(bn1.getBase());
           	return zero;
        }
        
        bignum temp(bn1);
        
		//if both numbers are negative, make the result positive
        if (bn1.getNegative() == n<0)
            temp.setPositive();
        
		//add the first number to itself n times
        for (int i=0; i<(n-1); i++)
        {
            temp += bn1;
        }
        
        temp.updateDigits();
        return temp;
    }
    
	//FUNCTION FOR MULTIPLYING NUMBERS
    bignum multiplyNumbers(bignum bn1, bignum bn2)
    {
		if (bn1.getBase() != bn2.getBase())
			bn2.convertBase(bn1.getBase());

        bignum temp(0);
        temp.setBase(bn1.getBase());
    
		//if either number is 0, return 0
		if (equals(bn1, temp) || equals(bn2, temp))
    		return temp;
    	
		//multiply bn1 by each digit of bn2 independently, then add the values together
    	int counter = bn2.getDigitRange();
    	for (int i=0; i<counter; i++)
    	{
    		int toMultiply = (PRECISION-bn2.getDecimalCount()) + i;

			//verify function isn't checking beyond bounds of the stored array
			if (toMultiply >= MAXDIGITS || toMultiply < 0)
				throw bignum_Error(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

    		bignum toAdd = multiplyNumbersSimple(bn1.absolute(), bn2.getDigit(toMultiply) );

			//verify toAdd would not overstep bounds
			if (toAdd.getDigitCount() == MAXDIGITS && i > 0)
				throw bignum_Error(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

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
    
	//FUNCTION FOR DIVIDING USING REPEATED SUBTRACTION
    bignum divideNumbersSimple (bignum bn1, bignum bn2, bool &remainder)
    {
    	bignum counter;
    	counter.setBase(bn1.getBase());
    	
    	while (!lessThan(bn1, bn2))
    	{
    		bn1-=bn2;
    		counter++;
    	}
    
		//adjusts bool passed for remainder
    	if (equals(counter * bn2, bn1))
    		remainder = false;
    
    	else remainder = true;
    
    	return counter;
    }
    
	//FUNCTION FOR DIVIDING NUMBERS
    bignum divideNumbers(bignum bn1, bignum bn2)
    {
		if (bn1.getBase() != bn2.getBase())
			bn2.convertBase(bn1.getBase());

		//throws an exception if the program is attempting to divide by zero
		bignum zero(0);
		zero.setBase(bn2.getBase());
    	if (equals(bn2, zero))
    		throw bignum_Error(__FILE__, __LINE__, "Cannot divide a number by zero");
    	
		//set base of the return value to match that of the passed values
        int baseSet=bn1.getBase();
    	bignum temp;
    	temp.setBase(baseSet);
    
		//set return value to negative if the numbers passed have different signs
    	if (bn1.getNegative() != bn2.getNegative())
            temp.setNegative();
    
		//sets both passed values to positive for purposes of division mechanic
    	bn1.setPositive();
    	bn2.setPositive();
    
    	bool remainder=false;
    	bool end=false;
    	int marker=bn1.getDigitCount()-1;

		//starting with the left-most digit, create a bignumber of that digit that matches the set base
    	bignum numberToCompare(bn1.getDigit(marker));
    	numberToCompare.setBase(baseSet);

		//ignore decimal places when comparing dividend to digits of the divisor
    	bignum nextNumber = divideNumbersSimple(numberToCompare, bn2.noDecimal(), remainder);
    	bignum numberToSubtract;
    	numberToSubtract.setBase(baseSet);
    
    	while (end != true)
    	{	
    		if (remainder==false && marker < PRECISION-bn1.getDecimalCount() )
    			end = true;
    
			if (marker >= MAXDIGITS || marker < 0)
				throw bignum_Error(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

    		temp.setDigit(marker, nextNumber.getDigit(PRECISION));
    		marker--;
    	
    		numberToSubtract = bn2.noDecimal() * nextNumber;
    		numberToSubtract.updateDigits();
    		numberToCompare -= numberToSubtract;
    		numberToCompare.timesTen(1);
    
			if (marker >= MAXDIGITS)
				throw bignum_Error(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

    		if (marker>=0)
    			numberToCompare += bn1.getDigit(marker);
    
    		nextNumber = divideNumbersSimple(numberToCompare, bn2.noDecimal(), remainder);
    
    		if (marker<0)
    			end = true;
    	}
    
    	temp.timesTen(bn2.getDecimalCount());
    	return temp;
    }
    
	//FUNCTION FOR CALCULATING FACTORIALS
    bignum factorial(bignum bn)
    {
		if (!lessThan(bn, bignum(500)))
			throw bignum_Error(__FILE__, __LINE__, "The desired calculation is too large");

        bignum temp(bn);
    
        for (bignum counter(bn); greaterThan(counter, (int) 1) ; counter--)
        {
            if (greaterThan(bn, counter))
                temp *= counter;
        }
    
    	temp.updateDigits();
    	return temp;
    }
    
	//FUNCTION FOR CALCULATING PERMUTATIONS
    bignum combinations(bignum bn1, bignum bn2)
    {
		if (bn1.getBase() != bn2.getBase())
			bn2.convertBase(bn1.getBase());

    	bignum first = factorial(bn1);
    	bignum second = bn1-bn2;
    	bignum third = factorial(second);
    	bignum fourth = factorial(bn2);
    	bignum fifth = third * fourth;
    
    	bignum temp = divideNumbers(first, fifth);
    
    	return temp;
    }
    
	//FUNCTION FOR CALCULATING EXPONENTS
    bignum exponent(bignum bn1, bignum bn2)
    {
		if (bn1.getBase() != bn2.getBase())
			bn2.convertBase(bn1.getBase());

		bignum one(1);
		one.setBase(bn1.getBase());
		one.setPositive();
		bignum zero;
		zero.setBase(bn1.getBase());

		if (bn2.getDecimalCount() > 0)
			throw bignum_Error(__FILE__, __LINE__, "Cannot use decimals as exponential powers");

        bignum counter = bn2.absolute();
        bignum temp = bn1;
        
		//n^0 always returns 1
        if (equals(bn2, zero))
            return one;
        
        while (greaterThan(counter, one))
        {
            temp *= bn1;
            counter--;
        }
        
		//if the power is negative, return 1/solution
        if (bn2.getNegative()==true)
            temp = divideNumbers(one, temp);
        
        return temp;
    }

	//----------------
    //BIGNUM OPERATORS
	//----------------

    void bignum::operator = (bignum b)
    {
		base = b.getBase();
        
    	int highestDigits=0;
    	int decimal=0;
    
		highestDigits = (digitCount < b.getDigitCount() ? b.getDigitCount() : digitCount);
    
		decimal = (decimalCount < b.getDecimalCount() ? b.getDecimalCount() : decimalCount);
        
    	for (int i=(PRECISION-decimal); i<highestDigits; i++)
    	{
			if (i >= MAXDIGITS)
				throw bignum_Error(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

			digits[i] = b.getDigit(i);
    	}
        
        negative = b.getNegative();
        
    	updateDigits();
    }
    
    void bignum::operator = (string s)
    {
    	bignum temp(s);   	
    	(*this) = temp;
    }
    
    void bignum::operator = (int n)
    {
    	bignum temp(n);	
    	(*this) = temp;
    }
    
    void bignum::operator -- (int)
    {
    	bignum temp(1);
		temp.setBase(base);
    	*this -= temp;
    }
    
    void bignum::operator ++ (int)
    {
    	bignum temp(1);
		temp.setBase(base);
    	*this += temp;
    }
    
	/*
    bool bignum::operator != (bignum b)
    {
    	return !(*this==b);
    }
    
    bool bignum::operator != (int n)
    {
    	bignum b(n);
    	return (*this!=b);
    }

    bool bignum::operator < (bignum b)
    {
		
    }
    
    bool bignum::operator <= (bignum b)
    {
		return (*this < b || *this == b ? true : false);
    }
    
    bool bignum::operator < (int n)
    {
    	bignum b(n);
    	updateDigits(); 
    	return (*this < b);
    }
    
    bool bignum::operator <= (int n)
    {
    	bignum b(n);
    	updateDigits(); 
    	return (*this <= b);
    }
    
    bool bignum::operator > (bignum b)
    {

    }
    
    bool bignum::operator >= (bignum b)
    {
		return (*this > b || *this == b ? true : false);
    }
    
    bool bignum::operator > (int n)
    {
		bignum b(n);
        updateDigits(); 
        return (*this > b);
    }
    
    bool bignum::operator >= (int n)
    {
        bignum b(n);
        updateDigits(); 
        return (*this >= b);
    }

	bignum bignum::operator * (bignum b)
	{
		return multiplyNumbers(*this, b);
	}

	bignum bignum::operator * (int n)
	{
		if (base != 10)
		{
			bignum temp(n);
			temp.convertBase(base);

			return multiplyNumbers(*this, temp);
		}

		bignum b(n);
		return multiplyNumbers(*this, b);
	}

	bignum bignum::operator / (bignum b)
	{
		return divideNumbers(*this, b);
	}

	bignum bignum::operator / (int n)
	{
		if (base != 10)
		{
			bignum temp(n);
			temp.convertBase(base);

			return divideNumbers(*this, temp);
		}

		bignum b(n);
		return divideNumbers(*this, b);
	}

	template <typename T>
	inline bignum bignum:: operator + (T passed)
	{
		bignum b(passed);

		//if (b != 3)
			//throw bignum_Error(__FILE__, __LINE__, b.getNumberString(false, false, 5));

		return addNumbers(*this, b);
	}
	
	bignum bignum:: operator + (int n)
	{
		if (base != 10)
		{
			bignum temp(n);
			temp.convertBase(base);

			return addNumbers(*this, temp);
		}

		bignum b(n);
		return addNumbers(*this, b);
	}
	

	bignum bignum:: operator - (bignum b)
	{
		return subtractNumbers(*this, b);
	}

	bignum bignum:: operator - (int n)
	{
		if (base != 10)
		{
			bignum temp(n);
			temp.convertBase(base);

			return subtractNumbers(*this, temp);
		}

		bignum b(n);
		return subtractNumbers(*this, b);
	}

	void bignum::operator *= (bignum b)
	{
		*this = *this * b;
	}

	void bignum::operator *= (int n)
	{
		if (base != 10)
		{
			bignum temp(n);
			temp.convertBase(base);

			*this = *this * temp;
		}

		else
		{
			bignum b(n);
			*this = *this * b;
		}
	}

	void bignum::operator /= (bignum b)
	{
		*this = *this / b;
	}

	void bignum::operator /= (int n)
	{
		if (base != 10)
		{
			bignum temp(n);
			temp.convertBase(base);

			*this = *this * temp;
		}

		else
		{
			bignum b(n);
			*this = *this / b;
		}	
	}

	void bignum::operator += (bignum b)
	{
		*this = *this + b;
	}

	void bignum::operator += (int n)
	{
		if (base != 10)
		{
			bignum temp(n);
			temp.convertBase(base);

			*this = *this + temp;
		}

		else
		{
			bignum b(n);
			*this = *this + b;
		}
	}

	void bignum::operator -= (bignum b)
	{
		*this = *this - b;
	}

	void bignum::operator -= (int n)
	{
		if (base != 10)
		{
			bignum temp(n);
			temp.convertBase(base);

			*this = *this - temp;
		}

		else
		{
			bignum b(n);
			*this = *this - b;
		}
	}
	*/
    
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
				throw bignum_Error(__FILE__, __LINE__, "the specified base is smaller than one or more of the bignum digits, could not set base manually");
		}

		base = n;
	}

	//FUNCTION FOR CONVERTING BASES BY COUNTING UP/DOWN IN PARALLEL
	void bignum::convertBaseSimple(int n)
	{
		if (base != n)
		{
			bignum zero(0);
			zero.setBase(base);

			bool original_negative = negative;

			if (equals(*this, zero))
				base = n;

			else
			{
				bignum counter = absolute();

				bignum converted;
				converted.setBase(n);

				while (greaterThan(counter, zero))
				{
					converted++;
					counter--;
				}

				*this = converted;
			}

			negative = original_negative;
			updateDigits();
		}
	}

	//FUNCTION FOR CONVERTING BASES
	void bignum::convertBase(int n)
	{
		bool original_negative = negative;

		bignum toAdd;
		bignum temp;
		bignum zero;
		zero.setBase(n);
		temp.setBase(n);

		if (equals(*this, zero))
			base = n;

		else if (base != n)
		{
			int counter = digitRange;
			
			for (int i = 0; i<counter; i++)
			{
				int marker(digitCount - i - 1);

				bignum converted_digit(getDigit(marker));
				converted_digit.convertBaseSimple(n);

				bignum ten(10);
				ten.setBase(base);
				ten.convertBaseSimple(n);

				bignum nth(marker - PRECISION);
				nth.convertBaseSimple(n);

				bignum multiplier = exponent(ten, nth);

				toAdd = multiplyNumbers(converted_digit, multiplier);

				temp += toAdd;

				/*
				//start marker at the left-most digit and continue through all digits
				int marker(digitCount - i - 1);

				if (marker >= MAXDIGITS || marker < 0)
					throw bignum_Error(__FILE__, __LINE__, "void bignum::convertBase(int n): The program has attempted to calculate a value outside of its limits");

				//convert individual digit to a different base
				bignum bn1(getDigit(marker));
				bn1.convertBaseSimple(n);

				//create a multiplier based on the position of the digit (Nth power)
				bignum bn2(10);
				bn2.setBase(base);

				bn2.convertBaseSimple(n);
				signed int power_int = marker - PRECISION;
				bignum power(power_int);

				power.convertBaseSimple(n);
				bignum multiplier = exponent(bn2, power);

				//add value to the temporary return value
				toAdd = multiplyNumbers(bn1, multiplier);

				temp += toAdd;
				*/
			}
			*this = temp;
		}
		negative = original_negative;
	}
    
	//returns number of digits including all decimal places set by PRECISION macro
    int bignum::getDigitCount()
    {
    	updateDigits();
    	return digitCount;
    }
    
	//returns number of non-zero digits below 0
    int bignum::getDecimalCount()
    {
    	updateDigits();
    	return decimalCount;
    }
    
	//returns a string char from an int passed
    string bignum::getDigitString(int n)
    {
		string temp;
        
        if (n<10)
        {
            char toAdd = '0';
            toAdd += n;
            temp += toAdd;
            return temp;
        }
            
        else 
        {
            char toAdd = 'A';
            toAdd += (n-10);
            temp += toAdd;
            return temp;
        }
    }
    
	//returns a string representing stored value
    string bignum::getNumberString(bool include_commas, bool percent, int decimal_places)
    {
	updateDigits();

        bignum temp(*this);
        string tempString;
       
        if (decimal_places>PRECISION)
            decimal_places=PRECISION;
       
    	temp.updateDigits();
  
    	if (equals(temp, bignum(0)))
    	{
    		tempString += "0";
    		
    		for (int i=0; i<decimal_places; i++)
    		{
    			tempString += (i==0 ? ".0" : "0");
    		}
    		
    		if (percent == true)
    			tempString +="%";
    		
    		return tempString;
    	}
    
		if (percent)
		{
			bignum hundred(100);
			hundred.setBase(base);
			temp *= hundred;
		}
    		
    	
    	int comma = (temp.getDigitCount()-PRECISION) % 3;
    
    	if (negative==true)
    		 tempString += "-";
    
    	temp.adjustPrecision(decimal_places);
    
        int counter = temp.getDigitRange();
    	for (int i=0; i<counter; i++)
    	{
    	    int target = temp.getDigitCount()-i-1;
    	    
    		if (target == (PRECISION-1))
    			tempString += ".";
    
    		else if (comma==0 && (temp.getDigitCount()-i) >= PRECISION)
    		{
    			if (i>0 && include_commas==true)
    				tempString += ",";
    				 
    			comma = 3;
    		}
    
            tempString += temp.getDigitString(temp.getDigit(target));
    
    		comma--;
    	}
    	
    	if(percent==true)
    		tempString += "%";
    
    	return tempString;
    }
    
    void bignum::decrement()
    {
    	bignum temp(1);
    	*this -= temp;
    	updateDigits();
    }
    
	//multiplies the number by 10 in its native base by literally shifting digit
    void bignum::timesTen(int n)
    {
		if (digitCount >= MAXDIGITS - 1)
			throw bignum_Error(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

        for (int i=0; i<n; i++)
    	{
    		for (int c=0; c<digitRange; c++)
    		{
    			digits[digitCount-c] = digits[digitCount-1-c];
    		}
    
    		digits[PRECISION-decimalCount] = 0;
    		updateDigits();
    	}
    	updateDigits();
    }
    
	//divides the number by 10 in its native base by literally shifting digit
    void bignum::divideByTen(int n)
    {
		for (int i=n; i>0; i--)
    	{
    		for (int c=(PRECISION-decimalCount); c <= digitCount; c++)
    		{
				if (c == 0)
					throw bignum_Error(__FILE__, __LINE__, "The program has attempted to calculate a value outside of its limits");

    			digits[c-1] = digits[c];
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

		for (int i = MAXDIGITS - 1; i>0; i--)
		{
			if (i == PRECISION)
			{
				digitCount = PRECISION + 1;
				break;
			}

			if (digits[i]>0)
			{
				digitCount = (i + 1);
				break;
			}
		}

		for (int i = 0; i<PRECISION; i++)
		{
			if (digits[i]>0)
			{
				decimalCount = PRECISION - i;
				break;
			}
		}

		if (decimalCount == 0 && digitCount == (PRECISION + 1) && digits[PRECISION] == 0)
			setPositive();

		//sets number of operative digits
		//	12.077 ---> digitRange = 5
		digitRange = digitCount - (PRECISION - decimalCount);
	}
    
	//returns the golden ratio  based on fib(n)/fib(n-1)

    
	//returns absolute value of bignum
    bignum bignum::absolute()
    {
        bignum temp = *this;
    	temp.updateDigits();
        temp.setPositive();
        return temp;
    }
    
    //used in calculation for long division
    bignum bignum::noDecimal()
    {
    	bignum temp(*this);
    	temp.timesTen(getDecimalCount());
    	return temp;
    }
    
	//returns bignum with no decimal places
    bignum bignum::withoutDecimals()
    {
        bignum temp(*this);
        
        for (int i=0; i<PRECISION; i++)
        {
            temp.setDigit(i, 0);
        }
        
        return temp;
    }
    
	//rounds the number to a specific number of decimal places
    void bignum::adjustPrecision(int n)
    {
    	int marker = PRECISION-n;
    	
    	if (digits[marker-1] >= (base/2))
    	{
    		bignum increase;
    		increase.setBase(base);
    		increase.setDigit(marker, 1);
    		
    		(*this) += increase;
    	}
    	
    	for (int i=0; i<marker; i++)
    		digits[i]=0;
    		
    	updateDigits();
    }

    bignum golden(bignum b)
    {
		bignum temp1 = bignum(fibonacci(b));
        bignum temp2 = bignum(fibonacci(b-1));
        
        return (temp2 == 0 ? 0 : temp1/temp2);
    }
    
    bignum golden(int n)
    {
        bignum temp(n);
        return golden(temp);   
    }
    
	//return fibonacci number at location b of the sequence
    bignum fibonacci(bignum b)
    {
		if (b.getDecimalCount() > 0)
			throw bignum_Error(__FILE__, __LINE__, "fibonacci sequence values may only be derived from integers");

		bignum counter;
        bignum high;
        bignum low;
		bignum temp;
		bignum zero;

		counter.setBase(b.getBase());
		high.setBase(b.getBase());
		low.setBase(b.getBase());
		zero.setBase(b.getBase());
        
		if (greaterThan(b, zero))
		{
			high ++;

			while (lessThan(counter, b))
			{
				temp = low;
				low = high;
				high += temp;
				counter++;
			}

			return low;
		}

		else if (lessThan(b, zero))
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
    bignum randomNumberAddPrecision(bignum bn1, bignum bn2, int add_precision)
    {
    	if (add_precision>PRECISION)
    		throw bignum_Error( __FILE__ , __LINE__, "specified precision was too fine");
    	
    	if(bn1.getBase()!=bn2.getBase())
    		throw bignum_Error( __FILE__ , __LINE__, "random numbers can only be generated between numbers of the same base");
    		
    	if (equals(bn1, bn2))
    		return bn1;
    	
    	bignum temp;
    	bignum difference;
    	difference.setBase(bn1.getBase());
    	temp.setBase(bn1.getBase());
    	
		difference = (greaterThan(bn1, bn2) ? bn1 - bn2 : bn2 - bn1);
    	
    	int start = difference.getDigitCount()-1;
    	int tempdigit;
    	
    	int counter = (add_precision>0 ? difference.getDigitRange()+add_precision : difference.getDigitRange());
    	
    	for (int i=0; i<counter; i++)
    	{
    		int marker = (start-i);
    		tempdigit = (rand() % difference.getBase());
    		temp.setDigit(marker, tempdigit);
    	}
    	
    	bignum increment;
    	increment.setBase(difference.getBase());
    	increment.setDigit((difference.getDigitCount()-counter), 1);
    	
    	difference += increment;
    	
    	while(!lessThan(temp, difference) && temp.getNegative()==false)
    	{
    		temp -= difference;
    	}
    	
		return (greaterThan(bn1, bn2) ? bn2 + temp : bn1 + temp);
    }
    
	//returns random number within range with a specified resolution
    bignum randomNumberForcePrecision(bignum bn1, bignum bn2, int force_precision)
    {
    	if (force_precision>PRECISION)
    		throw bignum_Error( __FILE__ , __LINE__, "specified precision was too fine");
    	
    	if(bn1.getBase()!=bn2.getBase())
    		throw bignum_Error( __FILE__ , __LINE__, "random numbers can only be generated between numbers of the same base");
    		
		if (equals(bn1, bn2))
    		return bn1;
    	
    	bignum temp;
    	bignum difference;
    	difference.setBase(bn1.getBase());
    	temp.setBase(bn1.getBase());
    	
    	bn1.adjustPrecision(force_precision);
    	bn2.adjustPrecision(force_precision);
    	
		difference = (greaterThan(bn1, bn2) ? bn1 - bn2 : bn2 - bn1);
    	
    	int start = difference.getDigitCount()-1;
    	int tempdigit;
    	
    	int counter = (difference.getDigitCount()-(PRECISION-force_precision));
    	
    	for (int i=0; i<counter; i++)
    	{
    		int marker = (start-i);
    		tempdigit = (rand() % difference.getBase());
    		temp.setDigit(marker, tempdigit);
    	}
    	
    	bignum increment;
    	increment.setBase(difference.getBase());
    	increment.setDigit((difference.getDigitCount()-counter), 1);
    	
    	difference += increment;
    	
    	while(!lessThan(temp, difference))
    	{
    		temp -= difference;
    	}
    	
		return (greaterThan(bn1, bn2) ? bn2 + temp : bn1 + temp);
    }
    
	//returns average of all values passed
    bignum average(vector<bignum> numbers_passed)
    {
    	bignum counter((int)numbers_passed.size());
    	bignum total;
    	
    	for (int i=0; i<numbers_passed.size(); i++)
    	{
    		total += numbers_passed.at(i);
    	}
    	
		if (equals(total, bignum(0)) || equals(counter, bignum(0)))
    	{
    		return bignum(0);
    	}
    	
    	return divideNumbers(total, counter);
    }

	string bignum_Error::getErrorReport()
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
