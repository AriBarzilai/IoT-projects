#include <Ultrasonic.h>

#define BUZZER_PIN 2 // Pin to connect the buzzer
#define DELAY_TIME 300 // the number of milliseconds between the start of one sound and the next
#define PLAY_TIME 250 // the number of milliseconds to play a sound
#define TONE_PWM_CHANNEL 0 
#define EPSILON 15 // a change in distance (cm) smaller than this is ignored

Ultrasonic ultrasonics[] = {Ultrasonic(3, 4), Ultrasonic(5, 6), Ultrasonic(7, 8), Ultrasonic(9, 10)};
int distances[4];                            
int lastPlayedDist[4] = {-1000, -1000, -1000, -1000}; // the distance last used responsible for an update
double prevTime = 0.0;                                  // the time in milliseconds previous loop() cycle ended 
double deltaTime = 0.0;
double delayCounter = 0;                              // the time until next playing a new note
double playCounter = 0;                               // the time left until we stop playing the current note
bool playSound = false;                               // if true, play a new sound
bool SensorCauseSound = -1;                           // the sensor responsible for playing the current sound; the sensor with largest distance change

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT); // Set the buzzer pin as output
  prevTime = millis();
}


void loop() {
  update_distances();
  double deltaTime = getDeltaTime();
  update_sound(deltaTime, distances);
  printLog(deltaTime);
  prevTime = millis();
}

void update_distances() {
  bool hasUpdated = false;
  int diff = 0;
  int largestDiff = 0;
  int largestSensor = -1;

  for (int i = 0; i < 4; i++) {
    distances[i] = ultrasonics[i].read() - 1; // scale down the distance by 1 to be in the range of 1-300
    if (distances[i] > 300 || distances[i] <= 0) {
      distances[i] = 0;
    }

    diff = abs(distances[i] - lastPlayedDist[i]);
    if (distances[i] != 0 && diff > EPSILON) {
      lastPlayedDist[i] = distances[i];
      if (largestDiff <= diff) {
        largestDiff = diff;
        largestSensor = i;
      }
      hasUpdated = true;
    }
  }

  if (hasUpdated && delayCounter <= 0) { 
      delayCounter = DELAY_TIME;
      playCounter = PLAY_TIME;
      playSound = true;
      SensorCauseSound = largestSensor;
  }
}

double getDeltaTime() {
  return millis() - prevTime;
}

void update_sound(double deltaTime, int* distances) {
  if (playCounter <= 0) {
    noTone(BUZZER_PIN);
  } else {
    playCounter -= deltaTime;
  }
  
  if (playSound) {
    playSound = false;
    switch (SensorCauseSound) {
      case 0:
        tone(BUZZER_PIN, 65);
        break;
      case 1:
        tone(BUZZER_PIN, 98);
        break;
      case 2:
        tone(BUZZER_PIN, 523);
        break;
      case 3:
        tone(BUZZER_PIN, 659);
        break;
    }
  }

  if (delayCounter > 0) {
    delayCounter -= deltaTime;
  }
}

void printLog(double deltaTime) {
  Serial.print("dT: " + String(deltaTime));
  Serial.print(" DCtr: " + String(delayCounter));
  Serial.print(" PCtr: " + String(playCounter));
  Serial.print(" (dist, LastDist): ");
  for (int i = 0; i < 4; i++) {
    Serial.print("(" + String(distances[i]) + "," + String(lastPlayedDist[i])+ ") ");
  } 
  Serial.print("isPlay: " + String(playCounter > 0));
  Serial.print(" Sensor: " + String(SensorCauseSound));
  Serial.println();
}