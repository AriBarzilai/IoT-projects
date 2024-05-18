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
#define LCD_D4 5  // data pins
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

#define DEBUG_MODE true

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // LED
DHT dht(DHT_PIN, DHT_TYPE);                                        // humidity+temperature sensor
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7); // screen display

double lightVal;
float humidity;
float temperature;
int gradient_mode = 0;
bool fanOn = false; // checks if fan is currently powered
double aT = 0.0;

double interval = 3000; // time between each check
unsigned long currTime = 0.0;
unsigned long prevTime = -interval;
bool doUpdate = false; // used to update fan checks

void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(FAN_PIN, OUTPUT);

  strip.begin();
  strip.show();
  strip.setBrightness(150);

  dht.begin();

  lcd.begin(16, 2); // number of columns and rows in the LCD
}

void loop()
{
  // update light sensor reading
  lightVal = 1023 - analogRead(A0);

  currTime = millis();
  if (currTime - prevTime >= interval)
  {
    // update humidity + temperature readings (in Celsius)
    double tempH = dht.readHumidity();
    double tempT = dht.readTemperature();
    if (!isnan(tempH) && !isnan(tempT))
    { // update only if successfully read temperature / humidity
      humidity = tempH;
      temperature = tempT;
      prevTime = currTime;
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

  if (lightVal < 200)
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
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  for (int i = 1; i <= 3; i++)
  {
    if (currTime - prevTime >= i * 1000)
    {
      lcd.print(". ");
    }
  }
  lcd.setCursor(0, 1);
  char *stringTemp = (int)aT + "." + (int)(aT * 10 - aT);
  lcd.print(stringTemp);

  if (fanOn)
  {
    lcd.setCursor(12, 1);
    lcd.print("HOT");
  }

  if (lightVal < 200)
  {
    lcd.setCursor(11, 1);
    lcd.print("DARK");
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
    if (doUpdate)
    {
      Serial.print(" UpdateTimer ");
    }
    Serial.println();
  }
}
