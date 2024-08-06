#include <Arduino.h>

#define IR_SEND_PIN  4  // D4

#include <IRremote.hpp>

struct AcCommand {  
    uint16_t headerMark;
    uint16_t headerSpace;
    uint16_t bitMark;
    uint16_t OneSpace;
    uint16_t ZeroSpace;
    IRRawDataType rawData[2];
  
    
};

AcCommand ON_COLD_1[] = {
    {9050, 4500, 600, 600, 1700, {0x20006011F747C3, 0xB301002000}},
    {9050, 4500, 600, 600, 1700, {0x20006011F747C3, 0xB301002000}},
    {9050, 4500, 600, 600, 1700, {0x20006011F747C3, 0xB301002000}},
    {9050, 4500, 600, 600, 1700, {0x20006011F747C3, 0xB301002000}},
    {9050, 4500, 600, 600, 1700, {0x20006011F747C3, 0xB301002000}},
    {9050, 4500, 600, 600, 1700, {0x20006011F747C3, 0xB301002000}},
    {9050, 4500, 600, 600, 1700, {0x20006011F747C3, 0xB301002000}},
    {9050, 4500, 600, 600, 1700, {0x20006011F747C3, 0xB301002000}},
    {9050, 4500, 600, 600, 1700, {0x20006011F747C3, 0xB301002000}},
    {9050, 4500, 600, 600, 1700, {0x20006011F747C3, 0xB301002000}}
   
};

    

void setup() {
    Serial.begin(115200);
    IrSender.begin(IR_SEND_PIN); // Removed the ENABLE_LED_FEEDBACK parameter
    
                                                                                                         

}



void loop() {
    delay(1000);
    AcCommand c = ON_COLD_1[0];
     uint64_t tRawData[]={0x20006011F747C3, 0xB301002000};
    IrSender.sendPulseDistanceWidthFromArray(38, c.headerMark, c.headerSpace, c.bitMark, c.OneSpace, c.bitMark, c.ZeroSpace, &c.rawData[0] , 104, PROTOCOL_IS_LSB_FIRST, 0, 0);


    
}