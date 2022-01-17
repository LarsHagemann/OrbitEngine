#pragma once
#include "implementation/engine/components/KeyboardComponent.hpp"
#include "implementation/engine/GameObject.hpp"

class WindowController : public orbit::GameObject
{
private:
    std::shared_ptr<orbit::KeyboardComponent> m_kCom;

    bool m_isFullscreen;
public:
    bool LoadImpl(std::ifstream*) override { 
		return true;
	}
	void UnloadImpl() override {}

    virtual void Init() override;
    virtual void Update(const orbit::Time& dt) override;
};
