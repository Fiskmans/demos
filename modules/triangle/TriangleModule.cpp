#include "TriangleModule.h"

#include "GL/gl.h"
#include "SDL3/SDL.h"

TriangleModule::TriangleModule(Engine* aEngine)
{
    myUpdateHandle = aEngine->OnUpdate.Register(std::bind(&TriangleModule::Update, this, std::placeholders::_1));
    myPaintHandle = aEngine->OnPaint.Register(std::bind(&TriangleModule::Paint, this, std::placeholders::_1, std::placeholders::_2));
    myImGuiHandle = aEngine->RegisterImgui("Triangle", std::bind(&TriangleModule::ImGui, this));

    float vertices []
    {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };
    
    SDL_GPUDevice* device = aEngine->GetDevice();

    SDL_GPUBufferCreateInfo bufferInfo;

    bufferInfo.props = 0;
    bufferInfo.size = sizeof(vertices);
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;

    myVertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);

    if (!myVertexBuffer)
    {
        SDL_Log("Failed to create vertex buffer: %s", SDL_GetError());
        return;
    }
    

    SDL_GPUCommandBuffer* commands = SDL_AcquireGPUCommandBuffer(device);

    if (!commands)
    {
        SDL_Log("Failed to acquire command buffer: %s", SDL_GetError());
        return;
    }

    SDL_GPUTransferBufferCreateInfo transferInfo;

    transferInfo.size = sizeof(vertices);
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

    SDL_GPUTransferBuffer* transferbuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);
    if (!transferbuffer)
    {
        SDL_Log("Failed to create transferbuffer: %s", SDL_GetError());
        return;
    }


    void* transferMemory = SDL_MapGPUTransferBuffer(device, transferbuffer, false);
    if (!transferMemory)
    {
        SDL_Log("Failed to map transferbuffer: %s", SDL_GetError());
        return;
    }


    memcpy(transferMemory, vertices, sizeof(vertices));
    SDL_UnmapGPUTransferBuffer(device, transferbuffer);

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commands);

    SDL_GPUTransferBufferLocation transferLocation;

    transferLocation.transfer_buffer = transferbuffer;
    transferLocation.offset = 0;

    SDL_GPUBufferRegion  bufferRegion;

    bufferRegion.buffer = myVertexBuffer;
    bufferRegion.offset = 0;

    SDL_UploadToGPUBuffer(copyPass, &transferLocation, &bufferRegion, false);
    SDL_EndGPUCopyPass(copyPass);
    if (!SDL_SubmitGPUCommandBuffer(commands))
    {
        SDL_Log("Failed to submit triangle setup commands: %s", SDL_GetError());
        return;
    }

    SDL_ReleaseGPUTransferBuffer(device, transferbuffer);

}

void TriangleModule::Update(Engine::TimeDelta aDelta)
{
}

void TriangleModule::Paint(SDL_GPUDevice* aDevice, SDL_GPUCommandBuffer* aCommandBuffer)
{
    
}

void TriangleModule::ImGui()
{
}
