#include "ViewerScene.hpp"
#include "ViewerUI.hpp"
#include "3DView.hpp"

ViewerScene::ViewerScene()
{
    auto ui = std::make_shared<ViewerUI>();
    auto o3dview = std::make_shared<O3DView>();
    AddObject("Viewer", ui);
    AddObject("3DView", o3dview);

    SetCamera(o3dview->GetCamera());
}

bool ViewerScene::Load()
{
    if (orbit::AsyncScene::Load())
    {
        AddLight(orbit::Light::CreateDirectionalLight(
            Eigen::Vector4f::Ones(),
            Eigen::Vector4f{ -1.f, -1.f, -1.f, 0.f }.normalized()
        ));
        return true;
    }
    return false;
}
