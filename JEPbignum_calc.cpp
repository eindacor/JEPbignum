#include "JEPbignum_calc.h"

namespace jep
{
	symbol_item::symbol_item(char c) : calc_item(SYMBOL)
	{
		stored = c;

		switch (c)
		{
		case '+': symbol_type = ADD;
			break;

		case '-': symbol_type = SUBTRACT;
			break;

		case '*': symbol_type = MULTIPLY;
			break;

		case 'x': symbol_type = MULTIPLY;
			break;

		case '/': symbol_type = DIVIDE;
			break;

		case '^': symbol_type = EXPONENT;
			break;

		case '!': symbol_type = FACTORIAL;
			break;

		case '%': symbol_type = MODULO;
			break;

		case 'r': symbol_type = ROOT;
			break;

		case 'c': symbol_type = ITERATION;
			break;

		default: symbol_type = ERROR;
			break;
		}
	}

	paren_item::paren_item(char c) : calc_item(PAREN)
	{
		stored = c;

		switch (c)
		{
		case '(': open = true;
			break;

		case ')': open = false;
			break;

		default: cout << "\nAn error has occured: " << __FILE__ << ", " << __LINE__;
			break;

		}

		parenID = 0;
	}

	calc_item* calc_ptr::duplicate(calc_ptr &original)
	{
		calc_item* temp = NULL;


		switch (original->getItemType())
		{
		case SYMBOL:
		{
			char originalChar = original->getStoredChar();
			temp = new symbol_item(originalChar);
			return temp;
		}

		case NUMBER:
		{
			bignum originalNumber = original->getStoredNumber();
			temp = new numberItem(originalNumber);
			return temp;
		}

		case PAREN:
		{
			char originalChar = original->getStoredChar();
			temp = new paren_item(originalChar);
			return temp;
		}

		default:
		{
			cout << endl << __FILE__ << " (line " << __LINE__ << "): An error has occurred.";
			break;
		}
		}

		return temp;
	}

	calc_ptr::calc_ptr(const calc_ptr &original)
	{
		if (original.readStored() == NULL)
			stored = NULL;

		else
		{

			switch (original->getItemType())
			{
			case SYMBOL:
			{
				char originalChar = original->getStoredChar();
				stored = new symbol_item(originalChar);
				break;
			}

			case NUMBER:
			{
				bignum originalNumber = original->getStoredNumber();
				stored = new numberItem(originalNumber);
				break;
			}

			case PAREN:
			{
				char originalChar = original->getStoredChar();
				stored = new paren_item(originalChar);
				break;
			}

			default:
			{
				cout << endl << __FILE__ << " (line " << __LINE__ << "): An error has occurred.";
				stored = NULL;
				break;
			}
			}
		}
	}

	calc_ptr::calc_ptr(calc_item* original)
	{
		if (original == NULL)
			stored = NULL;

		else
		{

			switch (original->getItemType())
			{
			case SYMBOL:
			{
				char originalChar = original->getStoredChar();
				stored = new symbol_item(originalChar);
				break;
			}

			case NUMBER:
			{
				bignum originalNumber = original->getStoredNumber();
				stored = new numberItem(originalNumber);
				break;
			}

			case PAREN:
			{
				char originalChar = original->getStoredChar();
				stored = new paren_item(originalChar);
				break;
			}

			default:
			{
				cout << endl << __FILE__ << " (line " << __LINE__ << "): An error has occurred.";
				stored = NULL;
				break;
			}
			}
		}
	}

	number_primer::number_primer()
	{
		decimalCount = 0;
		commaNumbers = 0;
		numbers = 0;

		decimal = false;
		comma = false;
		done = false;
		printExact = false;
		printStats = false;

		manualSet = false;
	}

	void number_primer::reset()
	{
		decimalCount = 0;
		commaNumbers = 0;
		numbers = 0;

		decimal = false;
		comma = false;
		done = false;
		printExact = false;
		printStats = false;

		manualSet = false;
	}

	bool checkValid(list<calc_ptr> &itemList)
	{
		if (itemList.size() == 0)
		{
			RETURN_FALSE;
		}

		//FAIL CONDITIONS IF THE LIST HAS ONLY ONE ELEMENT
		if (itemList.size() == 1)
		{
			list<calc_ptr>::iterator i = itemList.begin();

			if ((*i)->getItemType() == NUMBER)
			{
				RETURN_TRUE;
			}

			else RETURN_FALSE;
		}

		int oParenCount = 0;
		int cParenCount = 0;

		for (list<calc_ptr>::iterator i = itemList.begin(); i != itemList.end(); ++i)
		{
			if (itemList.size() > 2 && i != itemList.begin() && i != std::next(itemList.begin(), 1))
			{
				list<calc_ptr>::iterator i3 = i;
				list<calc_ptr>::iterator i2 = std::prev(i, 1);
				list<calc_ptr>::iterator i1 = std::prev(i, 2);

				//  3 symbols in a row
				if ((*i1)->getItemType() == SYMBOL && (*i2)->getItemType() == SYMBOL && (*i3)->getItemType() == NUMBER && (*i3)->getStoredNumber().isNegative())
				{
					RETURN_FALSE;
				}

				//  -- followed by a negative number   
				if ((*i1)->getItemType() == SYMBOL && (*i2)->getItemType() == SYMBOL && (*i3)->getItemType() == SYMBOL)
				{
					RETURN_FALSE;
				}
			}

			//FAIL CONDITIONS FOR FIRST ITEM:
			if (i == itemList.begin())
			{
				list<calc_ptr>::iterator after = std::next(i, 1);

				switch ((*i)->getItemType())
				{
				case PAREN:
					if ((*i)->getOpen())
					{
						//  ()
						if ((*after)->getItemType() == PAREN && !(*after)->getOpen())
						{
							RETURN_FALSE;
						}

						//  (*  
						if ((*after)->getItemType() == SYMBOL && (*after)->getsymbol_type() != SUBTRACT)
						{
							RETURN_FALSE;
						}

						oParenCount++;
						break;
					}

					else
					{
						//  ) as first item
						RETURN_FALSE;
					}

					break;

				case SYMBOL: RETURN_FALSE;

				case NUMBER:
					//  34 52
					if ((*after)->getItemType() == NUMBER)
					{
						RETURN_FALSE;
					}

					//  -4!
					if ((*i)->getStoredNumber().isNegative() && (*after)->getItemType() == SYMBOL && (*after)->getsymbol_type() == FACTORIAL)
					{
						RETURN_FALSE;
					}

					//  .4!
					if ((*i)->getStoredNumber().getDecimalCount() > 0 && (*after)->getItemType() == SYMBOL && (*after)->getsymbol_type() == FACTORIAL)
					{
						RETURN_FALSE;
					}

					break;

				default: RETURN_FALSE;
				}
			}

			//FAIL CONDITIONS FOR LAST ITEM
			list<calc_ptr>::iterator lastItem = std::prev(itemList.end(), 1);
			if (i == lastItem)
			{
				switch ((*i)->getItemType())
				{
				case PAREN:
					//  open paren as last item
					if ((*i)->getOpen())
					{
						RETURN_FALSE;
					}

					else
					{
						cParenCount++;
						break;
					}

					break;

				case SYMBOL:
					//  symbol other than ! as last item
					if ((*i)->getsymbol_type() != FACTORIAL)
					{
						RETURN_FALSE;
					}

					break;

				case NUMBER: break;

				default: RETURN_FALSE;
				}
			}

			//FAIL CONDITIONS FOR MIDDLE ITEMS
			if (i != itemList.begin() && i != lastItem)
			{
				list<calc_ptr>::iterator after = std::next(i, 1);

				switch ((*i)->getItemType())
				{
				case PAREN:
					if ((*i)->getOpen())
					{
						//  ()
						if ((*after)->getItemType() == PAREN && !(*after)->getOpen())
						{
							RETURN_FALSE;
						}

						//  (*
						if ((*after)->getItemType() == SYMBOL && (*after)->getsymbol_type() != SUBTRACT)
						{
							RETURN_FALSE;
						}

						oParenCount++;
					}

					else cParenCount++;

					break;

				case SYMBOL:
					//  !!
					if ((*after)->getItemType() == SYMBOL && (*after)->getsymbol_type() == FACTORIAL && (*i)->getsymbol_type() == FACTORIAL)
					{
						RETURN_FALSE;
					}

					//  *+
					if ((*after)->getItemType() == SYMBOL && (*after)->getsymbol_type() != SUBTRACT && (*i)->getsymbol_type() != FACTORIAL)
					{
						RETURN_FALSE;
					}

					//  ^.7
					if ((*i)->getsymbol_type() == EXPONENT && (*after)->getItemType() == NUMBER && (*after)->getStoredNumber().getDecimalCount() > 0)
					{
						RETURN_FALSE;
					}

					//  +)
					if ((*i)->getsymbol_type() != FACTORIAL && (*after)->getItemType() == PAREN && !(*after)->getOpen())
					{
						RETURN_FALSE;
					}

					break;

				case NUMBER:
					//  34 51
					if ((*after)->getItemType() == NUMBER)
					{
						RETURN_FALSE;
					}

					// -4!
					if ((*i)->getStoredNumber().isNegative() && (*after)->getItemType() == SYMBOL && (*after)->getsymbol_type() == FACTORIAL)
					{
						RETURN_FALSE;
					}

					//  .4!
					if ((*i)->getStoredNumber().getDecimalCount() > 0 && (*after)->getItemType() == SYMBOL && (*after)->getsymbol_type() == FACTORIAL)
					{
						RETURN_FALSE;
					}

					break;

				default: RETURN_FALSE;
				}
			}

			//more closed parentheses than open
			if (cParenCount > oParenCount) {
				RETURN_FALSE;
			}
		}

		//inequal paren counts
		if (oParenCount != cParenCount) {
			RETURN_FALSE;
		}

		else RETURN_TRUE;
	}

	list<calc_ptr>::iterator addCharToList(list <calc_ptr> &itemList, list <calc_ptr>::iterator i, char c)
	{
		calc_item* temp = NULL;

		if (c == '(' || c == ')')
			temp = new paren_item(c);


		else temp = new symbol_item(c);

		calc_ptr toAdd(temp);

		itemList.insert(i, toAdd);

		--i;

		delete temp;
		temp = NULL;

		return i;
	}


	bool swapItems(list<calc_ptr> &itemList)
	{
		//PRINTLIST(itemList);

		if (itemList.size() < 2)
		{
			if (!checkValid(itemList))
			{
				RETURN_FALSE;
			}

			else RETURN_TRUE;
		}

		list<calc_ptr>::iterator i = itemList.begin();

		do
		{
			//PRINTLIST(itemList);

			if (!checkValid(itemList))
			{
				RETURN_FALSE;
			}

			//creates iterator for second element
			list <calc_ptr>::iterator second = std::next(itemList.begin(), 1);

			//checks groups of 3 elements, i1, i2, and i3
			if (itemList.size() > 2 && i != (itemList.begin()) && i != second)
			{
				list<calc_ptr>::iterator i3 = i;
				list<calc_ptr>::iterator i2 = std::prev(i, 1);
				list<calc_ptr>::iterator i1 = std::prev(i, 2);

				//if the problem has 2 symbols followed by a number, checkvalid confirms the second symbol is a negative, which means the number is negative
				if ((*i1)->getItemType() == SYMBOL && (*i2)->getItemType() == SYMBOL && (*i3)->getItemType() == NUMBER && (*i1)->getsymbol_type() != FACTORIAL)
				{
					itemList.erase(i2);
					bignum temp = (*i3)->getStoredNumber();
					temp.setNegative();
					addNumberToList(itemList, i3, temp);
					itemList.erase(i3);
					i = itemList.begin();
					continue;
				}

				//IF A PROBLEM SAYS "(-N", CONVERT THE N TO NEGATIVE AND ELIMINATE THE SYMBOL
				if ((*i1)->getItemType() == PAREN && (*i1)->getOpen() && (*i2)->getItemType() == SYMBOL && (*i3)->getItemType() == NUMBER)
				{
					itemList.erase(i2);
					bignum temp = (*i3)->getStoredNumber();
					temp.setNegative();
					addNumberToList(itemList, i3, temp);
					itemList.erase(i3);
					i = itemList.begin();
					continue;
				}
			}

			//first item
			if (i == itemList.begin())
			{
				list<calc_ptr>::iterator after = std::next(i, 1);

				switch ((*i)->getItemType())
				{
					//if the first item is a number and the next item is an open parenthesis
				case NUMBER:
					// "N(" becomes "N*("
					if ((*after)->getItemType() == PAREN && (*after)->getOpen())
					{
						addCharToList(itemList, after, '*');
						i = itemList.begin();
						continue;
					}
					break;

				default: break;
				}
			}

			list<calc_ptr>::iterator lastItem = std::prev(itemList.end(), 1);

			//items between first and last
			if (i != itemList.begin() && i != lastItem)
			{
				list<calc_ptr>::iterator after = std::next(i, 1);

				switch ((*i)->getItemType())
				{
				case PAREN:
					// ")N" becomes ")*N"
					if (!(*i)->getOpen() && (*after)->getItemType() == NUMBER)
					{
						addCharToList(itemList, after, '*');
						i = itemList.begin();
						continue;
					}

					// ")(" becomes ")*("
					if (!(*i)->getOpen() && (*after)->getItemType() == PAREN && (*after)->getOpen())
					{
						addCharToList(itemList, after, '*');
						i = itemList.begin();
						continue;
					}
					break;

				case SYMBOL:
					// "!(" becomes "!*("
					if ((*i)->getsymbol_type() == FACTORIAL && (*after)->getItemType() == PAREN && (*after)->getOpen())
					{
						addCharToList(itemList, after, '*');
						i = itemList.begin();
						continue;
					}
					break;

				case NUMBER:
					// "N(" becomes "N*("
					if ((*after)->getItemType() == PAREN && (*after)->getOpen())
					{
						addCharToList(itemList, after, '*');
						i = itemList.begin();
						continue;
					}
					break;

				default: RETURN_FALSE;
				}
			}

			++i;

		} while (i != itemList.end());

		if (!checkValid(itemList))
		{
			RETURN_FALSE;
		}

		else
		{
			RETURN_TRUE;
		}
	}

	list<calc_ptr>::iterator simplifyRange(list <calc_ptr> &itemList, list <calc_ptr>::iterator i1, list <calc_ptr>::iterator i2, bignum bn)
	{
		list <calc_ptr>::iterator insertionPoint = addNumberToList(itemList, i2, bn);
		itemList.erase(i1, insertionPoint);
		return insertionPoint;
	}

	void simplify_simpleProblem(list<calc_ptr> &itemList)
	{
		//PRINTLIST(itemList);
		bignum temp;

		//setting iterators for the 1st, 2nd, and 3rd elements: i1, i2, and i3 respectively
		list<calc_ptr>::iterator i1 = itemList.begin();
		list<calc_ptr>::iterator i2 = std::next(itemList.begin(), 1);
		list<calc_ptr>::iterator i3 = std::next(itemList.begin(), 2);

		//generates a new number based on the symbol between the numbers passed
		switch ((*i2)->getsymbol_type())
		{
		case ADD:
			temp = addNumbers((*i1)->getStoredNumber(), (*i3)->getStoredNumber());
			break;

		case SUBTRACT:
			temp = subtractNumbers((*i1)->getStoredNumber(), (*i3)->getStoredNumber());
			break;

		case MULTIPLY:
			temp = multiplyNumbers((*i1)->getStoredNumber(), (*i3)->getStoredNumber());
			break;

		case DIVIDE:
			temp = divideNumbers((*i1)->getStoredNumber(), (*i3)->getStoredNumber());
			break;

		case EXPONENT:
			temp = exponent((*i1)->getStoredNumber(), (*i3)->getStoredNumber());
			break;

		case ITERATION:
			temp = combinations((*i1)->getStoredNumber(), (*i3)->getStoredNumber());
			break;

		case MODULO:
			temp = modulo((*i1)->getStoredNumber(), (*i3)->getStoredNumber());
			break;

		case ROOT:
			temp = root((*i1)->getStoredNumber(), (*i3)->getStoredNumber());
			break;

		default: cout << __FILE__ << ", line " << __LINE__ << ": An error has occurred.";
			break;
		}

		//replaces the range given with the new number
		simplifyRange(itemList, itemList.begin(), itemList.end(), temp);
	}

	void simplify_factorialProblem(list<calc_ptr> &itemList)
	{
		//PRINTLIST(itemList);
		list<calc_ptr>::iterator i1 = itemList.begin();
		list<calc_ptr>::iterator i2 = std::next(itemList.begin(), 1);

		bignum factResult = factorial((*i1)->getStoredNumber());

		itemList.clear();
		addNumberToList(itemList, itemList.end(), factResult);
	}

	void simplify_pfemdasProblem(list<calc_ptr> &itemList)
	{
		//PRINTLIST(itemList);
		bignum temp;

		while (itemList.size() > 1)
		{
			bool listChanged = false;

			//runs through each type of problem from pfemdas prioritization, checks for applicable equations, simplifies, them, and replaces them with a value
			for (int typeCheck = 0; typeCheck < 7; ++typeCheck)
			{
				//PRINTLIST(itemList);
				STYPE symbol_type = (STYPE)typeCheck;

				//runs through all elements, checking for that symbol
				for (list<calc_ptr>::iterator i = itemList.begin(); i != itemList.end(); ++i)
				{
					//if the symbol found matches the current pfemdas symbol
					if ((*i)->getItemType() == SYMBOL && (*i)->getsymbol_type() == typeCheck)
					{
						//special case for factorial symbol
						if ((*i)->getsymbol_type() == FACTORIAL)
						{
							//create iterator for number before the symbol, simplify
							list<calc_ptr>::iterator before = std::prev(i, 1);
							list<calc_ptr>::iterator after = std::next(i, 1);
							temp = factorial((*before)->getStoredNumber());
							simplifyRange(itemList, before, after, temp);
							listChanged = true;
						}

						else
						{
							//create iterators for the numbers before and after the symbol found
							list<calc_ptr>::iterator before = std::prev(i, 1);
							list<calc_ptr>::iterator after = std::next(i, 1);
							switch (typeCheck)
							{
							case EXPONENT: temp = exponent((*before)->getStoredNumber(), (*after)->getStoredNumber());
								break;

							case ITERATION: temp = combinations((*before)->getStoredNumber(), (*after)->getStoredNumber());
								break;

							case MULTIPLY: temp = multiplyNumbers((*before)->getStoredNumber(), (*after)->getStoredNumber());
								break;

							case DIVIDE: temp = divideNumbers((*before)->getStoredNumber(), (*after)->getStoredNumber());
								break;

							case ADD: temp = addNumbers((*before)->getStoredNumber(), (*after)->getStoredNumber());
								break;

							case SUBTRACT: temp = subtractNumbers((*before)->getStoredNumber(), (*after)->getStoredNumber());
								break;

							default: cout << __FILE__ << ", line " << __LINE__ << ": An error has occurred.";
								break;
							}
							simplifyRange(itemList, before, std::next(after, 1), temp);
							listChanged = true;
						}
					}//if symbol matches target symbol

					if (listChanged)
						break;
				}//for loop checking list for target symbol

				if (listChanged)
					break;
			}//for loop cycling through the pfemdas order
		}//while loop to simplify until a final number exists
	}

	bool simplify(list<calc_ptr> &itemList)
	{
		if (!checkValid(itemList))
		{
			RETURN_FALSE;
		}

		int parenCount = 0;
		int parenHigh = 0;

		//count parentheses
		for (list<calc_ptr>::iterator pCount = itemList.begin(); pCount != itemList.end(); ++pCount)
		{
			if ((*pCount)->getItemType() == PAREN)
			{
				if ((*pCount)->getOpen())
				{
					parenCount++;
					if (parenCount > parenHigh)
						parenHigh = parenCount;

					(*pCount)->setParenID(parenCount);
				}

				else
				{
					(*pCount)->setParenID(parenCount);
					parenCount--;
				}
			}
		}

		//if there are any parentheses in the problem, find elements within the highest parentheses and simplify them
		if (parenHigh > 0)
		{
			list<calc_ptr> replace;
			list<calc_ptr>::iterator insertionPoint;
			list<calc_ptr>::iterator rangeToReplace_start;
			list<calc_ptr>::iterator rangeToReplace_end;
			bool add = false;

			list<calc_ptr>::iterator i = itemList.begin();

			do
			{
				if ((*i)->getItemType() == PAREN && (*i)->getParenID() == parenHigh)
				{
					if ((*i)->getOpen())
					{
						rangeToReplace_start = i;
						add = true;
					}

					else
					{
						rangeToReplace_end = std::next(i, 1);
						insertionPoint = std::next(i, 1);
						itemList.erase(rangeToReplace_start, rangeToReplace_end);
						simplify(replace);

						itemList.insert(insertionPoint, replace.begin(), replace.end());
						replace.clear();

						RETURN_TRUE;
					}
				}

				else if (add)
					replace.insert(replace.end(), (*i));

				++i;

			} while (i != itemList.end());
		}

		else
		{
			//must be a single number, do nothing
			if (itemList.size() == 1)
			{
				RETURN_TRUE;
			}

			//only 2 items, must be factorial
			if (itemList.size() == 2)
			{
				simplify_factorialProblem(itemList);
				RETURN_TRUE;
			}

			//only 3 items, must be a simple problem
			else if (itemList.size() == 3)
			{
				simplify_simpleProblem(itemList);
				RETURN_TRUE;
			}

			//more than 3 items, must be a problem with multiple elements
			else
			{
				simplify_pfemdasProblem(itemList);
				RETURN_TRUE;
			}
		}
	}

	solution solve(string &entered, bignum &previous, settings &user)
	{
		list<calc_ptr> itemList;
		bool usePrevious = false;

		bignum temp;

		//populate list
		if (!generateProblem(entered, itemList, user, previous))
		{
			RETURN_ERROR;
		}

		//if nothing has been entered, return previous
		if (itemList.size() == 0)
		{
			addNumberToList(itemList, itemList.end(), previous);
			usePrevious = true;
		}

		//if it starts with a symbol
		list<calc_ptr>::iterator start = itemList.begin();
		if ((*start)->getItemType() == SYMBOL)
		{
			//if it's anything other than subtract, add previous
			if ((*start)->getsymbol_type() != SUBTRACT)
			{
				addNumberToList(itemList, itemList.begin(), previous);
				usePrevious = true;
			}

			//if its subtract
			else if (itemList.size() > 1)
			{
				list<calc_ptr>::iterator after = std::next(start, 1);

				//if the 2nd is a subtract symbol, add previous
				if ((*after)->getItemType() == SYMBOL && (*after)->getsymbol_type() == SUBTRACT)
				{
					addNumberToList(itemList, itemList.begin(), previous);
					usePrevious = true;
				}

				//if the 2nd is a number 
				if ((*after)->getItemType() == NUMBER)
				{
					//if the calculator is clear, set the number to negative and remove the symbol
					if (previous == 0)
					{
						bignum firstNumber = (*after)->getStoredNumber();
						firstNumber.setNegative();
						itemList.erase(start);
						itemList.erase(after);
						addNumberToList(itemList, itemList.begin(), firstNumber);
					}

					//if there was a number stored, add it to the problem
					else
					{
						addNumberToList(itemList, itemList.begin(), previous);
						usePrevious = true;
					}
				}
			}
			else RETURN_ERROR;
		}

		//initial swap
		if (!swapItems(itemList))
		{
			RETURN_ERROR;
		}

		//simplification loop
		do
		{
			simplify(itemList);

			if (!swapItems(itemList))
			{
				RETURN_ERROR;
			}

		} while (itemList.size() > 1);


		list<calc_ptr>::iterator solved = itemList.begin();
		temp = (*solved)->getStoredNumber();

		RETURN_OK(temp);
	}

	list<calc_ptr>::iterator addNumberToList(list <calc_ptr> &itemList, list <calc_ptr>::iterator i, bignum bn)
	{
		calc_item* temp = new numberItem(bn);
		calc_ptr toAdd(temp);

		itemList.insert(i, toAdd);

		--i;

		delete temp;
		temp = NULL;

		return i;
	}

	bool checkWord(string c, int start, string compare)
	{
		int sizeCheck = start + compare.size();
		if (sizeCheck > c.size())
		{
			RETURN_FALSE;
		}

		for (int i = 0; i < compare.size(); i++)
		{
			if (c[start + i] != compare[i])
			{
				RETURN_FALSE;
			}
		}

		RETURN_TRUE;
	}

	bignum numberFromVector(vector<int> &vec, int dec, settings &user)
	{
		bignum temp;
		temp.setBase(user.getBase());

		for (int i = 0; i < vec.size(); i++)
		{
			int numberToUse = vec.at(vec.size() - i - 1);
			int locationToSet = ONES_PLACE + i;
			temp.setDigit(locationToSet, numberToUse);
		}

		for (int i = 0; i < dec; i++)
			temp /= 10;

		return temp;
	}

	bool isSpace(char const &c)
	{
		return (c == ' ');
	}

	STYPE checkSymbol(char &c)
	{
		switch (c)
		{
		case '+': return ADD;
		case '-': return SUBTRACT;
		case '*': return MULTIPLY;
		case '/': return DIVIDE;
		case '^': return EXPONENT;
		case '!': return FACTORIAL;
		case '%': return MODULO;
		case 'r': return ROOT;
		case 'c': return ITERATION;
		default: return ERROR;
		}
	}

	//returns the number of the passed character
	int checkNumber(char const &c)
	{
		int i = c - '0';

		if (i >= 0 && i <= 9)
		{
			return i;
		}

		else
		{
			i = 10 + (c - 'A');
			return i;
		}
	}

	//verifies the character passed is a valid number within the current base
	bool isNumber(char const &c, settings &user)
	{
		return (checkNumber(c) >= 0 && checkNumber(c) <= (user.getBase() - 1));
	}

	//verifies the character passed is a valid symbol
	bool isSymbol(char &c)
	{
		return (checkSymbol(c) != ERROR);
	}

	bool generateProblem(string &c, list <calc_ptr> &itemList, settings &user, const bignum &previous)
	{
		vector<int> numberVector;

		bignum temp;
		temp.setBase(user.getBase());

		number_primer primer;

		int counter = c.size();

		for (int i = 0; i < counter; i++)
		{
			//if "ans" has been entered, add previous number and advance the counter
			if (checkWord(c, i, "ans"))
			{
				if (primer.getNumbers()>0)
				{
					temp = numberFromVector(numberVector, primer.getDecimalCount(), user);
					addNumberToList(itemList, itemList.end(), temp);
					primer.reset();
					numberVector.clear();

					//add * between the number and pi
					addCharToList(itemList, itemList.end(), '*');
				}

				addNumberToList(itemList, itemList.end(), previous);
				numberVector.clear();

				i += 3;
			}

			if (i == counter)
				break;

			//if it isn't a space, number, symbol, end marker, or decimal point, return error
			if (!isSpace(c[i]) &&
				!isNumber(c[i], user) &&
				!isSymbol(c[i]) &&
				c[i] != ',' &&
				c[i] != '(' &&
				c[i] != ')' &&
				c[i] != '.')
			{
				RETURN_FALSE;
			}

			if (c[i] == ',')
			{
				if (!primer.getComma())
				{
					if (primer.getDecimal() || primer.getNumbers() == 0 || primer.getNumbers() > 3)
					{
						RETURN_FALSE;
					}

					else primer.setComma(true);
				}

				else if (primer.getCommaNumbers() != 3)
				{
					RETURN_FALSE;
				}

				primer.setCommaNumbers(0);
			}

			//if it's a space
			else if (isSpace(c[i]))
			{
				//if it's preceeded by a number, finish the number and add to the problem
				if (isNumber(c[i - 1], user))
				{
					temp = numberFromVector(numberVector, primer.getDecimalCount(), user);
					addNumberToList(itemList, itemList.end(), temp);
					primer.reset();
					numberVector.clear();
				}
			}

			//if it's a number
			else if (isNumber(c[i], user))
			{
				numberVector.push_back(checkNumber(c[i]));
				primer.incrementNumbers();

				if (primer.getComma())
					primer.incrementCommaNumbers();

				if (primer.getDecimal())
					primer.incrementDecimalCount();
			}

			//if it's a symbol
			else if (checkSymbol(c[i]) != ERROR)
			{
				//if the primer has been primed with numbers, add that number to the problem
				if (primer.getNumbers() > 0)
				{
					temp = numberFromVector(numberVector, primer.getDecimalCount(), user);
					addNumberToList(itemList, itemList.end(), temp);
					primer.reset();
					numberVector.clear();
				}

				//add the symbol item to the problem 
				addCharToList(itemList, itemList.end(), c[i]);
			}

			//if it's a parenthesis
			else if (c[i] == '(' || c[i] == ')')
			{
				//if the primer has been primed with numbers, add that number to the problem
				if (primer.getNumbers() > 0)
				{
					temp = numberFromVector(numberVector, primer.getDecimalCount(), user);
					addNumberToList(itemList, itemList.end(), temp);
					primer.reset();
					numberVector.clear();
				}

				//add the paren item to the problem  
				addCharToList(itemList, itemList.end(), c[i]);
			}

			//if it's a decimal point
			else if (c[i] == '.')
			{
				//if there's already been a decimal point, return error
				if (primer.getDecimal())
				{
					RETURN_FALSE;
				}

				//if there isn't a number after, return error
				else if (!isNumber(c[i + 1], user))
				{
					RETURN_FALSE;
				}

				//if the comma segment is incomplete
				else if (primer.getComma() && primer.getCommaNumbers() != 3)
				{
					RETURN_FALSE;
				}

				else primer.setDecimal(true);
			}
		}

		//if the primer has been primed with numbers, add that number to the problem
		if (primer.getNumbers() > 0)
		{
			temp = numberFromVector(numberVector, primer.getDecimalCount(), user);
			addNumberToList(itemList, itemList.end(), temp);
			primer.reset();
			numberVector.clear();
		}

		RETURN_TRUE;
	}
}