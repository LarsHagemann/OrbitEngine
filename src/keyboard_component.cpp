#include "keyboard_component.hpp"
#include "exception.hpp"
#include "engine.hpp"

#define KEYDOWN(name, key) (name[static_cast<char>(key)] & 0x80)

namespace orbit {

	KeyboardComponent::KeyboardComponent(EnginePtr engine, ObjectPtr boundObject) :
		Component(engine, boundObject),
		_directInput(engine->GetInputDevice())
	{
		ORBIT_INFO_LEVEL(FormatString("Creating KeyboardComponent."), 5);

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
				_engine->GetHandle(),
				/*DISCL_FOREGROUND*/ DISCL_BACKGROUND | DISCL_NONEXCLUSIVE
		), "Failed to set device cooperative level");

		ORBIT_THROW_IF_FAILED(_keyboardDevice->Acquire(), "Failed to acquire keyboard device");
	}

	std::shared_ptr<KeyboardComponent> KeyboardComponent::create(EnginePtr engine, ObjectPtr boundObject)
	{
		return std::make_shared<KeyboardComponent>(engine, boundObject);
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