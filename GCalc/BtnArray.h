/**
   GCalc 3.0 - controler for button array 4x4
   autor: Martin Krcma
   krcmamartin@outlook.com
   8. 10. 2019
*/
#ifndef _BTNARRAY_H_
#define _BTNARRAY_H_

//null key
#define NULL_BTN_KEY 255

/**
   Digital pins and analog pins
*/
int rows[] = {7, 8, 9, 10};
int cols[] = {14, 15, 16, 17};  //A0, A1, A2, A3

//values for long key press detection
uint8_t val1 = NULL_BTN_KEY, val2 = NULL_BTN_KEY;
long c1 = 0, c2 = 0;

/**
   Init btn array
*/
void BTNARRAY_init() {
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(rows[i], INPUT);
    pinMode(cols[i], OUTPUT);
  }
}

/**
   Return two pressed keys (its useless storing more then
   two keys because this software use max two pressed keys)
*/
void BTNARRAY_getPressedKeys(uint8_t keys[]) {
  uint8_t index = 0;
  for (uint8_t i = 0; i < 4; i++) {
    digitalWrite(cols[i], HIGH);
    delay(5);
    for (uint8_t j = 0; j < 4; j++) {
      if (digitalRead(rows[j]) == HIGH) {
        //write to pressed keys
        keys[index] = i * 4 + j;
        index++;
      }
    }
    digitalWrite(cols[i] , LOW);
    delay(5);
  }
  //for long presse
  if (val1 == NULL_BTN_KEY) {
    c1 = millis();
  }
  val1 = keys[0];
  if (val2 == NULL_BTN_KEY) {
    c2 = millis();
  }
  val2 = keys[1];
}

/**
   Return value of key when you pressed it for longer time (1s = 1000ms)
*/
uint8_t BTNARRAY_longPress() {
  if (millis() - c1 > 1000) {
    c1 = millis();
    return val1;
  } else if (millis() - c2 > 1000) {
    c2 = millis();
    return val2;
  } else {
    return NULL_BTN_KEY;
  }
}

/**
 * Return released key
 */
uint8_t last_v1 = NULL_BTN_KEY, last_v2 = NULL_BTN_KEY;
uint8_t BTNARRAY_keyReleased() {
  uint8_t rel = NULL_BTN_KEY;
  //val1 released
  if(last_v1 != val1 && val1 == NULL_BTN_KEY){
    rel = last_v1;
  }
  //val2 released
  if(last_v2 != val2 && val2 == NULL_BTN_KEY){
    rel = last_v2;
  }
  last_v1 = val1;
  last_v2 = val2;  
  return rel;
}

#endif
