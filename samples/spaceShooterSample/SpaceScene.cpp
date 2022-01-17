#include "SpaceScene.hpp"
#include "SpaceShip.hpp"
#include "WindowController.hpp"
#include "AsteroidController.hpp"

#include "implementation/rendering/Light.hpp"

SpaceScene::SpaceScene()
{
    auto object = std::make_shared<SpaceShip>();
    AddObject("SpaceShip", object);
    AddObject("WindowController", std::make_shared<WindowController>());
    AddObject("AsteroidController", std::make_shared<AsteroidController>());
    SetCamera(object->GetCamera());
}

bool SpaceScene::Load()
{
    if (orbit::AsyncScene::Load())
    {
        AddLight(orbit::Light::CreateDirectionalLight(
            { .9f, .7f, .65f, 1.f },
            orbit::Vector4f(0.f, 0.2f, -1.f, 1.f))
        );
        return true;
    }

    return false;
}
