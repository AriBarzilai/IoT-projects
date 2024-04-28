#include <Ultrasonic.h>
#include "pitches.h"

#define BUZZER_PIN 2       // Pin to connect the buzzer
#define DELAY_TIME 300     // the number of milliseconds between the start of one sound and the next
#define PLAY_TIME 250      // the number of milliseconds to play a sound
#define TONE_PWM_CHANNEL 0 // the channel it plays in, we just put 0 for this
#define THRESHOLD 15       // a change in distance (cm) smaller than this is ignored
#define SENSOR_COUNT 4     // for debug purpose\s, used to quickly iterate+test over less sensors

Ultrasonic ultrasonics[] = {Ultrasonic(3, 4), Ultrasonic(5, 6), Ultrasonic(7, 8), Ultrasonic(9, 10)};
int distances[4];                                     // the distance read by each sensor
int maxDistances[4] = {300, 150, 300, 150};           // if the distance is greater than this, we do not store it
int thresholds[4] = {15, 20, 15, 20};                 // if the change in distance = abs(current cycle - previous cycle) is greater than this, it is significant
int lastPlayedDist[4] = {-1000, -1000, -1000, -1000}; // the distance last used responsible for an update
int diff[4] = {0, 0, 0, 0};                           // used for debugging purposes only; the difference between the previous update and the current
double prevTime = 0.0;                                // the time in milliseconds previous loop() cycle ended
double deltaTime = 0.0;
double delayCounter = 0;   // the time until next playing a new note
double playCounter = 0;    // the time left until we stop playing the current note
bool playSound = false;    // if true, play a new sound
int SensorCauseSound = -1; // the sensor responsible for playing the current sound; the sensor with largest distance change

// the pre-defined melody we play upon turning on the device
// note followed by its duration
const PROGMEM int melody[] = {
    // The first notes of Bloody Tears, from Castlevania II
    // Arranged by Bobby Lee. THe flute part was used
    // https://musescore.com/user/263171/scores/883296

    // B-flat major Bb Eb
    REST,
    4,
    NOTE_G5,
    4,
    NOTE_A5,
    4,
    NOTE_AS5,
    4,
    NOTE_A5,
    4,
    NOTE_F5,
    4,
    NOTE_A5,
    4,
    NOTE_G5,
    4,
    REST,
    4,
};

void setup()
{
  // Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT); // Set the buzzer pin as output
  delay(300);
  playStartupMelody();
  delay(2000);
  prevTime = millis();
}

void loop()
{
  updateDistances();
  double deltaTime = getDeltaTime();
  update_sound(deltaTime, distances);
  // printLog(deltaTime);
  prevTime = millis();
}

// plays a short pre-defined melody
void playStartupMelody()
{
  int tempo = 144;
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;

  // this calculates the duration of a whole note in ms (60s/tempo)*4 beats
  int wholenote = (60000 * 4) / tempo;

  int divider = 0, noteDuration = 0;
  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2)
  {

    // calculates the duration of each note
    divider = pgm_read_word_near(melody + thisNote + 1);
    if (divider > 0)
    {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    }
    else if (divider < 0)
    {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(BUZZER_PIN, pgm_read_word_near(melody + thisNote), noteDuration * 0.9);
    // Wait for the specief duration before playing the next note.
    delay(noteDuration);
    // stop the waveform generation before the next note.
    noTone(BUZZER_PIN);
  }
}

// updates the distances read by each sensor, and the difference from the previous value.
// if the distance is invalid (less than 1 or larger than maxDistance), we ignore it
void updateDistances()
{
  bool hasUpdated = false;
  int largestDiff = 0;
  int largestSensor = -1;

  for (int i = 0; i < SENSOR_COUNT; i++)
  {
    distances[i] = ultrasonics[i].read() - 1; // scale down the distance by 1 to be in the range of 1-300
    delay(10);
    if (distances[i] > maxDistances[i] || distances[i] <= 0)
    {
      distances[i] = 0;
    }

    diff[i] = abs(distances[i] - lastPlayedDist[i]);
    if (distances[i] != 0 && diff[i] > thresholds[i])
    {
      lastPlayedDist[i] = distances[i];
    }
  }
}

double getDeltaTime()
{
  return millis() - prevTime;
}

// stops playing a note when playSound timer is not positive.
// does not play a new note while delayCounter is positive.
// else if significant change in distance (movement) is detected, plays a new note corresponding to the largest change
void update_sound(double deltaTime, int *distances)
{
  SensorCauseSound = -1;
  if (delayCounter <= 0)
  {
    int maxDiff = 0;
    for (int i = 0; i < SENSOR_COUNT; i++)
    {
      if (diff[i] >= maxDiff && distances[i] != 0)
      {
        SensorCauseSound = i;
        maxDiff = diff[i];
      }
    }

    if (maxDiff > THRESHOLD)
    {
      delayCounter = DELAY_TIME;
      playCounter = PLAY_TIME;
      playSound = true;
    }
  }

  if (playCounter <= 0)
  {
    noTone(BUZZER_PIN);
  }
  else
  {
    playCounter -= deltaTime;
  }

  if (playSound)
  {
    playSound = false;
    switch (SensorCauseSound)
    {
    case 0:
      tone(BUZZER_PIN, NOTE_G5);
      break;
    case 1:
      tone(BUZZER_PIN, NOTE_E5);
      break;
    case 2:
      tone(BUZZER_PIN, NOTE_C4);
      break;
    case 3:
      tone(BUZZER_PIN, NOTE_D5);
      break;
    }
  }

  if (delayCounter > 0)
  {
    delayCounter -= deltaTime;
  }
}

// for debug purposes
void printLog(double deltaTime)
{
  Serial.print("dT: " + String(deltaTime));
  Serial.print(" DCtr: " + String(delayCounter));
  Serial.print(" PCtr: " + String(playCounter));
  Serial.print(" (dist, LastDist): ");
  for (int i = 0; i < SENSOR_COUNT; i++)
  {
    Serial.print("(" + String(distances[i]) + "," + String(lastPlayedDist[i]) + "," + String(diff[i]) + ") ");
  }
  Serial.print("isPlay: " + String(playCounter > 0));
  Serial.print(" isDelay: " + String(delayCounter > 0));
  Serial.print(" Sensor: " + String(SensorCauseSound));
  Serial.println();
}