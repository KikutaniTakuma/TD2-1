#include "Fade.h"
#include "Engine/WinApp/WinApp.h"
#include "externals/imgui/imgui.h"

Fade::Fade():
	isInStart_(false),
	isOutStart_(false),
	tex_(),
	fadeTime_(1.0f)
{
	tex_.scale = WinApp::GetInstance()->GetWindowSize();
}

void Fade::OutStart() {
	if (!isInStart_ && !isOutStart_) {
		isOutStart_ = true;
		ease_.Start(
			false,
			fadeTime_
		);
	}
}
void Fade::InStart() {
	if (!isInStart_ && !isOutStart_) {
		isInStart_ = true;
		ease_.Start(
			false,
			fadeTime_
		);
	}
}
bool Fade::OutEnd() const {
	return isOutStart_.OnExit();
}
bool Fade::OutStay() const {
	return isOutStart_.OnStay();
}

bool Fade::InEnd() const {
	return isInStart_.OnExit();
}
bool Fade::InStay() const {
	return isInStart_.OnStay();
}

void Fade::Update() {
	isOutStart_.Update();
	isInStart_.Update();

	if (ease_.ActiveExit()) {
		isInStart_ = false;
		isOutStart_ = false;
	}
	
	if (isInStart_) {
		tex_.color = ColorLerp(0xff, 0x00, ease_.GetT());
	}
	else if (isOutStart_) {
		tex_.color = ColorLerp(0x00, 0xff, ease_.GetT());
	}

	tex_.Update();

	ease_.Update();
}
void Fade::Draw(const Mat4x4& viewProjection) {
	tex_.Draw(viewProjection);
}