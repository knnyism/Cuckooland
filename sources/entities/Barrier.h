#pragma once

#include <game.h>

// Physics barrier entity
class Barrier : public Entity {
public:
    void Load(Vec3 pos = Vec3::sZero(), Quat rot = Quat::sIdentity(), Vec3 size = Vec3::sReplicate(1.0f), ObjectLayer layer = Layers::MOVING) {
        bodyId = body_interface->CreateAndAddBody(BodyCreationSettings(new BoxShape(size / 2), pos, rot, EMotionType::Static, layer), EActivation::Activate);
    };

    void Destroy() override {
        body_interface->RemoveBody(bodyId);
        body_interface->DestroyBody(bodyId);
    }
private:
    BodyID bodyId;
};