#include "engine/Engine.h"

#include "SDL3/SDL.h"

#include "Engine.h"
#include <SDL3/SDL_opengl.h>
#include <dlfcn.h>
#include <filesystem>

#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlgpu3.h"

Engine::Engine(SDL_Window* aWindow, SDL_GPUDevice* aDevice, std::string aModulesDirectory)
{
	myWindow = aWindow;
	myDevice = aDevice;
	myIsShowingMainWindow = true;
	myWantsClose = false;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplSDLGPU3_InitInfo initinfo{
		.Device = aDevice,
		.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(myDevice, myWindow),
		.MSAASamples = SDL_GPU_SAMPLECOUNT_1,
		.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
		.PresentMode = SDL_GPU_PRESENTMODE_VSYNC
	};

	ImGui_ImplSDL3_InitForSDLGPU(aWindow);
	ImGui_ImplSDLGPU3_Init(&initinfo);

	FindModules(aModulesDirectory);
	myLastUpdate = Clock::now();
	myDrawImguiHandle = OnPaint.Register(std::bind(&Engine::DrawImGui, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	myImGuiHandle = RegisterImgui("Engine", std::bind(&Engine::ImGui, this));
}

Engine::~Engine()
{
	for (std::unique_ptr<Module>& mod : myModules)
	{
		if (mod->IsLoaded())
			mod->Unload();
	}

	SDL_WaitForGPUIdle(myDevice);
	ImGui_ImplSDLGPU3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}

bool Engine::WantsClose()
{
	return myWantsClose;
}

void Engine::Shutdown()
{
	myWantsClose = true;
}

bool Engine::LoadModule(std::string aName)
{
	for (std::unique_ptr<Module>& module : myModules)
	{
		if (module->Name() == aName)
		{
			module->Load(this);
			return true;
		}
	}

	return false;
}

void Engine::Update()
{
	Clock::time_point now = Clock::now();
	OnUpdate.Fire(now - myLastUpdate);
	myLastUpdate = now;
}

void Engine::Paint()
{
	ImGui_ImplSDLGPU3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	SDL_GPUCommandBuffer* commands = SDL_AcquireGPUCommandBuffer(myDevice);
	SDL_GPUTexture* swapchainTexture;
	SDL_AcquireGPUSwapchainTexture(commands, myWindow, &swapchainTexture, nullptr, nullptr);

	if (swapchainTexture) // Theres nothing to render to, i.e minimized or similar
	{

		SDL_GPUColorTargetInfo clearInfo{
			.texture = swapchainTexture,
			.clear_color{
				.r = myClearColor.x,
				.g = myClearColor.y,
				.b = myClearColor.z,
				.a = 1.f
			},
			.load_op = SDL_GPU_LOADOP_CLEAR,
			.store_op = SDL_GPU_STOREOP_STORE
		};
		SDL_GPURenderPass* clearPass = SDL_BeginGPURenderPass(commands, &clearInfo, 1, nullptr);
		if (clearPass)
			SDL_EndGPURenderPass(clearPass);
		else
			SDL_Log("Failed to clear screen: %s", SDL_GetError());


		OnPaint.Fire(myDevice, commands, swapchainTexture);
	}

	SDL_SubmitGPUCommandBuffer(commands);

}

Engine::ImGuiRegistration Engine::RegisterImgui(std::string aName, std::function<void()> aFunction)
{
	myWindows.emplace(aName, ImguiWindow{aFunction, false});

	SDL_Log("Registering imgui window: %s", aName.c_str());
	return {this, aName};
}

bool Engine::HandleEvent(SDL_Event* aEvent)
{
	switch (aEvent->type)
	{
	case SDL_EventType::SDL_EVENT_KEY_DOWN:
		switch (aEvent->key.key)
		{
		case SDLK_F1:
			myIsShowingMainWindow = !myIsShowingMainWindow;
			return true;

		default:
			break;
		}
		break;
	}

	return ImGui_ImplSDL3_ProcessEvent(aEvent);
}

SDL_GPUDevice* Engine::GetDevice()
{
	return myDevice;
}

SDL_Window* Engine::GetWindow()
{
	return myWindow;
}

void Engine::UnregisterImGui(ImGuiRegistration& aRegistration)
{
	SDL_Log("Unregistering imgui window: %s", aRegistration.myName.c_str());
	myWindows.erase(aRegistration.myName);
}

void Engine::DrawImGui(SDL_GPUDevice* aDevice, SDL_GPUCommandBuffer* aCommandBuffer, SDL_GPUTexture* aBackBuffer)
{
	ImGui_ImplSDLGPU3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
	
	{
		ImGui::Begin("Windows", &myIsShowingMainWindow);
		for (auto& [key, window] : myWindows)
		{
			if (myIsShowingMainWindow)
				ImGui::Checkbox(key.c_str(), &window.myOpen);

			if (window.myOpen)
			{
				if (ImGui::Begin(key.c_str(), &window.myOpen))
					window.myCallback();

				ImGui::End();
			}
		}
		ImGui::End();
	}

	ImGui::Render();
	
	ImGui_ImplSDLGPU3_PrepareDrawData(ImGui::GetDrawData(), aCommandBuffer);

	SDL_GPUColorTargetInfo target_info = {};
	target_info.texture = aBackBuffer;
	target_info.clear_color = SDL_FColor { myClearColor.x, myClearColor.y, myClearColor.z, myClearColor.w };
	target_info.load_op = SDL_GPU_LOADOP_LOAD;
	target_info.store_op = SDL_GPU_STOREOP_STORE;
	target_info.mip_level = 0;
	target_info.layer_or_depth_plane = 0;
	target_info.cycle = false;
	SDL_GPURenderPass* imGuiRenderPass = SDL_BeginGPURenderPass(aCommandBuffer, &target_info, 1, nullptr);

	ImGui_ImplSDLGPU3_RenderDrawData(ImGui::GetDrawData(), aCommandBuffer, imGuiRenderPass);

	SDL_EndGPURenderPass(imGuiRenderPass);
	ImGui::EndFrame();
}

void Engine::FindModules(std::string aDirectory)
{
	std::filesystem::directory_iterator iter(aDirectory);

	for (const std::filesystem::directory_entry& entry : iter)
	{
		if (!entry.exists())
			continue;

		if (entry.path().string().find(".so") == std::string::npos)
			continue;

		if (entry.path().string().find("module") == std::string::npos)
			continue;

		if (entry.status().type() != std::filesystem::file_type::regular)
			return;

		std::optional<std::unique_ptr<Module>> module = Module::TryLoadFrom(entry.path().string());

		if (!module)
			continue;

		myModules.push_back(std::move(*module));
	}
}

void Engine::ImGui()
{
	ImGui::ColorEdit3("Clear color", &myClearColor.x);

	ImGui::BeginTable("Modules", 2);

	ImGui::TableHeader("Module");
	ImGui::TableHeader("Status");

	ImGui::TableHeadersRow();

	for (std::unique_ptr<Module>& mod : myModules)
	{
		ImGui::PushID(mod.get());
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(mod->Name().c_str());

		ImGui::TableNextColumn();
		if (mod->IsLoaded())
		{
			if (ImGui::Button("Unload"))
				mod->Unload();
		}
		else
		{
			if (ImGui::Button("Load"))
				mod->Load(this);
		}
		ImGui::PopID();
	}

	ImGui::EndTable();
}

Engine::ImGuiRegistration::ImGuiRegistration(Engine* aEngine, std::string aName)
{
	myEngine = aEngine;
	myName = aName;
}

Engine::ImGuiRegistration::ImGuiRegistration(ImGuiRegistration&& aOther)
{
	myEngine = aOther.myEngine;
	myName = aOther.myName;

	aOther.myEngine = nullptr;
	aOther.myName = "";
}

Engine::ImGuiRegistration& Engine::ImGuiRegistration::operator=(ImGuiRegistration&& aOther)
{
	if (myEngine)
		myEngine->UnregisterImGui(*this);

	myEngine = aOther.myEngine;
	myName = aOther.myName;

	aOther.myEngine = nullptr;
	aOther.myName = "";

	return *this;
}

Engine::ImGuiRegistration::~ImGuiRegistration()
{
	if (myEngine)
		myEngine->UnregisterImGui(*this);
}
