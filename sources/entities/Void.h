#pragma once

#include <game.h>

#include <entities/Player.h>
#include <entities/Prop.h>

// Void entity that checks if a player or a prop has fallen beneath the provided height value every tick
// If a player falls into the void, they die and props load their last saved states
// If a prop falls into the void, they will reset their state
class Void : public Entity {
public:
    f32 height;

    void Tick() override;
private:
    void HandlePlayer(Player* player) const;
};