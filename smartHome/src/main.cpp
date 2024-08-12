#include <ESP32Servo.h>
#include <Arduino.h>
Servo myservo;
int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33
// Possible PWM GPIO pins on the ESP32-S2: 0(used by on-board button),1-17,18(used by on-board LED),19-21,26,33-42
// Possible PWM GPIO pins on the ESP32-S3: 0(used by on-board button),1-21,35-45,47,48(used by on-board LED)
// Possible PWM GPIO pins on the ESP32-C3: 0(used by on-board button),1-7,8(used by on-board LED),9-10,18-21
int servoPin = 23;

void setup() {
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2500); // attaches the servo onto pin
}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);    // tell servo to go to position in variable 'pos'
    delay(10);             // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);    // tell servo to go to position in variable 'pos'
    delay(10);             // waits 15ms for the servo to reach the position
  }
}
// #define BLYNK_TEMPLATE_ID "TMPL6iDeYqVZn"
// #define BLYNK_TEMPLATE_NAME "SmartHome"
// #define BLYNK_AUTH_TOKEN "DjhUtGEx1XjIda41tiuOXP5WWmHD4_Xv"

// char ssid[]  = "crib";
// char pass[] = "135792468";


// #include "env.h"

// #include <Arduino.h>
// #include "electraRemote\electraRemote.h"
// #include <WiFi.h>
// #include <WiFiClient.h>
// #include <BlynkSimpleEsp32.h>

// #define BLYNK_PRINT Serial // prints to serial

// bool DEBUG_MODE = true;


// ElectraRemote remote;

// void setup()
// {
//     Serial.begin(9600);
//     remote = ElectraRemote();
//     Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
// }

// double interval = 1000; // time between each check of light sensor
// unsigned long currTime = -interval;

// void loop()
// {
//     Blynk.run();
// }

// // POWER STATE (on/off)
// BLYNK_WRITE(V0)
// {
//     int pinValue = param.asInt();

//     PowerState pwr = (PowerState)pinValue;
//     remote.setPowerState(pwr);
//     if (DEBUG_MODE)
//     {
//         Serial.print("Power: ");
//         Serial.println(pwr == PowerState::ON ? "ON" : "OFF");
//     }
// }

// // TEMPERATURE MODE (cold/hot)
// BLYNK_WRITE(V1)
// {
//     int pinValue = param.asInt();

//     TemperatureMode tempMode = (TemperatureMode)pinValue;
//     remote.setTemperatureMode(tempMode);
//     if (DEBUG_MODE)
//     {
//         Serial.print("Temperature Mode: ");
//         Serial.println(tempMode == TemperatureMode::COLD ? "COLD" : "HOT");
//     }
// }

// // FAN SPEED (1/2/3/auto)
// BLYNK_WRITE(V2)
// {
//     int pinValue = param.asInt();

//     FanSpeed fan = (FanSpeed)pinValue;
//     remote.setFanSpeed(fan);
//     if (DEBUG_MODE)
//     {
//         Serial.print("Fan Speed: ");
//         switch (fan)
//         {
//         case FanSpeed::FAN_1:
//             Serial.println("1");
//             break;
//         case FanSpeed::FAN_2:
//             Serial.println("2");
//             break;
//         case FanSpeed::FAN_3:
//             Serial.println("3");
//             break;
//         case FanSpeed::FAN_AUTO:
//             Serial.println("AUTO");
//             break;
//         }
//     }
// }

// // TEMPERATURE (16-30)
// BLYNK_WRITE(V3)
// {
//     int pinValue = param.asInt();

//     remote.setTemperature(pinValue);
//     if (DEBUG_MODE)
//     {
//         Serial.print("Temperature: ");
//         Serial.println(pinValue);
//     }
// }

// /*
// // used for manual command input, for debugging
// void parseCommand(String command)
// {
//     if (command == "on")
//     {
//         remote.setPowerState(PowerState::ON);
//     }
//     else if (command == "off")
//     {
//         remote.setPowerState(PowerState::OFF);
//     }
//     else if (command == "fan1")
//     {
//         remote.setFanSpeed(FanSpeed::FAN_1);
//     }
//     else if (command == "fan2")
//     {
//         remote.setFanSpeed(FanSpeed::FAN_2);
//     }
//     else if (command == "fan3")
//     {
//         remote.setFanSpeed(FanSpeed::FAN_3);
//     }
//     else if (command == "fanAuto")
//     {
//         remote.setFanSpeed(FanSpeed::FAN_AUTO);
//     }
//     else
//     {
//         // Try to parse the command as an integer for temperature
//         int temp = command.toInt();
//         if (temp != 0 || command == "0")
//         { // toInt() returns 0 for invalid input
//             remote.setTemperature(temp);
//         }
//         else
//         {
//             Serial.println("Invalid command");
//         }
//     }
// }
// */