
#include <cstdio>
#include <cstdlib>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "launcher/App.h"

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
	SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN |
								  SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_BORDERLESS;

	SDL_Window* window = SDL_CreateWindow("Hello World", 800, 600, windowFlags);
	if (!window)
	{
		SDL_Log("Failed to create window");
		return SDL_APP_FAILURE;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, Engine::OpenGLMajorVersionNumber);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, Engine::OpenGLMinorVersionNumber);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (!glContext)
	{
		SDL_Log("Failed to create glContext");
		return SDL_APP_FAILURE;
	}

    SDL_GL_MakeCurrent(window, glContext);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

	*appstate = new App(window, glContext);

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