#include <Ultrasonic.h>

class multiHandler
{
private:
    Ultrasonic[4] ultrasonics;
    double deltaTime = 0.0;
   
public:
    int distances[4];
    multiHandler(int trig1, int echo1, int trig2, int echo2, int trig3, int echo3, int trig4, int echo4)
    {
        ultrasonics[0] = Ultrasonic(trig1, echo1);
        ultrasonics[1] = Ultrasonic(trig2, echo2);
        ultrasonics[2] = Ultrasonic(trig3, echo3);
        ultrasonics[3] = Ultrasonic(trig4, echo4);
    }


    int[] get_distances();
    {
        deltaTime = millis();
        for (int i = 0; i < 4; i++)
        {
         distance[i] = ultrasonics[i].read() - 1; 
        // scale down the distance by 1 to by in the range of 1-300
        if (distance > 300 || distance <= 0)
        {
           distance[i] = 0;
        }
    }
    return distances;
    }

    double getDeltaTime()
    {
        return millis() - deltaTime;
    }
};
