#include "raylib-cpp.hpp"

#include <game.h>
#include <flavor.h>
#include <physics.h>
#include <globals.h>

#include <entities/Map.h>
#include <entities/Prop.h>
#include <entities/Player.h>
#include <entities/SlidingDoor.h>
#include <entities/MatterManPickup.h>
#include <lighting.h>

#include <iostream>

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
    window.SetIcon(LoadImage("C:/Users/canis/Repositories/Cuckooland/assets/icon.png"));
    window.SetTargetFPS(GetMonitorRefreshRate(0));
    DisableCursor();

    camera = new raylib::Camera({ 0.0f, 1.0f, 1.0f }, Vector3Zero(), { 0.0f, 1.0f, 0.0f }, 90.0f, CAMERA_PERSPECTIVE);

    LoadLighting();

    Map* map = CreateEntity<Map>();
    map->Load("C:/Users/canis/Repositories/Cuckooland/assets/models/map.glb");

    CreateEntity<MatterManPickup>()->Spawn(Vec3(-17.063f, 4.075f, 55.625f));
    CreateEntity<Prop>()->Load("table", Vec3(-17.063f, 1.688f, 56.813f));

    CreateEntity<Prop>()->Load("plank", Vec3(-26.9f, -9.95f, 64.2f), Quat::sEulerAngles(Vec3(0, -80.84, 180) * DEG2RAD), 1000000.0f);
    CreateEntity<Prop>()->Load("plank", Vec3(-23.7f, -1.995f, 61.35f), Quat::sEulerAngles(Vec3(89.0f, 180, 180) * DEG2RAD), 1000000.0f);
    CreateEntity<Prop>()->Load("plank", Vec3(-21.3f, -1.995f, 61.35f), Quat::sEulerAngles(Vec3(89.0f, 180, 180) * DEG2RAD), 1000000.0f);
    CreateEntity<Prop>()->Load("plank", Vec3(-13.5f, -1.995f, 61.35f), Quat::sEulerAngles(Vec3(89.0f, 180, 180) * DEG2RAD), 1000000.0f);
    CreateEntity<Prop>()->Load("plank", Vec3(-11.2f, -1.995f, 61.35f), Quat::sEulerAngles(Vec3(89.0f, 180, 180) * DEG2RAD), 1000000.0f);


    CreateEntity<SlidingDoor>()->Spawn(Vec3(-10.5f, 3.582f, 0), Vec3(-10.5f, 9.582f, 0));
    CreateEntity<SlidingDoor>()->Spawn(Vec3(-17.0f, 3.582f, 38.5f), Vec3(-17.0f, 9.582f, 38.5f), Quat::sEulerAngles(Vec3(0, PI / 2, 0)));
    CreateEntity<SlidingDoor>()->Spawn(Vec3(-3.0f, 3.582f, 59.5f), Vec3(-3.0f, 4.582f, 59.5f), Quat::sEulerAngles(Vec3(0, PI / 2, 0)));

    CreateLight(LIGHT_POINT, { -2, 1, -2 }, { 0.0f, 0.0f, 0.0f }, raylib::Color(255, 255, 0, 255), 40.0f);

    //lighting->CreateLight(LIGHT_POINT, { 2, 1, 2 }, { 0.0f, 0.0f, 0.0f }, raylib::Color(255, 0, 255, 255), 40.0f);
    //lighting->CreateLight(LIGHT_POINT, { -2, 1, 2 }, { 0.0f, 0.0f, 0.0f }, raylib::Color(0, 0, 255, 255), 40.0f);
    //lighting->CreateLight(LIGHT_POINT, { 2, 1, -2 }, { 0.0f, 0.0f, 0.0f }, raylib::Color(0, 255, 0, 255), 40.0f);


    Player* player = CreateEntity<Player>();
    player->Spawn();

    physics_system->OptimizeBroadPhase();

    while (!window.ShouldClose())
    {
        // Update ticks
        while (currentTick < CalculateTickFromTime(GetTime()))
        {
            currentTick++;

            for (auto& bucket : entities.buckets)
                for (Entity* entity : bucket->data)
                    if (entity->IsAlive)
                        entity->Tick();

            physics_system->Update(TICK_DURATION, 1, temp_allocator, job_system);
        }

        window.BeginDrawing();
        {
            window.ClearBackground();

            camera->BeginMode();
            {
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