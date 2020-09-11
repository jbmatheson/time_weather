#include <Adafruit_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

#include <stdint.h>
#include "TouchScreen.h"

#include <DHT.h>              //DTH Library
#include <DHT_U.h>            //DTH Library

#include <Wire.h>
#include <Time.h>
#include <RTClib.h>
#include <DS1307RTC.h>

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define DHTPIN 52                    // what pin we're connected to DHT Sensor
#define DHTTYPE DHT11                // DHT 11
DHT dht(DHTPIN, DHTTYPE);            // DHT Object Declaration
DS1307RTC clock;

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

char daysOfTheWeek[7][12] = {"Sunday","Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup(void) {
  Serial.begin(9600);

  dht.begin();

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = 0x9341;
  tft.begin(identifier);
  tft.setRotation(3);
  tft.fillScreen(BLACK);
  tft.setCursor(15,30);
  tft.println("TEMPERATURE");
  
  tft.setCursor(90,30);
  tft.println("HUMIDITY");

  tft.setCursor(155,30);
  tft.println("HEAT INDEX");
}

void loop(void) {

  float t = getTemperature(true);
  float h = getHumidity();
  float hI = getHeatIndex();


  tft.setCursor(15,45);
  tft.setTextColor(WHITE, BLACK);
  tft.print(t);
  
  tft.setCursor(90,45);
  tft.setTextColor(WHITE, BLACK);
  tft.print(h);

  tft.setCursor(155,45);
  tft.setTextColor(WHITE, BLACK);
  tft.print(hI);
    
  // a point object holds x y and z coordinates
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);  //.kbv ALWAYS restore pinMode()
  pinMode(YP, OUTPUT);

  if (p.z > ts.pressureThreshhold) {
     Serial.print("X = "); Serial.print(p.x);
     Serial.print("\tY = "); Serial.print(p.y);
     Serial.print("\tPressure = "); Serial.println(p.z);
  }
  
  tft.setCursor(15,90);
  printTime();
}

float getTemperature(bool tF) {
    return dht.readTemperature(tF);
}

float getHumidity() {
    return dht.readHumidity();
}

float getHeatIndex() {
  float t = getTemperature(true);
  float h = getHumidity();
  return dht.computeHeatIndex();
}

void printTime() {
  char buf1[20];
  DateTime now = clock.get();
  sprintf(buf1, "%02d:%02d:%02d %02d/%02d/%02d",  now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());
  tft.println(buf1);
}
