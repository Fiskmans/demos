
#include "engine/Engine.h"

class TriangleModule
{
public:
	TriangleModule(Engine* aEngine);

    void Update(Engine::TimeDelta aDelta);
    void Paint(SDL_GPUDevice* aDevice, SDL_GPUCommandBuffer* aCommandBuffer);

    void ImGui();
private:
    SDL_GPUBuffer* myVertexBuffer;

    fisk::tools::EventReg myPaintHandle;
    fisk::tools::EventReg myUpdateHandle;
    Engine::ImGuiRegistration myImGuiHandle;
};