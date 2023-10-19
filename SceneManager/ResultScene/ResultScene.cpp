#include "ResultScene.h"
#include "Engine/WinApp/WinApp.h"

ResultScene::ResultScene():
	BaseScene(BaseScene::ID::Result),
	models_(),
	texs_(),
	particles_(),
	globalVariables_(),
	clearTime_(),
	stars_()
{

}

void ResultScene::Initialize() {
	camera_.farClip = 3000.0f;

	globalVariables_.LoadFile();

	Vector2 windowSize = WinApp::GetInstance()->GetWindowSize();

	for (size_t i = 0; i < backGround_.size(); i++) {
		backGround_[i].scale = 
		{ 
			windowSize.x,
			windowSize.y / 3.0f
		};
		backGround_[i].pos.y = -windowSize.y / 3.0f + ((windowSize.y / 3.0f) * i);
	}
	backGround_[0].color = 0x90f6eeff;
	backGround_[1].color = 0x39f2e3ff;
	backGround_[2].color = 0x0ff4daff;

	backGroundBlur_.Initialize(
		"./Resources/Shaders/PostShader/Post.VS.hlsl",
		"./Resources/Shaders/PostShader/PostHeightBlur.PS.hlsl"
	);


	speechBubble_.LoadTexture("./Resources/Result/hukidasi.png");
	speechBubble_.scale = speechBubble_.GetTexSize() * 0.85f;
	speechBubble_.pos = { 218.0f,26.0f,0.01f };

	for (size_t i = 0; i < stars_.size();i++) {
		stars_[i].pos_.x = 51.0f + (178.0f * static_cast<float>(i));
	}
	currentStar_ = 0;
	stars_[currentStar_].Start();
	currentStar_++;

	stars_[1].pos_.y = 16.0f;
	stars_[2].isSpecial_ = true;
	starEffectDuration_ = std::chrono::milliseconds(500);
	startTime_ = std::chrono::steady_clock::now();
}

void ResultScene::Finalize() {

}

void ResultScene::Update() {
	auto nowTime = std::chrono::steady_clock::now();

	for (size_t i = 0; i < backGround_.size(); i++) {
		//backGround_[i].Debug("backGroung : " + std::to_string(i));
		backGround_[i].Update();
	}

	for (auto& model : models_) {
		model.Update();
	}

	for (auto& tex : texs_) {
		tex.Update();
	}
	
	speechBubble_.Update();


	if (currentStar_ < stars_.size() && starEffectDuration_ < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime_)) {
		stars_[currentStar_].Start();
		startTime_ = nowTime;
		currentStar_++;
	}

	if (stars_[2].GetEndFlg().OnEnter()) {
		for (size_t i = 0; i < stars_.size(); i++) {
			stars_[i].NormalStart();
		}
	}

	for (size_t i = 0; i < stars_.size(); i++) {
		stars_[i].Update();
	}
}

void ResultScene::Draw() {
	camera_.Update();

	backGroundBlur_.PreDraw();
	for (auto& i : backGround_) {
		i.Draw(camera_.GetViewOthographics());
	}
	backGroundBlur_.Draw(Pipeline::None);

	for (auto& model : models_) {
		model.Draw(camera_.GetViewProjection(), camera_.GetPos());
	}

	for (auto& tex : texs_) {
		tex.Draw(camera_.GetViewOthographics());
	}

	speechBubble_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);

	for (auto& i : stars_) {
		i.Draw(camera_.GetViewOthographics());
	}
}