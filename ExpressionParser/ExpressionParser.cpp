#include "ExpressionParser.h"

//public
//##############################################################################################

ExpressionParser::ExpressionParser(uint8_t decimal, bool radians) {
	ExpressionParser::DECIMAL = decimal;
	ExpressionParser::RADIAN = radians;
}

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

//private
//##############################################################################################

void ExpressionParser::bracket(String* expression, bool logic, bool aritmetic) {
	//first find start index of bracket (go from left) and after that find end index (right bracket)

	//is absolute value of bracket 'A'
	bool A = false;
	char c;

	//find start
	int8_t start = -1;	//start index
	for (int8_t i = 0; i < expression->length(); i++) {
		c = (*expression)[i];
		if (c == '|' && !logic) {
			//is absolut value
			A = true;
			start = i + 1;	//+1 -> without bracket symbol 
			break;
		}
		else if (c == '(') {
			start = i + 1;
			break;
		}
	}

	//if start is alwais -1 than start index of braket not found -> return (bracket dosn't exist)
	if (start == -1) {
		return;
	}

	//find end
	int8_t end = 0;	//end index
	for (int8_t i = expression->length() - 1; i >= 0; i--) {
		c = (*expression)[i];
		if ((c == ')' && !A) || (c == '|' && A && !logic)) {
			//set end index
			end = i;
			//if after bracker is power then must retype normal power to power p (with sign)
			if(i + 1 < expression->length())
				if ((*expression)[i + 1] == '^')
					(*expression)[i + 1] = 'p';
			break;
		}
	}

	//get value of bracket
	double val = ExpressionParser::getValueOfExpression(substr(expression, start, end - start), logic, aritmetic);

	//abs ?
	val = A ? abs(val) : val;

	//insert value tot the expression
	*expression = substr(expression, 0, start-1) + 
			String(val, ExpressionParser::DECIMAL) + 
			substr(expression, end+1, expression->length() -1);
}

void ExpressionParser::unaryOperator(String* expression) {
	char c;
	uint8_t position;
	for (uint8_t i = 0; i < expression->length(); i++) {
		c = (*expression)[i];
		//if c (char) is symbol for some function (functions: 128<->135 ASCII)
		#ifdef DEBUG
			Serial.println(String(c) +" -> "+String(int(c)));
		#endif
		if (c >= -128 && c <= -121 || c == '!') {
			//calculate function
			position = i + 1;
			double val = ExpressionParser::nextNumber(expression, &position);
			val = ExpressionParser::RADIAN ? val : (val * 180.0 / 3.1415);
			//compute function
			switch (c) {
			case SIN:	//sin
				#ifdef DEBUG
					Serial.println("SIN");
				#endif
				val = sin(val);
				break;
			case COS:	//cos
				#ifdef DEBUG
					Serial.println("COS");
				#endif
				val = cos(val);
				break;
			case TG:	//tg
				#ifdef DEBUG
					Serial.println("TG");
				#endif
				val = tan(val);
				break;
			case ASIN:	//asin
				#ifdef DEBUG
					Serial.println("ASIN");
				#endif
				val = asin(val);
				break;
			case ACOS:	//acos
				#ifdef DEBUG
					Serial.println("ACOS");
				#endif
				val = acos(val);
				break;
			case ATG:	//atg
				#ifdef DEBUG
					Serial.println("ATG");
				#endif
				val = atan(val);
				break;
			case LOG:	//log
				#ifdef DEBUG
					Serial.println("LOG");
				#endif
				val = log(val);
				break;	//log10
			case LOG10:
				#ifdef DEBUG
					Serial.println("LOG10");
				#endif
				val = log10(val);
				break;
			case '!':	//not
				#ifdef DEBUG
					Serial.println("NOT");
				#endif
				val = val != 1;
				break;
			}
			//insert result of function to the expression
			*expression = substr(expression, 0, i) + 
				String(val, ExpressionParser::DECIMAL) + 
				substr(expression, position, expression->length() - position);
		}
	}
}

/*
	Compute all binary operations with operation
	expression (String)
	operation (char)
	forward = true -> computing nodes from begining to end in expression
*/
void ExpressionParser::binaryOperations(String* expression, char operation, bool forward) {
	uint8_t position = 0;
	//start index of number 1, start index of number 2
	uint8_t* info = new uint8_t[2]{ 0, 0 };
	uint8_t start_n1, start_n2;
	//campute all nodes
	while (position < expression->length()) {
		//get info of next binary node	
		if (forward) {
			delete[] info;
			//forward calculating from begin to end
			info = ExpressionParser::nextNodes(expression, operation, &position);
		}
		else while (true) {
			//from end to begin (read all node conf from begining and return last node)
			uint8_t* f = ExpressionParser::nextNodes(expression, operation, &position);
			if (*f == *(f + 1)) {
				delete[] f;
				//break loop -> info is last node info from expression
				break;
			}
			delete[] info;
			info = f;
		}
		//start index of number 1 and 2 cant be same
		if (*info != *(info + 1)) {
			start_n1 = *info;
			start_n2 = *(info + 1);
			double n1 = ExpressionParser::nextNumber(expression, &start_n1);
			double n2 = ExpressionParser::nextNumber(expression, &start_n2);
			
			#ifdef DEBUG
				Serial.println((String)n1 + operation + (String)n2);
			#endif

			//operation list
			switch((*expression)[*(info + 1) - 1]){
				//aritmetic
			case '*':
				n1 = n1 * n2;
				break;
			case '/':
				n1 = n1 / n2;
				break;
			case '^':	//normal power (without sign)
				n1 = pow(abs(n1), n2) * (n1 >= 0 ? 1 : -1);
				break;
			case POWER_WITH_SIGN:	//power with sign
				n1 = pow(n1, n2);
				break;
				//logic
			case '&':
				n1 = n1 == 1 && n2 == 1;
				break;
			case '|':
				n1 = n1 == 1 || n2 == 1;
				break;
			case '=':
				n1 = n1 == n2;
				break;
			}

			//final value of result, if is positive then must add + (5+5*3 -> 5+15 -- without '+'-> 515) 
			String result = (n1 >= 0 ? "+" : "") + String(n1, ExpressionParser::DECIMAL);

			#ifdef DEBUG
				Serial.println("BIN= " + result);
				Serial.println("BIN1: " + *expression);
			#endif
			
			//place result [from info[0] - start of number 1 to start_n2 (after next number reading => start_n2 will be pointing on end of number 2) ]
			*expression = substr(expression, 0, *info) + result + substr(expression, start_n2, expression->length() - start_n2);
			//set position on 0 must begin on start
			
			#ifdef DEBUG
				Serial.println("BIN2: " + *expression);
			#endif

			//set position on 0 must begin on start
			position = 0;
			*info = 0;
			*(info + 1) = 0;
		}
	}
	delete[] info;
}

//sum all numbers is expression (String)
double ExpressionParser::sum(String* expression) {
	//total value
	double total = 0.0;
	//if position pointer in expression is lower then length -> add next number
	uint8_t position = 0;
	while (position < expression->length()) {
		//get next number from expression and add to the total value
		total += ExpressionParser::nextNumber(expression, &position);
	}

	#ifdef DEBUG
		Serial.println("Final: " + String(total));
	#endif

	//return total value
	return total;
}

/*
	return uint8_t[]{start_index_number1, start_index_number2} 
	expamples: 
	clutch = '*', expression: "5-3*21-4" return: {1, 4}
	clutch = '*', expression: "5-3-4" return: {0, expression->length()} -> not found
*/
uint8_t* ExpressionParser::nextNodes(String* expression, char clutch, uint8_t* start) {

	uint8_t* info = new uint8_t[2]{0, 0};
	char c;
	uint8_t s_index = (*start);
	//find node
	while ((*start) < expression->length()) {
		//next char
		c = (*expression)[*start];
		//start index
		if (c != clutch && !(c == POWER_WITH_SIGN && clutch == '^')) {
			s_index = !ExpressionParser::isDigit(c) && ExpressionParser::isDigit((*expression)[*start + 1]) ? (*start) : s_index;
		} else {	//only p and ^ have same priority and diferent computing
			//build info
			c = (*expression)[s_index];
			//start index number 1
			*info = s_index + (c != '+' && c != '-' && !ExpressionParser::isDigit(c) ? 1 : 0);
			//increment start
			(*start)++;
			//start index number 2
			*(info + 1) = *start;
			//retrun info
			return info;
		}
		//next index
		(*start)++;
	}
	//return info
	return info;
}

//get next number, start reading on position (start)
double ExpressionParser::nextNumber(String* expression, uint8_t* start) {
	//number
	double num = 0;
	//index for digits after floating point
	int8_t floatingPT = -1;
	//sign of num
	int8_t sign = 1;
	//sing side = true -> singns can change value of node
	bool singSide = true;
	//reading number from expression
	char c;
	while ((*start) < expression->length()) {
		c = (*expression)[*start];
		//undefined char
		if (!(ExpressionParser::isDigit(c) || c == '+' || c == '-')) {
			if (!singSide) {
				//return number
				break;
			}
			(*start)++;
			continue;
		}
		//default number reading
		switch (c)
		{
		case '-':
			//change sing, if is in sing side (left side of number), if not the only return number in case +
			if (singSide)
				sign *= -1;
		case '+':
			//return nubere sign side must be false
			if (!singSide) {
				//return number
				return num * sign;
			}
			break;
		case '.':
			//change mode to floating
			floatingPT = (*start);
			break;
		default:
			singSide = false;
			if (floatingPT == -1) {
				//normal mode
				num *= 10;
				num += charToDigit(c);
			}
			else
			{
				//floating mode
				num += charToDigit(c) / pow(10, (*start) - floatingPT);
			}
		}
		//next index
		(*start)++;
	}
	//return number * sign
	return num * sign;
}

String ExpressionParser::substr(String* str, uint8_t offset, uint8_t length) {
	String sub = "";
	for (uint8_t i = offset; i < str->length() && i - offset < length; i++) {
		sub += (*str)[i];
	}
	return sub;
}

//Convert char to digit, char must be digit
uint8_t ExpressionParser::charToDigit(char c) {
	return uint8_t(c - 48);	//48 is ASCII value of '0' char
}

//test if char is digit (all numbers and .)
bool ExpressionParser::isDigit(char c) {
	return (c >= 48 && c < 58) || c == '.';
}

//variable by number, if number is powered by some number then retype normal power to power (with sign) 'p'
String ExpressionParser::replaceVar(String* function, char var, String num) {
	String n = "";
	for (int i = 0; i < function->length(); i++) {
		if ((*function)[i] == var) {
			n += num;
			//retype power
			if (i + 1 < function->length()) {
				if ((*function)[i + 1] == '^') {
					n += 'p';
					i++;
				}
			}
		}
		else {
			n += (*function)[i];
		}
	}
	return n;
}