#include "implementation/backends/Platform.hpp"
#include "implementation/engine/Engine.hpp"

#include "SpaceScene.hpp"

//#undef new

int main()
{
	{
		orbit::EngineInitDesc desc;
		desc.msaa = 8;
		desc.numThreads = 1;

		auto window = orbit::Window::Create({ 1080, 720 }, L"Simple Sample");
		ENGINE->Init(window, desc);
        ENGINE->SetClearColor(20, 10, 20);
		ENGINE->EnableVSync(true);
		ENGINE->RMParseFile("spaceShooterAssets.orb");

		auto scene = std::make_shared<SpaceScene>();
		ENGINE->RegisterScene("SpaceScene", scene);
		ENGINE->EnterScene("SpaceScene");

		ENGINE->Run();

		//ENGINE->Device()->
		ENGINE->Shutdown();
	}

	shutdown_monitoring();
	return 0;
}
