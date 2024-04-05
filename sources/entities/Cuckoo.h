#pragma once

#include <game.h>
#include <entities/Prop.h>
#include <entities/Player.h>
#include <entities/Trigger.h>

class Cuckoo : public Entity {
public:
    Prop* prop;

    void Load(Vec3 position, Quat rotation, bool playLongAnimation = false);
    void SetTriggerFunc(std::function<void(BodyID)> func);

    void Tick();
    void Render();

    void AfterCamera() override;
private:
    void OnTriggerTouched(BodyID bodyId);

    std::function<void(BodyID)> triggerFunc;
    Trigger* trigger;

    bool playLongAnimation;
    raylib::Model* animationModel;

    game::Sound cuckooGetSound = game::Sound("cuckoo_get");
    game::Sound cuckooAddSound = game::Sound("cuckoo_add");

    bool added = false;
    bool acquired = false;
    bool animationFinished = false;

    Mat44 lastTransform;
    Mat44 cameraRelativeMatrix;

    f32 acquireTime;
};