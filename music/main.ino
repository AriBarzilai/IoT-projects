#include <arduino.h>

#define buzzer 12
multiHandler multiHandler(2, 3, 4, 5, 6, 7, 8, 9);
musicPlayer musicPlayer(buzzer);


void setup()
{
    pinMode(buzzer, OUTPUT);
}


viod loop(){
    musicPlayer.update(multiHandler.getDeltaTime() ,multiHandler.get_distances());
}
