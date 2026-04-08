
#include "engine/Engine.h"

class TriangleModule
{
public:
	TriangleModule(Engine* aEngine);
    ~TriangleModule();

    void Update(Engine::TimeDelta aDelta);
    void Paint(SDL_GPUDevice* aDevice, SDL_GPUCommandBuffer* aCommandBuffer, SDL_GPUTexture* aSwapTexture);

    void ImGui();
private:
    Engine* myEngine = nullptr;

    SDL_GPUBuffer* myVertexBuffer = nullptr;
    SDL_GPUGraphicsPipeline* myGraphicsPipeline = nullptr;

    fisk::tools::EventReg myPaintHandle;
    fisk::tools::EventReg myUpdateHandle;
    Engine::ImGuiRegistration myImGuiHandle;
};