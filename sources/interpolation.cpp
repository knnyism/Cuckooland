#include <interpolation.h>

void InterpState::Update(f64 time, Vec3 position, Vec3 rotationAxis, f32 rotationAngle) {
    lastTime = nextTime;
    nextTime = time;

    lastPosition = nextPosition;
    nextPosition = position;

    lastRotationAxis = nextRotationAxis;
    nextRotationAxis = rotationAxis;

    lastRotationAngle = nextRotationAngle;
    nextRotationAngle = rotationAngle;
}

void InterpState::UpdatePosition(f64 time, Vec3 position) {
    lastTime = nextTime;
    nextTime = time;

    lastPosition = nextPosition;
    nextPosition = position;
}

Vec3 InterpState::GetPosition() const {
    f64 alpha = std::clamp((GetTime() - TICK_DURATION - lastTime) / (nextTime - lastTime), 0.0, 1.0);
    return lastPosition + (nextPosition - lastPosition) * alpha;
}

Vec3 InterpState::GetRotationAxis() const {
    f64 alpha = std::clamp((GetTime() - TICK_DURATION - lastTime) / (nextTime - lastTime), 0.0, 1.0);
    return lastRotationAxis + (nextRotationAxis - lastRotationAxis) * alpha;
}

f32 InterpState::GetRotationAngle() const {
    f64 alpha = std::clamp((GetTime() - TICK_DURATION - lastTime) / (nextTime - lastTime), 0.0, 1.0);
    return lastRotationAngle + (nextRotationAngle - lastRotationAngle) * alpha;
}