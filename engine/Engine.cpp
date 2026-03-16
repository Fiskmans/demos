#include "engine/Engine.h"

#include "SDL3/SDL.h"

#include "Engine.h"
#include <dlfcn.h>
#include <filesystem>
#include <SDL3/SDL_opengl.h>

#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"

Engine::Engine(SDL_Window* aWindow, SDL_GLContext aContext, std::string aModulesDirectory)
{
	myWindow = aWindow;
	myContext = aContext;
	myIsShowingMainWindow = true;

    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplSDL3_InitForOpenGL(aWindow, aContext);
    ImGui_ImplOpenGL3_Init(OpenGLVersion);

	FindModules(aModulesDirectory);
	myLastUpdate = Clock::now();

	auto t = OnPaint.Register([]() {});
}

Engine::~Engine()
{
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
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
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	OnPaint.Fire();

	if (myIsShowingMainWindow)
    {
        if (ImGui::Begin("Windows", &myIsShowingMainWindow))
        {
            for (auto& [key, window] : myWindows)
            {
                ImGui::Checkbox(key.c_str(), &window.myOpen);
                
                if (window.myOpen)
                {
                    if (ImGui::Begin(key.c_str(), &window.myOpen))
					window.myCallback();
                    
                    ImGui::End();
                }
            }
        }
        ImGui::End();
    }

    ImGuiIO& io = ImGui::GetIO();
	ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(myClearColor.x * myClearColor.w, myClearColor.y * myClearColor.w, myClearColor.z * myClearColor.w, myClearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Engine::RegisterImgui(std::string aName, std::function<void()> aFunction)
{
	myWindows.emplace(aName, ImguiWindow{aFunction, false});
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