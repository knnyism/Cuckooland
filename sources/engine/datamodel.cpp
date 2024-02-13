#include <engine/datamodel.h>

using namespace datamodel;

void Game::Tick()
{
	static TempAllocatorImpl temp_allocator(10 * 1024 * 1024);
	static JobSystemThreadPool job_system(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

    while (!shouldExit)
    {
		auto startTime = std::chrono::steady_clock::now();

		physics_system->Update(TICK_DURATION, 1, &temp_allocator, &job_system);

		auto elapsedTime = std::chrono::steady_clock::now() - startTime;

		// Sleep for the remaining time of the tick
		std::chrono::milliseconds sleepTime(TICK_DURATION);
		std::this_thread::sleep_for(sleepTime - elapsedTime);
	}
}

void Game::Render()
{
	InitAudioDevice();
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	raylib::Window window(1024, 768, "Cuckooland");
	Image image = LoadImage(ASSETS_PATH "icon.png");
	Sound sound = LoadSound(ASSETS_PATH "sounds/cuckoo_chirp.wav");

	window.SetIcon(image);
	window.SetTargetFPS(GetMonitorRefreshRate(0));

	DisableCursor();

	raylib::Camera camera({ 0.0f, 20.0f, 20.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 90.0f, CAMERA_PERSPECTIVE);
	raylib::Model model(ASSETS_PATH "models/map.glb");

	Vector3 position = { 0.0f, 0.0f, 0.0f };

	// Main game loop
		
    while (!WindowShouldClose())
    {
		camera.Update(CAMERA_FREE);

		// Draw the game
		BeginDrawing();
		{
			window.ClearBackground();

			camera.BeginMode();
			{
				model.Draw(position, 1.0f);
			}
			camera.EndMode();

			window.DrawFPS(10, 10);
		}	
		EndDrawing();
	}
	shouldExit = true;
}