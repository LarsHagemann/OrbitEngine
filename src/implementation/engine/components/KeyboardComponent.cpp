#include "implementation/engine/components/KeyboardComponent.hpp"
#include "implementation/engine/Engine.hpp"

#if defined ORBIT_DIRECTX_11 || defined ORBIT_DIRECTX_12
#define KEYDOWN(name, key) (name[static_cast<char>(key)] & 0x80)
#endif

namespace orbit {

	KeyboardComponent::KeyboardComponent(GameObject* boundObject) :
		Updatable(boundObject),
#if defined ORBIT_DIRECTX_11 || defined ORBIT_DIRECTX_12
		_directInput(ENGINE->DirectInput())
#endif
	{
		ORBIT_INFO_LEVEL(ORBIT_LEVEL_MISC, "Creating KeyboardComponent.");

		ZeroMemory(_downKeys, sizeof(_downKeys));
		ZeroMemory(_prevDownKeys, sizeof(_prevDownKeys));

#if defined ORBIT_DIRECTX_11 || defined ORBIT_DIRECTX_12
		ORBIT_THROW_IF_FAILED(_directInput->CreateDevice(
			GUID_SysKeyboard,
			(LPDIRECTINPUTDEVICE8W*)&_keyboardDevice,
			0
		), "Failed to create directX keyboard device");

		ORBIT_THROW_IF_FAILED(
			_keyboardDevice->SetDataFormat(&c_dfDIKeyboard),
			"Failed to set the keyboard data format"
		);

		ORBIT_THROW_IF_FAILED(
			_keyboardDevice->SetCooperativeLevel(
				ENGINE->Window()->GetHandle(),
				/*DISCL_FOREGROUND*/ DISCL_BACKGROUND | DISCL_NONEXCLUSIVE
		), "Failed to set device cooperative level");

		ORBIT_THROW_IF_FAILED(_keyboardDevice->Acquire(), "Failed to acquire keyboard device");
#endif
	}

	std::shared_ptr<KeyboardComponent> KeyboardComponent::create(GameObject* boundObject)
	{
		return std::make_shared<KeyboardComponent>(boundObject);
	}

	void KeyboardComponent::Update(const Time&)
	{
		memcpy_s(_prevDownKeys, sizeof(_prevDownKeys), _downKeys, sizeof(_downKeys));
#if defined ORBIT_DIRECTX_11 || defined ORBIT_DIRECTX_12
		_keyboardDevice->GetDeviceState(sizeof(_downKeys), (LPVOID)&_downKeys);
#endif
	}

	bool KeyboardComponent::keydown(KeyCode key)
	{
		return KEYDOWN(_downKeys, key);
	}

	bool KeyboardComponent::keyup(KeyCode key)
	{
		return !KEYDOWN(_downKeys, key);
	}

	bool KeyboardComponent::keydownThisFrame(KeyCode key)
	{
		return KEYDOWN(_downKeys, key) && !KEYDOWN(_prevDownKeys, key);
	}

	bool KeyboardComponent::keyupThisFrame(KeyCode key)
	{
		return !KEYDOWN(_downKeys, key) && KEYDOWN(_prevDownKeys, key);
	}

}