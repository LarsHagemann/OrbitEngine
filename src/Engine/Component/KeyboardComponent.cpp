#include "Engine/Engine.hpp"
#include "Engine/Component/KeyboardComponent.hpp"
#include "Engine/Misc/Logger.hpp"

#define KEYDOWN(name, key) (name[static_cast<char>(key)] & 0x80)

namespace orbit {

	KeyboardComponent::KeyboardComponent(ObjectPtr boundObject) :
		Component(boundObject),
		EventDriven(),
		_directInput(Engine::Get()->GetDirectInput())
	{
		ORBIT_INFO_LEVEL(FormatString("Creating KeyboardComponent."), 5);

		set_flag(EventDrivenFlag);

		ZeroMemory(_downKeys, sizeof(_downKeys));
		ZeroMemory(_prevDownKeys, sizeof(_prevDownKeys));

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
				Engine::Get()->GetWindow()->GetHandle(),
				/*DISCL_FOREGROUND*/ DISCL_BACKGROUND | DISCL_NONEXCLUSIVE
		), "Failed to set device cooperative level");

		ORBIT_THROW_IF_FAILED(_keyboardDevice->Acquire(), "Failed to acquire keyboard device");
	}

	std::shared_ptr<KeyboardComponent> KeyboardComponent::create(ObjectPtr boundObject)
	{
		return std::make_shared<KeyboardComponent>(boundObject);
	}

	void KeyboardComponent::Update(Time)
	{
		memcpy_s(_prevDownKeys, sizeof(_prevDownKeys), _downKeys, sizeof(_downKeys));
		_keyboardDevice->GetDeviceState(sizeof(_downKeys), (LPVOID)&_downKeys);
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