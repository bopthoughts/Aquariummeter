#include <EEPROM.h>
#include "GravityTDS.h"
#include <LiquidCrystal_I2C.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <Wire.h>
#include "RTClib.h"
#include <DS3231.h> 

// Changable Vars
int setHourstart = 6; // Set hours to sunrise (24h)
int setMinstart = 0; // Set minute to sunrise
int setHourend = 14; // Set hours to sunset (24h)
int setMinend = 57; // Set minute to sunset
int setMaxTDS = 400; //Default max TDS

const int ledPin1 = 8;
const int ledPin = 9;
const int button1 = 10;
const int button2 = 11;
 
#define ONE_WIRE_BUS 2
#define TdsSensorPin A1
 
LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire(ONE_WIRE_BUS); 
GravityTDS gravityTds;
DallasTemperature sensors(&oneWire);
DS3231  rtc(A2, A3);
Time t;
 
float tdsValue = 0;
 
void setup(){
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  sensors.begin();
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);
  gravityTds.setAdcRange(1024);  
  gravityTds.begin(); 
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin, OUTPUT);
  rtc.begin();
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
}
 
void loop(){
//Water Level

  int value = analogRead(A0);
  
  lcd.setCursor(0, 0);
  lcd.print("Value:");
  lcd.print(value);
  lcd.print("   ");
  Serial.println(value);

  lcd.setCursor(0, 1);
  lcd.print("Level:");
  
  if (value == 0) {
    lcd.print("Empty ");
  } else if (value > 1 && value < 350) {
    lcd.print("LOW   ");
  } else if (value > 350 && value < 510) {
    lcd.print("Medium");
  } else if (value > 510){
    lcd.print("HIGH  ");
  }

  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    lcd.scrollDisplayLeft();
  }
  
  delay(4000);

// TDS and Temp

  sensors.requestTemperatures();
 
  gravityTds.setTemperature(sensors.getTempCByIndex(0));
  gravityTds.update(); 
  tdsValue = gravityTds.getTdsValue(); 

  Serial.print(tdsValue,0);
  Serial.println("ppm");
  Serial.print("Temperature is: "); 
  Serial.print(sensors.getTempCByIndex(0));

  lcd.setCursor(16, 0);
  lcd.print("TDS  :");
  lcd.print(tdsValue,0);
  lcd.print(" (");
  lcd.print(setMaxTDS);
  lcd.print(")");

  lcd.setCursor(16, 1);
  lcd.print("Temp :");
  lcd.print(sensors.getTempCByIndex(0));
  lcd.print("");
  
  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    lcd.scrollDisplayRight();
  }

  delay(4000);

//MaxTDS Setting

  if (digitalRead(button1) == 0) {
    setMaxTDS = setMaxTDS+10;
  } 
  
  if (digitalRead(button2) == 0) {
    setMaxTDS = setMaxTDS-10;
  }

  if (tdsValue>setMaxTDS) {
    digitalWrite(ledPin1, HIGH);
  } else {
    digitalWrite(ledPin1, LOW);
  }
  

//LED Timer

  t = rtc.getTime(); 

  Serial.print("Uhr: ");
  Serial.println(rtc.getTimeStr());

  if (t.hour == setHourstart && t.min == setMinstart) // Check if it's sunrise!
  {
    digitalWrite(ledPin, HIGH);
  }

  if (t.hour == setHourend && t.min == setMinend) // Check if it's sunset!
  {
    digitalWrite(ledPin, LOW);
  }
}
