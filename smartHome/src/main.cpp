#include <Arduino.h>

#include "electraRemote\electraRemote.h"

ElectraRemote remot;

void setup() {
    Serial.begin(9600);
    remot = ElectraRemote();
}

void loop() {
    Serial.println("Enter command:");
    
    while(!Serial.available());
    
    String command = Serial.readStringUntil('\n');
    command.trim();  

    if (command == "on") {
        remot.setPowerState(PowerState::ON);
    } else if (command == "off") {
        remot.setPowerState(PowerState::OFF);
    } else if (command == "fan1") {
        remot.setFanSpeed(FanSpeed::FAN_1);
    } else if (command == "fan2") {
        remot.setFanSpeed(FanSpeed::FAN_2);
    } else if (command == "fan3") {
        remot.setFanSpeed(FanSpeed::FAN_3);
    } else if (command == "fanAuto") {
        remot.setFanSpeed(FanSpeed::FAN_AUTO);
    } else {
            // Try to parse the command as an integer for temperature
            int temp = command.toInt();
            if (temp != 0 || command == "0") {  // toInt() returns 0 for invalid input
                remot.setTemperature(temp);
            } else {
                Serial.println("Invalid command");
            }
    }
    delay(1000);
    
}

