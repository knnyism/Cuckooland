#pragma once

#include <game.h>

class Prop : public Entity {
public:
    game::Model model;

    void Load(const string path, Vec3 pos = Vec3::sZero(), Quat rot = Quat::sIdentity(), f32 overrideMass = -1.0f);

    void Tick() override;
    void Render() override;
    void Destroy() override;
};