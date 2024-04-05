#include <entities/Void.h>

void Void::HandlePlayer(Player* player) const {
    if (player->moveHelper.position.GetY() < height && player->IsPlayerAlive) {
        player->Kill();

        for (auto& bucket : entities.buckets) {
            for (Entity* entity : bucket->data) {
                if (!entity->IsAlive)
                    continue;

                Prop* prop = dynamic_cast<Prop*>(entity);

                if (prop) {
                    prop->LoadSavedState();
                }
            }
        }
    }
}

void Void::Tick() {
    for (auto& bucket : entities.buckets) {
        for (Entity* entity : bucket->data) {
            if (!entity->IsAlive)
                continue;

            Player* player = dynamic_cast<Player*>(entity);
            Prop* prop = dynamic_cast<Prop*>(entity);

            if (player) {
                HandlePlayer(player);
            }
            else if (prop) {
                if (body_interface->GetPosition(prop->model.bodyId).GetY() < height)
                    prop->ResetState();
            }
        }
    }
}