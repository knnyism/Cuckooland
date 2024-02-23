#pragma once

#include <flavor.h>
#include <physics.h>

// Because we have ticks and rendering at different rates (most likely...), we need to interpolate between the last and next state
// This struct holds the last and next state, and can be used to get the current state at any time
// Set the state during the tick, and interpolate during render
struct InterpState {
    f64 lastTime;
    f64 nextTime;

    Vec3 lastPosition;
    Vec3 nextPosition;

    Vec3 lastRotationAxis;
    Vec3 nextRotationAxis;

    f32 lastRotationAngle;
    f32 nextRotationAngle;

    void Update(f64 time, Vec3 position, Vec3 rotationAxis, f32 rotationAngle);
    void UpdatePosition(f64 time, Vec3 position);

    Vec3 GetPosition() const;
    Vec3 GetRotationAxis() const;
    f32 GetRotationAngle() const;
};