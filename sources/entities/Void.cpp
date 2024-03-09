#include <entities/Void.h>

void Void::HandlePlayer(Player* player) const {
    if (player->moveHelper.position.GetY() < height && player->IsPlayerAlive) {
        player->Kill();
    }
}

void Void::Tick() {
    for (auto& bucket : entities.buckets) {
        for (Entity* entity : bucket->data) {
            Player* player = dynamic_cast<Player*>(entity);

            if (player) {
                HandlePlayer(player);
            }
        }
    }
}