#pragma once
#include "event_driven.hpp"
#include "component.hpp"
#include "helper.hpp"
#include <dinput.h>

namespace orbit {

	class KeyboardComponent : public EventDriven, public Component
	{
	public:
		using KeyCode = unsigned;
	private:
		// @brief: the direct input device
		wrl::ComPtr<IDirectInput8W> _directInput;
		// @brief: the direct input keyboard device
		wrl::ComPtr<IDirectInputDevice8W> _keyboardDevice;
		// @brief: key state of the current keyboard
		char _downKeys[256];
		// @brief: key state of the previous keyboard
		char _prevDownKeys[256];
	public:
		KeyboardComponent(EnginePtr engine, ObjectPtr boundObject);
		static std::shared_ptr<KeyboardComponent> create(EnginePtr engine, ObjectPtr boundObject);
		// @brief: Update the keyboard state
		//	call this function every frame
		void Update(Time dt) override;
		// @brief: Determine if a key is down
		// @param key: the key code to test for
		// @return: true if the key is down, false otherwise
		bool keydown(KeyCode key);
		// @brief: Determine if a key is not down
		// @param key: the key code to test for
		// @return: true if the key is up, false otherwise
		bool keyup(KeyCode key);
		// @brief: Determine if a key has been pressed this frame
		// @param key: the key code to test for
		// @return: true if the key has been pressed in this frame, 
		//	false otherwise
		bool keydownThisFrame(KeyCode key);
		// @brief: Determine if a key has been released this frame
		// @param key: the key code to test for
		// @return: true if the key has been released in this frame, 
		//	false otherwise
		bool keyupThisFrame(KeyCode key);
	};

}