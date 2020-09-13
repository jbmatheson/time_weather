#include <Elegoo_GFX.h> // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

#include <stdint.h>
#include "TouchScreen.h"

#include <DHT.h>              //DTH Library
#include <DHT_U.h>            //DTH Library

#include <Wire.h>
#include "RTClib.h"
RTC_DS3231 rtc;

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
DHT dht(DHTPIN, DHTTYPE);   

Elegoo_GFX_Button button1;
Elegoo_GFX_Button button2;
Elegoo_GFX_Button button3;

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

char daysOfTheWeek[7][12] = {"Sunday","Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

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
  tft.setCursor(15,30);
  tft.println("TEMPERATURE");
  
  tft.setCursor(90,30);
  tft.println("HUMIDITY");

  tft.setCursor(155,30);
  tft.println("HEAT INDEX");

  button1.initButton(&tft, 60, 200, 90, 30, BLUE, WHITE, BLACK, "Weather", 1);
  button1.drawButton();

  button2.initButton(&tft, 155, 200, 90, 30, BLUE, WHITE, BLACK, "Time", 1);
  button2.drawButton();

  button3.initButton(&tft, 250, 200, 90, 30, BLUE, WHITE, BLACK, "Options", 1);
  button3.drawButton();
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
    DateTime now = rtc.now();

    tft.setCursor(15,145);
    tft.println("Current Date & Time: ");

    tft.setCursor(15, 160);
    tft.print(now.year(), DEC);
    tft.print('/');
    tft.print(now.month(), DEC);
    tft.print('/');
    tft.print(now.day(), DEC);
    tft.print(" (");
    tft.print(daysOfTheWeek[now.dayOfTheWeek()]);
    tft.print(") ");
    tft.print(now.hour(), DEC);
    tft.print(':');
    tft.print(now.minute(), DEC);
    tft.print(':');
    tft.print(now.second(), DEC);
    tft.println();
}
