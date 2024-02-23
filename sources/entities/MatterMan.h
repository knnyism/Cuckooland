#pragma once

#include <game.h>
#include <flavor.h>

class MatterMan : public Entity {
public:
    void Spawn();
    void Destroy() override;
    void Tick() override;
    void Render() override;
};