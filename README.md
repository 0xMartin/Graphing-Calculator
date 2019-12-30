# Graphing-Calculator
Graphing calculator using atmega328p. Calculator is able to calculating mathematical and logical expressions. As display use nokia5110 and for input use 4x4 array of micro switches + 1x power switch. Input input voltage of pcb board is 7.2-15V.

## Calculator modes
 * Calculator
 * Grapher
 * Ploter
 * Logic parser
 
## Expression parser
Parser in programed in c++ and work for arduino and desktop aplications (with some corestions). This parser first replace all constants (e, pi) by numbers and after that evaluate all brackets than all unary operations, binary operations and on end sum all particions of expression. 

<img src="https://github.com/0xMartin/Graphing-Calculator/blob/master/doc/expression_parser_diagram.PNG" width=50%>

### Supported operations
* Math
  * Default operations: +, -, *, /, ^
  * Brackets: (, ), |
  * Functions: sin, cos, tan, asin, acos, atan, log, log10
    * All functions hava specific ascii code in expression string
      ```c++
      #define SIN -128
      #define COS -127
      #define TG -126
      #define ASIN -125
      #define ACOS -124
      #define ATG -123
      #define LOG -122
      #define LOG10 -121
      #define PI -120
      ```
* Logic
  * Not: !
  * Or: |
  * And: &
  * Equivalency: =

### Main part of parser

Arguments are expression and bool logic + aritmetic. If bool logic is true then parser will parsing logic expression or if bool aritmetis is true then parser will parsing aritmetic expression. Both bool can be true in same time.

```c++
double ExpressionParser::getValueOfExpression(String expression, bool logic, bool aritmetic) {
	
	if (expression.length() == 0) {
		return 0.0;
	}

	expression = ExpressionParser::replaceVar(&expression, 'e', "2.71828");
	expression = ExpressionParser::replaceVar(&expression, PI, "3.14159");

	#ifdef DEBUG
		Serial.println("IN: " + expression);
	#endif

	//brackets
	ExpressionParser::bracket(&expression, logic, aritmetic);

	#ifdef DEBUG
		Serial.println("BRACKET: " + expression);
	#endif

	//unary operators: sin, cos, tg, asin, acos, atg, log, log10, not
	ExpressionParser::unaryOperator(&expression);

	#ifdef DEBUG
		Serial.println("UNARY: " + expression);
	#endif

	if (aritmetic) {
		//default math operation
		ExpressionParser::binaryOperations(&expression, '^', false);	//power 
		ExpressionParser::binaryOperations(&expression, '/', true);	//divide
		ExpressionParser::binaryOperations(&expression, '*', true);	//multiply
	}
	if (logic) {
		//logic operatores	
		ExpressionParser::binaryOperations(&expression, '&', true);	//logic and
		ExpressionParser::binaryOperations(&expression, '|', true);	//logic or
		ExpressionParser::binaryOperations(&expression, '=', true);	//equivalency
	}

	#ifdef DEBUG
		Serial.println("SUM: " + expression);
	#endif

	//sum all values and return
	return ExpressionParser::sum(&expression);
}
```

## Mode: Calculator

## Mode: Grapher

## Mode: Ploter

## Mode: Logic parser

## Circuit schematic

## PCB design
 
## Case model

# Author
* Martin Krčma

# License
* This project is licensed under Apache License 2.0 - see the [LICENSE.md](https://github.com/0xMartin/Graphing-Calculator/blob/master/LICENSE) file for details
