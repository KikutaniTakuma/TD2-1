#pragma once
#include <array>
#include <wrl.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

/// <summary>
/// キーボードの入力
/// </summary>
class KeyInput {
private:
	KeyInput() = delete;
	KeyInput(const KeyInput&) = delete;
	KeyInput(KeyInput&&) = delete;
	KeyInput(IDirectInput8* input);
	~KeyInput();
	KeyInput& operator=(const KeyInput&) = delete;
	KeyInput& operator=(KeyInput&&) = delete;
	

public:
	void Input();

	bool GetKey(uint8_t keyType) {
		return (instance->key[keyType] & 0x80);
	}

	bool GetPreKey(uint8_t keyType) {
		return (instance->preKey[keyType] & 0x80);
	}

	bool Pushed(uint8_t keyType);
	bool LongPush(uint8_t keyType);
	bool Released(uint8_t keyType);

	/// <summary>
	/// 何かしらのキーが押された
	/// </summary>
	/// <returns>押されたらtrue</returns>
	bool PushAnyKey();

public:
	static void Initialize(IDirectInput8* input);
	static void Finalize();

private:
	static KeyInput* instance;

public:
	static KeyInput* const GetInstance() {
		return instance;
	}


private:
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyBoard;

	// キー入力バッファー
	std::array<BYTE, 0x100> key;
	std::array<BYTE, 0x100> preKey;

	bool initalizeSucceeded;
};