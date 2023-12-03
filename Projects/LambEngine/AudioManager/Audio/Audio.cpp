#include <fstream>
#include <filesystem>
#include "AudioManager/AudioManager.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "imgui.h"

Audio::Audio():
	wfet_(),
	pBuffer_(nullptr),
	bufferSize_(0u),
	pSourceVoice_(nullptr),
	loopFlg_(false),
	isStart_(false),
	volume_(1.0f),
	isLoad_{false},
	fileName_{}
{}

Audio::~Audio() {
	delete[] pBuffer_;

	pBuffer_ = nullptr;
	bufferSize_ = 0;
	wfet_ = {};
}

void Audio::Load(const std::string& fileName, bool loopFlg) {
	if (std::filesystem::path{fileName}.extension() != ".wav") {
		Log::ErrorLog(("This file is not wav -> " + fileName), "Load()", "Audio");
		return;
	}

	if (!std::filesystem::exists(std::filesystem::path{ fileName })) {
		Log::ErrorLog("Load()", ("This file is not found -> " + fileName), "Audio");
		return;
	}

	std::ifstream file(fileName, std::ios::binary);
	if (file.fail()) {
		Log::ErrorLog("Load()", ("File open failed -> " + fileName), "Audio");
		return;
	}

	fileName_ = fileName;
	loopFlg_ = loopFlg;

	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	if (strncmp(riff.chunk_.id_.data(), "RIFF", 4) != 0) {
		Log::ErrorLog("Load()", "Not found RIFF", "Audio");
		return;
	}
	if (strncmp(riff.type_.data(), "WAVE", 4) != 0) {
		Log::ErrorLog("Load()", "Not found WAVE", "Audio");
		return;
	}

	FormatChunk format{};
	file.read((char*)&format, sizeof(ChunkHeader));
	int32_t nowRead = 0;
	while (strncmp(format.chunk_.id_.data(), "fmt ", 4) != 0) {
		file.seekg(nowRead, std::ios_base::beg);
		if (file.eof()) {
			Log::ErrorLog("Load()", "Not found fmt", "Audio");
			return;
		}
		nowRead++;
		file.read((char*)&format, sizeof(ChunkHeader));
	}

	if (format.chunk_.size_ > sizeof(format.fmt_)) {
		Log::ErrorLog("Load()",
			"format chunk size is too big ->" + std::to_string(format.chunk_.size_) + " byte (max is " + std::to_string(sizeof(format.fmt_)) + " byte)", 
			"Audio"
		);
		return;
	}
	file.read((char*)&format.fmt_, format.chunk_.size_);

	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	if (strncmp(data.id_.data(), "JUNK", 4) == 0) {
		file.seekg(data.size_, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}

	while (strncmp(data.id_.data(), "data", 4) != 0) {
		file.seekg(data.size_, std::ios_base::cur);
		if (file.eof()) {
			Log::ErrorLog("Load()", "Not found data", "Audio");
			return;
		}
		file.read((char*)&data, sizeof(data));
	}

	char* pBufferLocal = new char[data.size_];
	file.read(pBufferLocal, data.size_);

	file.close();

	wfet_ = format.fmt_;
	pBuffer_ = reinterpret_cast<BYTE*>(pBufferLocal);
	bufferSize_ = data.size_;

	HRESULT hr = AudioManager::GetInstance()->xAudio2_->CreateSourceVoice(&pSourceVoice_, &wfet_);
	if (!SUCCEEDED(hr)) {
		Log::ErrorLog("Load()", "CreateSourceVoice() failed", "Audio");
	}

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = pBuffer_;
	buf.AudioBytes = bufferSize_;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loopFlg_ ? XAUDIO2_LOOP_INFINITE : 0;

	hr = pSourceVoice_->SubmitSourceBuffer(&buf);

	isLoad_ = true;
}


void Audio::Start(float volume) {
	if (!isLoad_) {
		return;
	}

	HRESULT hr;
	volume_ = volume;

	Stop();
	if (!pSourceVoice_) {
		hr = AudioManager::GetInstance()->xAudio2_->CreateSourceVoice(&pSourceVoice_, &wfet_);
		XAUDIO2_BUFFER buf{};
		buf.pAudioData = pBuffer_;
		buf.AudioBytes = bufferSize_;
		buf.Flags = XAUDIO2_END_OF_STREAM;
		buf.LoopCount = loopFlg_ ? XAUDIO2_LOOP_INFINITE : 0;

		if (!SUCCEEDED(hr)) {
			Log::ErrorLog("SubmitSourceBuffer() failed", "Start()", "Audio");
			return;
		}
		hr = pSourceVoice_->SubmitSourceBuffer(&buf);
	}
	hr = pSourceVoice_->Start();
	if (!SUCCEEDED(hr)) {
		Log::ErrorLog("function is something error", "Start()", "Audio");
		return;
	}
	pSourceVoice_->SetVolume(volume_);

	isStart_ = true;
}

void Audio::Pause() {
	if (!isLoad_) {
		return;
	}
	if (pSourceVoice_) {
		pSourceVoice_->Stop();
		isStart_ = false;
	}
}

void Audio::ReStart() {
	if (!isLoad_) {
		return;
	}
	if (pSourceVoice_) {
		pSourceVoice_->Start();
		isStart_ = true;
	}
}

void Audio::Stop() {
	if (!isLoad_) {
		return;
	}
	if (pSourceVoice_) {
		pSourceVoice_->DestroyVoice();
	}
	pSourceVoice_ = nullptr;

	isStart_ = false;
}

void Audio::SetAudio(float volume) {
	volume_ = volume;
	if (pSourceVoice_ && isStart_) {
		pSourceVoice_->SetVolume(volume_);
	}
}

void Audio::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat("volume", &volume_, 0.001f, 0.0f, 1.0f);
	SetAudio(volume_);
	
	ImGui::Checkbox("isLoop", &loopFlg_);
	if (ImGui::Button("Start")) {
		Start(volume_);
	}
	if (ImGui::Button("Stop")) {
		Stop();
	}
	if (ImGui::Button("ReStart")) {
		ReStart();
	}
	if (ImGui::Button("Pause")) {
		Pause();
	}

	ImGui::End();
#endif // _DEBUG
}