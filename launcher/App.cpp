#include "App.h"

App::App(SDL_Window* aWindow, SDL_GPUDevice* aDevice)
    : myEngine(aWindow, aDevice, "./")
{
    myWantsClose = false;
    myWindow = aWindow;
    myDevice = aDevice;

    myEngine.LoadModule("Chaos");
}

App::~App()
{
    SDL_DestroyGPUDevice(myDevice);
    myDevice = nullptr;

    SDL_DestroyWindow(myWindow);
    myWindow = nullptr;
}

void App::Update()
{
    myEngine.Update();
}

void App::Paint()
{
    myEngine.Paint();
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

    return myEngine.HandleEvent(aEvent);
}

bool App::WantsClose()
{
    return myWantsClose;
}

void App::Close()
{
    myWantsClose = true;
}
