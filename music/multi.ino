

/*
 * Pass as a parameter the trigger and echo pin, respectively,
 * or only the signal pin (for sensors 3 pins), like:
 * Ultrasonic ultrasonic(13);
 */
#include <Arduino.h>
#include <Ultrasonic.h>

#define buzzer 12

Ultrasonic ultrasonic(27, 26);
Ultrasonic ultrasonic2(22, 23);
Ultrasonic ultrasonic3(18, 19);
Ultrasonic ultrasonic4(33, 32);

int distance, destance2, destance3, destance4;

void setup() {
  Serial.begin(9600);
  pinMode(buzzer,OUTPUT);
}


void loop() {
  unsigned char i;
  while(1)
  {
  distance = ultrasonic.read();
  destance2 = ultrasonic2.read(); 
  destance3 = ultrasonic3.read();
  destance4 = ultrasonic4.read();
  
  Serial.print("Sensor 1: ");
  Serial.print(distance);
  Serial.print(" Sensor 2: ");
  Serial.print(destance2);
  Serial.print(" Sensor 3: ");
  Serial.print(destance3);
  Serial.print(" Sensor 4: ");
  Serial.println(destance4);
    //output an frequency
    for(i=0;i<80;i++)
    {
      digitalWrite(buzzer,HIGH);
      delay(1);//wait for 1ms
      digitalWrite(buzzer,LOW);
      delay(1);//wait for 1ms
    }
    //output another frequency
    for(i=0;i<100;i++)
    {
      digitalWrite(buzzer,HIGH);
      delay(2);//wait for 2ms
      digitalWrite(buzzer,LOW);
      delay(2);//wait for 2ms
    }
  }

  // Pass INC as a parameter to get the distance in inches
 
  
  
}
