// Multi-Purpose Wizard Helm: It lights up in the dark, cools you down in the heat, protects your head, and displays the temperature
// Read the attached PDF file for additional code explanation

#include <Adafruit_NeoPixel.h>
#include "DHT.h"
#include <LiquidCrystal.h> // v1.07

// LED PINS
#define LED_PIN 2
#define LED_COUNT 12
// DHT PINS
#define DHT_PIN 3
#define DHT_TYPE DHT22
// FAN PINS
#define FAN_PIN 4
// LCD SCREEN PINS
#define LCD_RS 12 // register pin
#define LCD_EN 11 // enable pin, enables writing to registers
#define LCD_D4 7  // data pins
#define LCD_D5 8
#define LCD_D6 9
#define LCD_D7 10

#define DEBUG_MODE true
#define LIGHT_THRESHOLD 200

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // LED
DHT dht(DHT_PIN, DHT_TYPE);                                        // humidity+temperature sensor
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7); // screen display

double lightVal;
float humidity;
float temperature;
int gradient_mode = 0;
bool fanOn = false; // checks if fan is currently powered
double aT = 0.0;

double tempInterval = 3000; // time between each check of temperature/humidity sensor
unsigned long currTempTime = 0.0;
unsigned long prevTempTime = -tempInterval;
bool doUpdate = false; // used to update fan checks

double printInterval = 1000;
unsigned long currPrintTime = 0.0;
unsigned long prevPrintTime = -printInterval;
int updateCounter = 0;

// we use this custom character when printing temperature
byte degreeSymbol[] = {
    B00100,
    B01010,
    B00100,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000};

void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(FAN_PIN, OUTPUT);

  strip.begin();
  strip.show();
  strip.setBrightness(150);

  dht.begin();

  lcd.createChar(0, degreeSymbol);
  lcd.begin(16, 2); // number of columns and rows in the LCD
}

void loop()
{
  // update light sensor reading
  lightVal = 1023 - analogRead(A0);

  currTempTime = millis();
  currPrintTime = currTempTime;

  if (currTempTime - prevTempTime >= tempInterval)
  {
    // update humidity + temperature readings (in Celsius)
    double tempH = dht.readHumidity();
    double tempT = dht.readTemperature();
    if (!isnan(tempH) && !isnan(tempT))
    { // update only if successfully read temperature / humidity
      humidity = tempH;
      temperature = tempT;
      prevTempTime = currTempTime;
      doUpdate = true;
      aT = 1.1 * temperature + 0.0261 * humidity - 3.944; // apparent (perceived) temperature. simple formula taken from online
    }
  }

  logData();

  if (doUpdate)
  {
    if (aT > 25 && !fanOn)
    {
      analogWrite(FAN_PIN, 255);
      fanOn = true;
    }
    else if (aT <= 25 && fanOn)
    {
      analogWrite(FAN_PIN, 0);
      fanOn = false;
    }
    doUpdate = false;
  }

  if (lightVal < LIGHT_THRESHOLD)
  {
    switch (gradient_mode)
    {
    case 0:
      colorGradient(12, 0, 12, 50);
      break;
    case 1:
      colorGradient(0, 12, 12, 50);
      break;
    case 2:
      colorGradient(12, 0, 12, 50);
      break;
    }
    gradient_mode = (gradient_mode + 1) % 3;
  }
  else
  {
    colorWipe(strip.Color(0, 0, 0), 50);
    gradient_mode = 0;
  }
  screenPrint();
}

// sets each pixel color to a predefined strip.Color(R,G,B)
void colorWipe(uint32_t color, int wait)
{
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

// increments each pixel color by pre-defined (R,G,B) per pixel
void colorGradient(int R, int G, int B, int wait)
{
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, strip.Color(R * (i + 1), G * (i + 1), B * (i + 1)));
    strip.show();
    delay(wait);
  }
}

// prints to the LCD screen attached to the helmet
// displays current temperature, small "update" animation (adds up to 3 periods modularically)
// and prints HOT or DARK if fans/lights are on respectively
void screenPrint()
{
  if (currPrintTime - prevPrintTime >= printInterval)
  {
    prevPrintTime = currPrintTime;
    lcd.clear();
    // FIRST ROW
    lcd.print("Temp: "); // cols 0-5
    for (int i = 0; i < updateCounter; i++)
    {                                                
      lcd.print(". "); // cols 6-7, 8-9, 10-11
    }

    lcd.setCursor(12, 0);
    if (lightVal < LIGHT_THRESHOLD) // cols 12-15
    {
      lcd.print("DARK");
    }

    // SECOND ROW:
    // print temperature (XX format)
    lcd.setCursor(2, 1); // cols 0-4
    lcd.print(int(aT));
    lcd.write(byte(0));
    lcd.print("C");

    lcd.setCursor(13, 1);
    if (fanOn) // cols 13-15
    {
      lcd.print("HOT");
    }

    updateCounter = (updateCounter+1)%4;
  }
}

void logData()
{
  if (DEBUG_MODE)
  {
    Serial.print("L: ");
    Serial.print(lightVal);
    Serial.print("  H: ");
    Serial.print(humidity);
    Serial.print("  T: ");
    Serial.print(temperature);
    Serial.print("  aT: ");
    Serial.print(aT);
    Serial.print("  DelL:  ");
    Serial.print(currTempTime - prevTempTime);
    if (doUpdate)
    {
      Serial.print(" UL");
    }
    Serial.print(" DelP: ");
    Serial.print(currPrintTime - prevPrintTime);
    if (currPrintTime - prevPrintTime >= printInterval)
    {
      Serial.print(" UP");
    }
    Serial.println();
  }
}
