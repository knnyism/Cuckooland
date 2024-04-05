#pragma once

#include <game.h>
#include <flavor.h>
#include <globals.h>

#include <entities/Player.h>

#include <random>

enum CrosshairState {
    NEUTRAL = 1,
    AVAILABLE,
    BLOCKED,
    HOLDING
};

class MatterMan : public Entity {
private:
    SpringState<Vector<2>> sway;
    Quat lastCameraRotation; // Used for viewmodel sway
    f32 lastAngleY;
public:
    Player* player;
    raylib::Model* viewmodel = GetModel("v_matman");

    // Holding mechanics
    BodyID holdBody; // The BodyID of the body that is currently being held
    Mat44 holdMatrixRelative; // Matrix of the hold body relative to the camera
    Mat44 holdPointRelative; // Point THE BODY is being held from, relative to the body

    CrosshairState crosshairState;

    raylib::Texture crosshairNeutral = raylib::Texture(GetAssetPath("textures/crosshair_neutral.png"));
    raylib::Texture crosshairAvailable = raylib::Texture(GetAssetPath("textures/crosshair_available.png"));
    raylib::Texture crosshairBlocked = raylib::Texture(GetAssetPath("textures/crosshair_blocked.png"));
    raylib::Texture crosshairHolding = raylib::Texture(GetAssetPath("textures/crosshair_holding.png"));

    void Spawn(Player* plr);
    void Tick() override;
    void Render() override;
    void AfterCamera() override;
};