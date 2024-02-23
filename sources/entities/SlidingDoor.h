#pragma once

#include <game.h>
#include <interpolation.h>

#include <entities/Player.h>

class SlidingDoor : public Entity {
public:
    game::Model model;

    bool isOpen;
    bool isMoving;

    bool sensorActive = true;
    f32 sensorRange = 6.0f;
    f32 moveSpeed = 0.05f;
    f32 idleTime = 5.0f;

    f32 lastCheckTime = 0.0f;

    void Spawn(Vec3 closedPosition, Vec3 openPosition, Quat rotation = Quat::sIdentity());
    void Tick() override;
    void Render() override;
private:
    Vec3 closedPosition;
    Vec3 openPosition;

    Vec3 position;
    f32 rotation;
};