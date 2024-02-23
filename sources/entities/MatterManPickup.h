#pragma once

#include <game.h>
#include <flavor.h>
#include <physics.h>

#include <entities/MatterMan.h>
#include <entities/Player.h>
#include <entities/Prop.h>

// Pickup entity for the matter manipulator
// When the player picks this up, they can use the matter manipulator and this entity will destroy itself
class MatterManPickup : public Entity {
public:
    Prop* prop;

    void Spawn(Vec3 position);
    void Tick() override;
    void Render() override;
    void Destroy() override;
};