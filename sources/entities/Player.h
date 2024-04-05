#pragma once

#include <game.h>
#include <tween.h>
#include <flavor.h>
#include <globals.h>
#include <physics.h>
#include <interpolation.h>
#include <tween.h>

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

const f32 FOV_DEFAULT = 85.0f;
const f32 FOV_ZOOMED = 30.0f;

using namespace JPH;

class Player : public Entity {
public:

    // States
    bool IsPlayerAlive;

    f32 lookAngleX;
    f32 lookAngleY;

    bool isGrounded;

    bool movementLocked;
    bool lookLocked;

    BodyID holdProp;
    BodyID currentLadder;
    Vec3 ladderNormal;

    // Camera
    Mat44 cameraMatrix;
    SpringState<f32> fov;

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

    game::Sound deathSound = game::Sound("bodysplat");
    game::Sound spawnSound = game::Sound("sneak_spawn");

    Vec3 lastStepPosition;

    void Load();

    void Kill();
    void Spawn();

    void Tick() override;
    void BeforeCamera() override;

    void AfterCamera() override;

    Vec3 spawnPoint = Vec3(0, 5, 0);
private:
    f32 deathTime;
    raylib::Rectangle deathRectangle;
    game::UIObject<raylib::Text> deathLabel1 = game::UIObject<raylib::Text>(game::UDim2(0.5, 0, 0.35, 0), game::UDim2(0, 0, 0.03, 0), Vector2{ 0.5f, 1.0f }, "UNABLE TO MEASURE VITAL SIGNS");
    game::UIObject<raylib::Text> deathLabel2 = game::UIObject<raylib::Text>(game::UDim2(0.5, 0, 0.45, 0), game::UDim2(0, 0, 0.03, 0), Vector2{ 0.5f, 1.0f }, "RECOMBOBULATING...");

    f32 spawnTime;

    void TraceToLadder(Ref<const Shape> shape, BodyFilter& filter);
    void CheckForLadder(Ref<const Shape> shape, BodyFilter& filter);
};