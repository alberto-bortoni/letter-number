/*****************************************************************/

/*   created by: alberto bortoni
 *   date:       2017-01-01
 *
 * hardware:
 *   - Arduino pro-mini 5V 16Mhz atmega 328Mhz
 *   - DS3231 Precision RTC on Adafruit breakout
 *   - HT16K33 driver chip with 16x8 1.2" LED Matrix
 *   - Adafruit 5V 2.4A Switching Power Supply
 *
 *  This program displays a letter from the alphabet in one matrix
 *  and the corresponding number in the ohter (eg. A 1).
 *  This is made so that I can memorize the numbers and "add letters"
 *  as I walk arround on my day-to-day.
 *  The letters greater than 10, are fromed by adding the numbers in
 *  their 2 digit form, eg. K2,L3,S1.
 *
 *  RTC is used to dim the brightness of the display at night and change
 *  letter displayed every 3 seconds at random.
 *
*/
/*---------------------------------------------------------------*/

#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"


/***********************************************************/
/*                   SYSTEM VARIABLES                      */
/*---------------------------------------------------------*/

Adafruit_8x16matrix matrix = Adafruit_8x16matrix();

RTC_DS3231 rtc;

const int bttnPin  =  2;
const int ledPin   = 13;
const char abc[] =  "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char numb[] = "12345678912345678912345678";
int randomNumber = 0;
int latch = 0;
int nowtime = 0;
int nowhour = 0;

/***********************************************************/
/*                  FUNCTION PROTOTYPES                    */
/*---------------------------------------------------------*/
void setup() {

  matrix.begin(0x70);
  matrix.setTextSize(1);
  matrix.setTextWrap(false);
  matrix.setTextColor(LED_ON);
  matrix.setRotation(1);
  matrix.clear();
  matrix.setBrightness(2);
      
  rtc.begin();

  pinMode(ledPin, OUTPUT);
  pinMode(bttnPin, INPUT_PULLUP);

  randomSeed(analogRead(0));

  randomNumber = random(26);
  disp(abc[randomNumber], numb[randomNumber]);
  delay(1000);
}
/*------------------------------------------------*/



void loop() {

  DateTime now = rtc.now();
  delay(3000);

  nowhour = now.hour();
  if(nowhour<6 || nowhour>21){
    nowtime = 0;}   //night
  else{
    nowtime = 1;}   //day

  switch (nowtime*(nowhour%3 + 1)){
    case 0:     // night
      disp(' ', ' ');
      latch = 1;
      break;
    case 1:     //change
      if(latch){
        randomNumber = random(26);
        disp(abc[randomNumber], numb[randomNumber]);
        latch = 0;
      }
      break;
    case 2:     //nothing
      latch = 1;
      break;
    case 3:     //nothing
      break;
    }
  
  if(digitalRead(bttnPin) == 0){
    randomNumber = random(26);
    disp(abc[randomNumber], numb[randomNumber]);
    delay(3000);
    }
}
/*------------------------------------------------*/



void disp( char letter, char number){
  matrix.clear();
  matrix.setCursor(1,0);
  matrix.print(letter);
  matrix.setCursor(10,0);
  matrix.print(number);
  matrix.writeDisplay();
}
/*------------------------------------------------*/

/*EOF*/
