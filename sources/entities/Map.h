#pragma once

#include <raylib-cpp.hpp>
#include <engine/entity.h>
#include <globals.h>

class Map : public Entity {
public:
    raylib::Model model;
    BodyID bodyId;

    void Load(const char* path);
    void Tick() override;
    void Render() override;
};