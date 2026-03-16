#include "App.h"

App::App(SDL_Window* aWindow, SDL_GLContext aContext)
    : myEngine(aWindow, aContext, "./")
{
    myWantsClose = false;
    myWindow = aWindow;
    myContext = aContext;

    myEngine.LoadModule("Chaos");
}

App::~App()
{
    SDL_GL_DestroyContext(myContext);
    myContext = nullptr;

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
