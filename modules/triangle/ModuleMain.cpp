
#include "engine/Module.h"
#include "engine/Engine.h"

#include "SDL3/SDL.h"

#include "modules/triangle/TriangleModule.h"

static TriangleModule* triangleModule = nullptr;

extern "C" {

    const char* EngineModule_Name =  "Triangle";

    bool EngineModule_Load(int aEngineVersion, Engine* aEngine)
    {
        if (aEngineVersion != Module::EngineVersion)
        {
            SDL_Log("Incompatible version");
            return false;
        }
        
        if (triangleModule)
        {
            SDL_Log("Already Loaded");
            return false;
        }
        triangleModule = new TriangleModule(aEngine);
        return true;
    }

    void EngineModule_Unload()
    {
        if (!triangleModule)
        {
            SDL_Log("Not Loaded");
            return;
        }
        delete triangleModule;
        triangleModule = nullptr;
    }
    
}
