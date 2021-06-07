#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DXGI.lib")

// Set the renderer you want to use here:
// _DX12, _DX11, _OPENGL
// This must match the renderer you compiled Orbit with
#define ORBIT_DX12

#include <iostream>
#include <Engine/Window.hpp>
#include <Engine/Engine.hpp>
#include <Engine/Scene.hpp>
#include <Engine/Rendering/Cameras/ThirdPersonCamera.hpp>

std::shared_ptr<orbit::Engine> EngineInit();

int main()
{
	try {
		auto engine = EngineInit();
		engine->Run();
	}
	catch (orbit::Exception& e)
	{
		std::cout << e.what() << '\n';
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << '\n';
	}
}

std::shared_ptr<orbit::Engine> EngineInit()
{
	auto window = orbit::Window::Create({ 1080, 600 }, L"###PROJECT_NAME###");
	auto desc = orbit::InitDesc::GetDefaultDesc();
	auto engine = orbit::Engine::Init(window, &desc, L"###PROJECT_NAME###");

	auto scene = orbit::Scene::Create();
	auto camera = orbit::ThirdPersonCamera::Create();
	scene->SetCamera(camera);
	engine->SetScene(scene);

	return engine;
}