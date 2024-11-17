#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>

RTC_DS3231 rtc;
const int buzzerPin = 13;        

int pushVal = 0;
int val2;
int addr = 0;

const int push1pin = 9;
const int push2pin = 8;
const int push3pin = 7;
const int stopPin = A0;

long previousMillis = 0;
long interval = 500;

int buzz8amHH = 10;
int buzz8amMM = 0;
int buzz8amSS = 0;

int buzz2pmHH = 14;
int buzz2pmMM = 0;
int buzz2pmSS = 0;

int buzz8pmHH = 20;
int buzz8pmMM = 0;
int buzz8pmSS = 0;


String dayOfWeekStr(int day) {
  switch (day) {
    case 1: return "Sunday";
    case 2: return "Monday";
    case 3: return "Tuesday";
    case 4: return "Wednesday";
    case 5: return "Thursday";
    case 6: return "Friday";
    case 7: return "Saturday";
    default: return "";
  }
}

void setup() {
  Serial.begin(9600);
  rtc.adjust(DateTime(2024, 11, 7, 19, 59, 30)); 

  pinMode(push1pin, INPUT_PULLUP);
  pinMode(push2pin, INPUT_PULLUP);
  pinMode(push3pin, INPUT_PULLUP);
  pinMode(stopPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  val2 = EEPROM.read(addr);
  switch (val2) {
    case 1:
      pushVal = 1;
      break;
    case 2:
      pushVal = 2;
      break;
    case 3:
      pushVal = 3;
      break;
  }
}

void loop() {
  push1();
  push2();
  push3();

  if (pushVal == 1) {
    at8am();
  } else if (pushVal == 2) {
    at8am();
    at8pm();
  } else if (pushVal == 3) {
    at8am();
    at2pm();
    at8pm();
  }

  stopPins();
}

void push1() {
  if (digitalRead(push1pin) == LOW) {
    pushVal = 1;
    EEPROM.write(addr, 1);

    at8am();
    displayReminder("Once/day");
    Serial.println("Remainder set for once/day");
    delay(2000); 
    displayTime(rtc.now());
  }
}

void push2() {
  if (digitalRead(push2pin) == LOW) {
    pushVal = 2;
    EEPROM.write(addr, 2);
    at8am();
    at8pm();
   
    displayReminder("Twice/day");
    Serial.println("Remainder set for Twice/day");
    delay(2000); 
    displayTime(rtc.now());
  }
}

void push3() {
  if (digitalRead(push3pin) == LOW) {
    pushVal = 3;
    EEPROM.write(addr, 3);
    at8am();
    at2pm();
    at8pm();
    displayReminder("Thrice/day");
    Serial.println("Remainder set for Thrice/day");
    delay(2000);  
    displayTime(rtc.now());
  }
}

void stopPins() {
  if (digitalRead(stopPin) == LOW) {
    digitalWrite(buzzerPin, LOW);
    Serial.println("stop button pressed");
    displayMessage("Take Medicine", "with Warm Water");
    delay(1200);
    displayTime(rtc.now());  
    delay(1200);
  }
}

void displayMessage(String line1, String line2) {
}

void displayReminder(String reminder) {
  displayMessage("Reminder set", "for " + reminder);
}

void at8am() {
  DateTime now = rtc.now();
  if (now.hour() == buzz8amHH && now.minute() == buzz8amMM && now.second() == buzz8amSS && Serial.read()=='name') {
    Serial.println("8 AM reached. Turning on the buzzer");
    startBuzz();
  }
}

void at2pm() {
  DateTime now = rtc.now();
  if (now.hour() == buzz2pmHH && now.minute() == buzz2pmMM && now.second() == buzz2pmSS && Serial.read()=='name') {
    startBuzz();
  }
}

void at8pm() {
  DateTime now = rtc.now();
  if (now.hour() == buzz8pmHH && now.minute() == buzz8pmMM && now.second() == buzz8pmSS && Serial.read()=='narayana') {
    startBuzz();
  }
}

void startBuzz() {

  digitalWrite(buzzerPin, HIGH);
}

void updateAlarm(int &hours, int &minutes, int &seconds) {

  hours = rtc.now().hour();
  minutes = rtc.now().minute();
  seconds = rtc.now().second() + 5; 
}

void displayTime(DateTime now) {
  Serial.print("Current Time: ");
  Serial.print(now.hour());
  Serial.print(':');
  Serial.print(now.minute());
  Serial.print(':');
  Serial.println(now.second());
}