/**
 * autor: Martin Krcma
 * Last update: 19. 12. 2019
 */
#ifndef _GRAPHER_H_
#define _GRAPHER_H_

//draw function on display
void _GRAPHER_drawFunctionX(
  ExpressionParser* parser,
  Adafruit_PCD8544* display,
  String* function,
  int8_t* conf) {

  display->clearDisplay();

  //draw cross
  if (*(conf + 4) == -120 ) {
    //horisontal line
    display->drawLine(
      display->width() / 2,
      0,
      display->width() / 2,
      display->height(),
      BLACK
    );
    //vertical line
    display->drawLine(
      0,
      display->height() / 2,
      display->width(),
      display->height() / 2,
      BLACK
    );
    //ponts
    for (uint8_t i = 0; i < display->width() / 3; i++) {
      display->drawPixel(i * 3, display->height() / 2 + 1, BLACK);
    }
    for (uint8_t i = 0; i < display->height() / 3; i++) {
      display->drawPixel(display->width() / 2 + 1, i * 3, BLACK);
    }
  }

  //draw function
  float scale = 3 / (*(conf + 3)); //scale of graph
  float st = float(*(conf + 1) - *conf) / float( *(conf + 2)); //step in x axis
  int8_t ly, y;
  double w;
  
  for (double x = *conf; x < * (conf + 1); x += st) {
    //replace variable X
    //evaluate function
    w = parser->getValueOfExpression(
          parser->replaceVar(function, 'X', String(x, parser->DECIMAL)),
          false, true) * scale;
    //Serial.println(replaceVar(function, 'X', String(x, parser->DECIMAL)) + " = " + String(w/scale));
    //bounds
    y = w < -127 ? -1 : (int8_t)w;
    y = w > 127 ? display->height() + 1 : (int8_t)w;
    //draw line between two points
    if (x != *(conf)) {
      //distance between y and ly must be lower or equal then display height
      if (abs(y - ly) <= display->height()) {
        display->drawLine(
          (int)(display->width() / 2 + x * scale),
          (int)(display->height() / 2 - y),
          (int)(display->width() / 2 + (x - st) * scale),
          (int)(display->height() / 2 - ly),
          BLACK
        );
      }
    }
    //redraw
    display->display();
    //store last y
    ly = y;
  }

}

void _GRAPHER_drawFunctionXY(
  ExpressionParser* parser,
  Adafruit_PCD8544* display,
  String* function,
  char mode) {

  display->clearDisplay();

  String w;
  double val;

  int half_w = display->width() / 2;
  int half_h = display->height() / 2;
  for (int x = -half_w; x < half_w; x++) {
    for (int y = -half_h; y < half_h; y++) {
      //replace variable X Y
      w = parser->replaceVar(function, 'X', (String)x);
      w = parser->replaceVar(&w, 'Y', (String)y);
      val = parser->getValueOfExpression(w, false, true);
      //Serial.println((String)x + "," + (String)y + " -> " + w + " = " + (String)val);
      switch (mode) {
        case '=':
          if (abs(val) < 2) {
            display->drawPixel(x + half_w, -y + half_h, BLACK);
          }
          break;
        case '>':
          if (val > 0) {
            display->drawPixel(x + half_w, -y + half_h, BLACK);
          }
          break;
        case '<':
          if (val < 0) {
            display->drawPixel(x + half_w, -y + half_h, BLACK);
          }
          break;
      }
    }
    display->display();
  }

}

#endif
