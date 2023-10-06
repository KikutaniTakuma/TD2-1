#include "AudioManager.h"
#include "Engine/ErrorCheck/ErrorCheck.h"
#include <cassert>
#include <filesystem>

AudioManager* AudioManager::instance = nullptr;
void AudioManager::Inititalize() {
	instance = new AudioManager();
	if (instance == nullptr) {
		ErrorCheck::GetInstance()->ErrorTextBox("Inititalize() : Instance failed", "AudioManager");
	}
}
void AudioManager::Finalize() {
	delete instance;
	instance = nullptr;
}

AudioManager::AudioManager() :
	xAudio2(),
	masterVoice(nullptr)
{
	HRESULT hr = XAudio2Create(xAudio2.GetAddressOf(), 0u, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("Constructor : XAudio2Create() failed", "AudioManager");
	}

	hr = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(hr));
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("Constructor : CreateMasteringVoicey() failed", "AudioManager");
	}
}
AudioManager::~AudioManager() {
	xAudio2.Reset();
}

Audio* AudioManager::LoadWav(const std::string& fileName, bool loopFlg) {
	if (!std::filesystem::exists(std::filesystem::path(fileName))) {
		ErrorCheck::GetInstance()->ErrorTextBox("LoadWav() Fialed : There is not this file -> " + fileName, "AudioManager");
	}

	if (audios.empty()) {
		auto audio = std::make_unique<Audio>();
		audio->Load(fileName, loopFlg);
		audios.insert({ fileName, std::move(audio) });
	}
	else {
		auto itr = audios.find(fileName);

		if (itr == audios.end()) {
			auto audio = std::make_unique<Audio>();
			audio->Load(fileName, loopFlg);
			audios.insert({ fileName, std::move(audio) });
		}
	}

	return audios[fileName].get();
}