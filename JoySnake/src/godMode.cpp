#include <Arduino.h>


#define MICROPHONE_PIN 0 // to do 
#define MICROPHONE_THRESHOLD 0 // to do 
#define LIHT_SENSOR_PIN 0 // to do


bool godMode = false;

void initializeGodMode(){
    pinMode(MICROPHONE_PIN, INPUT);
    pinMode(LIHT_SENSOR_PIN, INPUT);
}


bool godModeIsActive(){
  return godMode;
}


char getGodModeKey(){

  return ' ';
}