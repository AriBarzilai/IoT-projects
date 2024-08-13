#include "plantWateringSystem.h"

plantWateringSystem::plantWateringSystem(int servoPin, int soil_sensor)
    : servoPin(servoPin), soilSensorPin(soil_sensor) ,moistureThreshold(2000), wateringDuration(5000)
{
    // initialize();
}

plantWateringSystem::plantWateringSystem()
    : servoPin(23), soilSensorPin(34) , moistureThreshold(2000), wateringDuration(2000)
{
    // initialize();
}

void plantWateringSystem::initialize()
{   
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    myservo.setPeriodHertz(50);    // standard 50 hz servo
    myservo.attach(servoPin, 500, 2400); // attaches the servo onto pin with adjusted pulse width
    myservo.write(0);                    // sets the servo to 0 degrees (closed)
    pinMode(soilSensorPin, INPUT);
    isWateringFlag = false;
}

void plantWateringSystem::startWatering()
{
    if (!isWateringFlag)
    {
        myservo.write(180); // open the valve (tourn on the pump)
        isWateringFlag = true;
        pumpIsOpen = millis();
    }
}

bool plantWateringSystem::needsWatering()
{
    return soilMoistureValue > moistureThreshold;
}

void plantWateringSystem::update()
{   
    soilMoistureValue = analogRead(soilSensorPin);
    if (isWateringFlag && millis() - pumpIsOpen >= wateringDuration)
    {
        myservo.write(0); // close the valve (turn off the pump)
        isWateringFlag = false;
    }
    
}

void plantWateringSystem::setWateringDuration(int duration)
{
    if (duration > 0)
        wateringDuration = duration;
}

void plantWateringSystem::setMoistureThreshold(int threshold)
{
    if (threshold > 0)
        moistureThreshold = threshold;
}

bool plantWateringSystem::isWatering()
{
    return isWateringFlag;
}