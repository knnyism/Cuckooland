#pragma once

#include <game.h>
#include <entities/Prop.h>
#include <entities/Player.h>

class Cuckoo : public Entity {
public:
    Prop* prop;

    void Load(Vec3 position, Quat rotation, bool playLongAnimation = false);
    void HandlePlayer(Player* player);

    void Tick();
    void Render();
private:
    bool playLongAnimation;
    raylib::Model* animationModel;

    game::Sound cuckooGetSound = game::Sound("cuckoo_get");

    bool acquired = false;
    bool animationFinished = false;

    Mat44 cameraRelativeMatrix;

    f32 acquireTime;
};