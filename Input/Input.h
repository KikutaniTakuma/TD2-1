#pragma once
#include <wrl.h>
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#include "Input/Gamepad/Gamepad.h"
#include "Input/KeyInput/KeyInput.h"
#include "Input/Mouse/Mouse.h"
#include <cassert>

class Input {
private:
	Input();
	Input(const Input&) = delete;
	Input(Input&&) = delete;
	~Input();

	Input& operator=(const Input&) = delete;
	Input& operator=(Input&&) = delete;

public:
	static void Initialize();
	static void Finalize();

	static Input* const GetInstance() {
		assert(instance_);
		return instance_;
	}

private:
	static Input* instance_;

public:
	void InputStart();

private:
	Microsoft::WRL::ComPtr<IDirectInput8> directInput_;

	Gamepad* gamepad_;
	KeyInput* key_;
	Mouse* mouse_;

public:
	inline Gamepad* const GetGamepad() const {
		return gamepad_;
	}
	inline KeyInput* const GetKey() const {
		return key_;
	}
	inline Mouse* const GetMouse() const {
		return mouse_;
	}
};