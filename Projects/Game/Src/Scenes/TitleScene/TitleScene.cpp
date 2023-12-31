#include "TitleScene.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Scenes/StageSelect/StageSelect.h"
#include <numbers>
#include "AudioManager/AudioManager.h"
#include "Utils/Random/Random.h"

TitleScene::TitleScene():
	BaseScene{BaseScene::ID::Title},
	backGround_{},
	bgm_(nullptr),
	currentPlayerAnimation_(),
	decideSE_(nullptr),
	isPlayerAnimationCoolTime_(),
	isPlayerAnimationTurnBack_(),
	playerAnimationCoolStartTime_(),
	playerAnimationDuration_(),
	staticCamera_{ new Camera{} }
{
	camera_->farClip = 3000.0f;
	camera_->pos.z = -1000.0f;
	//hoge_.LoadSettingDirectory("layer_heal");
	staticCamera_->Update();
}

void TitleScene::Initialize() {
	backGroundBlur_.reset(new PeraRender{});
	player_.reset(new Model{});

	titleTex_.reset(new Texture2D{});
	aButtonHud_.reset(new Texture2D{});
	spaceHud_.reset(new Texture2D{});
	startHud_.reset(new Texture2D{});
	floor_.reset(new Texture2D{});
	keyEscHud_.reset(new Texture2D{});
	padStartHud_.reset(new Texture2D{});
	backToHud_.reset(new Texture2D{});

	backGroundParticle_.reset(new Particle{});
	smoke_.reset(new Particle{});

	playerScaleEaseing_.reset(new Easing{});
	playerPosEaseing_.reset(new Easing{});
	titleEase_.reset(new Easing{});
	hudAlphaEase_.reset(new Easing{});


	// 背景の設定
	Vector2 windowSize = WindowFactory::GetInstance()->GetClientSize();
	for (size_t i = 0; i < backGround_.size(); i++) {
		backGround_[i].reset(new Texture2D{});

		backGround_[i]->scale =
		{
			windowSize.x,
			windowSize.y / 3.0f
		};
		backGround_[i]->pos.y = -windowSize.y / 3.0f + ((windowSize.y / 3.0f) * i);
	}

	// 背景の色設定
	backGround_[0]->color = 0x90f6eeff;
	backGround_[1]->color = 0x39f2e3ff;
	backGround_[2]->color = 0x0ff4daff;

	// 背景にブラーをかける
	backGroundBlur_->Initialize(
		"./Resources/Shaders/PostShader/Post.VS.hlsl",
		"./Resources/Shaders/PostShader/PostHeightBlur.PS.hlsl"
	);

	player_->LoadObj("./Resources/Player/player.obj");
	player_->light.ligDirection = { 0.0f, 0.0f, 1.0f };
	player_->light.ligColor = Vector3::kIdentity;
	player_->light.ptRange = std::numeric_limits<float>::max();
	player_->scale *= 60.0f;
	player_->pos.x = -400.0f;
	player_->pos.y = -160.5f;
	player_->rotate.y = std::numbers::pi_v<float>;
	playerScale_.first = Vector3{ player_->scale.x + 20.0f, player_->scale.y - 10.0f, player_->scale.z };
	playerScale_.second = Vector3{ player_->scale.x-20.0f, player_->scale.y+10.0f,player_->scale.z };
	playerScaleEaseing_->Start(true, 0.3f, Easing::OutCirc);
	playerPos_.first = player_->pos;
	playerPos_.second = player_->pos;
	playerPos_.second.y = 20.0f;
	playerPosEaseing_->Start(true, 0.3f, Easing::OutCirc);

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


	titleTex_->LoadTexture("./Resources/Title/titleLOGO.png");
	titleTex_->isSameTexSize = true;
	titleTex_->texScalar = 1.37f;
	titleScaleDuration_ = { titleTex_->texScalar * 0.9f, titleTex_->texScalar * 1.0f };
	titleTex_->pos = Vector2{ 202.0f, 43.0f };
	titleTex_->rotate.z = 0.11f;
	titleEase_->Start(
		true, 0.2f, Easing::InExpo
	);

	floor_->LoadTexture("./Resources/layer/layer1.png");
	floor_->scale = Vector2{ 1280.0f, 151.0f };
	floor_->pos.y = -286.0f;

	backGroundParticle_->LoadSettingDirectory("backGroundParticle");
	backGroundParticle_->ParticleStart();

	for (auto& i : backGround_) {
		i->Update();
	}

	player_->Update();
	titleTex_->Update();
	floor_->Update();
	playerScaleEaseing_->Update();
	playerPosEaseing_->Update();
	backGroundParticle_->Update();

	bgm_ = audioManager_->LoadWav("./Resources/Audio/BGM/BGM/title.wav", true);
	decideSE_ = audioManager_->LoadWav("./Resources/Audio/kouka/kouka/UI_kettei.wav", false);
	bgm_->Start(0.15f);

	smoke_->LoadSettingDirectory("smoke");

	aButtonHud_->LoadTexture("./Resources/HUD/controler_UI_A.png");
	aButtonHud_->uvSize.x = 0.5f;
	aButtonHud_->scale = { 81.0f, 81.0f };
	aButtonHud_->pos = { -488.0f, -285.0f };
	spaceHud_->LoadTexture("./Resources/HUD/keys_UI_space.png");
	spaceHud_->uvSize.x = 0.5f;
	spaceHud_->scale = { 122.0f, 171.0f };
	spaceHud_->pos = { -488.0f, -275.0f };

	startHud_->LoadTexture("./Resources/HUD/title_UI_start.png");
	startHud_->isSameTexSize = true;
	startHud_->texScalar = 0.31f;
	startHud_->pos = Vector2{ -315.0f, -291.0f };

	hudAlphaEase_->Start(true, 1.0f, Easing::InOutQuad);

	keyEscHud_->LoadTexture("./Resources/HUD/keys_UI_esc.png");
	keyEscHud_->uvSize.x = 0.5f;
	keyEscHud_->scale = Vector2{ 115.0f, 113.0f };
	keyEscHud_->pos = Vector2{ -555.0f,291.0f };
	padStartHud_->LoadTexture("./Resources/HUD/controler_UI_pose.png");
	padStartHud_->uvSize.x = 0.5f;
	padStartHud_->scale = Vector2{ 70.0f, 70.0f };
	padStartHud_->pos = Vector2{ -555.0f,291.0f };
	backToHud_->LoadTexture("./Resources/HUD/title_UI_owaru.png");
	backToHud_->isSameTexSize = true;
	backToHud_->texScalar = 0.25f;
	backToHud_->pos = Vector2{ -549.0f, 232.0f };

	backGroundBlur_->scale = WindowFactory::GetInstance()->GetClientSize();
}

void TitleScene::Finalize() {
	bgm_->Stop();
}

void TitleScene::Update() {
	auto nowTime = std::chrono::steady_clock::now();

	if (input_->GetKey()->Pushed(DIK_R)) {
		sceneManager_->isClearStage_ = {};
		Lamb::TextBox("All clear flag reset succeeded", "TitleScene");
	}

	for (auto& i : backGround_) {
		i->Update();
	}

	player_->scale = playerScaleEaseing_->Get(playerScale_.first, playerScale_.second);
	player_->pos = playerPosEaseing_->Get(playerPos_.first, playerPos_.second);
	if (player_->pos.y <= playerPos_.first.y) {
		smoke_->ParticleStart();
		smoke_->emitterPos = player_->pos;
		smoke_->emitterPos.y -= 40.0f;
	}


	if (isPlayerAnimationCoolTime_ && playerAnimationCoolTime_ < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - playerAnimationCoolStartTime_)) {
		isPlayerAnimationCoolTime_ = false;
	}

	if (!isPlayerAnimationCoolTime_ && playerAnimationDuration_ < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - playerAnimationStartTime_)) {
		isPlayerAnimationTurnBack_ ? --currentPlayerAnimation_ : ++currentPlayerAnimation_;
		if (currentPlayerAnimation_ >= static_cast<int32_t>(playerAnimationTex_.size()) - 1) {
			isPlayerAnimationTurnBack_ = true;
			playerAnimationCoolTime_ = std::chrono::milliseconds{
				Lamb::Random(playerAnimationCoolTimeDuration_.first, playerAnimationCoolTimeDuration_.second)
			};
		}
		else if (currentPlayerAnimation_ <= 0) {
			isPlayerAnimationTurnBack_ = false;
			isPlayerAnimationCoolTime_ = true;
			playerAnimationCoolStartTime_ = nowTime;
		}
		currentPlayerAnimation_ = std::clamp(currentPlayerAnimation_, 0, static_cast<int32_t>(playerAnimationTex_.size()) - 1);
		player_->ChangeTexture("face", playerAnimationTex_[currentPlayerAnimation_]);
		playerAnimationStartTime_ = nowTime;
	}
	
	player_->Update();
	titleTex_->texScalar = titleEase_->Get(titleScaleDuration_.first, titleScaleDuration_.second);
	titleTex_->Update();
	floor_->Update();
	playerScaleEaseing_->Update();
	playerPosEaseing_->Update();
	titleEase_->Update();
	backGroundParticle_->Update();
	smoke_->Update();

	if (input_->GetKey()->GetKey(DIK_SPACE) ||
		input_->GetGamepad()->GetButton(Gamepad::Button::A)) {
		aButtonHud_->uvPibot.x = 0.5f;
		spaceHud_->uvPibot.x = 0.5f;
	}
	else {
		aButtonHud_->uvPibot.x = 0.0f;
		spaceHud_->uvPibot.x = 0.0f;
	}
	
	aButtonHud_->Update();
	spaceHud_->Update();
	aButtonHud_->color = Vector4ToUint(hudAlphaEase_->Get(Vector4::kIdentity, Vector4{ Vector3::kIdentity, 0.2f }));
	spaceHud_->color = Vector4ToUint(hudAlphaEase_->Get(Vector4::kIdentity, Vector4{ Vector3::kIdentity, 0.2f }));
	startHud_->Update();

	hudAlphaEase_->Update();

	if (input_->GetGamepad()->GetButton(Gamepad::Button::START)) {
		padStartHud_->uvPibot.x = 0.5f;
	}
	else {
		padStartHud_->uvPibot.x = 0.0f;
	}
	if (input_->GetKey()->GetKey(DIK_ESCAPE)) {
		keyEscHud_->uvPibot.x = 0.5f;
	}
	else {
		keyEscHud_->uvPibot.x = 0.0f;
	}
	padStartHud_->Update();
	keyEscHud_->Update();

	backToHud_->Update();

	if (input_->GetKey()->Pushed(DIK_SPACE) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::A)
		) {
		bgm_->Stop();
		decideSE_->Start(0.2f);


		auto nextScene = new StageSelect{};
		assert(nextScene);
		nextScene->SetStartStage(1);
		sceneManager_->SceneChange(nextScene);
	}

	backGroundBlur_->Update();
}

void TitleScene::Draw() {

	camera_->Update();

	backGroundBlur_->PreDraw();
	for (auto& i : backGround_) {
		i->Draw(camera_->GetViewOthographics());
	}
	backGroundBlur_->Draw(staticCamera_->GetOthographics(), Pipeline::None);
	backGroundParticle_->Draw(camera_->rotate, camera_->GetViewOthographics());

	floor_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
	titleTex_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);

	player_->Draw(camera_->GetViewOthographics(), camera_->pos);

	smoke_->Draw(camera_->rotate, camera_->GetViewOthographics());

	if (sceneManager_->GetIsPad()) {
		aButtonHud_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
		padStartHud_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
	}
	else {
		spaceHud_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
		keyEscHud_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
	}
	backToHud_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
	startHud_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
	//hoge_.Draw(camera_->GetViewOthographics(), Pipeline::Normal);
}