#pragma once 

#include <game.h>

constexpr f32 SKYBOX_SCALE = 500.0f;

class Skybox : public Entity {
public:
    void Load() {
        model = new raylib::Model(GetAssetPath("models/skybox.glb"));
    }

    void Render() override {
        model->Draw(camera->position, SKYBOX_SCALE, { 200, 249, 180, 225 });
    }
private:
    raylib::Model* model;
};