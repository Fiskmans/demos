
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

    void UploadVertexBuffer(SDL_GPUCommandBuffer* aCommandBuffer);

    template<typename T>
    static void SerializeVar(std::byte*& aInOutDestination, T&& aValue)
    {
        memcpy(aInOutDestination, &aValue, sizeof(aValue));
        aInOutDestination += sizeof(aValue);
    }

    struct Vertex
    {
        static constexpr size_t SerializedSize = sizeof(float) * 8;
        float myPos[4];
        float myColor[4];

        void Serialize(std::byte*& aInOutDestination);
        bool ImGui();
    };
    
    struct Tri
    {
        static constexpr size_t SerializedSize = Vertex::SerializedSize * 3;
        Vertex myA;
        Vertex myB;
        Vertex myC;

        void Serialize(std::byte*& aInOutDestination);
        bool ImGui();
    };

    Tri myTri;

    Engine* myEngine = nullptr;

    SDL_GPUBuffer* myVertexBuffer = nullptr;
    SDL_GPUGraphicsPipeline* myGraphicsPipeline = nullptr;
    SDL_GPUTransferBuffer* myVertexTransferBuffer = nullptr;
    bool myNeedsVertexUpload;

    fisk::tools::EventReg myPaintHandle;
    fisk::tools::EventReg myUpdateHandle;
    Engine::ImGuiRegistration myImGuiHandle;
};