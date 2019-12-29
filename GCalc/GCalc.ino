/**
   GCalc 3.0
   autor: Martin Krcma
   krcmamartin@outlook.com
   Last update: 19. 12. 2019
*/

#define _BTN_DEBUG_

//#ifdef _BTN_DEBUG_

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
Adafruit_PCD8544 display = Adafruit_PCD8544(2, 3, 4, 5, 6);

int rows[] = {7, 8, 9, 10};
int cols[] = {14, 15, 16, 17};  //A0, A1, A2, A3

#else

//button array 4x4 controler
#include "BtnArray.h"
//graphics calculator system
#include "GCSys.h"

#endif

void setup() {
#ifdef _BTN_DEBUG_

  for (uint8_t i = 0; i < 4; i++) {
    pinMode(rows[i], INPUT);
    pinMode(cols[i], OUTPUT);
  }

  display.begin();

#else

  //init button array
  BTNARRAY_init();

  //init graphics calculator system
  GCSYS_init();

#endif

  //Serial.begin(9600);

}

#ifdef _BTN_DEBUG_
void btnTest() {
  display.clearDisplay();
  display.setTextColor(BLACK);

  uint8_t index = 0;
  for (uint8_t i = 0; i < 4; i++) {
    digitalWrite(cols[i], HIGH);
    delay(5);
    for (uint8_t j = 0; j < 4; j++) {
      display.setCursor(display.width() / 4 * i, display.height() / 4 * j);
      if (digitalRead(rows[j]) == HIGH) {
        display.println("1");
      } else {
        display.println("0");
      }
    }
    digitalWrite(cols[i] , LOW);
    delay(5);
  }

  display.display();
}
#endif

void loop() {

#ifdef _BTN_DEBUG_

  btnTest();

#else

  //events
  uint8_t keys[] = {NULL_BTN_KEY, NULL_BTN_KEY};

  //read pressed keys
  BTNARRAY_getPressedKeys(keys);

  //do event (released keys, long key press)
  GCSYS_event(
    BTNARRAY_keyReleased(),
    BTNARRAY_longPress()
  );

  //render
  GCSYS_render();

#endif

  //delay
  delay(250);

}
