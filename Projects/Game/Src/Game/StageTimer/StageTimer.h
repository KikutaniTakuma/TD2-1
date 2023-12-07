#pragma once
#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"

#include "GlobalVariables/GlobalVariables.h"
#include <memory>
#include <array>
#include <string>

class StageTimer
{
public:
	StageTimer();

	void Init();

	void Update();

	void Draw2D(const Mat4x4& viewProjection);

	void SetPlayTime(const float* playTime) { playTime_ = playTime; }

private:

	void SetGlobalVariable();

	void ApplyGlobalVariable();

	void SetNumTeces();

	void SetSpriteSize();

	void SetSpritePos();

	void TecesUpdate();

private:

	int time_;

	const float* playTime_ = nullptr;

	static const int MaxDigits = 2;

	std::array<std::unique_ptr<Texture2D>, 3> numTeces_;

	std::array<std::unique_ptr<Texture2D>, 2> teces_;

	const std::string groupName_ = "Timer";

	std::unique_ptr<GlobalVariables> globalVariables_;

	enum V2ItemNames {
		kTexPos,
		kV2ItemCount,
	};

	std::array<std::string, V2ItemNames::kV2ItemCount> v2ItemNames_ = {
		"点々のポジション",
	};

	std::array<Vector2, V2ItemNames::kV2ItemCount> v2Info_;

	enum FInfoNames {
		kTecesInterval,
		kNumPosY,
		kMinutesPosX,
		kSecondPosX,
		kSecondInterval,
		kTexScale,
		kNumScale,
		kFInfoCount,
	};

	std::string fInfoNames_[FInfoNames::kFInfoCount] = {
		"点々の間隔",
		"数字のy座標",
		"分のx座標",
		"秒のx座標",
		"秒の間隔",
		"点々のスケール",
		"数字のスケール",
	};

	std::array<float, FInfoNames::kFInfoCount> fInfo_;

};
