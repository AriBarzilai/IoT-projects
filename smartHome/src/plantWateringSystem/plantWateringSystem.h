#ifndef PLANT_WATERING_SYSTEM_H
#define PLANT_WATERING_SYSTEM_H

#include <ESP32Servo.h>
/**
 * @class plantWateringSystem
 * @brief A class to control a plant watering system using a servo motor to control and a soil moisture sensor .
 */

class plantWateringSystem
{
public:
    /**
     * @brief Constructor for the plantWateringSystem class.
     * @param servoPin The pin number connected to the servo motor.
     * @param soil_sensor The pin number connected to the soil moisture sensor.
     */
    plantWateringSystem(int servoPin, int soil_sensor);

    /**
     * @brief Constructor for the plantWateringSystem class.
     * @param servoPin The pin number set to be 25.
     * @param soil_sensor The pin number set to be 34.
     */
    plantWateringSystem();

    /**
     * @brief Initializes the plant watering system by setting up the servo motor and soil moisture sensor.
     */
    void initialize();

    /**
     * @brief Starts the watering process by opening the valve.
     */
    void startWatering();


    /**
     * @brief Checks if the plant needs watering based on the moisture sensor value.
     * @return True if the plant needs watering, false otherwise.
     */
    bool needsWatering();

    /**
     * @brief Updates the soil moisture sensor value and the system state,
     *        including checking the moisture sensor and controlling the watering process.
     */
    void update();

    /**
     * @brief Sets the duration for which the watering should occur.
     * @param duration The duration in milliseconds.
     */
    void setWateringDuration(int duration);

    /**
     * @brief Sets the moisture threshold below which the plant needs watering.
     * @param threshold The moisture threshold value.
     */
    void setMoistureThreshold(int threshold);

    /**
     * @brief Checks if the plant watering system is currently watering.
     * @return True if the system is watering, false otherwise.
     */
    bool isWatering();

    /**
     * @brief get the moisture value of the soil moisture value.
     * @return The moisture value of the soil moisture sensor.
     */
    int getMoistureValue();

private:
    int servoPin;             ///< The pin number connected to the servo motor.
    int soilSensorPin;        ///< The pin number connected to the soil moisture sensor.
    Servo myservo;            ///< Servo object to control the servo motor.
    bool isWateringFlag;  ///< Flag to indicate if watering is in progress.
    unsigned long pumpIsOpen; ///< Timestamp when the pump was opened.
    int soilMoistureValue;    ///< Current value from the soil moisture sensor.
    int wateringDuration;     ///< Duration for which the watering should occur.
    int moistureThreshold;    ///< Moisture threshold below which the plant needs watering.
};

#endif // PLANT_WATERING_SYSTEM_H