/*
* Level entity that loads the main level
*/

#pragma once

#include <game.h>

#include <entities/Map.h>
#include <entities/Door.h>
#include <entities/Prop.h>
#include <entities/SlidingDoor.h>
#include <entities/Skybox.h>
#include <entities/MatterMan.h>
#include <entities/Elevator.h>
#include <entities/Void.h>
#include <entities/Cuckoo.h>
#include <entities/Timer.h>
#include <entities/Barrier.h>
#include <entities/Intro.h>
#include <entities/SoundPlayer.h>
#include <entities/Checkpoint.h>

static void CreateAmbienceTrigger(Player* player, string soundPath, Vec3 position, Vec3 size) {
    SoundPlayer* ambSound = CreateEntity<SoundPlayer>();
    game::Sound3D* sound = new game::Sound3D(soundPath, position, 1, 1, true);
    sound->maxDistance = 20.0f;

    ambSound->Load(sound, false);

    Trigger* ambTrigger = CreateEntity<Trigger>();
    ambTrigger->Load(new BodyCreationSettings(new BoxShape(size / 2), position, Quat::sIdentity(), EMotionType::Static, Layers::TRIGGER),
        [player, ambSound](BodyID bodyId) {
            if (bodyId == player->body->GetID()) {
                ambSound->sound->position = game::GetCameraMatrix().GetTranslation();
            }
        });
    ambSound->Play();
}

class MainLevel : public Entity {
public:
    void Load() {
        MatterMan* matterMan = CreateEntity<MatterMan>();

        CreateLight(LIGHT_SUN, { -10.0f, -10.0f, -10.0f }, { 10.0f, 10.0f, 10.0f }, { 200, 249, 120 }, 0.01f);

        CreateLight(LIGHT_SPOT, { -17.0f, 9.934f, -2.0f }, { -17.0f, 0.0f, -2.0f }, { 200, 249, 120 }, 5.0f);
        CreateLight(LIGHT_SPOT, { -17.0f, 9.934f, 14.0f }, { -15.0f, 0.0f, 14.0f }, { 200, 249, 120 }, 5.0f);
        CreateLight(LIGHT_SPOT, { -17.0f, 9.934f, 30.0f }, { -15.0f, 0.0f, 30.0f }, { 200, 249, 120 }, 5.0f);

        CreateLight(LIGHT_SPOT, { -17.0f, 7.934f, -2.0f }, { -17.0f, 9.0f, -2.0f }, { 200, 249, 120 }, 5.0f);
        CreateLight(LIGHT_SPOT, { -17.0f, 7.934f, 14.0f }, { -17.0f, 9.0f, 14.0f }, { 200, 249, 120 }, 5.0f);
        CreateLight(LIGHT_SPOT, { -17.0f, 7.934f, 30.0f }, { -17.0f, 9.0f, 30.0f }, { 200, 249, 120 }, 5.0f);

        CreateLight(LIGHT_SPOT, { -17.0f, 9.934f, 14.0f }, { -19.0f, 0.0f, 14.0f }, { 200, 249, 120 }, 5.0f);
        CreateLight(LIGHT_SPOT, { -17.0f, 9.934f, 30.0f }, { -19.0f, 0.0f, 30.0f }, { 200, 249, 120 }, 5.0f);
        CreateLight(LIGHT_SPOT, { -17.0f, 9.934f, 30.0f }, { -17.0f, 0.0f, 32.0f }, { 200, 249, 120 }, 5.0f);

        CreateLight(LIGHT_SPOT, { -23.0f, 22.934f, 49.0f }, { -23.0f, 20.934f, 49.0f }, { 200, 249, 120 }, 20.0f);
        CreateLight(LIGHT_SPOT, { -12.0f, 22.934f, 49.0f }, { -12.0f, 20.934f, 49.0f }, { 200, 249, 120 }, 20.0f);
        CreateLight(LIGHT_SPOT, { -1.0f, 22.934f, 49.0f }, { -1.0f, 20.934f, 49.0f }, { 200, 249, 120 }, 20.0f);

        CreateLight(LIGHT_SPOT, { -6.0f, 12.434f, 103.5f }, { -6.0f, 10.434f, 103.5f }, { 200, 249, 120 }, 40.0f);
        CreateLight(LIGHT_SPOT, { -9.0f, 12.434f, 103.5f }, { -9.0f, 10.434f, 103.5f }, { 200, 249, 120 }, 40.0f);
        CreateLight(LIGHT_SPOT, { -24.0f, 12.434f, 103.5f }, { -24.0f, 10.434f, 103.5f }, { 200, 249, 120 }, 40.0f);

        CreateLight(LIGHT_SPOT, { 20.0f, 9.934f, 103.5f }, { 20.0f, 4.934f, 103.5f }, { 200, 249, 120 }, 40.0f);
        CreateLight(LIGHT_SPOT, { 35.0f, 9.934f, 103.5f }, { 35.0f, 4.934f, 103.5f }, { 200, 249, 120 }, 40.0f);
        CreateLight(LIGHT_SPOT, { 52.0f, 9.934f, 103.5f }, { 52.0f, 4.934f, 103.5f }, { 200, 249, 120 }, 40.0f);

        CreateEntity<Skybox>()->Load();

        Player* player = CreateEntity<Player>();
        player->Load();
        player->Spawn();

        Intro* intro = CreateEntity<Intro>();
        intro->Play(player, Vec3(-2.5, 2, -2.5));

        player->lookLocked = true;
        player->movementLocked = true;

        Hud* hud = CreateEntity<Hud>();

        Map* map = CreateEntity<Map>();
        map->Load("map");

        Void* killVoid = CreateEntity<Void>();
        killVoid->height = -130.0f;

        // Spawn
        {
            CreateAmbienceTrigger(player, "roomtone2", Vec3(-5.875, 5, 14), Vec3(33.75, 10, 48.5));

            SlidingDoor* spawnDoor = CreateEntity<SlidingDoor>();
            spawnDoor->Spawn(Vec3(-10.5f, 3.582f, 0), Vec3(-10.5f, 9.582f, 0));
            spawnDoor->automatic = false;

            Timer* spawnDoorTimer = CreateEntity<Timer>();

            spawnDoorTimer->AddEvent(5.0f,
                [hud]() {
                    hud->SetSubtitle("wake_up");
                }
            );

            spawnDoorTimer->AddEvent(12.0f,
                [hud]() {
                    hud->SetSubtitle("glad_wokeup");
                }
            );

            spawnDoorTimer->AddEvent(17.0f,
                [hud]() {
                    hud->SetSubtitle("time_to_test");
                }
            );

            spawnDoorTimer->AddEvent(24.0f,
                [hud]() {
                    hud->SetSubtitle("will_you");
                }
            );

            Prop* spawnBellProp = CreateEntity<Prop>();
            spawnBellProp->Load("bell", Vec3(-9.767, 9.048, 0), Quat::sEulerAngles(Vec3(0, -90, 0) * DEG2RAD), 3000.0f, 0.5f, {}, {}, EMotionType::Static);

            spawnDoorTimer->AddEvent(26.0f,
                [hud, spawnBellProp]() {
                    SoundPlayer* bellSound = CreateEntity<SoundPlayer>();
                    bellSound->Load(new game::Sound3D("warningbell", Vec3(-9.767, 9.048, 0), 1, 1), true);
                    bellSound->Play();
                }
            );

            spawnDoorTimer->AddEvent(27.0f,
                [spawnDoor, player, spawnDoorTimer]() {
                    spawnDoor->SetState(true);

                    Trigger* trigger = CreateEntity<Trigger>();
                    trigger->Load(new BodyCreationSettings(new BoxShape(Vec3(5, 4, 1)), Vec3(-15.5, 4, 15), Quat::sIdentity(), EMotionType::Static, Layers::TRIGGER),
                        [spawnDoor, player, trigger](BodyID bodyId) {
                            if (bodyId == player->body->GetID()) {
                                spawnDoor->SetState(false);
                                DestroyEntity(trigger);
                            }
                        });

                    DestroyEntity(spawnDoorTimer);
                }
            );
            spawnDoorTimer->SetActive(true);
        }

        // Hallway
        {
            CreateEntity<SlidingDoor>()->Spawn(Vec3(-17.0f, 3.582f, 38.5f), Vec3(-17.0f, 9.582f, 38.5f), Quat::sEulerAngles(Vec3(0, PI / 2, 0)));
        }

        // MatterMan room
        {
            CreateEntity<Checkpoint>()->Load(Vec3(-17, 3.376, 41.25), Vec3(4, 6.75, 4), false);
            CreateAmbienceTrigger(player, "lab_loop1", Vec3(-12, 11.75, 49), Vec3(34, 23.5, 20));

            SlidingDoor* brokenDoor = CreateEntity<SlidingDoor>();
            brokenDoor->Spawn(Vec3(-3.0f, 3.582f, 59.5f), Vec3(-3.0f, 4.582f, 59.5f), Quat::sEulerAngles(Vec3(0, PI / 2, 0)));
            brokenDoor->automatic = false;

            CreateEntity<Prop>()->Load("table", Vec3(-17.063f, 1.688f, 56.813f), Quat::sIdentity(), 3000.0f);

            CreateEntity<Door>()->Load("fire_door", Vec3(-29.5f, 3.35f, 47.5f), Quat::sIdentity(), 30.0f);

            CreateEntity<Prop>()->Load("cardboard_box1", Vec3(1.339f, 4.143f, 41.718f), Quat::sIdentity(), 3000.0f);
            CreateEntity<Prop>()->Load("cardboard_box2", Vec3(2.923f, 4.5f, 44.729f), Quat::sIdentity(), 3000.0f);
            CreateEntity<Prop>()->Load("cardboard_box3", Vec3(0.5f, 1.5f, 41.5f), Quat::sIdentity(), 3000.0f);
            CreateEntity<Prop>()->Load("cardboard_box4", Vec3(3.5f, 1.5f, 45.5f), Quat::sIdentity(), 3000.0f);

            Barrier* fireDoorBarrier = CreateEntity<Barrier>();
            fireDoorBarrier->Load(Vec3(-28, 4, 47.5), Quat::sIdentity(), Vec3(1, 6, 4));

            Barrier* boxesBarrier = CreateEntity<Barrier>();
            boxesBarrier->Load(Vec3(-2, 4, 43.25), Quat::sIdentity(), Vec3(1, 8, 8.5));

            Barrier* boxesBarrier2 = CreateEntity<Barrier>();
            boxesBarrier2->Load(Vec3(1.25, 4, 48.25), Quat::sIdentity(), Vec3(7.5, 8, 1.5));

            Timer* mattermanTimer = CreateEntity<Timer>();

            mattermanTimer->AddEvent(1.0f,
                [hud]() {
                    hud->SetSubtitle("this_is");
                }
            );

            mattermanTimer->AddEvent(6.0f,
                [hud]() {
                    hud->SetSubtitle("ignore_fancy");
                }
            );

            mattermanTimer->AddEvent(10.0f,
                [hud, boxesBarrier, boxesBarrier2, mattermanTimer]() {
                    hud->SetSubtitle("practice");
                    DestroyEntity(boxesBarrier);
                    DestroyEntity(boxesBarrier2);
                    DestroyEntity(mattermanTimer);
                }
            );

            Prop* mattermanPickup = CreateEntity<Prop>();
            mattermanPickup->Load("matterman", Vec3(-17.063f, 4.075f, 55.125f), Quat::sRotation(Vec3::sAxisY(), -PI / 2), 1000.0f);
            mattermanPickup->SetTriggerFunc(
                [player, mattermanPickup, mattermanTimer, matterMan](BodyID bodyId) {
                    if (bodyId == player->body->GetID()) {
                        matterMan->Spawn(player);
                        DestroyEntity(mattermanPickup);

                        mattermanTimer->SetActive(true);
                    }
                });

            Timer* cuckooTimer = CreateEntity<Timer>();

            cuckooTimer->AddEvent(2.0f,
                [hud]() {
                    hud->showCuckoos = true;
                    hud->SetSubtitle("whats_that");
                }
            );

            cuckooTimer->AddEvent(7.5f,
                [hud]() {
                    hud->SetSubtitle("theyre_everywhere");
                }
            );

            cuckooTimer->AddEvent(11.0f,
                [hud]() {
                    hud->SetSubtitle("goto_door");
                }
            );

            Prop* bellProp = CreateEntity<Prop>();
            bellProp->Load("bell", Vec3(-3, 9, 58.75), Quat::sIdentity(), 3000.0f, 0.5f, {}, {}, EMotionType::Static);

            cuckooTimer->AddEvent(13.0f,
                [hud]() {
                    SoundPlayer* bellSound = CreateEntity<SoundPlayer>();
                    bellSound->Load(new game::Sound3D("warningbell", Vec3(-3, 9, 58.75), 1, 1), true);
                    bellSound->Play();
                }
            );

            Trigger* brokenDoorTrigger = CreateEntity<Trigger>();
            brokenDoorTrigger->IsActive = false;

            Timer* brokenDoorTimer = CreateEntity<Timer>();

            brokenDoorTimer->AddEvent(3.0f,
                [hud, fireDoorBarrier, brokenDoorTimer]() {
                    hud->SetSubtitle("door_stuck");

                    DestroyEntity(fireDoorBarrier);
                    DestroyEntity(brokenDoorTimer);
                }
            );

            brokenDoorTrigger->Load(new BodyCreationSettings(new BoxShape(Vec3(2.5f, 4.0f, 3.0f)), Vec3(-3.0f, 4.0f, 55.5f), Quat::sIdentity(), EMotionType::Static, Layers::TRIGGER),
                [brokenDoor, player, brokenDoorTrigger, brokenDoorTimer](BodyID bodyId) {
                    if (bodyId == player->body->GetID()) {
                        brokenDoor->SetState(true);
                        brokenDoorTimer->SetActive(true);
                        DestroyEntity(brokenDoorTrigger);
                    }
                });

            cuckooTimer->AddEvent(14.0f,
                [cuckooTimer, brokenDoorTrigger]() {
                    brokenDoorTrigger->IsActive = true;
                    DestroyEntity(cuckooTimer);
                }
            );

            Cuckoo* cuckoo = CreateEntity<Cuckoo>();
            cuckoo->Load(Vec3(3.539f, 0.012f, 41.114f), Quat::sEulerAngles(Vec3(0.0f, 180.0f, 0.0f) * DEG2RAD), true);
            cuckoo->SetTriggerFunc(
                [cuckooTimer, player](BodyID bodyId) {
                    if (bodyId == player->body->GetID()) {
                        cuckooTimer->SetActive(true);
                    }
                }
            );
        }

        // Outside
        {
            CreateEntity<Checkpoint>()->Load(Vec3(-33, -7, 64), Vec3(4, 6, 4));
            Trigger* checkpointTrigger = CreateEntity<Trigger>();
            checkpointTrigger->Load(new BodyCreationSettings(new BoxShape(Vec3(2, 3, 2)), Vec3(-33, -7, 64), Quat::sIdentity(), EMotionType::Static, Layers::TRIGGER),
                [player, checkpointTrigger, hud](BodyID bodyId) {
                    if (bodyId == player->body->GetID()) {
                        hud->SetSubtitle("checkpoint");
                        DestroyEntity(checkpointTrigger);
                    }
                });

            CreateAmbienceTrigger(player, "amb_outside", Vec3(-30, -11.875, 73.5), Vec3(40.5, 19.25, 69));

            CreateEntity<Cuckoo>()->Load(Vec3(-17.499f, -9.988f, 42.114f), Quat::sRotation(Vec3::sAxisY(), PI));

            CreateEntity<Prop>()->Load("plank", Vec3(-21.197f, -9.849f, 47.998f), Quat::sEulerAngles(Vec3(0, 1, 0)* DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
            CreateEntity<Prop>()->Load("plank", Vec3(-21.197f, -8.849f, 47.998f), Quat::sEulerAngles(Vec3(0, -1, 0)* DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
            CreateEntity<Prop>()->Load("plank", Vec3(-21.197f, -7.849f, 47.998f), Quat::sEulerAngles(Vec3(0, 0, 0)* DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
            CreateEntity<Prop>()->Load("plank", Vec3(-21.197f, -6.849f, 47.998f), Quat::sEulerAngles(Vec3(0, 1, 0)* DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
            CreateEntity<Prop>()->Load("plank", Vec3(-21.197f, -5.849f, 47.998f), Quat::sEulerAngles(Vec3(0, -1, 0)* DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);

            CreateEntity<Prop>()->Load("plank", Vec3(-13.7f, -9.849f, 47.998f), Quat::sEulerAngles(Vec3(0, 1, 0)* DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
            CreateEntity<Prop>()->Load("plank", Vec3(-13.7f, -8.849f, 47.998f), Quat::sEulerAngles(Vec3(0, -1, 0)* DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
            CreateEntity<Prop>()->Load("plank", Vec3(-13.7f, -7.849f, 47.998f), Quat::sEulerAngles(Vec3(0, 0, 0)* DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
            CreateEntity<Prop>()->Load("plank", Vec3(-13.7f, -6.849f, 47.998f), Quat::sEulerAngles(Vec3(0, 2, 0)* DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
            CreateEntity<Prop>()->Load("plank", Vec3(-13.7f, -5.849f, 47.998f), Quat::sEulerAngles(Vec3(0, -2, 0)* DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);

            CreateEntity<Prop>()->Load("ladder", Vec3(-3.0f, 15.25f, 70.25f), Quat::sEulerAngles(Vec3(0, 90, 0)* DEG2RAD), 1000000.0f, 1.0f, Layers::LADDER);
            CreateEntity<Cuckoo>()->Load(Vec3(0.386f, 11.012f, 84.498f), Quat::sEulerAngles(Vec3(0.0f, 135.0f, 0.0f)* DEG2RAD));

            CreateEntity<Cuckoo>()->Load(Vec3(-33.001f, -9.988f, 103.386f), Quat::sIdentity());
        }

        // Puzzle 1
        {
            CreateEntity<Checkpoint>()->Load(Vec3(-27, -7, 103.5), Vec3(4, 6, 4));
            CreateAmbienceTrigger(player, "garage", Vec3(21.25, -1.875, 109.625), Vec3(99.5, 39.25, 43.75));

            CreateEntity<Door>()->Load("fire_door", Vec3(-29.5f, -6.65f, 103.5f), Quat::sIdentity(), 30.0f);

            CreateEntity<Prop>()->Load("table", Vec3(-26.813f, -8.313f, 95.063f), Quat::sEulerAngles(Vec3(0, 90, 0) * DEG2RAD), 2000.0f);
            CreateEntity<Prop>()->Load("chair", Vec3(-22.958f, -8.425f, 92.529f), Quat::sEulerAngles(Vec3(0, -52.472, 0) * DEG2RAD), 1000.0f);

            CreateEntity<Prop>()->Load("barrel", Vec3(8.388f, -8.095f, 89.5f), Quat::sIdentity(), 3000.0f);
            CreateEntity<Prop>()->Load("barrel", Vec3(9.069f, -8.095f, 92.255f), Quat::sIdentity(), 3000.0f);
            CreateEntity<Prop>()->Load("barrel", Vec3(9.261f, -4.289f, 90.703f), Quat::sIdentity(), 3000.0f);

            CreateEntity<Prop>()->Load("cardboard_box1", Vec3(6.052f, -5.857f, 114.218f), Quat::sIdentity(), 3000.0f);
            CreateEntity<Prop>()->Load("cardboard_box2", Vec3(6.714f, -8.5f, 114.192f), Quat::sIdentity(), 3000.0f);
            CreateEntity<Prop>()->Load("cardboard_box3", Vec3(7.214f, -8.5f, 117.5f), Quat::sIdentity(), 3000.0f);
            CreateEntity<Prop>()->Load("cardboard_box4", Vec3(9.357f, -8.661f, 114.782f), Quat::sIdentity(), 3000.0f);

            CreateEntity<Prop>()->Load("dryer", Vec3(-23.508f, 5.5f, 108.911f), Quat::sEulerAngles(Vec3(0, 17.743f, 0) * DEG2RAD), 20000.0f);

            CreateEntity<Elevator>()->Load(Vec3(0.0f, -9.0f, 103.5f), Quat::sIdentity());

            CreateEntity<Cuckoo>()->Load(Vec3(1.795f, -24.0f, 108.89f), Quat::sIdentity());
            CreateEntity<Cuckoo>()->Load(Vec3(1.886f, 1.0f, 103.501f), Quat::sRotation(Vec3::sAxisY(), PI / 2));

            Timer* puzzle1Timer = CreateEntity<Timer>();

            puzzle1Timer->AddEvent(2.0f,
                [hud]() {
                    hud->SetSubtitle("pulley");
                });


            puzzle1Timer->AddEvent(7.0f,
                [hud, puzzle1Timer]() {
                    hud->SetSubtitle("shaft");
                    DestroyEntity(puzzle1Timer);
                });

            Trigger* puzzle1Trigger = CreateEntity<Trigger>();
            puzzle1Trigger->Load(new BodyCreationSettings(new BoxShape(Vec3(4.75f, 10.0f, 7.0f) / 2.0f), Vec3(-26.375f, -5.0f, 104.0f), Quat::sIdentity(), EMotionType::Static, Layers::TRIGGER),
                [player, puzzle1Trigger, puzzle1Timer](BodyID bodyId) {
                    if (bodyId == player->body->GetID()) {
                        puzzle1Timer->SetActive(true);
                        DestroyEntity(puzzle1Trigger);
                    }
                });


            Trigger* puzzle1EndTrigger = CreateEntity<Trigger>();
            puzzle1EndTrigger->Load(new BodyCreationSettings(new BoxShape(Vec3(11.75f, 10.0f, 8.0f) / 2.0f), Vec3(5.125f, 5.0f, 103.5f), Quat::sIdentity(), EMotionType::Static, Layers::TRIGGER),
                [player, puzzle1EndTrigger, hud](BodyID bodyId) {
                    if (bodyId == player->body->GetID()) {
                        hud->SetSubtitle("nice");
                        DestroyEntity(puzzle1EndTrigger);
                    }
                });
        }

        // Puzzle 2
        {
            CreateEntity<SlidingDoor>()->Spawn(Vec3(11.5f, 3.582f, 103.5f), Vec3(11.5f, 9.582f, 103.5f));
            CreateEntity<Door>()->Load("fire_door", Vec3(41.5f, 3.35f, 101.4f), Quat::sIdentity(), 30.0f, 0.0f);
            CreateEntity<Door>()->Load("fire_door", Vec3(41.5f, 3.35f, 105.6f), Quat::sEulerAngles(Vec3(0, -180, 0) * DEG2RAD), 30.0f, -90.0f, 0.0f);
            CreateEntity<Prop>()->Load("plank2", Vec3(42.25f, 3.55f, 103.45f), Quat::sIdentity(), 600000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);

            CreateEntity<Cuckoo>()->Load(Vec3(37.668f, 1.5f, 113.712f), Quat::sIdentity());
            CreateEntity<Cuckoo>()->Load(Vec3(49.886f, 1.5f, 103.501f), Quat::sRotation(Vec3::sAxisY(), PI / 2));

            Trigger* puzzle2Trigger = CreateEntity<Trigger>();
            puzzle2Trigger->Load(new BodyCreationSettings(new BoxShape(Vec3(1.0f, 6.75f, 8.5f) / 2.0f), Vec3(41.5f, 3.375f, 103.5f), Quat::sIdentity(), EMotionType::Static, Layers::TRIGGER),
                [player, puzzle2Trigger, hud](BodyID bodyId) {
                    if (bodyId == player->body->GetID()) {
                        hud->SetSubtitle("what_is");
                        DestroyEntity(puzzle2Trigger);
                    }
                });


            Trigger* puzzle2EndTrigger = CreateEntity<Trigger>();
            puzzle2EndTrigger->Load(new BodyCreationSettings(new BoxShape(Vec3(4.75f, 10.0f, 7.0f) / 2.0f), Vec3(48.625f, 5.0f, 114.0f), Quat::sIdentity(), EMotionType::Static, Layers::TRIGGER),
                [player, puzzle2Trigger, puzzle2EndTrigger, hud](BodyID bodyId) {
                    if (bodyId == player->body->GetID()) {
                        hud->SetSubtitle("i_wonder");

                        if (puzzle2Trigger->IsAlive)
                            DestroyEntity(puzzle2Trigger);
                        DestroyEntity(puzzle2EndTrigger);
                    }
                });
        }

        // Puzzle 3
        {
            CreateEntity<Checkpoint>()->Load(Vec3(58.125, 3.875, 120.5), Vec3(4.25, 7.75, 4));
            CreateAmbienceTrigger(player, "amb_outside2", Vec3(55.75, -42.125, 190.375), Vec3(85, 184.75, 144.25));

            SlidingDoor* puzzle3Door = CreateEntity<SlidingDoor>();
            puzzle3Door->automatic = false;
            puzzle3Door->Spawn(Vec3(58.1f, 3.582f, 118.0f), Vec3(58.1f, 9.582f, 118.0f), Quat::sEulerAngles(Vec3(0, -90, 0) * DEG2RAD));

            Timer* puzzle3Timer = CreateEntity<Timer>();

            Prop* puzzle3BellProp = CreateEntity<Prop>();
            puzzle3BellProp->Load("bell", Vec3(58.25, 8.889, 117.267), Quat::sIdentity(), 3000.0f, 0.5f, {}, {}, EMotionType::Static);

            puzzle3Timer->AddEvent(6.0f,
                [hud]() {
                    hud->SetSubtitle("ladder");
                });

            puzzle3Timer->AddEvent(12.0f,
                [hud]() {
                    hud->SetSubtitle("good_luck");
                });

            puzzle3Timer->AddEvent(13.0f,
                [puzzle3BellProp]() {
                    SoundPlayer* bellSound = CreateEntity<SoundPlayer>();
                    bellSound->Load(new game::Sound3D("warningbell", Vec3(58.25, 8.889, 117.267), 1, 1), true);
                    bellSound->Play();
                }
            );
            puzzle3Timer->AddEvent(14.0f,
                [puzzle3Door, puzzle3Timer]() {
                    puzzle3Door->SetState(true);
                    DestroyEntity(puzzle3Timer);
                });


            Trigger* puzzle3Trigger = CreateEntity<Trigger>();
            puzzle3Trigger->Load(new BodyCreationSettings(new BoxShape(Vec3(12.0f, 10.0f, 6.0f) / 2.0f), Vec3(58.0f, 5.0f, 114.25f), Quat::sIdentity(), EMotionType::Static, Layers::TRIGGER),
                [player, puzzle3Trigger, puzzle3Timer, hud](BodyID bodyId) {
                    if (bodyId == player->body->GetID()) {
                        hud->SetSubtitle("next_test");
                        puzzle3Timer->SetActive(true);
                        DestroyEntity(puzzle3Trigger);
                    }
                });

            CreateEntity<Prop>()->Load("ladder", Vec3(65.162f, 6.442f, 119.66f), Quat::sEulerAngles(Vec3(82, 0, 0) * DEG2RAD), 1000000.0f, 1.0f, Layers::LADDER);

            CreateEntity<SlidingDoor>()->Spawn(Vec3(52.9f, 13.582f, 263.0f), Vec3(52.9f, 19.582f, 263.0f), Quat::sEulerAngles(Vec3(0, -90, 0) * DEG2RAD));

            CreateEntity<Cuckoo>()->Load(Vec3(55.499f, 32.5f, 172.886f), Quat::sIdentity());
            CreateEntity<Cuckoo>()->Load(Vec3(25.999f, -27.588f, 194.886f), Quat::sIdentity());
            CreateEntity<Cuckoo>()->Load(Vec3(55.501f, 11.0f, 199.114f), Quat::sRotation(Vec3::sAxisY(), PI));
            CreateEntity<Cuckoo>()->Load(Vec3(52.799, 11.012f, 266.386f), Quat::sIdentity());
            CreateEntity<Cuckoo>()->Load(Vec3(68.386f, 1.5f, 120.429f), Quat::sRotation(Vec3::sAxisY(), -135.0f * DEG2RAD));

            Trigger* puzzle3EndTrigger = CreateEntity<Trigger>();
            puzzle3EndTrigger->Load(new BodyCreationSettings(new BoxShape(Vec3(12.0f, 10.0f, 2.0f) / 2.0f), Vec3(52.8f, 15.0f, 270.5f), Quat::sIdentity(), EMotionType::Static, Layers::TRIGGER),
                [player, puzzle3EndTrigger, hud, matterMan](BodyID bodyId) {
                    if (bodyId == player->body->GetID()) {
                        player->movementLocked = true;
                        player->lookLocked = true;

                        DestroyEntity(matterMan);

                        hud->ShowEndGameScreen();
                        DestroyEntity(puzzle3EndTrigger);
                    }
                });
        }
    };
};