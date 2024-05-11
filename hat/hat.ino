#include <Adafruit_NeoPixel.h>
#define LED_PIN 2
#define LED_COUNT 12
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

double val;
int mode = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  strip.begin();
  strip.show();
  strip.setBrightness(150);
}

void loop()
{
  val = 1023 - analogRead(A0);
  Serial.println(val);

  if (val < 200)
  {
    switch (mode)
    {
    case 0:
      colorGradient(12, 0, 12, 50);
    case 1:
      colorGradient(0, 12, 12, 50);
    case 2:
      colorGradient(12, 0, 12, 50);
    }
    mode = (mode + 1) % 3;
  }
  else
  {
    colorWipe(strip.Color(0, 0, 0), 50);
    mode = 0;
  }
}

// sets each pixel color to a predefined strip.Color(R,G,B)
void colorWipe(uint32_t color, int wait)
{
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, strip.setPixelColor(i, color));
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
