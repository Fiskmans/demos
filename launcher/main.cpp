
#include <cstdio>
#include <cstdlib>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "launcher/App.h"

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

	SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS;

	float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

	if (scale < 0.01f)
		SDL_Log("Get scale error: %s", SDL_GetError());
	else
		SDL_Log("Scale: %f", scale);

	SDL_GPUDevice* device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, nullptr);
	if (!device)
	{
		SDL_Log("Failed to create gpu device");
		return SDL_APP_FAILURE;
	}

	SDL_Window* window = SDL_CreateWindow("Hello World", 1280 * scale, 800 * scale, windowFlags);
	if (!window)
	{
		SDL_Log("Failed to create window");
		return SDL_APP_FAILURE;
	}

	int w;
	int h;

	SDL_GetWindowSize(window, &w, &h);

	SDL_Log("Window created with size (%d, %d)", w, h);

	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(window);

	SDL_GetWindowSize(window, &w, &h);
	SDL_Log("Window shown with size (%d, %d)", w, h);


	if (!SDL_ClaimWindowForGPUDevice(device, window))
	{
		SDL_Log("Failed to claim window: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	
	*appstate = new App(window, device);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	App* app = (App*)appstate;

	app->Update();
	app->Paint();

	if (app->WantsClose())
		return SDL_AppResult::SDL_APP_FAILURE;

	return SDL_AppResult::SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	App* app = (App*)appstate;
	app->Event(event);

	if (app->WantsClose())
		return SDL_AppResult::SDL_APP_FAILURE;

	return SDL_AppResult::SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	App* app = (App*)appstate;
	delete app;
}