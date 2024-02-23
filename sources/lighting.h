#pragma once

#include <globals.h>

#include <raylib-cpp.hpp>

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            120
#endif

#define MAX_LIGHTS  4           // Max dynamic lights supported by shader

enum LightType {
    LIGHT_DIRECTIONAL = 0,
    LIGHT_POINT,
    LIGHT_SPOT
};

struct Light {
    int type;
    int enabled;
    Vector3 position;
    Vector3 target;
    float color[4];
    float intensity;

    // Shader light parameters locations
    int typeLoc;
    int enabledLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;
    int intensityLoc;
};

inline int lightsCount = 0;

inline raylib::Shader* shader;

void LoadLighting();
Light CreateLight(int type, Vector3 position, Vector3 target, raylib::Color color, float intensity);