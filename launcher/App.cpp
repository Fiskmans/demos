#include "App.h"

App::App(SDL_Window* aWindow, SDL_GPUDevice* aDevice)
{
    myEngine = new Engine(aWindow, aDevice, "./");
    myWindow = aWindow;
    myDevice = aDevice;
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
        case SDL_EventType::SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            Close();
            break;
    }
    
    if (WantsClose())
        return false;

    return myEngine->HandleEvent(aEvent);
}

bool App::WantsClose()
{
    return !myEngine;
}

void App::Close()
{
    delete myEngine;
    myEngine = nullptr;
}
