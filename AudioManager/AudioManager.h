#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <wrl.h>
#include <memory>
#include <unordered_map>
#include "Audio/Audio.h"

class AudioManager {
	friend Audio;

private:
	AudioManager();
	AudioManager(const AudioManager&) = delete;
	AudioManager(AudioManager&&) noexcept = delete;
	~AudioManager();
	AudioManager& operator=(const AudioManager&) = delete;
	AudioManager& operator=(AudioManager&&) noexcept = delete;

public:
	static void Inititalize();
	static void Finalize();
	static inline AudioManager* GetInstance() {
		return instance;
	}

private:
	static AudioManager* instance;

public:
	class Audio* LoadWav(const std::string& fileName, bool loopFlg);

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;

	std::unordered_map<std::string, std::unique_ptr<class Audio>> audios;
};