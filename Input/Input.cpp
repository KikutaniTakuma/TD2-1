#include "Input.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Input/Gamepad/Gamepad.h"
#include "Input/KeyInput/KeyInput.h"
#include "Input/Mouse/Mouse.h"

Input* Input::instance_ = nullptr;

void Input::Initialize() {
	instance_ = new Input();
}
void Input::Finalize() {
	delete instance_;
	instance_ = nullptr;
}

Input::Input() {
	HRESULT hr = DirectInput8Create(WinApp::GetInstance()->getWNDCLASSEX().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		reinterpret_cast<void**>(directInput_.GetAddressOf()), nullptr);
	assert(SUCCEEDED(hr));
	if (hr != S_OK) {
		ErrorCheck::GetInstance()->ErrorTextBox("InitializeInput() : DirectInput8Create() Failed", "Engine");
		return;
	}

	KeyInput::Initialize(directInput_.Get());
	Mouse::Initialize(directInput_.Get());

	gamepad_ = Gamepad::GetInstance();
	key_ = KeyInput::GetInstance();
	mouse_ = Mouse::GetInstance();
}

Input::~Input() {
	Mouse::Finalize();
	KeyInput::Finalize();
	directInput_.Reset();
}

void Input::InputStart() {
	gamepad_->Input();
	key_->Input();
	mouse_->Input();
}