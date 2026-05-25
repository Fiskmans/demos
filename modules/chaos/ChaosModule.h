#pragma once

#include "engine/Engine.h"
#include "tools/Event.h"

#include "modules/chaos/ChaoticPath.h"

class ChaosModule
{
public:
    ChaosModule(Engine* aEngine);
    ~ChaosModule();

    void Update(Engine::TimeDelta aDelta);
    void Paint(SDL_GPUDevice* aDevice, SDL_GPUCommandBuffer* aCommandBuffer);
    void ImGui();
    
private:
    Engine* myEngine;
    SDL_GPUGraphicsPipeline* myPipeline;

    int myUpdatesPerFrame = 0;
    std::unordered_map<std::string, std::unique_ptr<ChaoticPath>> myPaths;
    fisk::tools::EventReg myUpdateHandle;
    fisk::tools::EventReg myPaintHandle;
    Engine::ImGuiRegistration myImGuiHandle;
};