#pragma once
#include "implementation/rendering/ArcBallCamera.hpp"
#include "implementation/engine/components/KeyboardComponent.hpp"
#include "implementation/engine/components/MouseComponent.hpp"
#include "implementation/engine/components/StaticBatchComponent.hpp"
#include "implementation/engine/GameObject.hpp"

class O3DView : public orbit::GameObject
{
private:
    std::shared_ptr<orbit::KeyboardComponent> m_keyboard;
    std::shared_ptr<orbit::MouseComponent> m_mouse;
    std::shared_ptr<orbit::ArcBallCamera> m_camera;
    std::shared_ptr<orbit::StaticBatchComponent> m_batch;
    orbit::TransformPtr m_focus;
public:
    void LoadGeometry(const std::string& id);
    void LoadTexture(const std::string& id);
    void LoadMaterial(const std::string& id);

    bool LoadImpl(std::ifstream*) override { return true; }
	void UnloadImpl() override {}

    virtual void Init() override;
    virtual void Update(const orbit::Time& dt) override;

    std::shared_ptr<orbit::ICamera> GetCamera() const { return m_camera; }
};
