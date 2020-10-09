#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "Orbit.lib")

#include <iostream>
#include "engine.hpp"

using namespace orbit;

int main()
{
	try {
		auto desc = EngineDesc::DefaultDescriptor();
		desc.title = L"Simple Window Sample";
		auto engine = Engine::Create(&desc);
		
		auto scene = Scene::Create();
		engine->SetScene(scene);

		engine->Run();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << '\n';
	}
}
