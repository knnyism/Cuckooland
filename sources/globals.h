/*
* globals.h
*
* Contains global variables and functions that are used throughout the game.
* This is particularly useful for variables that are used across files, such as the tick rate.
*/

#pragma once

#include <raylib-cpp.hpp>
#include <flavor.h>

inline const s8 TICK_RATE = 60;
inline const f32 TICK_DURATION = 1.0f / TICK_RATE;

inline u32 currentTick = 0;
inline f64 startTime = GetTime();

inline bool quitRequested = false;

inline u32 totalCuckoos = 0;
inline u32 cuckooCount = 0;
inline u32 deathCount = 0;

inline f64 scrollDirection = 0;
inline raylib::Camera* camera;

inline u64 CalculateTickFromTime(f64 currentTime) {
    return std::ceil((currentTime - startTime) * TICK_RATE);
};

inline f64 CalculateTimeFromTick(s64 tick) {
    return (f64)tick / (f64)TICK_RATE;
}