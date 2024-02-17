#include "raylib-cpp.hpp"

#include <globals.h>
#include <flavor.h>
#include <datamodel.h>
#include <physics.h>

#include <entities/Map.h>
#include <entities/Prop.h>
#include <entities/Player.h>

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

    physics_system->SetGravity(JPH::Vec3(0, -56.0f, 0));

    // Initialize Raylib
    InitAudioDevice();
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    raylib::Window window(1024, 768, "Cuckooland");
    window.SetIcon(LoadImage("C:/Users/canis/Repositories/Cuckooland/assets/icon.png"));
    window.SetTargetFPS(GetMonitorRefreshRate(0));
    DisableCursor();

    Map* map = CreateEntity<Map>();
    map->Load("C:/Users/canis/Repositories/Cuckooland/assets/models/map.glb");

    Prop* prop = CreateEntity<Prop>();
    prop->Load("C:/Users/canis/Repositories/Cuckooland/assets/models/cardboard_box2.glb");

    Player* player = CreateEntity<Player>();
    player->Spawn();

    camera = new raylib::Camera({ 0.0f, 1.0f, 1.0f }, Vector3Zero(), { 0.0f, 1.0f, 0.0f }, 90.0f, CAMERA_PERSPECTIVE);

    while (!window.ShouldClose())
    {
        // Update ticks
        while (currentTick < CalculateTickFromTime(GetTime()))
        {
            currentTick++;
            // std::cout << TICK_DURATION << std::endl;

            for (Entity* entity : entities) {
                if (entity->IsAlive) {
                    entity->Tick();
                }
            }

            physics_system->Update(TICK_DURATION, 1, temp_allocator, job_system);
        }

        // TODO: Pre-render methods? Think about this

        BeginDrawing();
        {
            window.ClearBackground();

            camera->BeginMode();
            {
                for (Entity* entity : entities) {
                    if (entity->IsAlive) {
                        entity->Render();
                    }
                }
            }
            camera->EndMode();

            window.DrawFPS(10, 10);
        }
        EndDrawing();
    }

    return;
}