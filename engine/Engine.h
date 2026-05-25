#pragma once

#include "GL/gl.h"
#include "SDL3/SDL.h"
#include "engine/Module.h"
#include "imgui.h"
#include "tools/Event.h"

#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

class Engine
{
	using Clock = std::chrono::steady_clock;

public:
	static constexpr const char* OpenGLVersion = "#version 130";
	static constexpr int OpenGLMajorVersionNumber = 3;
	static constexpr int OpenGLMinorVersionNumber = 0;

	struct ImGuiRegistration
	{
        ImGuiRegistration() = default;
        ImGuiRegistration(Engine* aEngine, std::string aName);
        ImGuiRegistration(const ImGuiRegistration&) = delete;
        ImGuiRegistration(ImGuiRegistration&& aOther);
        ImGuiRegistration& operator= (ImGuiRegistration&& aOther);
		~ImGuiRegistration();
		Engine* myEngine = nullptr;
		std::string myName;
	};

	using TimeDelta = std::chrono::duration<float, std::ratio<1, 1>>;

	Engine(SDL_Window* aWindow, SDL_GPUDevice* aDevice, std::string aModulesDirectory);
	~Engine();

	bool WantsClose();

	void Shutdown();
	bool LoadModule(std::string aName);

	void Update();
	void Paint();
    SDL_NODISCARD
	ImGuiRegistration RegisterImgui(std::string aName, std::function<void()> aFunction);

	bool HandleEvent(SDL_Event* aEvent);

	fisk::tools::Event<TimeDelta> OnUpdate;
	fisk::tools::Event<SDL_GPUDevice*, SDL_GPUCommandBuffer*, SDL_GPUTexture*> OnPaint;

    SDL_GPUDevice* GetDevice();
    SDL_Window* GetWindow();

private:
	friend ImGuiRegistration;
	void UnregisterImGui(ImGuiRegistration& aRegistration);
	void DrawImGui(SDL_GPUDevice* aDevice, SDL_GPUCommandBuffer* aCommandBuffer, SDL_GPUTexture* aBackBuffer);
	void FindModules(std::string aDirectory);

	void ImGui();

	struct ImguiWindow
	{
		std::function<void()> myCallback;
		bool myOpen;
	};

	SDL_Window* myWindow;
	SDL_GPUDevice* myDevice;

	Clock::time_point myLastUpdate;

	bool myIsShowingMainWindow;
	bool myWantsClose;
	ImVec4 myClearColor;
	fisk::tools::EventReg myDrawImguiHandle;
	std::unordered_map<std::string, ImguiWindow> myWindows;
	std::vector<std::unique_ptr<Module>> myModules;
    ImGuiRegistration myImGuiHandle;
};