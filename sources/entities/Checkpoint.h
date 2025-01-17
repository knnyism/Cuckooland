#pragma once

#include <game.h>

#include <entities/Trigger.h>
#include <entities/Player.h>
#include <entities/Prop.h>
#include <entities/Hud.h>

// Checkpoint entity that saves the states of the player and props once touched
// can optionally not play the reachSound
class Checkpoint : public Entity {
public:
    Trigger* trigger;
    bool playSound;

    game::Sound reachSound = game::Sound("checkpoint_reached");

    void Destroy() override;
    void Load(Vec3 position, Vec3 size, bool playSound = true);
private:
    void OnTouch(BodyID bodyId);
};