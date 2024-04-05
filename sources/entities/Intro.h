#pragma once

#include <game.h>
#include <tween.h>

#include <entities/Player.h>

class Intro : public Entity {
public:
    void Play(Player* player, Vec3 pos);
    void BeforeCamera() override;
    void AfterCamera() override;
private:
    raylib::Rectangle fadeIn;
    Player* player;

    Vec3 startPos;

    game::Sound lightsOnSound = game::Sound("lights_on");

    bool lightsOn;
    f32 animationTime;
    Mat44 animationTransform;
};