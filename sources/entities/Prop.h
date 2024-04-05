#pragma once

#include <game.h>
#include <entities/Trigger.h>

enum PropMaterial {
    MATERIAL_METAL = 0,
    MATERIAL_WOOD,
};

class Prop : public Entity {
public:
    game::Model model;

    void Load(const string path, Vec3 pos = Vec3::sZero(), Quat rot = Quat::sIdentity(), f32 overrideMass = -1.0f, f32 overrideFriction = 0.5f, ObjectLayer layer = Layers::MOVING, PropMaterial material = MATERIAL_METAL, EMotionType motionType = EMotionType::Dynamic);

    void Tick() override;
    void Render() override;
    void Destroy() override;

    void SaveState();
    void ResetState();
    void LoadSavedState();
    void SetTriggerFunc(std::function<void(BodyID)> func);

    void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings);
private:
    string modelPath;
    Trigger* trigger;

    bool emitSound = false;
    game::Sound3D* hitSound;

    BodyID hitBody;
    BodyID lastHitBody;
    f32 hitVolume = 0.0f;
    f32 lastHitTime = 0.0f;

    Mat44 spawnMatrix;
    Mat44 savedMatrix;
};