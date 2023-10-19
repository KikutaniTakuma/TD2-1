#include "ResultScene.h"
#include "Engine/WinApp/WinApp.h"

ResultScene::ResultScene():
	BaseScene(BaseScene::ID::Result),
	models_(),
	texs_(),
	particles_(),
	globalVariables_(),
	stars_(),
	starsGray_(),
	grayPera_(),
	starEffectDuration_(),
	startTime_(),
	currentStar_(0),
	speechBubble_(),
	backGround_(),
	backGroundBlur_(),
	clearTime_(),
	clearTimeBasis_(),
	score_(0),
	updateStartTime_(),
	isUpdate_(false)
{
	clearTimeBasis_ = {
		std::chrono::milliseconds{30000},  // 30秒
		std::chrono::milliseconds{60000},  // 1分
		std::chrono::milliseconds{120000}  // 2分
	};
}

void ResultScene::Initialize() {
	camera_.farClip = 3000.0f;
	camera_.pos.z = -1000.0f;

	globalVariables_.LoadFile();

	// 背景の設定
	Vector2 windowSize = WinApp::GetInstance()->GetWindowSize();
	for (size_t i = 0; i < backGround_.size(); i++) {
		backGround_[i].scale = 
		{ 
			windowSize.x,
			windowSize.y / 3.0f
		};
		backGround_[i].pos.y = -windowSize.y / 3.0f + ((windowSize.y / 3.0f) * i);
	}

	// 背景の色設定
	backGround_[0].color = 0x90f6eeff;
	backGround_[1].color = 0x39f2e3ff;
	backGround_[2].color = 0x0ff4daff;

	// 背景にブラーをかける
	backGroundBlur_.Initialize(
		"./Resources/Shaders/PostShader/Post.VS.hlsl",
		"./Resources/Shaders/PostShader/PostHeightBlur.PS.hlsl"
	);

	// 吹き出し板ポリをロード
	speechBubble_.LoadTexture("./Resources/Result/hukidasi.png");
	speechBubble_.scale = speechBubble_.GetTexSize() * 0.85f;
	speechBubble_.pos = { 218.0f,26.0f,0.01f };


	// 星の位置設定
	for (size_t i = 0; i < stars_.size();i++) {
		stars_[i].pos_.x = 51.0f + (178.0f * static_cast<float>(i));
	}
	stars_[1].pos_.y = 16.0f;
	stars_[2].isSpecial_ = true;

	currentStar_ = 0;


	// 灰色の星の設定
	for (size_t i = 0; i < starsGray_.size(); i++) {
		assert(starsGray_.size() == stars_.size());
		starsGray_[i].pos_ = stars_[i].pos_;
		starsGray_[i].SetDefaultScale(0.95f);
	}

	// グレースケール化
	grayPera_.Initialize(
		"./Resources/Shaders/PostShader/Post.VS.hlsl",
		"./Resources/Shaders/PostShader/PostGrayScale.PS .hlsl"
	);


	// 星のアニメーション間隔(500ミリ秒)
	starEffectDuration_ = std::chrono::milliseconds(500);

	// 始まった時間を保存
	startTime_ = std::chrono::steady_clock::now();

	updateStartTime_ = std::chrono::milliseconds(500);



	// 一度だけアップデートしておく
	for (size_t i = 0; i < backGround_.size(); i++) {
		backGround_[i].Update();
	}

	for (auto& model : models_) {
		model.Update();
	}

	for (auto& tex : texs_) {
		tex.Update();
	}

	speechBubble_.Update();
	for (size_t i = 0; i < stars_.size(); i++) {
		stars_[i].Update();
	}

	for (auto& i : starsGray_) {
		i.Update();
	}

	player_.LoadObj("./Resources/Player/player.obj");
	player_.light.ligDirection = { 0.0f, 0.0f, 1.0f };
	player_.light.ligColor = Vector3::identity;
	player_.light.ptRange = std::numeric_limits<float>::max();
	player_.rotate = { -0.1f, 2.962f, -0.12f };
	player_.pos = { -357.0f, -33.7f,-400.0f };
	player_.scale *= 170.0f;
}

void ResultScene::SetClearTime(std::chrono::milliseconds clearTime) {
	clearTime_ = clearTime;
	score_ = static_cast<int32_t>(clearTimeBasis_.size());
	for (size_t i = 0; i < clearTimeBasis_.size(); i++) {
		if (clearTime_ <= clearTimeBasis_[i]) {
			break;
		}
		--score_;
	}

	score_ = std::clamp(score_, 0, static_cast<int32_t>(clearTimeBasis_.size()));
}

void ResultScene::Finalize() {

}

void ResultScene::Update() {
	auto nowTime = std::chrono::steady_clock::now();

	if (0 < score_ &&
		!isUpdate_ && 
		updateStartTime_ < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime_)) {
		isUpdate_ = true;

		stars_[currentStar_].Start();
		startTime_ = nowTime;
		currentStar_++;
	}
	if(isUpdate_){
		for (size_t i = 0; i < backGround_.size(); i++) {
			backGround_[i].Update();
		}

		for (auto& model : models_) {
			model.Update();
		}

		for (auto& tex : texs_) {
			tex.Update();
		}

		speechBubble_.Update();

		// 星のアニメーション
		if (currentStar_ < stars_.size() - score_ - 1 &&
			starEffectDuration_ < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime_))
		{
			stars_[currentStar_].Start();
			startTime_ = nowTime;
			currentStar_++;
		}

		if (stars_[score_-1].GetEndFlg().OnEnter()) {
			for (size_t i = 0; i < stars_.size(); i++) {
				stars_[i].NormalStart();
			}
		}

		for (size_t i = 0; i < stars_.size(); i++) {
			stars_[i].Update();
		}

		for (auto& i : starsGray_) {
			i.Update();
		}
	}

	player_.Debug("player");
	player_.Update();
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

	// グレースケール化する
	grayPera_.PreDraw();

	for (auto& i : starsGray_) {
		i.Draw(camera_.GetViewOthographics());
	}

	grayPera_.Draw(Pipeline::None);

	for (auto& i : stars_) {
		i.Draw(camera_.GetViewOthographics());
	}

	player_.Draw(camera_.GetViewOthographics(), camera_.pos);
}