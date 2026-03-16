#pragma once

#include "engine/Module.h"
#include "imgui.h"
#include "SDL3/SDL.h"
#include "tools/Event.h"

#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

class Engine
{
    using Clock = std::chrono::steady_clock;
public:
    static constexpr const char* OpenGLVersion = "#version 130";
    static constexpr int OpenGLMajorVersionNumber = 3;
    static constexpr int OpenGLMinorVersionNumber = 0;

    using TimeDelta = std::chrono::duration<float, std::ratio<1,1>>;

    Engine(SDL_Window* aWindow, SDL_GLContext aContext, std::string aModulesDirectory);
    ~Engine();
    
    bool LoadModule(std::string aName);

    void Update();
    void Paint();
    void RegisterImgui(std::string aName, std::function<void()> aFunction);

    bool HandleEvent(SDL_Event* aEvent);

    fisk::tools::Event<TimeDelta> OnUpdate;
    fisk::tools::Event<> OnPaint;

private:
    void FindModules(std::string aDirectory);
    
    struct ImguiWindow
    {
        std::function<void()> myCallback;
        bool myOpen;
    };

    SDL_Window* myWindow;
    SDL_GLContext myContext;

    Clock::time_point myLastUpdate;

    bool myIsShowingMainWindow;
    ImVec4 myClearColor;
    std::unordered_map<std::string, ImguiWindow> myWindows;
    std::vector<std::unique_ptr<Module>> myModules;
};