#pragma once

#include <game.h>

#include <entities/Map.h>
#include <entities/Skybox.h>
#include <entities/Door.h>
#include <entities/Prop.h>
#include <entities/Elevator.h>
#include <entities/Player.h>
#include <entities/Trigger.h>
#include <entities/Checkpoint.h>

class TestLevel : public Entity {
public:
    void Load() {
        CreateEntity<Skybox>()->Load();

        Map* map = CreateEntity<Map>();
        map->Load("testmap");

        Player* player = CreateEntity<Player>();
        player->Load();
        player->Spawn();

        Hud* hud = CreateEntity<Hud>();

        Intro* intro = CreateEntity<Intro>();
        intro->Play(player, Vec3(-2.5, 2, -2.5));

        player->lookLocked = true;
        player->movementLocked = true;

        CreateEntity<Elevator>()->Load(Vec3(-10.0f, 15.0f, 0.0f), Quat::sIdentity());
        CreateEntity<MatterMan>()->Spawn(player);

        Void* killVoid = CreateEntity<Void>();
        killVoid->height = -130.0f;

        CreateEntity<Checkpoint>()->Load(Vec3(10, 5, 0), Vec3::sReplicate(1.0f));

        for (int i = 0; i < 10; i++)
            CreateEntity<Prop>()->Load("cube", Vec3(-10.0f, 1.95f, i * 2.0f), Quat::sIdentity(), 5000.0f);

        CreateEntity<Prop>()->Load("table", Vec3(-6.813f, 4.313f, 6.063f), Quat::sIdentity());

        CreateLight(LIGHT_POINT, { -7.25f, 4.5f, -7.25f }, { }, { 255, 0, 0 }, 2.0f);
        CreateLight(LIGHT_POINT, { -7.25f, 4.5f, -5.25f }, { }, { 0, 255, 0 }, 2.0f);
        CreateLight(LIGHT_POINT, { -5.75f, 4.5f, -5.25f }, { }, { 0, 0, 255 }, 2.0f);

        CreateEntity<Prop>()->Load("ladder", Vec3(-3.0f, 20.25f, 15.25f), Quat::sEulerAngles(Vec3(90 * RAD2DEG, 0, 0)), 1000000.0f, 1.0f, Layers::LADDER);

        Trigger* endTrigger = CreateEntity<Trigger>();
        endTrigger->Load(new BodyCreationSettings(new BoxShape(Vec3::sReplicate(1.0f)), Vec3(0, 5, -15), Quat::sIdentity(), EMotionType::Dynamic, Layers::TRIGGER),
            [hud, player, endTrigger](BodyID bodyId) {
                if (bodyId == player->body->GetID()) {
                    player->movementLocked = true;

                    hud->ShowEndGameScreen();
                    DestroyEntity(endTrigger);
                }
            }
        );

        CreateEntity<Cuckoo>()->Load(Vec3(3.539f, 0.012f, 20.114f), Quat::sEulerAngles(Vec3(0.0f, 180.0f, 0.0f) * DEG2RAD), true);

        for (int i = 0; i < 10; i++)
            CreateEntity<Cuckoo>()->Load(Vec3(i * 2.539f, 2.012f, 24.114f), Quat::sEulerAngles(Vec3(0.0f, 180.0f, 0.0f) * DEG2RAD), false);
    }
};