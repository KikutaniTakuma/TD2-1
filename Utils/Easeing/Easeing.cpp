#include "Easeing.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include "externals/imgui/imgui.h"

#include "Utils/Math/Vector3.h"
#include "Utils/Math/Vector2.h"
#include <cmath>
#include <numbers>

void Easeing::Update() {
	if (isActive_) {
		t_ += spdT_ * FrameInfo::GetInstance()->GetDelta();
		t_ = std::clamp(t_, 0.0f, 1.0f);

		if (0.0f <= t_ || 1.0f <= t_) {
			if (isLoop_) {
				spdT_ *= -1.0f;
			}
			else {
				Stop();
			}
		}
	}
}

void Easeing::Start(
	bool isLoop, 
	float easeTime, 
	std::function<float(float)> ease
) {
	isActive_ = true;
	isLoop_ = isLoop;
	t_ = 0.0f;
	spdT_ = 1.0f / easeTime;

	ease_ = ease;
}
void Easeing::Pause() {
	isActive_ = false;
}

void Easeing::Restart() {
	isActive_ = true;
}
void Easeing::Stop() {
	isActive_ = false;
	isLoop_ = false;

	t_ = 0.0f;
	spdT_ = 0.0f;
}

template<>
Vector3 Easeing::Get(const Vector3& start, const Vector3& end) {
	return Vector3::Lerp(start, end, ease_(t_));
}

template<>
Vector2 Easeing::Get(const Vector2& start, const Vector2& end) {
	return Vector2::Lerp(start, end, ease_(t_));
}

void Easeing::Debug([[maybe_unused]]const std::string& debugName) {
#ifdef _DEBUG
	easeTime_ = 1.0f / spdT_;
	ImGui::Begin(debugName.c_str());
	ImGui::SliderInt("easeType", &easeType_, 0, 30);
	ImGui::DragFloat("easeSpd(seconds)", &easeTime_, 0.01f, 0.0f);
	ImGui::Checkbox("isLoop", &isLoop_);
	if (ImGui::Button("Start")) {
		isActive_ = true;
		t_ = 0.0f;

	    spdT_ = 1.0f / easeTime_;

		switch (easeType_)
		{
		default:
		case 0:
			ease_ = [](float t) {
				return t;
				};
			break;


		case 1:
			ease_ = InSine;
			break;
		case 2:
			ease_ = OutSine;
			break;
		case 3:
			ease_ = InOutSine;
			break;


		case 4:
			ease_ = InQuad;
			break;
		case 5:
			ease_ = OutQuad;
			break;
		case 6:
			ease_ = InOutQuad;
			break;


		case 7:
			ease_ = InCubic;
			break;
		case 8:
			ease_ = OutCubic;
			break;
		case 9:
			ease_ = InOutCubic;
			break;


		case 10:
			ease_ = InQuart;
			break;
		case 11:
			ease_ = OutQuart;
			break;
		case 12:
			ease_ = InOutQuart;
			break;


		case 13:
			ease_ = InQuint;
			break;
		case 14:
			ease_ = OutQuint;
			break;
		case 15:
			ease_ = InOutQuint;
			break;


		case 16:
			ease_ = InExpo;
			break;
		case 17:
			ease_ = OutExpo;
			break;
		case 18:
			ease_ = InOutExpo;
			break;


		case 19:
			ease_ = InCirc;
			break;
		case 20:
			ease_ = OutCirc;
			break;
		case 21:
			ease_ = InOutCirc;
			break;


		case 22:
			ease_ = InBack;
			break;
		case 23:
			ease_ = OutBack;
			break;
		case 24:
			ease_ = InOutBack;
			break;


		case 25:
			ease_ = InElastic;
			break;
		case 26:
			ease_ = OutElastic;
			break;
		case 27:
			ease_ = InOutElastic;
			break;


		case 28:
			ease_ = InBounce;
			break;
		case 29:
			ease_ = OutBounce;
			break;
		case 30:
			ease_ = InOutBounce;
			break;
		}
	}
	else if (ImGui::Button("Stop")) {
		isActive_ = false;
		t_ = 0.0f;
	}
	ImGui::End();

#endif // _DEBUG
}



float Easeing::InSine(float t) {
	return 1.0f - std::cos((t * std::numbers::pi_v<float>) / 2.0f);
}
float Easeing::OutSine(float t) {
	return std::sin((t * std::numbers::pi_v<float>) / 2.0f);
}
float Easeing::InOutSine(float t) {
	return -(std::cos(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f;
}

float Easeing::InQuad(float t) {
	return t * t;
}
float Easeing::OutQuad(float t) {
	return 1.0f - (1.0f - t) * (1.0f - t);
}
float Easeing::InOutQuad(float t) {
	return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}

float Easeing::InCubic(float t) {
	return t * t * t;
}
float Easeing::OutCubic(float t) {
	return 1.0f - std::pow(1.0f - t, 3.0f);
}
float Easeing::InOutCubic(float t) {
	return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

float Easeing::InQuart(float t) {
	return t * t * t * t;
}
float Easeing::OutQuart(float t) {
	return 1.0f - std::pow(1.0f - t, 4.0f);
}
float Easeing::InOutQuart(float t) {
	return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
}

float Easeing::InQuint(float t) {
	return t * t * t * t * t;
}
float Easeing::OutQuint(float t) {
	return 1.0f - std::pow(1.0f - t, 5.0f);
}
float Easeing::InOutQuint(float t) {
	return t < 0.5f ? 16.0f * t * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 5.0f) / 2.0f;
}

float Easeing::InExpo(float t) {
	return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * t - 10.0f);
}
float Easeing::OutExpo(float t) {
	return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
}
float Easeing::InOutExpo(float t) {
	return t == 0.0f
		? 0.0f
		: t == 1.0f
		? 1.0f
		: t < 0.5f ? std::pow(2.0f, 20.0f * t - 10.0f) / 2.0f
		: (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
}

float Easeing::InCirc(float t) {
	return 1.0f - std::sqrt(1.0f - std::pow(t, 2.0f));
}
float Easeing::OutCirc(float t) {
	return std::sqrt(1.0f - std::pow(t - 1.0f, 2.0f));
}
float Easeing::InOutCirc(float t) {
	return t < 0.5f
		? (1.0f - std::sqrt(1 - std::pow(2.0f * t, 2.0f))) / 2.0f
		: (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
}

float Easeing::InBack(float t) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return c3 * t * t * t - c1 * t * t;
}
float Easeing::OutBack(float t) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;

	return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
}
float Easeing::InOutBack(float t) {
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	return t < 0.5f
		? (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f
		: (std::pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
}

float Easeing::InElastic(float t) {
	const float c4 = (2.0f * std::numbers::pi_v<float>) / 3.0f;

	return t == 0.0f
		? 0.0f
		: t == 1.0f
		? 1.0f
		: -std::pow(2.0f, 10.0f * t - 10.0f) * std::sin((t * 10.0f - 10.75f) * c4);
}
float Easeing::OutElastic(float t) {
	const float c4 = (2.0f * std::numbers::pi_v<float>) / 3.0f;

	return t == 0.0f
		? 0.0f
		: t == 1.0f
		? 1.0f
		: std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
}
float Easeing::InOutElastic(float t) {
	const float c5 = (2.0f * std::numbers::pi_v<float>) / 4.5f;

	return t == 0.0f
		? 0.0f
		: t == 1.0f
		? 1.0f
		: t < 0.5f
		? -(std::pow(2.0f, 20.0f * t - 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f
		: (std::pow(2.0f, -20.0f * t + 10.0f) * std::sin((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
}

float Easeing::InBounce(float t) {
	return 1.0f - OutBounce(1.0f - t);
}
float Easeing::OutBounce(float t) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (t < 1.0f / d1) {
		return n1 * t * t;
	}
	else if (t < 2.0f / d1) {
		return n1 * (t -= 1.5f / d1) * t + 0.75f;
	}
	else if (t < 2.5f / d1) {
		return n1 * (t -= 2.25f / d1) * t + 0.9375f;
	}
	else {
		return n1 * (t -= 2.625f / d1) * t + 0.984375f;
	}
}
float Easeing::InOutBounce(float t) {
	return t < 0.5
		? (1.0f - OutBounce(1.0f - 2.0f * t)) / 2.0f
		: (1.0f + OutBounce(2.0f * t - 1.0f)) / 2.0f;
}