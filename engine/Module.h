#pragma once

#include <memory>
#include <optional>
#include <string>

class Engine;

class Module
{
public:
    using LoadHook = bool(int, Engine*);
    using UnloadHook = void();

    static constexpr int EngineVersion = 1;

    static std::optional<std::unique_ptr<Module>>
    TryLoadFrom(std::string aLibFile);

    Module(std::string aName, void* aHandle, LoadHook* aLoad, UnloadHook* aUnload);
    ~Module();

    const std::string& Name() const { return myName; }
    bool Load(Engine* aEngine);
    void Unload();

private:
    std::string myName;
    void* myHandle;
    LoadHook* myLoadHook;
    UnloadHook* myUnloadHook;
    bool myIsLoaded;
};