
#include "engine/Module.h"
#include "engine/Engine.h"

#include "SDL3/SDL.h"

#include "modules/chaos/ChaosModule.h"

static ChaosModule* chaosModule = nullptr;

extern "C" {

    const char* EngineModule_Name = "Chaos";

    bool EngineModule_Load(int aEngineVersion, Engine* aEngine)
    {
        if (aEngineVersion != Module::EngineVersion)
        {
            SDL_Log("Incompatible version");
            return false;
        }
        
        if (chaosModule)
        {
            SDL_Log("Already Loaded");
            return false;
        }
        chaosModule = new ChaosModule(aEngine);
        return true;
    }

    void EngineModule_Unload()
    {
        if (!chaosModule)
        {
            SDL_Log("Not Loaded");
            return;
        }
        delete chaosModule;
        chaosModule = nullptr;
    }
    
}
