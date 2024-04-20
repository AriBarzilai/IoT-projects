#include "pitches.h"

#define TRIGGER_PIN  16
#define ECHO_PIN     17
#define MAX_DISTANCE 100
#define TONE_OUTPUT_PIN 26
#define EPSILON 15

// The ESP32 has 16 channels which can generate 16 independent waveforms
// We'll just choose PWM channel 0 here
const int TONE_PWM_CHANNEL = 0;

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};


void setup() {
  Serial.begin(115200);
  ledcAttachPin(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);
}
int pauseBetweenNotes = 0;
int distanceLastPlayed = -1000; // the cm value when the note was last played
void loop() {
  int distanceCurrent = ultrasonic_measure(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // the current distance detected, in cm
  // to calculate the note duration, take one second divided by the note type.
  //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

  int difference = distanceCurrent - distanceLastPlayed;
  if (difference > EPSILON || difference < -1*EPSILON) { // we play a note if a large enough change is detected from the last time it was played
    int thisNote = map(distanceCurrent, 1,100,0,7);
    int noteDuration = 1000 / noteDurations[thisNote];
    ledcWriteTone(TONE_PWM_CHANNEL, melody[thisNote]);
    distanceLastPlayed = distanceCurrent;
    delay(noteDuration);
    ledcWrite(TONE_PWM_CHANNEL, 0);
    // to distinguish the not/es, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 0.20;
    delay(pauseBetweenNotes);
  }
  // stop the tone playing:
  Serial.println(String(distanceCurrent) + "   " + String(distanceLastPlayed) + "   " + String(distanceCurrent - distanceLastPlayed));

}

int ultrasonic_measure(int trigPin, int echoPin, int max_distance) {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH, max_distance * 59);
  return duration / 59;
}
