#pragma once

#include <game.h>
#include <tween.h>
#include <interpolation.h>

#include <entities/Player.h>
#include <entities/Trigger.h>

class SlidingDoor : public Entity {
public:
    game::Model model;

    game::Sound3D openSound = game::Sound3D("techdoor_open");
    game::Sound3D closeSound = game::Sound3D("techdoor_close");

    bool isOpen;
    bool isMoving;

    bool automatic = true;
    bool sensorHit = false;

    f32 sensorRange = 5.0f;
    f32 idleTime = 5.0f;
    f32 moveTime = 1.7f;

    f32 moveStartTime = 0.0f;
    f32 lastCheckTime = 0.0f;

    void Spawn(Vec3 closedPosition, Vec3 openPosition, Quat rotation = Quat::sIdentity());

    void SetState(bool value);

    void HandleState();

    void Animate();

    void Tick() override;
    void Render() override;

    Vec3 position;
    Quat rotation;
private:
    void OnTriggerTouched(BodyID bodyId);

    Trigger* trigger;

    Vec3 destination;
    Vec3 closedPosition;
    Vec3 openPosition;
};