#include <Adafruit_NeoPixel.h>
#include "DHT.h"

#define LED_PIN 2
#define LED_COUNT 12
#define DHTPIN 7
#define DHTTYPE DHT22

#define DEBUG_MODE true

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
DHT dht(DHTPIN, DHTTYPE);

double lightVal;
float humidity;
float temperature;
int gradient_mode = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  strip.begin();
  strip.show();
  strip.setBrightness(150);
  dht.begin();
}

void loop()
{
  // update light sensor reading
  lightVal = 1023 - analogRead(A0);
  // update humidity + temperature readings (in Celsius)
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  logData();

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

void logData()
{
  if (DEBUG_MODE)
  {
    Serial.print("L: ");
    Serial.print(lightVal);
    Serial.print("  H: ");
    Serial.print(humidity);
    Serial.print("  T: ");
    Serial.println(temperature);
  }
}
