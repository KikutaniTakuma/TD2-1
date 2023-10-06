#include "Audio.h"
#include <fstream>
#include "AudioManager/AudioManager.h"
#include "Engine/ErrorCheck/ErrorCheck.h"

Audio::Audio():
	wfet(),
	pBuffer(nullptr),
	bufferSize(0u),
	pSourceVoice(nullptr),
	loopFlg(false),
	isStart(false)
{}

Audio::~Audio() {
	delete[] pBuffer;

	pBuffer = nullptr;
	bufferSize = 0;
	wfet = {};
}

void Audio::Load(const std::string& fileName, bool loopFlg_) {
	std::ifstream file(fileName, std::ios::binary);
	if (!file) {
		ErrorCheck::GetInstance()->ErrorTextBox("Load() : Not found file", "Audio");
	}

	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	if (strncmp(riff.chunk.id.data(), "RIFF", 4) != 0) {
		ErrorCheck::GetInstance()->ErrorTextBox("Load() : Not found RIFF", "Audio");
		return;
	}
	if (strncmp(riff.type.data(), "WAVE", 4) != 0) {
		ErrorCheck::GetInstance()->ErrorTextBox("Load() : Not found WAVE", "Audio");
		return;
	}

	FormatChunk format{};
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id.data(), "fmt ", 4) != 0) {
		ErrorCheck::GetInstance()->ErrorTextBox("Load() : Not found fmt", "Audio");
		return;
	}

	if (format.chunk.size > sizeof(format.fmt)) {
		ErrorCheck::GetInstance()->ErrorTextBox("Load() : format.chunk.size > sizeof(format.fmt)", "Audio");
		return;
	}
	file.read((char*)&format.fmt, format.chunk.size);

	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	if (strncmp(data.id.data(), "JUNK", 4) == 0) {
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}

	while (strncmp(data.id.data(), "data", 4) != 0) {
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
		if (file.eof()) {
			ErrorCheck::GetInstance()->ErrorTextBox("Load() : Not found data", "Audio");
			return;
		}
	}

	char* pBufferLocal = new char[data.size];
	file.read(pBufferLocal, data.size);

	file.close();

	wfet = format.fmt;
	pBuffer = reinterpret_cast<BYTE*>(pBufferLocal);
	bufferSize = data.size;

	HRESULT hr = AudioManager::GetInstance()->xAudio2->CreateSourceVoice(&pSourceVoice, &wfet);
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("Load() : CreateSourceVoice() failed", "Audio");
	}

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = pBuffer;
	buf.AudioBytes = bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loopFlg_ ? XAUDIO2_LOOP_INFINITE : 0;

	hr = pSourceVoice->SubmitSourceBuffer(&buf);

	loopFlg = loopFlg_;
}


void Audio::Start(float volume) {
	HRESULT hr;

	Stop();
	if (!pSourceVoice) {
		hr = AudioManager::GetInstance()->xAudio2->CreateSourceVoice(&pSourceVoice, &wfet);
		XAUDIO2_BUFFER buf{};
		buf.pAudioData = pBuffer;
		buf.AudioBytes = bufferSize;
		buf.Flags = XAUDIO2_END_OF_STREAM;
		buf.LoopCount = loopFlg ? XAUDIO2_LOOP_INFINITE : 0;

		if (!SUCCEEDED(hr)) {
			ErrorCheck::GetInstance()->ErrorTextBox("Start() : SubmitSourceBuffer() failed", "Audio");
			return;
		}
		hr = pSourceVoice->SubmitSourceBuffer(&buf);
	}
	hr = pSourceVoice->Start();
	if (!SUCCEEDED(hr)) {
		ErrorCheck::GetInstance()->ErrorTextBox("Start() : Start() failed", "Audio");
		return;
	}
	pSourceVoice->SetVolume(volume);

	isStart = true;
}

void Audio::Pause() {
	if (pSourceVoice) {
		pSourceVoice->Stop();
		isStart = false;
	}
}

void Audio::ReStart() {
	if (pSourceVoice) {
		pSourceVoice->Start();
		isStart = true;
	}
}

void Audio::Stop() {
	if (pSourceVoice) {
		pSourceVoice->DestroyVoice();
	}
	pSourceVoice = nullptr;

	isStart = false;
}