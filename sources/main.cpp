#include "raylib-cpp.hpp"

#include <game.h>
#include <flavor.h>
#include <physics.h>
#include <lighting.h>


#include <entities/Levels/MainLevel.h>
#include <entities/Levels/TestLevel.h>

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

    virtual void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override
    {
        for (auto& bucket : entities.buckets)
            for (Entity* entity : bucket->data)
                if (entity->IsAlive)
                    entity->OnContactRemoved(inSubShapePair);
    }
};

void main() {
    // Register allocation hook
    JPH::RegisterDefaultAllocator();

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

    // Now we can create the actual physics system
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

    physics_system->OptimizeBroadPhase();
    physics_system->SetContactListener(new GameContactListener());

    // Load level
    CreateEntity<MainLevel>()->Load(); // CreateEntity<TestLevel>()->Load();

    while (!window.ShouldClose() && !quitRequested)
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

            for (auto& bucket : entities.buckets)
                for (Entity* entity : bucket->data)
                    if (entity->IsAlive)
                        entity->AfterCamera();
        }
        window.EndDrawing();
    }


    return;
}