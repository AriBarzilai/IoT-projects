#include <Arduino.h>

void electraRemoteSetup();
void sendAccommand(bool on, bool cold, int8_t fan ,int8_t tmp);


void setup() {
    Serial.begin(115200);
    electraRemoteSetup();
}

void loop() {
    for(int j = 1; j <= 4; j++){
    for(int i = 16; i <= 30; i++){
            sendAccommand(true, true, j, i);
            delay(1000);
        }
    }
}