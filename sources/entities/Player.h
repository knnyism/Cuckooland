#pragma once

#include <game.h>
#include <tween.h>
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

const u32 PLAYER_ACCELERATE_AIR = 120;
const f32 PLAYER_GAIN_AIR = 1;
const u32 PLAYER_MAX_SPEED_AIR = 15;
const f32 PLAYER_FRICTION_AIR = 0.2;

const u32 PLAYER_IMPULSE_JUMP = 17;

const f32 CAM_LOOK_UP = 90;
const f32 CAM_LOOK_DOWN = -90;

using namespace JPH;

class Player : public Entity {
public:

    // States
    bool IsPlayerAlive;

    f32 lookAngleX;
    f32 lookAngleY;

    bool isGrounded;
    bool isCrouching;
    bool lookLocked;

    BodyID holdProp;
    BodyID currentLadder;
    Vec3 ladderNormal;

    // Camera
    Mat44 cameraMatrix;

    // Physics
    const f32 cameraHeightStanding = 2.2f;
    const f32 characterHeightStanding = 1.5f;
    const f32 characterRadiusStanding = 1.8f;

    Ref<Shape> standingShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * characterHeightStanding + characterRadiusStanding, 0), Quat::sIdentity(), new CapsuleShape(0.5f * characterHeightStanding, characterRadiusStanding)).Create().Get();

    Body* body;
    MoveHelper moveHelper;

    BodyID lastGroundBody;
    Vec3 lastGroundPosition;

    // Interpolation
    InterpState<Vec3> positionState;
    InterpState<Vec3> velocityState;

    // Effects
    f32 landTime;
    s8 landDirection = 1;
    f32 landPower;

    // Sounds
    game::Sound landSound = game::Sound("jumpland");
    game::Sound stepSound = game::Sound("step", 8, 4);
    game::Sound ladderSound = game::Sound("ladder", 8, 4);

    Vec3 lastStepPosition;

    void Load();

    void Kill();
    void Spawn(Vec3 atPos = Vec3(0, 5, 0));

    void Tick() override;
    void BeforeCamera() override;
private:
    void TraceToLadder(Ref<const Shape> shape, BodyFilter& filter);
    void CheckForLadder(Ref<const Shape> shape, BodyFilter& filter);
};