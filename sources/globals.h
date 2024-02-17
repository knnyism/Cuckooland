// Header file for global variables

#pragma once

#include <raylib-cpp.hpp>
#include <flavor.h>

inline const s8 TICK_RATE = 60;
inline const f32 TICK_DURATION = 1.0f / TICK_RATE;

inline u32 currentTick = 0;
inline f64 startTime = GetTime();

inline raylib::Camera* camera;

u64 CalculateTickFromTime(f64 currentTime);
f64 CalculateTimeFromTick(s64 tick);