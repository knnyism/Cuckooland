#pragma once

#include <game.h>

enum PropMaterial {
    MATERIAL_METAL = 0,
    MATERIAL_WOOD,
};

class Prop : public Entity {
public:
    game::Model model;

    void Load(const string path, Vec3 pos = Vec3::sZero(), Quat rot = Quat::sIdentity(), f32 overrideMass = -1.0f, f32 overrideFriction = 0.5f, ObjectLayer layer = Layers::MOVING, PropMaterial material = MATERIAL_METAL);

    void Tick() override;
    void Render() override;
    void Destroy() override;

    void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings);
private:
    bool emitSound = false;
    game::Sound3D* hitSound;

    BodyID hitBody;
    BodyID lastHitBody;
    f32 hitVolume = 0.0f;
    f32 lastHitTime = 0.0f;
};