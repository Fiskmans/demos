#include "Module.h"

#include "SDL3/SDL.h"
#include <dlfcn.h>

std::optional<std::unique_ptr<Module>> Module::TryLoadFrom(std::string aLibFile)
{
    void* handle = dlopen(aLibFile.c_str(), RTLD_GLOBAL | RTLD_NOW);

    if (!handle)
    {
        SDL_Log("could not open lib %s", dlerror());
        return {};
    }
    
    void* load = dlsym(handle, "EngineModule_Load");
    if (!load)
    {
        dlclose(handle);
        SDL_Log("%s does not define a EngineModule_Load symbol", aLibFile.c_str());
        return {};
    }
    
    void* unload = dlsym(handle, "EngineModule_Unload");
    if (!unload)
    {
        dlclose(handle);
        SDL_Log("%s does not define a EngineModule_Unload symbol", aLibFile.c_str());
        return {};
    }
    std::string name = aLibFile;

    void* namePtr = dlsym(handle, "EngineModule_Name");
    if (namePtr)
        name = *reinterpret_cast<const char**>(namePtr);
    
    return std::make_unique<Module>(name, handle, reinterpret_cast<LoadHook*>(load), reinterpret_cast<UnloadHook*>(unload));
}

Module::Module(std::string aName, void* aHandle, LoadHook* aLoad, UnloadHook* aUnload)
{
    myName = aName;
    myHandle = aHandle;
    myIsLoaded = false;
    myLoadHook = aLoad;
    myUnloadHook = aUnload;

    SDL_Log("[Module] %s Ready", myName.c_str());
}

Module::~Module()
{
    dlclose(myHandle);
}

bool Module::Load(Engine* aEngine)
{
    if (!myIsLoaded)
    {
        if (!myLoadHook(EngineVersion, aEngine))
        {
            SDL_Log("[Module] Failed to load %s", myName.c_str());
            return false;
        }

        SDL_Log("[Module] %s Loaded", myName.c_str());
        myIsLoaded = true;
    }
    
	return true;
}

void Module::Unload()
{
    if (!myIsLoaded)
        return;

    myUnloadHook();
    SDL_Log("[Module] %s Unloaded", myName.c_str());
    myIsLoaded = false;
}

bool Module::IsLoaded()
{
	return myIsLoaded;
}
