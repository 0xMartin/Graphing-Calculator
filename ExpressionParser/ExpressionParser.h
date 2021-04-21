/*
	Expression parser 3.2
	Last-update: 29. 10. 2019
	Author: Martin Krèma

	Math operations: 
	+	add	
	-	subtract
	*	multiply
	/	divide
	^	power noraml		-2^2 = -4		
	p	power (with sign)	-2p2 = 4 -> (-2)^2
	( ) and absolut value |

	Trigonometric ascii id table:
	sin		128
	cos		129
	tg		130
	asin	131
	acos	132
	atg		133
	log		134
	log10	135

	Logic operations:
	! not
	| or
	& and
	= equivalency

	Const:
	PI, e
*/
#ifndef _EXPRESSION_PARSER_H_
#define _EXPRESSION_PARSER_H

//#define DEBUG

#include <Arduino.h>
#include <math.h>

/*
	power with sign, this operations is same as normal power but this include sing of a (a^b) in computing
	Example:
	String ex = "-5^3+4";
	uint8_t pos = 0;
	nextNodes(&ex, '^', &pos);	-> compute: 5^3
	ex = "-5p3+4";
	nextNodes(&ex, '^', &pos);	-> compute: -5^3
	ex = "-5^3+4";
	nextNodes(&ex, 'p', &pos);	-> null
*/
#define POWER_WITH_SIGN 'p'	

//none symbol
#define NONE_SYMBOL ' '

//math functions
#define SIN -128
#define COS -127
#define TG -126
#define ASIN -125
#define ACOS -124
#define ATG -123
#define LOG -122
#define LOG10 -121
#define PI -120

/*
	Parser class
*/
class ExpressionParser
{
	private:
		//sub functions

		//is char digit
		bool isDigit(char c);

		//convert char to digit, char must be digit
		uint8_t charToDigit(char c);

		//substring string
		String substr(String* str, uint8_t offset, uint8_t length);

		//get next number, start reading is (start)
		double nextNumber(String* expression, uint8_t* start);

		//get next two number node clutched be some char * / ..., start reading is (start)
		uint8_t* nextNodes(String* expression, char clutch, uint8_t* start);


		//main functions

		//sum all values
		double sum(String* expression);

		//binary operations × ÷ ^ & | =
		void binaryOperations(String* expression, char operation, bool forward);

		//unary operator (for one number)
		void unaryOperator(String* expression);

		//bracket
		void bracket(String* expression, bool logic, bool aritmetic);

	public:
		//calculatin mode (RADINS = true -> calculate with radians)
		bool RADIAN;

		//number of decimal places before floating point
		uint8_t DECIMAL;

		//constructor
		ExpressionParser(uint8_t decimal, bool radians);

		//get value if expression
		double getValueOfExpression(String expression, bool logic, bool aritmetic);


		//replace var by number
		String replaceVar(String* function, char var, String num);

};

#endif

