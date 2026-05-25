#include "App.h"

App::App(SDL_Window* aWindow, SDL_GPUDevice* aDevice)
{
    myEngine = new Engine(aWindow, aDevice, "./");
    myWindow = aWindow;
    myDevice = aDevice;
    myIsReadyToUpdate = false;
}

App::~App()
{
    delete myEngine;
    myEngine = nullptr;

    SDL_DestroyGPUDevice(myDevice);
    myDevice = nullptr;

    SDL_DestroyWindow(myWindow);
    myWindow = nullptr;
}

void App::Update()
{
    myEngine->Update();
}

void App::Paint()
{
    myEngine->Paint();
    SDL_GL_SwapWindow(myWindow);
}

bool App::Event(SDL_Event* aEvent)
{
    switch(aEvent->type)
    {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        case SDL_EVENT_QUIT:
            Close();
            break;
        case SDL_EVENT_WINDOW_SHOWN:
            SDL_Log("Window shown");
            myEngine->LoadModule("Triangle");
            myIsReadyToUpdate = true;
            break;
    }
    
    if (WantsClose())
        return false;

    return myEngine->HandleEvent(aEvent);
}

bool App::WantsClose()
{
    return !myEngine || myEngine->WantsClose();
}

void App::Close()
{
    delete myEngine;
    myEngine = nullptr;
    SDL_Log("App closed");
}

bool App::IsReady()
{
	return myIsReadyToUpdate;
}
