
#include <Ultrasonic.h>
#include <Arduino.h>
#include <pitches.h>

#define BPM 120
#define COUNTER_DELAY 500 // the number of milliseconds between each beat
#define TONE_PWM_CHANNEL 0  
#define buzzer 2

Ultrasonic ultrasonics[] = {Ultrasonic(3, 4), Ultrasonic(5, 6), Ultrasonic(7, 8), Ultrasonic(9, 10)};
double deltaTime = 0.0;
int distances[4];

int toneOutputPin;
int epsilon = 15;                                     // a change in distance (cm) smaller than this is ignored
int lastPlayedDist[4] = {-1000, -1000, -1000, -1000}; // the distance last used responsible for an update
int lastPlayedSensor = -1;                            // the most recent sensor responsible for playing a sound              // the duration of the beat played
double delayCounter = 0;                              // the time until next playing a note
bool isPlaying = false;          



void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT); // Set the buzzer pin as output
  Serial.begin(9600);
  Serial.println("Hello World");
}

void loop() {
  get_distances();
  double deltaTime = getDeltaTime();
  Serial.println("daltaTime: " + String(deltaTime)+ " Counter: " + delayCounter + " distances: " + String(distances[0
  ]) + " lastPlayedDist: " + String(lastPlayedDist[0]));
  update(deltaTime, distances);
}




void multiHandler(int trig1, int echo1, int trig2, int echo2, int trig3, int echo3, int trig4, int echo4) {
  ultrasonics[0] = Ultrasonic(trig1, echo1);
  ultrasonics[1] = Ultrasonic(trig2, echo2);
  ultrasonics[2] = Ultrasonic(trig3, echo3);
  ultrasonics[3] = Ultrasonic(trig4, echo4);
}

void get_distances() {
  deltaTime = millis();
  for (int i = 0; i < 1; i++) {
    distances[i] = ultrasonics[i].read() - 1; // scale down the distance by 1 to be in the range of 1-300
    if (distances[i] > 300 || distances[i] <= 0) {
      distances[i] = 0;
    }
  }
}

double getDeltaTime() {
  return millis() - deltaTime;
}


void update(double deltaTime, int* distances) {
  if (delayCounter > 0 && isPlaying) {
    delayCounter -= deltaTime;
    return;
  
  }
  if (isPlaying) {
    noTone(buzzer);
    delayCounter = COUNTER_DELAY; // we add a small delay between each note
    isPlaying = false;
    return;
  }
  int sensorLargestChange = -1;
  for (int i = 0; i < 4; i++) {
    if (abs(distances[i] - lastPlayedDist[i]) > epsilon) {
      lastPlayedDist[i] = distances[i];
      if (sensorLargestChange == -1 || lastPlayedDist[sensorLargestChange] > lastPlayedDist[i]) {
        sensorLargestChange = i;
      }
    }
  }
  playSound(sensorLargestChange);
}

void setEpsilon(int ep) {
  epsilon = ep;
}

void playSound(int sensorNum) {
  if (isPlaying) {
    return;
  }
  switch (sensorNum) {
    case 0:
      tone(buzzer, NOTE_C2);
      break;
    case 1:
      tone(buzzer, NOTE_G2);
      break;
    case 2:
      tone(buzzer, NOTE_C5);
      break;
    case 3:
      tone(buzzer, NOTE_E5);
      break;
  }
  delayCounter = COUNTER_DELAY;
  isPlaying = true;
}
