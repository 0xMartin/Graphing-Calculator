//expression parser
#include "ExpressionParser.h"
ExpressionParser* parser;

void setup() {

  //parser will calculate with radians and 3 digits after floating point
  parser = new ExpressionParser(3, true);
  
  Serial.begin(9600);
  
  //compute this expression
  String ex = "4+3*(2-5*2)-2/4";
  double val = parser->getValueOfExpression(ex, true, false);
}

void loop() {
  // put your main code here, to run repeatedly:
}
