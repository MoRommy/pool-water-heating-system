/*
   Copyright [2020] [MoRommy]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

//Set start and stop time of activity
#define startActivityHour 9
#define startActivityMinutes 0
#define stopActivityHour 18
#define stopActivityMinutes 30
/* You will modify those   ⬆ settings as you wish
Example:
If you want your activity to start at 09:00 and stop at 18:34, your values will be:
#define startActivityHour 9
#define startActivityMinutes 0
#define stopActivityHour 18
#define stopActivityMinutes 34
*/
const long waterCirculationCycleDuration = 300000; // 5 minutes in miliseconds (5*60*1000)
const long waterHeatingCycleDuration = 1800000; // 30 minutes in miliseconds(30*60*1000)
//Set digital pin for relay
#define relayPin 4
//relay is using normally open mode (I prefer normally closed mode, but that's how I found it.)
bool didWaterCirculateInitially = false;
int dailyCycleCounter;

void setup() { //This is setup activity
  Serial.begin(9600);
  while (!Serial) ; // wait for serial
  delay(200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
}

void loop() { // This is main activity
  tmElements_t currentDateAndTime;
  if(didWaterCirculateInitially) startNewCycle();
  if (RTC.read(currentDateAndTime)) {
    if (isTimeToStartDailyWork(currentDateAndTime) && didWaterCirculateInitially == false) startDailyWork();
      else if (isTimeToStopDailyWork(currentDateAndTime) && didWaterCirculateInitially == true) stopDailyWork();
  } else printClockErrorMessage(); 
}

  void printClockErrorMessage() {
    if (RTC.chipPresent()) {
      customPrint("The DS1307 is stopped.  Please run the SetTime example to initialize the time and begin running.");
     } 
    else {
      customPrint("DS1307 read error!  Please check the circuitry.");
    }
  }

  void startDailyWork() {
    dailyCycleCounter = 0;
    customPrint("Daily work started !");
    didWaterCirculateInitially = true;
    circulateWater();
    Serial.println();
  }

  void stopDailyWork() {
    customPrint("Daily work stopped !");
    didWaterCirculateInitially = false;
  }

  void startNewCycle() {
    dailyCycleCounter++;
    String message = "Cycle ";
    message = message + dailyCycleCounter;
    message = message + " started !";
    customPrint(message);
    heatWater();
    circulateWater();
    customPrint("Cycle finished !");
    Serial.println();
  }

  void heatWater() {
    customPrint("Waiting for water to heat...");
    delay(waterHeatingCycleDuration);
    customPrint("Heat complete !");
  }

  void circulateWater() {
    customPrint("Circulating water...");
    digitalWrite(relayPin, LOW);
    delay(waterCirculationCycleDuration);
    digitalWrite(relayPin, HIGH);
    customPrint("Stop circulating water.");
  }

  bool isTimeToStartDailyWork(tmElements_t currentDateAndTime) {
    if (currentDateAndTime.Hour < startActivityHour || currentDateAndTime.Hour > stopActivityHour) return false;
    if (currentDateAndTime.Hour == startActivityHour && currentDateAndTime.Minute < startActivityMinutes) return false;
    if (currentDateAndTime.Hour == stopActivityHour && currentDateAndTime.Minute >= stopActivityMinutes) return false;
    return true;
  }

  bool isTimeToStopDailyWork(tmElements_t currentDateAndTime) {
    if (currentDateAndTime.Hour == stopActivityHour && currentDateAndTime.Minute >= stopActivityMinutes) return true;
    if (currentDateAndTime.Hour > stopActivityHour) return true;
    return false;
  }


  void customPrint(String p) {
    String dateAndTime;
      tmElements_t currentDateAndTime2;
      if (RTC.read(currentDateAndTime2)) {
      dateAndTime = "[";
      dateAndTime += currentDateAndTime2.Hour;
      dateAndTime += ':';
      dateAndTime += currentDateAndTime2.Minute;
      dateAndTime += ':';
      dateAndTime += currentDateAndTime2.Second;
      dateAndTime += " - ";
      dateAndTime += currentDateAndTime2.Day;
      dateAndTime += '/';
      dateAndTime += currentDateAndTime2.Month;
      dateAndTime += '/';
      dateAndTime += tmYearToCalendar(currentDateAndTime2.Year);
      dateAndTime += "] - ";
      }
      dateAndTime += p;
      Serial.print(dateAndTime);
      Serial.println();
  }
