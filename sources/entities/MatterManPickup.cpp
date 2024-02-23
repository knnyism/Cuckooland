#include <entities/MatterManPickup.h>

void MatterManPickup::Spawn(Vec3 position) {
    prop = CreateEntity<Prop>();
    prop->Load("matterman", position, Quat::sEulerAngles(Vec3(0, -PI / 2, -PI / 2)), 100000.0f);
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
                    CreateEntity<MatterMan>();
                    DestroyEntity(this);

                    break;
                }
            }
        }
    }
}

void MatterManPickup::Render() {}
