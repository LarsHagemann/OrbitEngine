#include "implementation/engine/components/MouseComponent.hpp"

#if defined ORBIT_DIRECTX_11 || defined ORBIT_DIRECTX_12
#define BUTTONDOWN(name, key) (name.rgbButtons[static_cast<unsigned>(key)] & 0x80)
#endif

namespace orbit {

	MouseComponent::MouseComponent(GameObject* boundObject) :
		Updatable(boundObject),
		_directInput(ENGINE->DirectInput())
	{
		ORBIT_INFO_LEVEL(ORBIT_LEVEL_MISC, "Creating MouseComponent");

#if defined ORBIT_DIRECTX_11 || defined ORBIT_DIRECTX_12
		ZeroMemory(&_mouseState, sizeof(_mouseState));
		ZeroMemory(&_prevMouseState, sizeof(_prevMouseState));

		ORBIT_THROW_IF_FAILED(_directInput->CreateDevice(
			GUID_SysMouse,
			(LPDIRECTINPUTDEVICE8W*)&_mouseDevice,
			0
		), "Failed to create the input mouse device");

		ORBIT_THROW_IF_FAILED(
			_mouseDevice->SetDataFormat(&c_dfDIMouse),
			"Failed to set the mouse device data format"
		);

		ORBIT_THROW_IF_FAILED(_mouseDevice->SetCooperativeLevel(
			ENGINE->Window()->GetHandle(),
			/*DISCL_FOREGROUND*/ DISCL_BACKGROUND | DISCL_NONEXCLUSIVE
		), "Failed to set the device cooperative level");

		ORBIT_THROW_IF_FAILED(_mouseDevice->Acquire(), "Failed to acquire mouse device");
#endif
	}

	std::shared_ptr<MouseComponent> MouseComponent::create(GameObject* boundObject)
	{
		return std::make_shared<MouseComponent>(boundObject);
	}

	void MouseComponent::Update(const Time&)
	{
		memcpy_s(&_prevMouseState, sizeof(_prevMouseState), &_mouseState, sizeof(_mouseState));
#if defined ORBIT_DIRECTX_11 || defined ORBIT_DIRECTX_12
		_mouseDevice->GetDeviceState(sizeof(_mouseState), (LPVOID)&_mouseState);
		_mousePosition.x() += _mouseState.lX;
		_mousePosition.y() += _mouseState.lY;
#endif
	}

	LONG MouseComponent::wheelDelta() const
	{
		return _mouseState.lZ;
	}

	bool MouseComponent::mouseWheelMoved() const
	{
		return wheelDelta() != 0;
	}

	Vector2i MouseComponent::mousePosition() const
	{
		return _mousePosition;
	}

	Vector2i MouseComponent::mousePositionDelta() const
	{
		return Vector2i{
			_mouseState.lX,
			_mouseState.lY
		};
	}

	bool MouseComponent::mouseMoved() const
	{
		auto tmp = mousePositionDelta();
		return tmp.x() != 0 || tmp.y() != 0;
	}

	bool MouseComponent::buttonDown(MouseButton button) const
	{
		return BUTTONDOWN(_mouseState, button);
	}

	bool MouseComponent::buttonUp(MouseButton button) const
	{
		return !BUTTONDOWN(_mouseState, button);
	}

	bool MouseComponent::buttonDownThisFrame(MouseButton button) const
	{
		return BUTTONDOWN(_mouseState, button) && !BUTTONDOWN(_prevMouseState, button);
	}

	bool MouseComponent::buttonUpThisFrame(MouseButton button) const
	{
		return !BUTTONDOWN(_mouseState, button) && BUTTONDOWN(_prevMouseState, button);
	}

}