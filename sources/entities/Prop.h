#pragma once

#include <raylib-cpp.hpp>
#include <physics.h>
#include <entity.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>

#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <iostream>

class Prop : public Entity {
public:
    raylib::Model model;
    JPH::BodyID bodyId;

    Vector3 position;
    Quaternion rotation;

    void Load(const char* path);
    void Tick() override;
    void Render() override;
private:
    Vector3 raylib_position;
    Vector3 raylib_rotationAxis;
    float raylib_rotationAngle;
};