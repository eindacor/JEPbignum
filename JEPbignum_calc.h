#ifndef JEP_BIGNUM_CALC
#define JEP_BIGNUM_CALC

#include "JEPbignum.h"
#include <list>

#define MAXENTERED 256
#define RETURN_ERROR return solution(bignum(), __LINE__)
#define RETURN_OK(x) return solution(x, 0)
#define RETURN_FALSE return false
#define RETURN_TRUE return true
#define PRINTLIST(x)

using jep::bignum;
using std::list;

namespace jep
{
	enum STYPE { FACTORIAL = 0, ITERATION, EXPONENT, MULTIPLY, DIVIDE, ADD, SUBTRACT, MODULO, ERROR };
	enum ITYPE { NONE = 0, NUMBER, SYMBOL, PAREN };

	class calc_ptr;
	class settings;
	class solution;
	class symbol_item;
	
	bool checkValid(list<calc_ptr> &itemList);
	list<calc_ptr>::iterator addCharToList(list <calc_ptr> &itemList, list <calc_ptr>::iterator i, char c);
	bool swapItems(list<calc_ptr> &itemList);
	list<calc_ptr>::iterator simplifyRange(list <calc_ptr> &itemList, list <calc_ptr>::iterator i1, list <calc_ptr>::iterator i2, bignum bn);
	void simplify_simpleProblem(list<calc_ptr> &itemList);
	void simplify_factorialProblem(list<calc_ptr> &itemList);
	void simplify_pfemdasProblem(list<calc_ptr> &itemList);
	bool simplify(list<calc_ptr> &itemList);
	solution solve(string &entered, bignum &previous, settings &user);
	list<calc_ptr>::iterator addNumberToList(list <calc_ptr> &itemList, list <calc_ptr>::iterator i, bignum bn);
	bool checkWord(string c, int start, string compare);
	bignum numberFromVector(vector<int> &vec, int dec, settings &user);
	bool isSpace(char const &c);
	STYPE checkSymbol(char &c);
	int checkNumber(char const &c);
	bool isNumber(char const &c, settings &user);
	bool isSymbol(char &c);
	bool generateProblem(string &c, list <calc_ptr> &itemList, settings &user, bignum previous);

	class solution
	{
	public:
		solution(bignum bn, int n) : solved(bn), error(n) {};
		solution() : solved(bignum()), error(0) {};
		~solution(){};

		bignum getSolved() { return solved; }
		int getError() { return error; }

		void operator = (solution s) { solved = s.getSolved(); error = s.getError(); }

	private:
		bignum solved;
		int error;
	};

	class settings
	{
	public:
		settings() { round = ONES_PLACE; percent = false; showDigits = false; base = 10; }
		settings(int n, bool b, bool d) { round = n; percent = b; showDigits = d; base = 10; }
		~settings(){};

		void setRound(int n);
		void setBase(int n) { base = n; }
		void percentOff() { percent = false; }
		void percentOn() { percent = true; }
		void showDigitsOff() { showDigits = false; }
		void showDigitsOn() { showDigits = true; }

		int getRound() { return round; }
		bool getPercent() { return percent; }
		bool getShowDigits() { return showDigits; }
		int getBase() { return base; }

	private:
		int round;
		bool percent;
		bool showDigits;
		int base;
	};

	class calc_item
	{
	public:
		calc_item(ITYPE it) { itemType = it; }
		~calc_item(){};

		ITYPE getItemType() { return itemType; }

		virtual bignum getStoredNumber() { return bignum(0); }
		virtual STYPE getsymbol_type() { return ERROR; }
		virtual char getStoredChar() { return '0'; }
		virtual int getParenID() { return 0; }
		virtual void setParenID(int n) {};
		virtual bool getOpen() { return false; }

	private:
		ITYPE itemType;
	};

	class numberItem : public calc_item
	{
	public:
		numberItem(bignum bn) : calc_item(NUMBER), stored(bn) {};
		~numberItem(){};

		bignum getStoredNumber() { return stored; }

	private:
		bignum stored;
	};

	class symbol_item : public calc_item
	{
	public:
		symbol_item(char c);
		~symbol_item(){};

		char getStoredChar() { return stored; }
		STYPE getsymbol_type() { return symbol_type; }

	private:
		char stored;
		STYPE symbol_type;
	};

	class paren_item : public calc_item
	{
	public:
		paren_item(char c);
		~paren_item(){};

		int getParenID() { return parenID; }
		void setParenID(int n) { parenID = n; }
		char getStoredChar() { return stored; }

		bool getOpen() { return open; }

	private:
		char stored;
		int parenID;
		bool open;
	};

	class calc_ptr
	{
	public:
		calc_ptr(const calc_ptr &original);
		calc_ptr(calc_item* original);
		~calc_ptr(){ delete stored; }

		calc_item* getStored() { return stored; }
		calc_item* readStored() const { return stored; }
		void setStored(calc_item* &original) { stored = original; }
		void setNull() { stored = NULL; }

		calc_item& operator* () const { return *(stored); }

		calc_item* operator-> () const { return stored; }

		static calc_item* duplicate(calc_ptr &original);

	private:
		calc_item* stored;
	};

	class number_primer
	{
	public:
		number_primer();
		~number_primer(){};

		int getDecimalCount() { return decimalCount; }
		int getCommaNumbers() { return decimalCount; }
		int getNumbers() { return numbers; }

		void incrementDecimalCount() { decimalCount++; }
		void incrementCommaNumbers() { decimalCount++; }
		void incrementNumbers() { numbers++; }

		void setCommaNumbers(int n) { commaNumbers = n; }

		bool getDecimal() { return decimal; }
		bool getComma() { return comma; }
		bool getDone() { return done; }
		bool getPrintExact() { return printExact; }
		bool getPrintStats() { return printStats; }
		bool getManualSet() { return manualSet; }

		void setDecimal(bool b) { decimal = b; }
		void setComma(bool b) { comma = b; }
		void setDone(bool b) { done = b; }
		void setPrintExact(bool b) { printExact = b; }
		void sgetPrintStats(bool b) { printStats = b; }
		void sgetManualSet(bool b) { manualSet = b; }

		void reset();

	private:
		int decimalCount;
		int commaNumbers;
		int numbers;

		bool decimal;
		bool comma;
		bool done;
		bool printExact;
		bool printStats;
		bool manualSet;
	};
}

#endif