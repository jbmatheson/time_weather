#include <Elegoo_GFX.h> // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

#include <stdint.h>
#include "TouchScreen.h"

#include <DHT.h>              //DTH Library
#include <DHT_U.h>            //DTH Library

#include <Wire.h>
#include "RTClib.h"

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

#define DHTPIN 52                    // what pin we're connected to DHT Sensor
#define DHTTYPE DHT11                // DHT 11

RTC_DS3231 rtc;

DHT dht(DHTPIN, DHTTYPE);  

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

Elegoo_GFX_Button button1;
Elegoo_GFX_Button button2;
Elegoo_GFX_Button button3;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

char daysOfTheWeek[7][12] = {"Sunday","Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
uint8_t currScreen = 0; // 0 (Initial Screen) | 1 (Options Screen)

void setup(void) {
  Wire.begin();
  Serial.begin(9600);
  rtc.begin();

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // UNCOMMENT TO SET TIME & DATE TO THAT AT COMPILATION.

  dht.begin();

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = 0x9341;
  tft.begin(identifier);
  
  tft.setRotation(3);
  tft.fillScreen(BLACK);

  showInitialScreen();
}

void loop(void) {

  if(currScreen == 0) {  
    float t = getTemperature(true);
    float h = getHumidity();
    float hI = getHeatIndex();
  
    tft.setCursor(15,45);
    tft.setTextColor(YELLOW, BLACK);
    tft.setTextSize(2);
    tft.print(t);
    
    tft.setCursor(90,45);
    tft.print(h);
  
    tft.setCursor(155,45);
    tft.print(hI); 

    tft.setCursor(15,90);
    tft.setTextSize(3);
    
    printTime();
  }

    
  // a point object holds x y and z coordinates
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);  //.kbv ALWAYS restore pinMode()
  pinMode(YP, OUTPUT);

  if (p.z > ts.pressureThreshhold) {
     Serial.print("\tPressure = "); Serial.println(p.z);

     if(currScreen == 0) {
       if(p.x >= 143 && p.x <= 364 && p.y >= 110 && p.y <= 189) {
         currScreen = 1;
         showOptionsScreen();
       } else {
         Serial.print("X = "); Serial.print(p.x);
         Serial.print("\tY = "); Serial.print(p.y);
       }      
     } else if (currScreen == 1){
        currScreen = 0;
        showInitialScreen();
     }
  }
}

void showInitialScreen() {
  button3.initButton(&tft, 270, 220, 90, 30, BLUE, WHITE, BLACK, "Options", 1);
  button3.drawButton();
}

void showOptionsScreen() {
  tft.fillScreen(BLACK);
  tft.setCursor(25, 100);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("Options");

  button3.initButton(&tft, 270, 220, 90, 30, BLUE, WHITE, BLACK, "Save", 1);
  button3.drawButton();
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
    DateTime now = rtc.now();

    tft.setCursor(15, 145);
    tft.setTextColor(WHITE,BLACK);
    tft.println(daysOfTheWeek[now.dayOfTheWeek()]);
    tft.print(now.month(), DEC);
    tft.print('/');
    tft.print(now.day(), DEC);
    tft.print('/');
    tft.print(now.year(), DEC);
    tft.println();
    tft.print(now.hour(), DEC);
    tft.print(':');
    uint8_t m = now.minute();
    tft.print(m);
    tft.print(':');
    uint8_t s = now.second();
    tft.print(s);
    tft.println();
    delay(100);
}
