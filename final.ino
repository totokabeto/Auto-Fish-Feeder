#include <Servo.h>
#include "RTClib.h"
#include <Keypad.h>
#include <string.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
RTC_DS3231 rtc;

#define SERVO_PIN 10

Servo myservo;

// before open Servo, we rotate Servo for a moment with small angle
// increase this speed to make the shocking slower
int shocking_speed = 10;

// time open for feeding
int feed_duration = 1000;

// set time
String hourSet = "00";
String minuteSet = "00";

//Set up keypad
const byte ROWS = 4;
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

bool once = false;

void setup() {
  Serial.begin(9600);
  // set up servo
  myservo.attach(SERVO_PIN);
  myservo.write(0);
  // set up rtc
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
  // set up LCD
  lcd.init();   
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AUTO FEEDER 1.0");
  lcd.setCursor(0, 1);
  lcd.print("Press A");
}

void loop() {
  DateTime now = rtc.now();
  String hourGet(now.hour());
  String minuteGet(now.minute());
  //Set up time
  set_time();
  if (hourSet == hourGet && minuteGet == minuteSet) {
    int i = 0;
    for (i = 0; i < 3; i++) {
      shock();
    }
    feed();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wait a minute");
    delay(60000);
    disp();
  }
}

// shocking before feeding
void shock() {
  int i = 0;
  int j = 0;
  for (j = 1; j <= 20; j++) {
    myservo.write(j);
    delay(shocking_speed);
  }

  for (j = 20; j >= 0; j--) {
    myservo.write(j);
    delay(shocking_speed);
  }
}

void feed() {
  myservo.write(90);
  delay(feed_duration);
  myservo.write(0);
}

void disp()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("H: ");
  lcd.print(hourSet);
  lcd.print(" ");
  lcd.print("M: ");
  lcd.print(minuteSet);
  lcd.setCursor(0, 1);
  lcd.print("Press A");
  once = false;
}
void set_time(){
  if (once)
    disp();
  char key = keypad.getKey();
  if (key != NO_KEY)
    once  = true;
  if (key == 'A')
  {
    char k;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Feeding time: ");
    lcd.setCursor(0, 1);
    lcd.print("H: ");
    hourSet = "";
    minuteSet = "";
    char d[4];
    for (int i = 0; i < 2; i++)
    {
      k = keypad.waitForKey();
      lcd.print(k);
      if ((k != '0') || (i != 0))
        hourSet = hourSet + k;
    }
    lcd.print("  ");
    lcd.print("M: ");
    for (int i = 0; i < 2; i++)
    {
      k = keypad.waitForKey();
      lcd.print(k);
      if ((k != '0') || (i != 0))
        minuteSet = minuteSet + k;
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Feed duration");
    lcd.setCursor(0, 1);
    lcd.print("Milisecond: ");
    for (int i = 0 ; i < 4; i++)
    {
      d[i] = keypad.waitForKey();
      lcd.print(d[i]);
    }
    feed_duration = atoi(d);
    lcd.clear();
  }
}
