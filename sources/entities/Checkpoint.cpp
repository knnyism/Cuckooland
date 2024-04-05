#include <entities/Checkpoint.h>

static void SaveProps() {
    for (auto& bucket : entities.buckets) {
        for (Entity* entity : bucket->data) {
            if (!entity->IsAlive)
                continue;

            Prop* prop = dynamic_cast<Prop*>(entity);

            if (prop) {
                prop->SaveState();
            }

            Hud* hud = dynamic_cast<Hud*>(entity);

            if (hud)
                hud->ShowSaveIcon();
        }
    }
}

void Checkpoint::Destroy() {
    DestroyEntity(trigger);
}

void Checkpoint::OnTouch(BodyID bodyId) {
    for (auto& bucket : entities.buckets) {
        for (Entity* entity : bucket->data) {
            if (!entity->IsAlive)
                continue;

            Player* player = dynamic_cast<Player*>(entity);

            if (!player) {
                continue;
            }

            if (bodyId == player->body->GetID()) {
                player->spawnPoint = player->moveHelper.position;

                if (playSound)
                    reachSound.Play();

                SaveProps();
                DestroyEntity(this);
                break;
            }
        }
    }
}

void Checkpoint::Load(Vec3 position, Vec3 size, bool playSound) {
    this->playSound = playSound;
    trigger = CreateEntity<Trigger>();
    trigger->Load(new BodyCreationSettings(new BoxShape(size / 2.0f), position, Quat::sIdentity(), EMotionType::Static, Layers::TRIGGER), [this](BodyID bodyId) { OnTouch(bodyId); });
}