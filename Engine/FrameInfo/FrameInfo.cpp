#include "FrameInfo.h"
#include "Utils/Log/Log.h"
#include "externals/imgui/imgui.h"

#include <cmath>
#include <thread>
#include <Windows.h>
#undef max
#undef min
#include <fstream>
#include <string>
#include <format>
#include <algorithm>

FrameInfo::FrameInfo() :
	frameStartTime_(),
	deltaTime_(0.0),
	fps_(0.0),
	maxFps_(0.0),
	minFps_(0.0),
	frameCount_(0),
	fpsLimit_(0.0),
	minTime(),
	minCheckTime()
{
	//画面情報構造体
	DEVMODE mode{};

	//現在の画面情報を取得
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);

	// リフレッシュレート取得
	fps_ = static_cast<float>(mode.dmDisplayFrequency);
	minFps_ = maxFps_ = fps_;
	deltaTime_ = 1.0f / fps_;

	gameStartTime_ = std::chrono::steady_clock::now();
	reference_ = std::chrono::steady_clock::now();

	maxFpsLimit_ = fps_;

	SetFpsLimit(fps_);
}

FrameInfo::~FrameInfo() {
	auto end = std::chrono::steady_clock::now();

	//画面情報構造体
	DEVMODE mode{};

	//現在の画面情報を取得
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &mode);

	auto playtime =
		std::chrono::duration_cast<std::chrono::milliseconds>(end - gameStartTime_);

	double theoreticalFrameCount = (static_cast<double>(playtime.count()) / 1000.0) * static_cast<double>(mode.dmDisplayFrequency);

	maxFps_ = std::clamp(maxFps_, 0.0, static_cast<double>(mode.dmDisplayFrequency));
	minFps_ = std::clamp(minFps_, 0.0, static_cast<double>(mode.dmDisplayFrequency));

	Log::AddLog(std::format("Average Fps : {:.2f}\n", static_cast<double>(mode.dmDisplayFrequency) * (static_cast<double>(frameCount_) / theoreticalFrameCount)));
	if (std::chrono::duration_cast<std::chrono::seconds>(end - gameStartTime_) > std::chrono::seconds(1)) {
		Log::AddLog(std::format("Max Fps : {:.2f}\n", maxFps_));
		Log::AddLog(std::format("Min Fps : {:.2f}\n", minFps_));
	}
}

FrameInfo* const FrameInfo::GetInstance() {
	static FrameInfo instance;

	return &instance;
}

void FrameInfo::Start() {
	frameStartTime_ = std::chrono::steady_clock::now();
}

void FrameInfo::End() {
	static std::chrono::steady_clock::time_point end{};
	end = std::chrono::steady_clock::now();

	// 10^-6
	static constexpr double unitAdjustment = 0.000001;
	
	auto elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(end - reference_);

	if (elapsed < minTime) {
		while (std::chrono::steady_clock::now() - reference_ < minTime) {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	end = std::chrono::steady_clock::now();
	auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(end - frameStartTime_);

	deltaTime_ = static_cast<double>(frameTime.count()) * unitAdjustment;
	fps_ = 1.0f / deltaTime_;

	if (std::chrono::duration_cast<std::chrono::seconds>(end - gameStartTime_) > std::chrono::seconds(1)) {
		maxFps_ = std::max(fps_, maxFps_);
		minFps_ = std::min(fps_, minFps_);
	}

	frameCount_++;
	reference_ = std::chrono::steady_clock::now();
}

void FrameInfo::SetFpsLimit(double fpsLimit) {
	fpsLimit_ = std::clamp(fpsLimit, 10.0, maxFpsLimit_);

	minTime = std::chrono::microseconds(uint64_t(1000000.0 / fpsLimit_));
	minCheckTime = std::chrono::microseconds(uint64_t(1000000.0 / fpsLimit_) - 1282LLU);
}

void FrameInfo::Debug() {
#ifdef _DEBUG
	static float fpsLimit = static_cast<float>(fpsLimit_);
	fpsLimit = static_cast<float>(fpsLimit_);

	ImGui::Begin("fps");
	ImGui::Text("Frame rate: %3.0lf fps", GetFps());
	ImGui::Text("Delta Time: %.4lf", GetDelta());
	ImGui::Text("Frame Count: %llu", GetFrameCount());
	ImGui::DragFloat("fps limit", &fpsLimit, 1.0f, 10.0f, 165.0f);
	fpsLimit_ = static_cast<double>(fpsLimit);
	SetFpsLimit(fpsLimit_);
	ImGui::End();
#endif // _DEBUG
}