#pragma once

#include "SDL3/SDL.h"

#include "engine/Engine.h"

class App
{
public:
    App(SDL_Window* aWindow, SDL_GLContext aContext);
    ~App();

    void Update();
    void Paint();
    bool Event(SDL_Event* aEvent);

    bool WantsClose();
    void Close();

private:

    bool myWantsClose;
    Engine myEngine;
    SDL_Window* myWindow;
    SDL_GLContext myContext;
};