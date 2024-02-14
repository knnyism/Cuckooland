#include "raylib-cpp.hpp"

#include <engine/datamodel.h>
#include <engine/physics.h>

#include <entities/Map.h>
#include <globals.h>
#include <flavor.h>

inline s8 TICK_RATE = 60;
inline f32 TICK_DURATION = 1 / TICK_RATE;

const u32 MAX_BODIES = 65536;

const u32 NUM_BODY_MUTEXES = 0;

const u32 MAX_BODY_PAIRS = 65536;
const u32 MAX_CONTACT_CONSTRAINTS = 10240;

inline f64 startTime = GetTime();
inline s64 currentTick = 0;

static s64 calculateTickFromTime(f64 currentTime) {
    return std::ceil((currentTime - startTime) * TICK_RATE);
}

static f64 calculateTimeFromTick(s64 tick) {
    return (f64)tick / (f64)TICK_RATE;
}

void main() {
    // Initialize Jolt
    RegisterDefaultAllocator();
    Factory::sInstance = new Factory();

    physics_system = new PhysicsSystem();
    physics_system->Init(MAX_BODIES, NUM_BODY_MUTEXES, MAX_CONTACT_CONSTRAINTS, MAX_CONTACT_CONSTRAINTS, BPLayerInterfaceImpl(), ObjectVsBroadPhaseLayerFilterImpl(), ObjectLayerPairFilterImpl());

    // Initialize Raylib
    InitAudioDevice();
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    raylib::Window window(1024, 768, "Cuckooland");
    window.SetIcon(LoadImage("C:/Users/canis/Repositories/Cuckooland/assets/icon.png"));
    window.SetTargetFPS(GetMonitorRefreshRate(0));
    DisableCursor();

    // Load the map
    // TODO: Learn to use relative paths lol
    Map* map = CreateEntity<Map>();
    map->Load("C:/Users/canis/Repositories/Cuckooland/assets/models/map.glb");

    TempAllocatorImpl temp_allocator(10 * 1024 * 1024);
    JobSystemThreadPool job_system(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

    // TODO: Turn camera into an entity
    raylib::Camera camera({ 0.0f, 20.0f, 20.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 90.0f, CAMERA_PERSPECTIVE);

    while (!window.ShouldClose())
    {
        // Update ticks
        while (currentTick < calculateTickFromTime(GetTime()))
        {
            currentTick++;

            physics_system->Update(TICK_DURATION, 1, &temp_allocator, &job_system);

            for (Entity* entity : entities) {
                if (entity->IsAlive) {
                    entity->Tick();
                }
            }
        }

        // TODO: Pre-render methods? Think about this

        BeginDrawing();
        {
            window.ClearBackground();

            camera.BeginMode();
            {
                for (Entity* entity : entities) {
                    if (entity->IsAlive) {
                        entity->Render();
                    }
                }
            }
            camera.EndMode();

            window.DrawFPS(10, 10);
        }
        EndDrawing();
    }

    return;
}