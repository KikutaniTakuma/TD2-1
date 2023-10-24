#include "TitleScene.h"
#include "Engine/WinApp/WinApp.h"
#include "SceneManager/StageSelect/StageSelect.h"
#include <numbers>

TitleScene::TitleScene():
	BaseScene{BaseScene::ID::Title},
	backGround_{},
	bgm_(nullptr),
	currentPlayerAnimation_(),
	decideSE_(nullptr),
	isPlayerAnimationCoolTime_(),
	isPlayerAnimationTurnBack_(),
	playerAnimationCoolStartTime_(),
	playerAnimationDuration_()
{
	camera_.farClip = 3000.0f;
	camera_.pos.z = -1000.0f;
}

void TitleScene::Initialize() {
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

	player_.LoadObj("./Resources/Player/player.obj");
	player_.light.ligDirection = { 0.0f, 0.0f, 1.0f };
	player_.light.ligColor = Vector3::identity;
	player_.light.ptRange = std::numeric_limits<float>::max();
	player_.scale *= 50.0f;
	player_.pos.y = -160.5f;
	player_.rotate.y = std::numbers::pi_v<float>;
	playerScale_.first = Vector3{ player_.scale.x + 20.0f, player_.scale.y - 10.0f, player_.scale.z };
	playerScale_.second = Vector3{ player_.scale.x-20.0f, player_.scale.y+10.0f,player_.scale.z };
	playerScaleEaseing_.Start(true, 0.3f, Easeing::OutCirc);
	playerPos_.first = player_.pos;
	playerPos_.second = player_.pos;
	playerPos_.second.y = 0.0f;
	playerPosEaseing_.Start(true, 0.3f, Easeing::OutCirc);

	playerAnimationTex_.reserve(5);
	playerAnimationTex_.push_back(textureManager_->LoadTexture("./Resources/Player/player_face.png"));
	playerAnimationTex_.push_back(textureManager_->LoadTexture("./Resources/Player/player_face2.png"));
	playerAnimationTex_.push_back(textureManager_->LoadTexture("./Resources/Player/player_face3.png"));
	playerAnimationTex_.push_back(textureManager_->LoadTexture("./Resources/Player/player_face4.png"));
	playerAnimationTex_.push_back(textureManager_->LoadTexture("./Resources/Player/player_face5.png"));

	playerAnimationDuration_ = std::chrono::milliseconds{ 33 };
	currentPlayerAnimation_ = 0;
	isPlayerAnimationTurnBack_ = false;
	playerAnimationStartTime_ = std::chrono::steady_clock::now();

	playerAnimationCoolTime_ = std::chrono::milliseconds{ 1600 };
	playerAnimationCoolTimeDuration_ = {
		800,
		1600
	};
	isPlayerAnimationCoolTime_ = true;
	playerAnimationCoolStartTime_ = playerAnimationStartTime_;


	titleTex_.LoadTexture("./Resources/Title/titleLOGO.png");
	titleTex_.isSameTexSize = true;
	titleTex_.pos = Vector2{ 19.0f, 116.0f };
	floor_.LoadTexture("./Resources/layer/layer1.png");
	floor_.scale = Vector2{ 1280.0f, 151.0f };
	floor_.pos.y = -286.0f;

	backGroundParticle_.LoadSettingDirectory("backGroundParticle");
	backGroundParticle_.ParticleStart();

	for (auto& i : backGround_) {
		i.Update();
	}

	player_.Update();
	titleTex_.Update();
	floor_.Update();
	playerScaleEaseing_.Update();
	playerPosEaseing_.Update();
	backGroundParticle_.Update();

	bgm_ = audioManager_->LoadWav("./Resources/Audio/BGM/BGM/title.wav", true);
	decideSE_ = audioManager_->LoadWav("./Resources/Audio/kouka/kouka/UI_kettei.wav", false);
	bgm_->Start(0.15f);

	smoke_.LoadSettingDirectory("smoke");
}

void TitleScene::Finalize() {
	bgm_->Stop();
}

void TitleScene::Update() {
	auto nowTime = std::chrono::steady_clock::now();

	for (auto& i : backGround_) {
		i.Update();
	}

	/*player_.scale = playerScaleEaseing_.Get(playerScale_.first, playerScale_.second);
	player_.pos = playerPosEaseing_.Get(playerPos_.first, playerPos_.second);
	if (player_.pos.y <= playerPos_.first.y) {
		smoke_.ParticleStart();
		smoke_.emitterPos_ = player_.pos;
		smoke_.emitterPos_.y -= 40.0f;
	}*/


	if (isPlayerAnimationCoolTime_ && playerAnimationCoolTime_ < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - playerAnimationCoolStartTime_)) {
		isPlayerAnimationCoolTime_ = false;
	}

	if (!isPlayerAnimationCoolTime_ && playerAnimationDuration_ < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - playerAnimationStartTime_)) {
		isPlayerAnimationTurnBack_ ? --currentPlayerAnimation_ : ++currentPlayerAnimation_;
		if (currentPlayerAnimation_ >= static_cast<int32_t>(playerAnimationTex_.size()) - 1) {
			isPlayerAnimationTurnBack_ = true;
			playerAnimationCoolTime_ = std::chrono::milliseconds{
				UtilsLib::Random(playerAnimationCoolTimeDuration_.first, playerAnimationCoolTimeDuration_.second)
			};
		}
		else if (currentPlayerAnimation_ <= 0) {
			isPlayerAnimationTurnBack_ = false;
			isPlayerAnimationCoolTime_ = true;
			playerAnimationCoolStartTime_ = nowTime;
		}
		currentPlayerAnimation_ = std::clamp(currentPlayerAnimation_, 0, static_cast<int32_t>(playerAnimationTex_.size()) - 1);
		player_.ChangeTexture("face", playerAnimationTex_[currentPlayerAnimation_]);
		playerAnimationStartTime_ = nowTime;
	}
	
	player_.Debug("player_");
	player_.Update();
	titleTex_.Debug("titleTex_");
	titleTex_.Update();
	floor_.Debug("floor_");
	floor_.Update();
	playerScaleEaseing_.Update();
	playerPosEaseing_.Update();
	backGroundParticle_.Update();
	smoke_.Update();

	if (input_->GetKey()->Pushed(DIK_SPACE) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::A)
		) {
		bgm_->Stop();
		decideSE_->Start(0.2f);


		auto nextScene = new StageSelect{};
		assert(nextScene);

		sceneManager_->SceneChange(nextScene);
	}
}

void TitleScene::Draw() {

	camera_.Update();

	backGroundBlur_.PreDraw();
	for (auto& i : backGround_) {
		i.Draw(camera_.GetViewOthographics());
	}
	backGroundBlur_.Draw(Pipeline::None);
	backGroundParticle_.Draw(camera_.GetViewOthographics());

	floor_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	titleTex_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);

	player_.Draw(camera_.GetViewOthographics(), camera_.pos);

	smoke_.Draw(camera_.GetViewOthographics());
}