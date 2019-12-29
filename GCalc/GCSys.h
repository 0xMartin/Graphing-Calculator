/**
   GCalc 3.0
   System for calculator with atmega328p, nokia5110 display and 4x4 button array
   All menu, events and rendering
   autor: Martin Krcma
   krcmamartin@outlook.com
   Last update: 19. 12. 2019

   BUTTON ARRAY
   0  - 7
   1  - 4
   2  - 1
   3  - 0
   4  - 8
   5  - 5
   6  - 2
   7  - =
   8  - 9
   9  - 6
   10 - 3
   11 - enter
   12 - new
   13 - back
   14 - up
   15 - down
*/
#ifndef _GCSYS_H_
#define _GCSYS_H_

#include <EEPROM.h>

//display
//####################################################################
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(2, 3, 4, 5, 6);

#define BACK_LIGHT A5

//resul and expression
String result = "", expression = "";

//expression parser
#include "ExpressionParser.h"
ExpressionParser* parser;

//Grapher
#include "Grapher.h"

//logic tester
#define TRUE -120
#define FALSE 120

bool logicTesterValue = false;
int8_t logicTesterValues[6] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};

/**
   Menu variables
   logic tester - 9
   plot config menu - 8
   Display (for graph view) - 7
   grapher config menu - 6
   main menu - 5
   Config menu - 4
   Logic parser- 3
   Plot f(x, y) - 2
   Grapher f(x) - 1
   Calculator - 0
   menu_item_index are same as menu_indexes of menu

   <<< menu sturcture >>>
  MAIN MENU
  ├-Calculator
  ├-Grapher f(x)
  │      └-grapher config
  │             └-graphing display
  ├-Plot f(x, y)
  │      └-plot config
  │             └-graphing display
  ├-Logic
  │      └-logic testing
  └-Config
  Display
*/

#define MAIN_MENU 5
#define GRAPHICS_SCREEN 8

//lengths of all menu lists
#define G_CONFIG_L 5
#define P_CONFIG_L 1
#define CONFIG_L 3
#define MENU_L 5

int8_t menu_index = 5, menu_item_index = 0, menu_start = 0;
int8_t last_menu = 0;
//config lists
int8_t CONFIG[CONFIG_L] = { FALSE, 3, TRUE };
//graphics config
int8_t CONFIG_GRAPHER[G_CONFIG_L] = { -10, 10, 20, 1, TRUE}; //Start, End, Step, Scale, Grid
//plot config
int8_t CONFIG_PLOT[P_CONFIG_L] = { '=' }; //mode

//EVENT STATUS
#define STATUS_EVT_NONE 0
#define STATUS_EVT_CREQ 1
#define STATUS_EVT_GREQ 2
#define STATUS_EVT_PREQ 3

//##################################################################################

/**
   Init system
*/
void GCSYS_init() {
  *(CONFIG + 2) = EEPROM.read(10);
  *(CONFIG + 1) = EEPROM.read(11);
  *CONFIG = EEPROM.read(12);

  //back light
  pinMode(BACK_LIGHT, OUTPUT);

  digitalWrite(BACK_LIGHT, *(CONFIG + 2) == TRUE ? LOW : HIGH);
  //begin
  display.begin();
  //set contrast
  display.setContrast(50);
  //text size
  display.setTextSize(1);
  //init expression parser
  parser = new ExpressionParser(max(*(CONFIG + 1), 1), *CONFIG == TRUE);
}

/**
   Set index in menu, automatic menu list scrolling
*/
void setItemIndex(int8_t change, uint8_t list_lenght) {
  //only main menu list have 5 items, other hava 4 items
  if ((menu_item_index + change) >= 0 && (menu_item_index + change) < list_lenght) {
    menu_item_index += change;
    //menu list scroling
    if (menu_item_index - menu_start > 2) {
      menu_start++;
    } else if (menu_item_index - menu_start < 0) {
      menu_start--;
    }
  }
}

/**
   Find char (operation) in list
*/
uint8_t findIn(char list[], int length, char c) {
  for (uint8_t i = 0; i < length; i++) {
    if (c == list[i]) {
      return i;
    }
  }
  return 255;
}

char getBtnArrayNumber(uint8_t rKey) {
  switch (rKey) {
    case 0:
      return '7';
    case 1:
      return '4';
    case 2:
      return '1';
    case 3:
      return '0';
    case 4:
      return '8';
    case 5:
      return '5';
    case 6:
      return '2';
    case 8:
      return '9';
    case 9:
      return '6';
    case 10:
      return '3';
  }
  return ' ';
}

/**
    Expression editing (For calculator, grapher, ploter)
    rKey - key released
*/
bool symbol = false;
void expressionEvent(uint8_t rKey, uint8_t mode) {
  /*
     Symbols
     136 - PI, next symbols are from expression parser
  */

  //18 (default length of symbols), 18 + mode: (calculator - 0 -> default, grapher +1 -> add only X, plot +2 -> add X and Y)
  uint8_t symbols_length = mode == 3 ? 12 : 18 + mode;

  //list with all symbols
  char* s;
  if (mode == 3) {
    s = new char[12] {'!', '|', '&', '=', '(', ')', 'A', 'B', 'C', 'D', 'E', 'F'};
  } else {
    s = new char[20] {'+', '-', '*', '/', '^', '(', ')', '|', SIN, COS, TG, ASIN, ACOS, ATG, LOG, LOG10, 'e', 136, 'X', 'Y'};
  }

  uint8_t w;
  switch (rKey) {
    case 12:
      //symbol placer
      expression += s[0];
      break;
    case 13:
      //delete
      expression = expression.substring(0, expression.length() - 1);
      break;
    case 15:
      //last
      w = findIn(s, symbols_length, expression.charAt(expression.length() - 1));
      if (w == 255) {
        //operation not found
        return;
      } else {
        //opeartion found -> last operation
        w--;
      }
      w = w == 255 ? symbols_length - 1 : w;
      expression.setCharAt(expression.length() - 1, s[w]);
      break;
    case 14:
      //next
      w = findIn(s, symbols_length, expression.charAt(expression.length() - 1));
      if (w == 255) {
        //operation not found
        return;
      } else {
        //operation found -> next operation
        w++;
      }
      w = w > symbols_length - 1 ? 0 : w;
      expression.setCharAt(expression.length() - 1, s[w]);
      break;
    case 7:
      //= -> null
      break;
    default:
      //number pressed on btn array add to expression
      if (mode == 3) {
        //in logic mode (3) is alowed only 1 and 0
        if (rKey == 2) {
          expression += '1';
        } else if (rKey == 3) {
          expression += '0';
        }
      } else {
        expression += getBtnArrayNumber(rKey);
      }
      break;
  }
  delete[] s;
}

/**
 * Compute logic expression and result save to "logicTesterValue"
 */
void computeLogic() {
  //replace all A,B, .. variables in expression
  String ex = expression;
  for (uint8_t i = 0; i < 6; i++) {
    ex = parser->replaceVar(&ex, 'A' + i, String(logicTesterValues[i] == TRUE ? 1 : 0));
  }
  //compute new Y value
  double val = parser->getValueOfExpression(ex, true, false);

  logicTesterValue = val == 1.0;
}

//sub events

void mainEvent(uint8_t rKey) {
  //main menu
  switch (rKey) {
    case 15:
      //down
      setItemIndex(1, MENU_L);
      break;
    case 14:
      //up
      setItemIndex(-1, MENU_L);
      break;
    case 11:
      //enter
      menu_index = menu_item_index;
      menu_item_index = 0;
      menu_start = 0;
      result = "";
      expression = "";
      break;
  }
}

void configEvent(uint8_t rKey) {
  //config
  switch (rKey) {
    case 15:
      //down
      setItemIndex(1, CONFIG_L);
      break;
    case 14:
      //up
      setItemIndex(-1, CONFIG_L);
      break;
    case 13:
      //back
      menu_index = MAIN_MENU;
      menu_item_index = 0;
      menu_start = 0;
      //set config
      //calculating mode
      parser->RADIAN = *CONFIG == TRUE;
      //number of decimals
      parser->DECIMAL = *(CONFIG + 1);
      //backlight
      if (*(CONFIG + 2) == TRUE) {
        digitalWrite(BACK_LIGHT, LOW);
      } else {
        digitalWrite(BACK_LIGHT, HIGH);
      }
      //eeprom
      EEPROM.write(10, *(CONFIG + 2));
      EEPROM.write(11, *(CONFIG + 1));
      EEPROM.write(12, *CONFIG);
      break;
    case 12:
      //#
      switch (menu_item_index) {
        case 0:
          *CONFIG = *CONFIG == TRUE ? FALSE : TRUE;
          break;
        case 1:
          *(CONFIG + 1) += 1;
          if (*(CONFIG + 1) > 5) {
            *(CONFIG + 1) = 1;
          }
          break;
        case 2:
          *(CONFIG + 2) = *(CONFIG + 2) == TRUE ? FALSE : TRUE;
          break;
      }
      break;
  }
}

void expressionViewEvent(uint8_t rKey, uint8_t lKey) {
  //calculator, grapher, ploter, logic parser
  if (lKey == 13) {
    //back to main menu
    menu_index = MAIN_MENU;
    menu_item_index = 0;
    menu_start = 0;
  } else if (rKey == 7) {
    //calcuate
    switch (menu_index) {
      case 0:
        //calculate expression and double result conver to string with x decimal places
        result = String(parser->getValueOfExpression(expression, false, true), parser->DECIMAL);
        break;
      case 1:
        //grapher cofig
        menu_index = 6;
        break;
      case 2:
        //plot cofig
        menu_index = 7;
        break;
      case 3:
        //logic tester
        menu_index = 9;
        //compute logic expression -> when menu will open result must be correct
        computeLogic();
        break;
    }
  }
  //editing expression event
  expressionEvent(rKey, menu_index);
}

void plotConfigEvent(uint8_t rKey, uint8_t lKey) {
  switch (rKey) {
    case 14: case 15:
      //change mode (up or down)
      switch (*CONFIG_PLOT) {
        case '=':
          *CONFIG_PLOT = '>';
          break;
        case '>':
          *CONFIG_PLOT = '<';
          break;
        case '<':
          *CONFIG_PLOT = '=';
          break;
      }
      break;
    case 13:
      //back
      menu_index = 2; //go to the plot
      menu_item_index = 0;
      menu_start = 0;
      break;
    case 7: case 11:
      //display graph
      last_menu = menu_index; //save this menu index (for go back)
      menu_index = GRAPHICS_SCREEN;
      _GRAPHER_drawFunctionXY(parser, &display, &expression, *CONFIG_PLOT);
      break;
  }
}

void grapherConfigEvent(uint8_t rKey, uint8_t lKey) {
  bool b = CONFIG_GRAPHER[menu_item_index] == TRUE || CONFIG_GRAPHER[menu_item_index] == FALSE;
  //grapher config event
  switch (rKey) {
    case 15:
      //down
      setItemIndex(1, G_CONFIG_L);
      break;
    case 14:
      //up
      setItemIndex(-1, G_CONFIG_L);
      break;
    case 13:
      //delete selected item
      if (!b) {
        CONFIG_GRAPHER[menu_item_index] = 0;
      }
      break;
    case 12:
      //change sign
      CONFIG_GRAPHER[menu_item_index] *= -1;
      break;
    case 7: case 11:
      //display graph
      last_menu = menu_index; //save this menu index (for go back)
      menu_index = GRAPHICS_SCREEN;
      //display graph
      _GRAPHER_drawFunctionX(parser, &display, &expression, CONFIG_GRAPHER);
    //break;
    default:
      if (!b) {
        //add digit to the selected item in list
        int16_t w = CONFIG_GRAPHER[menu_item_index];
        w *= 10;  //decadic shift
        w += (uint8_t)(getBtnArrayNumber(rKey) - 48); //add num
        //value bounds
        if (w > 100) {
          w = 100;
        } else if (w < -100) {
          w = -100;
        }
        //write final number
        CONFIG_GRAPHER[menu_item_index] = w;
      }
      break;
  }
  if (lKey == 13) {
    //back
    menu_index = 1; //go to the grapher
    menu_item_index = 0;
    menu_start = 0;
  }
  return STATUS_EVT_NONE;
}

void logicTesterEvent(uint8_t rKey, uint8_t lKey) {
  switch (rKey) {
    case 15:
      //down
      setItemIndex(1, 6);
      break;
    case 14:
      //up
      setItemIndex(-1, 6);
      break;
    case 13:
      //back
      menu_index = 3;
      break;
    case 12:
      //change status true<->false
      logicTesterValues[menu_item_index] *= -1;

      computeLogic();
      break;
  }
}

/**
   GCSYS main event
   rKey - key released
   lKey - key long press
*/
void GCSYS_event(uint8_t rKey, uint8_t lKey) {
  //255 must be same as null_key from BtnArray !!!!
  if (rKey == 255 && lKey == 255) {
    return;
  }

  switch (menu_index) {
    case MAIN_MENU:
      mainEvent(rKey);
      break;
    case 0: case 1: case 2: case 3:
      expressionViewEvent(rKey, lKey);
      break;
    case 4:
      configEvent(rKey);
      break;
    case 6:
      grapherConfigEvent(rKey, lKey);
      break;
    case 7:
      plotConfigEvent(rKey, lKey);
      break;
    case GRAPHICS_SCREEN:
      //GRAPHICS_SCREEN
      switch (rKey) {
        case 13:
          //back
          menu_index = last_menu;
          break;
        case 11:
          //enter
          break;
      }
      break;
    case 9:
      //logic tester
      logicTesterEvent(rKey, lKey);
      break;
  }
}

/**
   RENDERING
   ####################################################
*/

/**
   Draw menu list, title on up, list - menu items, lenght - lenght of list, it_index - index of selected item
*/
void _menuList(char* title, char* list[], int8_t* values, uint8_t lenght, uint8_t it_index, bool charM) {
  //title
  display.fillRect(0, 0, display.width(), 9, 1);
  display.setTextColor(WHITE);
  display.setCursor(1, 1);
  display.println(title);
  display.setTextColor(BLACK);
  int v;
  String val;
  //menu items
  for (uint8_t i = 0; i < 3; i++) {
    uint8_t j = i + menu_start;
    if (j < lenght) {
      if (j == it_index) {
        display.drawRect(0, 16 + 10 * i, display.width(), 11, 1);
      }
      display.setCursor(2, 18 + 10 * i);
      if (values != NULL) {
        v = *(values + i + menu_start);
        val = v == TRUE ? "TRUE" : (v == FALSE ? "FALSE" : (charM ? (String)((char)v) : String(v))); //true, false of number
        display.println(list[i + menu_start] + val);
      } else {
        display.println(list[i + menu_start]);
      }
    }
  }
}

/**
   Render expression - 0, grapher - 1, ploter - 2, logic parser - 3
*/
void _expressionView(uint8_t menu_type) {
  display.setTextColor(BLACK);
  display.setCursor(1, 1);
  String prefix = menu_type == 1 ? "f(x)=" : (menu_type == 2 ? "f(x,y)=" : (menu_type == 3 ? "Y=" : ""));
  display.println(prefix + expression);
  //result only for calculator mode
  if (menu_type == 0) {
    display.fillRect(0, display.height() - 9, display.width(), 9, 1);
    display.setCursor(1, display.height() - 8);
    display.setTextColor(WHITE);
    display.println('=' + result);
  }
}

/**
   Render actual menu
*/
void GCSYS_render() {
  if (menu_index == GRAPHICS_SCREEN) {
    return;
  }
  display.clearDisplay();
  char *logic[] = {"A: ", "B: ", "C: ", "D: ", "E: ", "F: "};
  char *plot[] = {"Mode: "};
  char *grapher[] = {"Start:", "End:", "Steps:", "Scale: 1/", "Cross:"};
  char *conf[] = {"RADIAN:", "Dec. plac.:", "Light:"};
  char *main[] = {"Calculator", "Grapher f(x)", "Plot f(x, y)", "Logic parser", "Config"};
  switch (menu_index) {
    case 0: case 1: case 2: case 3:
      _expressionView(menu_index);
      break;
    case 4:
      //config menu
      _menuList("Config", conf, CONFIG, CONFIG_L, menu_item_index, false);
      break;
    case MAIN_MENU:
      //main menu
      _menuList("GCalc 3.0", main, NULL, MENU_L, menu_item_index, false);
      break;
    case 6:
      //grapher config menu
      _menuList("Grapher", grapher, CONFIG_GRAPHER, G_CONFIG_L, menu_item_index, false);
      break;
    case 7:
      //plot config menu
      _menuList("Plot", plot, CONFIG_PLOT, P_CONFIG_L, menu_item_index, true);
      break;
    case 9:
      //logic tester
      if (logicTesterValue) {
        _menuList("Y = TRUE", logic, logicTesterValues, 6, menu_item_index, true);
      } else {
        _menuList("Y = FALSE", logic, logicTesterValues, 6, menu_item_index, true);
      }
      break;
  }
  display.display();
}

#endif
