#include "pitches.h"
#include <cstdlib>

#define BPM 120
#define COUNTER_DELAY (60 / BPM) * 1000 // the number of milliseconds between each beat
#define TONE_PWM_CHANNEL 0              // The ESP32 has 16 channels which can generate 16 independent waveforms. We just use channel 0 here.

static class MusicPlayer
{
private:
    int toneOutputPin;
    int epsilon = 15;                                     // a change in distance (cm) smaller than this is ignored
    int lastPlayedDist[4] = {-1000, -1000, -1000, -1000}; // the distance last used responsible for an update
    int lastPlayedSensor = -1;                            // the most recent sensor responsible for playing a sound
    int noteDuration[4] = {1, 1, 1, 1};                   // the duration of the beat played
    double delayCounter = 0;                              // the time until next playing a note
    bool isPlaying = false;                               // a toggle for if it's currently playing or not

public:
    // constructor
    MusicPlayer(int toneOutputPin)
    {
        ledcAttachPin(toneOutputPin, TONE_PWM_CHANNEL);
    }

    void update(double deltaTime, int distances[4])
    {
        if (delayCounter > 0)
        {
            delayCounter -= deltaTime;
            return;
        }

        if (isPlaying)
        {
            delayCounter = COUNTER_DELAY * noteDuration[lastPlayedSensor] * 0.2; // we add a small delay between each note
            isPlaying = false;
            return;
        }

        int sensorLargestChange = -1;
        for (int i = 0; i < 4; i++)
        {
            if (abs(distances[i] - lastPlayedDist[i]) > epsilon)
            {
                lastPlayedDist[i] = distances[i];
                if (sensorLargestChange == -1 || lastPlayedDist[sensorLargestChange] > lastPlayedDist[i])
                {
                    sensorLargestChange = i;
                }
            }
        }

        playSound(sensorLargestChange);
    }

    void setEpsilon(int epsilon)
    {
        this->epsilon = epsilon;
    }

    void playSound(int sensorNum)
    {
        if (isPlaying)
        {
            return;
        }

        switch (sensorNum)
        {
        case 0:
            ledcWriteTone(TONE_PWM_CHANNEL, NOTE_C2);
            break;
        case 1:
            ledcWriteTone(TONE_PWM_CHANNEL, NOTE_G2);
            break;
        case 2:
            ledcWriteTone(TONE_PWM_CHANNEL, NOTE_C5);
            break;
        case 3:
            ledcWriteTone(TONE_PWM_CHANNEL, NOTE_E5);
            break;
        }
        delayCounter = COUNTER_DELAY;
        isPlaying = true;
    }
};