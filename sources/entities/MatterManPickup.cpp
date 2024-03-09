#include <entities/MatterManPickup.h>

void MatterManPickup::Spawn(Vec3 position) {
    prop = CreateEntity<Prop>();
    prop->Load("matterman", position, Quat::sRotation(Vec3::sAxisY(), -PI / 2), 1000.0f);

    GetModel("v_matman"); // Load the viewmodel so later on MatterMan entity doesn't have to load it when a player picks us up
}

void MatterManPickup::Destroy() {
    DestroyEntity(prop);
}

void MatterManPickup::Tick() {
    for (auto& bucket : entities.buckets) {
        for (Entity* entity : bucket->data) {
            Player* player = dynamic_cast<Player*>(entity);

            if (player) {
                f32 distance = (player->moveHelper.position - body_interface->GetPosition(prop->model.bodyId)).Length();

                if (distance < 5.0f) {
                    CreateEntity<MatterMan>()->Spawn(player);
                    DestroyEntity(this);

                    break;
                }
            }
        }
    }
}

void MatterManPickup::Render() {}
