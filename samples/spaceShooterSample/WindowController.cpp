#include "WindowController.hpp"

void WindowController::Init()
{
    m_kCom = AddComponent<orbit::KeyboardComponent>("keyboard");
}

void WindowController::Update(const orbit::Time& dt)
{
	orbit::GameObject::Update(dt);
	if (m_kCom->keydownThisFrame(orbit::KeyCode::KEY_F11))
	{
		m_isFullscreen = !m_isFullscreen;
		ENGINE->Window()->ToggleFullscreen(m_isFullscreen);
	}
	if (m_kCom->keydownThisFrame(orbit::KeyCode::KEY_ESCAPE))
		ENGINE->Window()->Close();
}
