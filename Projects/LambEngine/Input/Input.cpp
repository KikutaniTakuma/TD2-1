#include "Input.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Utils/ExecutionLog/ExecutionLog.h"

Input* Input::instance_ = nullptr;

void Input::Initialize() {
	instance_ = new Input();
}
void Input::Finalize() {
	delete instance_;
	instance_ = nullptr;
}

Input::Input():
	gamepad_(nullptr),
	key_(nullptr),
	mouse_(nullptr)
{
	HRESULT hr = DirectInput8Create(
		WindowFactory::GetInstance()->GetWNDCLASSEX().hInstance, 
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(directInput_.GetAddressOf()),
		nullptr
	);
	assert(SUCCEEDED(hr));
	if (SUCCEEDED(hr)) {
		Lamb::AddLog("DirectInput create succeeded\n");
	}
	else {
		Lamb::ErrorLog("DirectInput8Create() Failed", "InitializeInput()", "Engine");
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
	if (WindowFactory::GetInstance()->IsThisWindowaActive()) {
		gamepad_->Input();
		key_->Input();
		mouse_->Input();
	}
	else {
		gamepad_->InputReset();
		key_->InputReset();
		mouse_->InputReset();
	}
}