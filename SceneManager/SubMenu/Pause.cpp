#include "Pause.h"
#include "Engine/WinApp/WinApp.h"
#include "Input/Input.h"
#include "SceneManager/SceneManager.h"
#include "SceneManager/StageSelect/StageSelect.h"
#include "SceneManager/TitleScene/TitleScene.h"

void Pause::Initialize() {
	backGround_.scale = WinApp::GetInstance()->GetWindowSize();
	backGround_.color = 220u;
	backGround2_.scale = { 1272.0f, 481.0f };
	backGround2_.pos = { 524.0f, -263.0f };
	backGround2_.color = 98u;
	backGround2_.rotate.z = -1.17f;
	backGround3_.scale = Vector2{ 1111.0f, 688.0f };
	backGround3_.rotate.z = -1.17f;
	backGround3_.color = 0xFFFFFF72;


	goToGame_.LoadTexture("./Resources/Pause/ingame_UI_modoru.png");
	goToGame_.isSameTexSize = true;
	goToGame_.texScalar = 0.66f;
	goToGame_.pos.y = 38.0f;
	goToStageSelect_.LoadTexture("./Resources/Pause/result_UI_stageSele.png");
	goToStageSelect_.isSameTexSize = true;
	goToStageSelect_.texScalar = 0.59f;
	goToStageSelect_.pos = Vector2{ 38.0f, -66.0f };
	goToTitle_.LoadTexture("./Resources/Pause/stageSelect_UI_titlehe.png");
	goToTitle_.isSameTexSize = true;
	goToTitle_.texScalar = 0.61f;
	goToTitle_.pos = Vector2{ -80.0f, -170.0f };

	pauseTex_.LoadTexture("./Resources/Pause/ingame_UI_pose.png");
	pauseTex_.isSameTexSize = true;
	pauseTex_.pos.y = 194.0f;
	pauseTex_.color = 0xACACACFF;

	arrow_.LoadTexture("./Resources/Pause/arrow.png");
	arrow_.isSameTexSize = true;
	arrow_.texScalar = 0.31f;
	arrow_.pos = Vector2{ -310.0f, 27.0f };

	arrowPosY_[0] = goToGame_.pos.y;
	arrowPosY_[1] = goToStageSelect_.pos.y;
	arrowPosY_[2] = goToTitle_.pos.y;

	arrowPosX_ = { arrow_.pos.x - 10.0f, arrow_.pos.x + 10.0f };

	arrowEase_.Start(true, 0.5f, Easeing::InOutQuad);

	input_ = Input::GetInstance();
	sceneManager_ = SceneManager::GetInstace();

	currentChoose_ = 0;
}

void Pause::Finalize() {

}

void Pause::Update() {
	arrow_.Debug("arrow_");

	backGround_.Update();
	backGround2_.Update();
	backGround3_.Update();
	goToTitle_.Update();
	goToStageSelect_.Update();
	goToGame_.Update();
	pauseTex_.Update();

	if (input_->GetKey()->Pushed(DIK_W) ||
		input_->GetKey()->Pushed(DIK_UP) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::UP) || 
		input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) > 0.3f
		) {
		currentChoose_--;
	}
	else if (input_->GetKey()->Pushed(DIK_S) ||
		input_->GetKey()->Pushed(DIK_DOWN) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::DOWN) ||
		input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) < -0.3f
		) {
		currentChoose_++;
	}

	currentChoose_ = std::clamp(currentChoose_, 0, static_cast<int32_t>(arrowPosY_.size())-1);
	arrow_.pos.y = arrowPosY_[currentChoose_];

	arrow_.pos.x = arrowEase_.Get(arrowPosX_.first, arrowPosX_.second);
	arrow_.Update();

	if (input_->GetKey()->Pushed(DIK_SPACE)|| 
		input_->GetGamepad()->Pushed(Gamepad::Button::A)
		) {
		if (currentChoose_ == 0) {
			isActive_ = false;
		}
		else if (currentChoose_ == 1) {
			sceneManager_->SceneChange(new StageSelect{});
		}
		else if (currentChoose_ == 2) {
			sceneManager_->SceneChange(new TitleScene{});
		}
	}

	arrowEase_.Update();
}

void Pause::Draw() {
	if (isActive_) {
		camera_.Update();
		backGround_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
		backGround2_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
		backGround3_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
		goToTitle_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
		goToStageSelect_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
		goToGame_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
		pauseTex_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
		arrow_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	}
}