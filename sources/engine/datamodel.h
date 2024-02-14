#pragma once

#include <raylib-cpp.hpp>
#include <engine/physics.h>

#include <chrono>
#include <iostream>
#include <thread>

namespace datamodel
{
    class Game
    {
    public:
        const int TICK_RATE = 60;
        const int TICK_DURATION = 1000 / TICK_RATE;

        const uint MAX_BODIES = 65536;

        const uint NUM_BODY_MUTEXES = 0;

        const uint MAX_BODY_PAIRS = 65536;
        const uint MAX_CONTACT_CONSTRAINTS = 10240;

        JPH::PhysicsSystem* physics_system{};

        Game() {
            // initialize physics system

            RegisterDefaultAllocator();
            Factory::sInstance = new Factory();
            RegisterTypes();

            physics_system = new PhysicsSystem();
            physics_system->Init(MAX_BODIES, NUM_BODY_MUTEXES, MAX_CONTACT_CONSTRAINTS, MAX_CONTACT_CONSTRAINTS, BPLayerInterfaceImpl(), ObjectVsBroadPhaseLayerFilterImpl(), ObjectLayerPairFilterImpl());

            // create tick and render threads

            std::thread tickThread(&Game::Tick, this);
            std::thread renderThread(&Game::Render, this);

            tickThread.join();
            renderThread.join();
        };
    private:
        bool shouldExit = false;

        void Tick();
        void Render();
    };
}