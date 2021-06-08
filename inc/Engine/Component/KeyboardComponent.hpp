#pragma once
#if defined ORBIT_DX12 || defined ORBIT_DX11
#include "Engine/Bindings/DX/COM.hpp"
#else

#endif
#include "EventDriven.hpp"
#include "Component.hpp"
#include "Engine/Misc/Helper.hpp"

#include <dinput.h>

namespace orbit {

    class Object;
    using ObjectPtr = std::shared_ptr<Object>;

	class KeyboardComponent : public Component, public EventDriven
	{
	public:
		using KeyCode = unsigned;
	private:
#if defined ORBIT_DX11 || defined ORBIT_DX12
		// @brief: the direct input device
		Ptr<IDirectInput8W> _directInput;
		// @brief: the direct input keyboard device
		Ptr<IDirectInputDevice8W> _keyboardDevice;
#endif
		// @brief: key state of the current keyboard
		char _downKeys[256];
		// @brief: key state of the previous keyboard
		char _prevDownKeys[256];
	public:
		KeyboardComponent(ObjectPtr boundObject);
		static std::shared_ptr<KeyboardComponent> create(ObjectPtr boundObject);
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