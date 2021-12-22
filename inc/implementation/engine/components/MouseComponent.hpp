#pragma once
#include "interfaces/engine/GameComponent.hpp"
#include "implementation/engine/Engine.hpp"

#include <dinput.h>

namespace orbit {

	// @brief: class to fetch mouse inputs
	// @example:
	//	auto mHandler = DXMouseHandler::create(myInputDevice, hWnd);
	//	mHandler->buttonDownThisFrame(DXMouseHandler::MouseButton::Left);
	// @endexample
	class MouseComponent : public Updatable
	{
	public:
		// @brief: identifier for mouse buttons
		enum class MouseButton
		{
			Left,  // @member: Left mouse button
			Right, // @member: Right mouse button
			Middle // @member: Middle mouse button
		};
	private:
#if defined ORBIT_DIRECTX_11 || defined ORBIT_DIRECTX_12
		// @member: the direct input device
		ComPtr<IDirectInput8W> _directInput;
		// @member: the direct input mouse device
		ComPtr<IDirectInputDevice8W> _mouseDevice;
		// @member: current frame's mouse state
		DIMOUSESTATE _mouseState;
		// @member: previous frame's mouse state
		DIMOUSESTATE _prevMouseState;
#endif
		// @member: current mouse position
		Vector2i _mousePosition;
	public:
		// @brief: default constructor
		// @param input: the direct input device
		// @param hWnd: the window handle
		MouseComponent(GameObject* boundObject);
		// @brief: create a new Keyboard Handler
		// @param input: the direct input device
		// @param hWnd: the window handle
		// @return: a shared_ptr to the newly created Keyboard Handler
		static std::shared_ptr<MouseComponent> create(GameObject* boundObject);
		// @brief: Updates the internal state
		//	call this function every frame
		void Update(const Time&) override;
		// @brief: Returns the mouse wheel delta this frame
		// @return: mouse wheel change
		LONG wheelDelta() const;
		// @brief: Returns wether the mouse wheel has been moved
		// @return: true if the mouse wheel moved, false otherwise
		bool mouseWheelMoved() const;
		// @brief: Returns the mouse cursor position
		// @return: mouse position
		Vector2i mousePosition() const;
		// @brief: Returns the mouse movement this frame
		// @return: mouse move delta
		Vector2i mousePositionDelta() const;
		// @brief: Returns wether the mouse has been moved
		// @return: true if the mouse has been moved, false otherwise
		bool mouseMoved() const;
		// @brief: Determine if a mouse button is down
		// @param button: the mouse button to test for
		// @return: true if the button is down, false otherwise
		bool buttonDown(MouseButton button) const;
		// @brief: Determine if a mouse button is down
		// @param button: the mouse button to test for
		// @return: true if the button is up, false otherwise
		bool buttonUp(MouseButton button) const;
		// @brief: Determine if a mouse button is down
		// @param button: the mouse button to test for
		// @return: true if the button has been pressed in this frame,
		//	false otherwise
		bool buttonDownThisFrame(MouseButton button) const;
		// @brief: Determine if a mouse button is down
		// @param button: the mouse button to test for
		// @return: true if the button has been released in this frame,
		//	false otherwise
		bool buttonUpThisFrame(MouseButton button) const;
	};

}