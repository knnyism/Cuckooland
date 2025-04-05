#pragma once

#include <game.h>
#include <entities/Prop.h>

#include <Jolt/Physics/Constraints/HingeConstraint.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

// Hinged door entity, seen as fire exit doors in-game
class Door : public Prop {
public:
    Body* body1;
    Body* body2;

    void Load(const std::string path, Vec3 pos = Vec3::sZero(), Quat rot = Quat::sIdentity(), f32 mass = 10.0f, f32 hingeLimitsMin = -90.0f, f32 hingeLimitsMax = 90.0f);
    void Tick() override;
    void Render() override;
};