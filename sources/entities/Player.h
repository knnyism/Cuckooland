#pragma once

#include <entity.h>
#include <physics.h>
#include <flavor.h>
#include <globals.h>

#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

struct InterpState {
    f64 lastTime;
    f64 nextTime;

    JPH::Vec3 lastPosition;
    JPH::Vec3 nextPosition;
};

class Player : public Entity {
public:
    const f32 cameraHeightStanding = 4.5f;
    const f32 characterHeightStanding = 4.0f;
    const f32 characterRadiusStanding = 1.0f;

    const f32 cameraHeightCrouching = 3.0f;
    const f32 characterHeightCrouching = 4.0f;
    const f32 characterRadiusCrouching = 1.0f;

    f32 lookAngleX = 0.0f;
    f32 lookAngleY = 0.0f;

    InterpState interpState;

    JPH::CharacterVirtual* character;

    void OnContactAdded(const JPH::CharacterVirtual* inCharacter, const JPH::BodyID& inBodyID2, const JPH::SubShapeID& inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings& ioSettings);

    void Spawn();
    void Tick() override;
    void Render() override;
private:
    JPH::Vec3 MoveAir(JPH::Vec3 moveDirection, JPH::Vec3 velocity);
    JPH::Vec3 MoveGround(JPH::Vec3 moveDirection, JPH::Vec3 velocity);
};