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
 *  This program displays a clock in 24h format in 5x3 numbers.
 *  The upper secion of the 8x16 display contains code for day of
 *  the week and day of the month in a 3x8 display each.
 *  Day of the week is coded in binary, 3 buts, where 0 is Monday.
 *  The day of the month is broken into BCD too but with a separate
 *  counter for the 10ths (2bits, 0~3) and the units (4 bits, 0~9).
 *  The display will turn off at night.
 *  The button will toggle display on/off regardless of the time.
 *
 *  If the board does not respond while programming, make sure
 *  the arduino is started with admin privileges, that arduino
 *  pro mini 328 16Mhz, and load the program. Then reset the board.
 *  if it fails, try again.
 *
*/
/*---------------------------------------------------------------*/

#include <Wire.h>
#include "arrays.h"
#include <stdlib.h>
#include "RTClib.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"


/***********************************************************/
/*                   SYSTEM VARIABLES                      */
/*---------------------------------------------------------*/

Adafruit_8x16matrix matrix = Adafruit_8x16matrix();

RTC_DS3231 rtc;

const int bttnPin = 2;
const int ledPin  = 13;
int latch         = 1;
int nowtime       = 0;
int lastMin       = 0;
int dst           = 1;

/***********************************************************/
/*                  FUNCTION PROTOTYPES                    */
/*---------------------------------------------------------*/
void setup() {

  matrix.begin(0x70);
  matrix.setTextWrap(false);
  matrix.setTextColor(LED_ON);
  matrix.setRotation(1);
  matrix.clear();
  matrix.setBrightness(1);

  rtc.begin();
  //following line sets the RTC to the date & time this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(ledPin, OUTPUT);
  pinMode(bttnPin, INPUT_PULLUP);

  delay(1000);
}
/*------------------------------------------------*/

void loop() {
  DateTime now = rtc.now();

  /* for daylight savings time */
  if (now.dayOfTheWeek() == 0 && now.month() == 3 && now.day() >= 8 && now.day() <= 16 && now.hour() == 2 && now.minute() == 0 && now.second() == 0 && dst == 0){
    dst = 1;
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), (now.hour()+1), now.minute(), now.second()));
    }
  if(now.dayOfTheWeek() == 0 && now.month() == 11 && now.day() >= 1 && now.day() <= 8 && now.hour() == 2 && now.minute() == 0 && now.second() == 0 && dst == 1){
    dst = 0;
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), (now.hour()-1), now.minute(), now.second()));
    } 


  if(now.hour()<6 || now.hour()>=23){
    nowtime = 0;  //night
  }
  else{
    nowtime = 1;  //day
  }


  if((nowtime==1 && latch==1)||(nowtime==0 && latch==0)){
    disp(now.dayOfTheWeek(), now.day(), now.hour(), now.minute());
    delay(400);
  }
  else{
    matrix.clear();
    matrix.writeDisplay();
    delay(1000);
  }


  if(digitalRead(bttnPin) == 0){
    latch = latch==1? 0:1;
  }

}
/*------------------------------------------------*/

void disp(int tweek, int tday, int thour, int tminute){

  int k;
  int low;
  int hi;
  uint8_t daysAr[2];
  uint8_t timeAr[5];

  if(abs(lastMin-tminute)!=0){
    matrix.clear();

    //display day of week
    daysAr[0] = week[tweek];
    daysAr[1] = daysAr[0];
    matrix.drawBitmap(0, 0, daysAr, 8, 2, LED_ON);


    //number of day
    low = tday%10;
    hi = (tday-low)/10;
    daysAr[0] = daysHi[hi] | daysLow[low];
    daysAr[1] = daysAr[0];
    matrix.drawBitmap(8, 0, daysAr, 8, 2, LED_ON);


    //hours
    low = thour%10;
    hi = (thour-low)/10;

    for(k=0; k<5; k++){
      if(hi!=0){
        timeAr[k] = numbHi[hi][k] | numbLow[low][k];
      }else{
        timeAr[k] = zeros[k] | numbLow[low][k];
      }
    }
    matrix.drawBitmap(0, 3, timeAr, 8, 5, LED_ON);


    //minutes
    low = tminute%10;
    hi = (tminute-low)/10;

    for(k=0; k<5; k++){
      timeAr[k] = numbHi[hi][k] | numbLow[low][k];
    }
    matrix.drawBitmap(8, 3, timeAr, 8, 5, LED_ON);


    matrix.writeDisplay();
  }
}
/*------------------------------------------------*/

/*EOF*/
