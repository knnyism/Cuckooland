#pragma once

#include <globals.h>

u64 CalculateTickFromTime(f64 currentTime) {
    return std::ceil((currentTime - startTime) * TICK_RATE);
}

f64 CalculateTimeFromTick(s64 tick) {
    return (f64)tick / (f64)TICK_RATE;
}