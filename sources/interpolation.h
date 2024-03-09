/*
* interpolation.h
*
* Because we have ticks and rendering at different rates (most likely...), we need to interpolate between the last and next state
* The InterpState struct holds the last and next state, and can be used to get the current state at any time.
* Set the state during the tick, and interpolate during render.
*/

#pragma once

#include <flavor.h>
#include <physics.h>

template <typename T>
struct InterpState {
    T lastState;
    T nextState;

    f64 lastTime;
    f64 nextTime;

    void Set(T state) {
        lastTime = nextTime;
        nextTime = GetTime();

        lastState = nextState;
        nextState = state;
    }

    T Get() const {
        f64 alpha = std::clamp((GetTime() - TICK_DURATION - lastTime) / (nextTime - lastTime), 0.0, 1.0);
        return lastState + (nextState - lastState) * alpha;
    }
};

template <typename T>
struct SpringState {
    T position;
    T velocity;

    T goal;

    f32 stiffness = 350;
    f32 dampening = 16;

    void Update(f32 deltaTime) {
        if (deltaTime > 1.0f / 20.0f) {
            position = goal;
            velocity = T();
            return;
        }

        T spring = -stiffness * (position - goal);
        T damper = -dampening * velocity;

        T A = spring + damper;

        velocity = velocity + A * deltaTime;
        position = position + velocity * deltaTime;
    }

    void UpdateWithGoal(T goal, f32 deltaTime) {
        this->goal = goal;

        return Update(deltaTime);
    }
};