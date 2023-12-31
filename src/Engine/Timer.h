#ifndef TIMER_H
#define TIMER_H

#include "Audio.h"

#include <string>

class Timer {
public:
    Timer() = default;
    ~Timer() = default;

    void update(float timescale);
    void reset();

    void setAudio(Audio* audio);
    void setTimer(int ms);
    void setTimerResetDefault(int ms);
    void setTimerAutoReset(bool autoReset);
    void changeToTimer();
    void changeToStopwatch();

    int getTimer();
    int getMostRecentSecond();
    std::string getTimerAsString();
    bool isZero();

private:
    Audio* _audio = nullptr;

    int _timer = 0;
    int _timerResetDefault = 0;
    int _mostRecentSecond = 0;
    int _countingCoefficient = -1;
    bool _autoTimerReset = false;

};

#endif