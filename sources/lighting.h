/*
* lighting.h
*
* To make the game look less eyesore, I wanted to add some lighting.
* Even though the project will not be assessed on the graphics, I wanted to learn
* to use GLSL shaders and how to communicate with them.
*
* The PBR shader used can be found here:
* https://github.com/Not-Nik/rlpbr
*
*/

#pragma once

#include <globals.h>
#include <raylib-cpp.hpp>

enum LightType {
    LIGHT_POINT = 1,
    LIGHT_SPOT = 2,
    LIGHT_SUN = 3
};

struct Light {
    int enabled;

    LightType type;

    raylib::Vector3 pos;
    raylib::Vector3 target;
    raylib::Color color;

    float intensity;

    // Shader light parameters locations
    int typeLoc;
    int enabledLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;
    int intensityLoc;
};

void LoadLighting();
void UpdateLighting();
Light CreateLight(LightType type, Vector3 position, Vector3 target, raylib::Color color, float intensity);
Material LoadPbrMaterial(Texture albedo, Texture normal, Texture roughness);
