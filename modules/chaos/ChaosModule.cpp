#include "modules/chaos/ChaosModule.h"
#include "modules/chaos/LorenzSystem.h"

#include <functional>

#include "ChaosModule.h"
#include "SDL3/SDL.h"
#include "imgui.h"
#include "GL/gl.h"

#include "engine/SDLHelper.h"

ChaosModule::ChaosModule(Engine* aEngine)
{
	myEngine = aEngine;
	
	myPipeline = SDLHelper::MakeSimplePipeline(aEngine->GetDevice(), aEngine->GetWindow(), "shaders/chaos/Vertex.spv", "shaders/chaos/Fragment.spv", {}, {}, {}, SDL_GPU_PRIMITIVETYPE_LINESTRIP);
	
	if (!myPipeline)
	{
		SDL_Log("Failed to make simple pipleine: %s", SDL_GetError());
		return;
	}
	
	myUpdateHandle = aEngine->OnUpdate.Register(std::bind(&ChaosModule::Update, this, std::placeholders::_1));
	myPaintHandle = aEngine->OnPaint.Register(std::bind(&ChaosModule::Paint, this, std::placeholders::_1, std::placeholders::_2));
	myImGuiHandle = aEngine->RegisterImgui("Chaos", std::bind(&ChaosModule::ImGui, this));
}

ChaosModule::~ChaosModule()
{
	SDL_ReleaseGPUGraphicsPipeline(myEngine->GetDevice(), myPipeline);
}

void ChaosModule::Update(Engine::TimeDelta aDelta)
{
	for (auto& [name, path] : myPaths)
	{
        for (size_t i = 0; i < myUpdatesPerFrame; i++)
            path->Update();
	}
}

void ChaosModule::Paint(SDL_GPUDevice* aDevice, SDL_GPUCommandBuffer* aCommandBuffer)
{
	for (auto& [name, path] : myPaths)
	{
	}
}

void ChaosModule::ImGui()
{
	static float s = 10;
	static float p = 28;
	static float b = 8.f / 3.f;

	ImGui::InputFloat("s", &s);
	ImGui::InputFloat("p", &p);
	ImGui::InputFloat("b", &b);

	if (ImGui::Button("Add"))
	{
		myPaths.emplace(std::to_string(myPaths.size()), std::make_unique<LorenzSystem>(s, p, b));
	}
	ImGui::InputInt("Updates", &myUpdatesPerFrame);

	for (auto& [name, path] : myPaths)
	{
		if (ImGui::CollapsingHeader(name.c_str()))
			path->Imgui();
	}
}
