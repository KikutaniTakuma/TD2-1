#include "Pause.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Input/Input.h"
#include "Scenes/Manager/SceneManager.h"
#include "Scenes/StageSelect/StageSelect.h"
#include "Scenes/TitleScene/TitleScene.h"
#include "AudioManager/AudioManager.h"

void Pause::Initialize() {
	backGround_.scale_ = WindowFactory::GetInstance()->GetClientSize();
	backGround_.color_ = 220u;
	backGround2_.scale_ = { 1272.0f, 481.0f };
	backGround2_.pos_ = { 524.0f, -263.0f };
	backGround2_.color_ = 98u;
	backGround2_.rotate_.z = -1.17f;
	backGround3_.scale_ = Vector2{ 1111.0f, 688.0f };
	backGround3_.rotate_.z = -1.17f;
	backGround3_.color_ = 0xFFFFFF72;


	goToGame_.LoadTexture("./Resources/Pause/ingame_UI_modoru.png");
	goToGame_.isSameTexSize_ = true;
	goToGame_.texScalar_ = 0.66f;
	goToGame_.pos_.y = 38.0f;
	goToStageSelect_.LoadTexture("./Resources/Pause/result_UI_stageSele.png");
	goToStageSelect_.isSameTexSize_ = true;
	goToStageSelect_.texScalar_ = 0.59f;
	goToStageSelect_.pos_ = Vector2{ 38.0f, -66.0f };
	goToTitle_.LoadTexture("./Resources/Pause/stageSelect_UI_titlehe.png");
	goToTitle_.isSameTexSize_ = true;
	goToTitle_.texScalar_ = 0.61f;
	goToTitle_.pos_ = Vector2{ -80.0f, -170.0f };

	pauseTex_.LoadTexture("./Resources/Pause/ingame_UI_pose.png");
	pauseTex_.isSameTexSize_ = true;
	pauseTex_.pos_.y = 194.0f;
	pauseTex_.color_ = 0xACACACFF;

	arrow_.LoadTexture("./Resources/Pause/arrow.png");
	arrow_.isSameTexSize_ = true;
	arrow_.texScalar_ = 0.31f;
	arrow_.pos_ = Vector2{ -310.0f, 27.0f };

	arrowPosY_[0] = goToGame_.pos_.y;
	arrowPosY_[1] = goToStageSelect_.pos_.y;
	arrowPosY_[2] = goToTitle_.pos_.y;

	arrowPosX_ = { arrow_.pos_.x - 10.0f, arrow_.pos_.x + 10.0f };

	arrowEase_.Start(true, 0.5f, Easing::InOutQuad);

	input_ = Input::GetInstance();
	sceneManager_ = SceneManager::GetInstance();

	currentChoose_ = 0;


	audioManager_ = AudioManager::GetInstance();
	audios_ = {
		audioManager_->LoadWav("./Resources/Audio/kouka/kouka/UI_kettei.wav", false),
		audioManager_->LoadWav("./Resources/Audio/kouka/kouka/UI_modoru.wav", false),
		audioManager_->LoadWav("./Resources/Audio/kouka/kouka/UI_sentaku.wav", false),
		audioManager_->LoadWav("./Resources/Audio/kouka/kouka/pose.wav", false)
	};

	isStick_ = false;
}

void Pause::Finalize() {

}

void Pause::Update() {
	arrow_.Debug("arrow_");

	if (input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) > -0.3f &&
		input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) < 0.3f) {
		isStick_ = false;
	}

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
		(input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) > 0.3f
			&& !isStick_)
		) {
		if (currentChoose_ != 0) {
			audios_[2]->Start(0.2f);
		}
		currentChoose_--;
		isStick_ = true;
	}
	else if (input_->GetKey()->Pushed(DIK_S) ||
		input_->GetKey()->Pushed(DIK_DOWN) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::DOWN) ||
		(input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) < -0.3f&&
			!isStick_)
		) {
		if (currentChoose_ != 2) {
			audios_[2]->Start(0.2f);
		}
		currentChoose_++;
		isStick_ = true;
	}

	currentChoose_ = std::clamp(currentChoose_, 0, static_cast<int32_t>(arrowPosY_.size())-1);
	arrow_.pos_.y = arrowPosY_[currentChoose_];

	arrow_.pos_.x = arrowEase_.Get(arrowPosX_.first, arrowPosX_.second);
	arrow_.Update();

	/*for (size_t i = 0; i < audios_.size();i++) {
		audios_[i]->Debug("se" + std::to_string(i));
	}*/

	arrowEase_.Update();
}

void Pause::SceneChange(int32_t nowStage) {
	if (isActive_) {
		nowStage = std::clamp(nowStage, 1, static_cast<int32_t>(sceneManager_->isClearStage_.size()));
		if (input_->GetKey()->Pushed(DIK_SPACE) ||
			input_->GetGamepad()->Pushed(Gamepad::Button::A)
			) {
			if (currentChoose_ == 0) {
				isActive_ = false;
				audios_[1]->Start(0.2f);
			}
			else if (currentChoose_ == 1) {
				auto nextScene = new StageSelect{};
				assert(nextScene);
				nextScene->SetStartStage(nowStage);
				sceneManager_->SceneChange(nextScene);

				audios_[0]->Start(0.125f);
			}
			else if (currentChoose_ == 2) {
				auto nextScene = new TitleScene{};
				assert(nextScene);
				sceneManager_->SceneChange(nextScene);
				audios_[0]->Start(0.125f);
			}
		}
	}
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

		if (isActive_.OnEnter()) {
			audios_[3]->Start(0.25f);
		}
	}
	if (isActive_.OnExit()) {
		audios_[1]->Start(0.2f);
	}
}