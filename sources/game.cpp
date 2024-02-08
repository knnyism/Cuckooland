#include "raylib-cpp.hpp"

#include <iostream>
#include <chrono>
#include <future>

int TICK_RATE = 60;
int TICK_DURATION = 1000 / TICK_RATE;
int currentTick = 0;
bool shouldExit = false;

void tick()
{
    while (!shouldExit)
    {
        auto startTime = std::chrono::steady_clock::now();

        currentTick++;

        // Perform tick logic here

        auto elapsedTime = std::chrono::steady_clock::now() - startTime;

        // Sleep for the remaining time of the tick
        std::chrono::milliseconds sleepTime(TICK_DURATION);
        std::this_thread::sleep_for(sleepTime - elapsedTime);
    }
}

void render()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    raylib::Window window(800, 650, "Cuckooland");

    SetTargetFPS(GetMonitorRefreshRate(0));

    while (!shouldExit)
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawFPS(0, 0);

        shouldExit = WindowShouldClose();

        EndDrawing();
    }
}

namespace game
{
    void start()
    {
        auto tickFuture = std::async(std::launch::async, tick);
        auto renderFuture = std::async(std::launch::async, render);

        tickFuture.wait();
        renderFuture.wait();
    }
};