#pragma once

#include <game.h>
#include <functional>


class TimerFunc {
public:
    f32 time;
    std::function<void()> func;
    bool isCalled;

    TimerFunc(f32 time, std::function<void()> func) : time(time), func(func), isCalled(false) {};
};

// Entity that calls a function after the time passes the given timestamp
class Timer : public Entity {
public:
    f32 time;
    std::vector<TimerFunc> funcs;
    bool isActive;

    void Timer::Reset() {
        time = 0.0f;
    }

    void Timer::SetActive(bool value) {
        isActive = value;
    }

    void Timer::AddEvent(f32 time, std::function<void()> func) {
        funcs.push_back(TimerFunc(time, func));
    };

    void Timer::Tick() override {
        if (!isActive) { return; }

        time += TICK_DURATION;

        for (TimerFunc& timerFunc : funcs) {
            if (timerFunc.isCalled) { continue; }

            if (time > timerFunc.time) {
                timerFunc.func();
                timerFunc.isCalled = true;
            }
        }
    }
};