#include "KeyInput.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include <cassert>
#include "Utils/ExecutionLog/ExecutionLog.h"

void KeyInput::Input() {
	if (!initalizeSucceeded) {
		return;
	}

	std::copy(key.begin(), key.end(), preKey.begin());
	
	// キーボード情報取得開始
	keyBoard->Acquire();

	// キー入力
	key = { 0 };
	keyBoard->GetDeviceState(DWORD(key.size()), key.data());
}

bool KeyInput::Pushed(uint8_t keyType) {
	if (!initalizeSucceeded) {
		return false;
	}
	return (key[keyType] & 0x80) && !(preKey[keyType] & 0x80);
}
bool KeyInput::LongPush(uint8_t keyType) {
	if (!initalizeSucceeded) {
		return false;
	}
	return (key[keyType] & 0x80) && (preKey[keyType] & 0x80);
}
bool KeyInput::Released(uint8_t keyType) {
	if (!initalizeSucceeded) {
		return false;
	}
	return !(key[keyType] & 0x80) && (preKey[keyType] & 0x80);
}

bool KeyInput::PushAnyKey() {
	return key != preKey;
}

KeyInput* KeyInput::instance = nullptr;

void KeyInput::Initialize(IDirectInput8* input) {
	assert(input);
	instance = new KeyInput(input);
}
void KeyInput::Finalize() {
	delete instance;
	instance = nullptr;
}

KeyInput::KeyInput(IDirectInput8* input):
	keyBoard(),
	key{0},
	preKey{0},
	initalizeSucceeded(false)
{
	
	HRESULT hr = input->CreateDevice(GUID_SysKeyboard, keyBoard.GetAddressOf(), NULL);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		Log::ErrorLog("CreateDevice failed","Constructor", "KeyInput");
		return;
	}

	hr = keyBoard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		Log::ErrorLog("SetDataFormat failed", "Constructor", "KeyInput");
		return;
	}

	hr = keyBoard->SetCooperativeLevel(WindowFactory::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		Log::ErrorLog("SetCooperativeLevel failed", "Constructor", "KeyInput");
		return;
	}

	initalizeSucceeded = true;
}

KeyInput::~KeyInput() {
	keyBoard->Unacquire();
}