#include "raylib-cpp.hpp"

#include <game.h>
#include <flavor.h>
#include <physics.h>
#include <lighting.h>

#include <entities/Map.h>
#include <entities/Door.h>
#include <entities/Prop.h>
#include <entities/Player.h>
#include <entities/SlidingDoor.h>
#include <entities/MatterManPickup.h>
#include <entities/Elevator.h>
#include <entities/Void.h>
#include <entities/Cuckoo.h>
#include <entities/Skybox.h>

#include <iostream>

class GameContactListener : public ContactListener
{
public:
    virtual void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
    {
        for (auto& bucket : entities.buckets)
            for (Entity* entity : bucket->data)
                if (entity->IsAlive)
                    entity->OnContactAdded(inBody1, inBody2, inManifold, ioSettings);
    }
};

void main() {
    // Register allocation hook
    JPH::RegisterDefaultAllocator();

    // Install callbacks
    JPH::Trace = TraceImpl;
    JPH::JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;);

    JPH::Factory::sInstance = new JPH::Factory();

    JPH::RegisterTypes();

    temp_allocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
    job_system = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

    const u32 cMaxBodies = 10240;
    const u32 cNumBodyMutexes = 0;
    const u32 cMaxBodyPairs = 10240;
    const u32 cMaxContactConstraints = 10240;

    BPLayerInterfaceImpl broad_phase_layer_interface;
    ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;
    ObjectLayerPairFilterImpl object_vs_object_layer_filter;

    // Now we can create the actual physics system.
    physics_system = new JPH::PhysicsSystem();
    physics_system->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broad_phase_layer_interface, object_vs_broadphase_layer_filter, object_vs_object_layer_filter);
    body_interface = &physics_system->GetBodyInterface();

    physics_system->SetGravity(JPH::Vec3(0, -56.0f, 0));

    // Initialize Raylib
    InitAudioDevice();
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    raylib::Window window(1024, 768, "Cuckooland");
    window.SetIcon(raylib::Image(GetAssetPath("icon.png")));
    window.SetTargetFPS(GetMonitorRefreshRate(0));
    DisableCursor();

    camera = new raylib::Camera({ 0.0f, 1.0f, 1.0f }, Vector3Zero(), { 0.0f, 1.0f, 0.0f }, 85.0f, CAMERA_PERSPECTIVE);

    LoadLighting();

    // THERE IS A GIANT STAR IN THE SKY
    CreateLight(LIGHT_SUN, { -10.0f, -10.0f, -10.0f }, { 10.0f, 10.0f, 10.0f }, { 200, 249, 120 }, 0.01f);
    CreateEntity<Skybox>()->Load();

    Void* killVoid = CreateEntity<Void>();
    killVoid->height = -130.0f;

    Player* player = CreateEntity<Player>();
    player->Load();
    player->Spawn();
    //player->Spawn(Vec3(58.5f, 5.0f, 125.0f));
    //CreateEntity<MatterMan>()->Spawn(player);

    // LOAD TEST LEVEL
    /*
    Map* map = CreateEntity<Map>();
    map->Load("C:/Users/canis/Repositories/Cuckooland/assets/models/testmap.glb");

    CreateEntity<Elevator>()->Load(Vec3(-10.0f, 5.0f, 0.0f), Quat::sIdentity());
    CreateEntity<MatterMan>()->Spawn(player);

    for (int i = 0; i < 10; i++)
        CreateEntity<Prop>()->Load("cube", Vec3(-10.0f, 1.95f, i * 2.0f), Quat::sIdentity(), 5000.0f);

    CreateEntity<Prop>()->Load("table", Vec3(-6.813f, 4.313f, 6.063f), Quat::sEulerAngles(Vec3(0, 180, 0)));

    CreateLight(LIGHT_POINT, { -7.25f, 4.5f, -7.25f }, { }, { 255, 0, 0 }, 2.0f);
    CreateLight(LIGHT_POINT, { -7.25f, 4.5f, -5.25f }, { }, { 0, 255, 0 }, 2.0f);
    CreateLight(LIGHT_POINT, { -5.75f, 4.5f, -5.25f }, { }, { 0, 0, 255 }, 2.0f);
    */

    // LOAD LEVEL

    Map* map = CreateEntity<Map>();
    map->Load("map");

    // Spawn
    CreateEntity<SlidingDoor>()->Spawn(Vec3(-10.5f, 3.582f, 0), Vec3(-10.5f, 9.582f, 0));
    CreateLight(LIGHT_SPOT, { 0.0f, 7.934f, 0.0f }, { -0.0f, 0.0f, -0.0f }, { 255, 0, 0 }, 20.0f);

    // Hallway
    CreateEntity<SlidingDoor>()->Spawn(Vec3(-17.0f, 3.582f, 38.5f), Vec3(-17.0f, 9.582f, 38.5f), Quat::sEulerAngles(Vec3(0, PI / 2, 0)));

    CreateLight(LIGHT_SPOT, { -17.0f, 9.934f, -2.0f }, { -17.0f, 0.0f, -2.0f }, { 200, 249, 120 }, 5.0f);
    CreateLight(LIGHT_SPOT, { -17.0f, 9.934f, 14.0f }, { -15.0f, 0.0f, 14.0f }, { 200, 249, 120 }, 5.0f);
    CreateLight(LIGHT_SPOT, { -17.0f, 9.934f, 30.0f }, { -15.0f, 0.0f, 30.0f }, { 200, 249, 120 }, 5.0f);

    CreateLight(LIGHT_SPOT, { -17.0f, 7.934f, -2.0f }, { -17.0f, 9.0f, -2.0f }, { 200, 249, 120 }, 5.0f);
    CreateLight(LIGHT_SPOT, { -17.0f, 7.934f, 14.0f }, { -17.0f, 9.0f, 14.0f }, { 200, 249, 120 }, 5.0f);
    CreateLight(LIGHT_SPOT, { -17.0f, 7.934f, 30.0f }, { -17.0f, 9.0f, 30.0f }, { 200, 249, 120 }, 5.0f);

    CreateLight(LIGHT_SPOT, { -17.0f, 9.934f, 14.0f }, { -19.0f, 0.0f, 14.0f }, { 200, 249, 120 }, 5.0f);
    CreateLight(LIGHT_SPOT, { -17.0f, 9.934f, 30.0f }, { -19.0f, 0.0f, 30.0f }, { 200, 249, 120 }, 5.0f);
    CreateLight(LIGHT_SPOT, { -17.0f, 9.934f, 30.0f }, { -17.0f, 0.0f, 32.0f }, { 200, 249, 120 }, 5.0f);

    // MatterMan room
    CreateEntity<SlidingDoor>()->Spawn(Vec3(-3.0f, 3.582f, 59.5f), Vec3(-3.0f, 4.582f, 59.5f), Quat::sEulerAngles(Vec3(0, PI / 2, 0)));

    CreateEntity<MatterManPickup>()->Spawn(Vec3(-17.063f, 4.075f, 55.625f));
    CreateEntity<Prop>()->Load("table", Vec3(-17.063f, 1.688f, 56.813f), Quat::sIdentity(), 3000.0f);

    CreateEntity<Door>()->Load("fire_door", Vec3(-29.5f, 3.35f, 47.5f), Quat::sIdentity(), 3000.0f);

    CreateEntity<Prop>()->Load("cardboard_box1", Vec3(1.339f, 4.143f, 41.718f), Quat::sIdentity(), 3000.0f);
    CreateEntity<Prop>()->Load("cardboard_box2", Vec3(2.923f, 4.5f, 44.729f), Quat::sIdentity(), 3000.0f);
    CreateEntity<Prop>()->Load("cardboard_box3", Vec3(0.5f, 1.5f, 41.5f), Quat::sIdentity(), 3000.0f);
    CreateEntity<Prop>()->Load("cardboard_box4", Vec3(3.5f, 1.5f, 45.5f), Quat::sIdentity(), 3000.0f);

    CreateEntity<Cuckoo>()->Load(Vec3(3.539f, 0.012f, 41.114f), Quat::sEulerAngles(Vec3(0.0f, 180.0f, 0.0f) * DEG2RAD), true);

    CreateLight(LIGHT_SPOT, { -23.0f, 22.934f, 49.0f }, { -23.0f, 20.934f, 49.0f }, { 200, 249, 120 }, 20.0f);
    CreateLight(LIGHT_SPOT, { -12.0f, 22.934f, 49.0f }, { -12.0f, 20.934f, 49.0f }, { 200, 249, 120 }, 20.0f);
    CreateLight(LIGHT_SPOT, { -1.0f, 22.934f, 49.0f }, { -1.0f, 20.934f, 49.0f }, { 200, 249, 120 }, 20.0f);

    // Outside
    CreateEntity<Door>()->Load("fire_door", Vec3(-17.5f, -6.65f, 60.5f), Quat::sEulerAngles(Vec3(0, 90, 0) * DEG2RAD), 3000.0f);

    CreateEntity<Cuckoo>()->Load(Vec3(-17.499f, -9.988f, 42.114f), Quat::sEulerAngles(Vec3(0.0f, 180.0f, 0.0f) * DEG2RAD));

    CreateEntity<Prop>()->Load("plank", Vec3(-21.197f, -9.849f, 47.998f), Quat::sEulerAngles(Vec3(0, 1, 0) * DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
    CreateEntity<Prop>()->Load("plank", Vec3(-21.197f, -8.849f, 47.998f), Quat::sEulerAngles(Vec3(0, -1, 0) * DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
    CreateEntity<Prop>()->Load("plank", Vec3(-21.197f, -7.849f, 47.998f), Quat::sEulerAngles(Vec3(0, 0, 0) * DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
    CreateEntity<Prop>()->Load("plank", Vec3(-21.197f, -6.849f, 47.998f), Quat::sEulerAngles(Vec3(0, 1, 0) * DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
    CreateEntity<Prop>()->Load("plank", Vec3(-21.197f, -5.849f, 47.998f), Quat::sEulerAngles(Vec3(0, -1, 0) * DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);

    CreateEntity<Prop>()->Load("plank", Vec3(-13.7f, -9.849f, 47.998f), Quat::sEulerAngles(Vec3(0, 1, 0) * DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
    CreateEntity<Prop>()->Load("plank", Vec3(-13.7f, -8.849f, 47.998f), Quat::sEulerAngles(Vec3(0, -1, 0) * DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
    CreateEntity<Prop>()->Load("plank", Vec3(-13.7f, -7.849f, 47.998f), Quat::sEulerAngles(Vec3(0, 0, 0) * DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
    CreateEntity<Prop>()->Load("plank", Vec3(-13.7f, -6.849f, 47.998f), Quat::sEulerAngles(Vec3(0, 2, 0) * DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);
    CreateEntity<Prop>()->Load("plank", Vec3(-13.7f, -5.849f, 47.998f), Quat::sEulerAngles(Vec3(0, -2, 0) * DEG2RAD), 60000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);

    CreateEntity<Prop>()->Load("ladder", Vec3(-3.0f, 15.25f, 70.25f), Quat::sEulerAngles(Vec3(90, 90, 0) * DEG2RAD), 1000000.0f, 1.0f, Layers::LADDER);
    CreateEntity<Cuckoo>()->Load(Vec3(0.386f, 9.012f, 84.498f), Quat::sEulerAngles(Vec3(0.0f, -135.0f, 0.0f) * DEG2RAD));

    CreateEntity<Cuckoo>()->Load(Vec3(-33.001f, -9.988f, 103.386f), Quat::sIdentity());

    // Puzzle 1
    CreateEntity<Door>()->Load("fire_door", Vec3(-29.5f, -6.65f, 103.5f), Quat::sIdentity(), 3000.0f);

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

    CreateLight(LIGHT_SPOT, { -6.0f, 12.434f, 103.5f }, { -6.0f, 10.434f, 103.5f }, { 200, 249, 120 }, 40.0f);
    CreateLight(LIGHT_SPOT, { -9.0f, 12.434f, 103.5f }, { -9.0f, 10.434f, 103.5f }, { 200, 249, 120 }, 40.0f);
    CreateLight(LIGHT_SPOT, { -24.0f, 12.434f, 103.5f }, { -24.0f, 10.434f, 103.5f }, { 200, 249, 120 }, 40.0f);

    // Puzzle 2
    CreateEntity<SlidingDoor>()->Spawn(Vec3(11.5f, 3.582f, 103.5f), Vec3(11.5f, 9.582f, 103.5f));
    CreateEntity<Door>()->Load("fire_door", Vec3(41.5f, 3.35f, 101.4f), Quat::sIdentity(), 3000.0f, 0.0f);
    CreateEntity<Door>()->Load("fire_door", Vec3(41.5f, 3.35f, 105.6f), Quat::sEulerAngles(Vec3(0, -180, 0) * DEG2RAD), 3000.0f, -90.0f, 0.0f);
    CreateEntity<Prop>()->Load("plank2", Vec3(42.25f, 3.55f, 103.45f), Quat::sIdentity(), 600000.0f, 0.8f, Layers::MOVING, MATERIAL_WOOD);

    CreateLight(LIGHT_SPOT, { 20.0f, 9.934f, 103.5f }, { 20.0f, 4.934f, 103.5f }, { 200, 249, 120 }, 40.0f);
    CreateLight(LIGHT_SPOT, { 35.0f, 9.934f, 103.5f }, { 35.0f, 4.934f, 103.5f }, { 200, 249, 120 }, 40.0f);
    CreateLight(LIGHT_SPOT, { 52.0f, 9.934f, 103.5f }, { 52.0f, 4.934f, 103.5f }, { 200, 249, 120 }, 40.0f);

    // Puzzle 3
    CreateEntity<SlidingDoor>()->Spawn(Vec3(58.1f, 3.582f, 118.0f), Vec3(58.1f, 9.582f, 118.0f), Quat::sEulerAngles(Vec3(0, -90, 0) * DEG2RAD));
    CreateEntity<Prop>()->Load("ladder", Vec3(65.162f, 6.442f, 119.66f), Quat::sEulerAngles(Vec3(82, 0, 0) * DEG2RAD), 1000000.0f, 1.0f, Layers::LADDER);

    CreateEntity<SlidingDoor>()->Spawn(Vec3(52.9f, 13.582f, 263.0f), Vec3(52.9f, 19.582f, 263.0f), Quat::sEulerAngles(Vec3(0, -90, 0) * DEG2RAD));

    physics_system->OptimizeBroadPhase();
    physics_system->SetContactListener(new GameContactListener());

    while (!window.ShouldClose())
    {
        scrollDirection += GetMouseWheelMove();

        // Update ticks
        while (currentTick < CalculateTickFromTime(GetTime()))
        {
            currentTick++;

            for (auto& bucket : entities.buckets)
                for (Entity* entity : bucket->data)
                    if (entity->IsAlive)
                        entity->Tick();

            physics_system->Update(TICK_DURATION, 4, temp_allocator, job_system); // Update physics 4x the tickrate so things don't freak out

            scrollDirection = 0;
        }

        // Update entities that need processing before the camera is updated
        // Useful for entities that modify the camera matrix since we can't update it while rendering
        for (auto& bucket : entities.buckets)
            for (Entity* entity : bucket->data)
                if (entity->IsAlive)
                    entity->BeforeCamera();

        game::UpdateSound3Ds();
        UpdateLighting();

        window.BeginDrawing();
        {
            window.ClearBackground();

            camera->BeginMode();
            {
                // Render entities
                for (auto& bucket : entities.buckets)
                    for (Entity* entity : bucket->data)
                        if (entity->IsAlive)
                            entity->Render();
            }
            camera->EndMode();

            DrawFPS(10, 10);
        }
        window.EndDrawing();
    }

    return;
}