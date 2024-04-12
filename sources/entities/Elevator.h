#pragma once

#include <game.h>
#include <Jolt/Physics/Constraints/PulleyConstraint.h>
#include <Jolt/Physics/Constraints/TwoBodyConstraint.h>
#include <Jolt/Physics/Constraints/SixDOFConstraint.h>

// Elevator entity used for one of the puzzles
class Elevator : public Entity {
public:
    game::Model base;
    game::Model shaft;

    Vec3 shaftPos;
    Vec3 basePos;

    Quat rot;

    void Load(Vec3 pos = Vec3::sZero(), Quat rot = Quat::sIdentity(), f32 distance = 9.75f);

    void Tick() override;
    void Render() override;
    void Destroy() override;
};