#pragma once

#include <game.h>
#include <flavor.h>
#include <globals.h>

#include <entities/Player.h>

#include <random>

class MatterMan : public Entity {
private:
    SpringState<Vector<2>> sway;
    Quat lastCameraRotation; // Used for viewmodel sway
    f32 lastAngleY;
public:
    Player* player;
    raylib::Model* viewmodel;

    // Holding mechanics
    BodyID holdBody;
    Mat44 holdMatrixRelative; // Matrix of the hold body relative to the camera
    Mat44 holdPointRelative; // Point THE BODY is being held from, relative to the body
    Mat44 holdPointWorld;

    void Spawn(Player* plr);
    void Tick() override;
    void Render() override;
};