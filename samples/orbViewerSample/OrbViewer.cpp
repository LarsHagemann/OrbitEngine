#include "implementation/backends/Platform.hpp"
#include "implementation/engine/Engine.hpp"

#include "ViewerScene.hpp"

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
		ENGINE->RMParseFile("orbViewerAssets.orb");

        auto scene = std::make_shared<ViewerScene>();
        ENGINE->RegisterScene("Viewer", scene);
        ENGINE->EnterScene("Viewer");

		ENGINE->Run();
		ENGINE->Shutdown();
	}

	shutdown_monitoring();
	return 0;
}
