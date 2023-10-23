#include "Pause.h"
#include "Engine/WinApp/WinApp.h"

void Pause::Initialize() {
	menuBar_.scale = { 855.0f, 505.0f };
	backGround_.scale = WinApp::GetInstance()->GetWindowSize();
	backGround_.color = 210u;

	goToTitle_;
	goToStageSelect_;
	goToGame_;
}

void Pause::Finalize() {

}

void Pause::Update() {
	menuBar_.Debug("menuBar_");
	goToTitle_.Debug("goToTitle_");
	goToStageSelect_.Debug("goToStageSelect_");
	goToGame_.Debug("goToGame_");
	menuBar_.Update();
	backGround_.Update();
	goToTitle_.Update();
	goToStageSelect_.Update();
	goToGame_.Update();
}

void Pause::Draw() {
	if (isActive_) {
		camera_.Update();
		backGround_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, true);
		menuBar_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, true);
		goToTitle_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, true);
		goToStageSelect_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, true);
		goToGame_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, true);
	}
}