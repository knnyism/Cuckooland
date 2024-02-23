#pragma once

#include <game.h>
#include <flavor.h>
#include <globals.h>
#include <physics.h>
#include <interpolation.h>

#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

const u32 PLAYER_ACCELERATE = 150;
const u32 PLAYER_MAX_SPEED = 15;
const u32 PLAYER_FRICTION = 13;

const u32 PLAYER_ACCELERATE_AIR = 200;
const f32 PLAYER_GAIN_AIR = 1;
const u32 PLAYER_MAX_SPEED_AIR = 15;
const f32 PLAYER_FRICTION_AIR = 0.2;

const u32 PLAYER_IMPULSE_JUMP = 17;

using namespace JPH;

class Player : public Entity {
public:
    bool isGrounded = false;
    bool isCrouching = false;
    bool wasCrouching = false;

    const f32 cameraHeightStanding = 2.0f;
    const f32 characterHeightStanding = 1.5f;
    const f32 characterRadiusStanding = 2.0f;

    const f32 cameraHeightCrouching = 1.0f;
    const f32 characterHeightCrouching = 0.5f;
    const f32 characterRadiusCrouching = 2.0f;

    f32 lookAngleX = 0.0f;
    f32 lookAngleY = 0.0f;

    Ref<Shape> standingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * characterHeightStanding + characterRadiusStanding, 0), Quat::sIdentity(), new CapsuleShape(0.5f * characterHeightStanding, characterRadiusStanding)).Create().Get();
    Ref<Shape> crouchingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * characterHeightCrouching + characterRadiusCrouching, 0), Quat::sIdentity(), new CapsuleShape(0.5f * characterHeightCrouching, characterRadiusCrouching)).Create().Get();

    MoveHelper moveHelper = MoveHelper(Vec3(0, 5, 0));

    InterpState interpState;

    void Spawn();
    void Tick() override;
    void Render() override;
};