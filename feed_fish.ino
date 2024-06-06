#include <Servo.h>
#include "RTClib.h"
#include "OneButton.h"
#define LIGHT_PIN 5
#define BUTTON_PIN  2


#define LIGHT_START_HOUR 8
#define LIGHT_END_HOUR 14
#define WATER_FRESH_START_HOUR 16
#define WATER_FRESH_END_HOUR 16
#define WATER_FRESH_START_MINUTE 0
//  //测试一分钟换水，两分钟结束
#define WATER_FRESH_END_MINUTE 15  

#define FEED_HOUR_1 9
#define FEED_HOUR_2 17

RTC_DS3231 rtc;
OneButton button(BUTTON_PIN, false);
Servo myservo;  // create servo object to control a servo


const int ledPin = 13;    // the number of the LED pin
const int water_pin = 6;
int buttonState = 0;  // variable for reading the pushbutton status
int servoStatus = 0;
int timeflag;
int savemin;
int feed_flag;
unsigned long lastExecutionTime = 0;
bool servoFeedExecuted1 = false;
bool servoFeedExecuted2 = false;

int water_fresh_long();
int servo_feed(int time);
int light_driver(int status);
int water_fresh(int time);
void feed();
void waterFreshToggle();



int servo_feed(int time) {
  //here is feed func
  myservo.attach(10);
  myservo.write(0);
  delay(300);
  myservo.write(15);
  delay(300);
  myservo.write(0);
  myservo.detach();  

  return 1;
}

int light_driver(int status) {
  //set high or low
  analogWrite(LIGHT_PIN,status);
  return status;
}

int water_fresh_long() {
      analogWrite(water_pin, 255);
      Serial.println("feed-by-long-");
      digitalWrite(13,HIGH);

    delay(60000);
    analogWrite(water_pin, 0);
    digitalWrite(13,LOW);
    Serial.println("feedbylong end");

}

void water_fresh(bool state)
{
    analogWrite(water_pin, state ? 255 : 0);
    //Serial.println("water fresh");
    digitalWrite(ledPin, state ? HIGH : LOW);
}

void feed() {
  servo_feed(50); // Feed when button is clicked
}

void waterFreshToggle() {
  static bool waterFreshState = false;
  waterFreshState = !waterFreshState;
  water_fresh(waterFreshState); // Toggle water fresh state when button is double clicked
}



void setup() {
  // initialize the LED pin as an output:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(8,OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(BUTTON_PIN, INPUT);
  pinMode(water_pin, OUTPUT);
  myservo.attach(10);
  myservo.write(0);
  myservo.detach();
  rtc.begin();
  Serial.begin(115200);
  Serial.println("hello");
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  //测试一分钟亮灯 
  //rtc.adjust(DateTime(2024, 5, 17, 7, 59, 0));
  //测试一分钟灭灯
  // rtc.adjust(DateTime(2024, 5, 17, 14, 59, 0));

    //测试一分钟换水，两分钟结束
  // rtc.adjust(DateTime(2024, 5, 17, 15, 59, 0));
    //测试一分钟喂食
   //rtc.adjust(DateTime(2024, 5, 17, 16, 59, 0));


  button.attachDoubleClick(water_fresh_long);
  button.attachClick(feed);
  DateTime starttime = rtc.now();
  lastExecutionTime = starttime.unixtime();


   Serial.print(starttime.year(), DEC);
  Serial.print('/');
  Serial.print(starttime.month(), DEC);
  Serial.print('/');
  Serial.print(starttime.day(), DEC);
  Serial.print(" (");
  Serial.print(") ");
  Serial.print(starttime.hour(), DEC);
  Serial.print(':');
  Serial.print(starttime.minute());
  Serial.print(':');
  Serial.print(starttime.second(), DEC);
  Serial.println();
}



void loop() {
  button.tick();
  DateTime now = rtc.now();


    // Light control
  if (now.hour() >= LIGHT_START_HOUR && now.hour() < LIGHT_END_HOUR) {
    light_driver(80); // Turn on light from 8:00 to 15:00
  } else {
    light_driver(0); // Turn off light outside the specified time
  }
  
  // Water fresh control
  if (now.hour() == WATER_FRESH_START_HOUR &&
      now.minute() >= WATER_FRESH_START_MINUTE &&
      now.minute() <= WATER_FRESH_END_MINUTE) {
    water_fresh(true);
  } else {
    water_fresh(false);
  }
  
 // 控制喂食，只在整点执行一次
  if (now.hour() == FEED_HOUR_1 && now.minute() == 0 && !servoFeedExecuted1) {
    servo_feed(50);
    servoFeedExecuted1 = true;
  } else if (now.hour() != FEED_HOUR_1) {
    servoFeedExecuted1 = false;
  }

  if (now.hour() == FEED_HOUR_2 && now.minute() == 0 && !servoFeedExecuted2) {
    servo_feed(50);
    servoFeedExecuted2 = true;
  } else if (now.hour() != FEED_HOUR_2) {
    servoFeedExecuted2 = false;
  }

  button.tick(); // 检测按钮状态

  // Serial.print(now.year(), DEC);
  // Serial.print('/');
  // Serial.print(now.month(), DEC);
  // Serial.print('/');
  // Serial.print(now.day(), DEC);
  // Serial.print(" (");
  // Serial.print(") ");
  // Serial.print(now.hour(), DEC);
  // Serial.print(':');
  // Serial.print(now.minute());
  // Serial.print(':');
  // Serial.print(now.second(), DEC);
  // Serial.println();
  //delay(1000);


}



