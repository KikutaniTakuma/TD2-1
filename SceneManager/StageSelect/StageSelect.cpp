#include "StageSelect.h"
#include "Engine/WinApp/WinApp.h"
#include "SceneManager/GameScene/GameScene.h"
#include <numbers>

StageSelect::StageSelect():
	BaseScene{BaseScene::ID::StageSelect},
	moon_{},
	moonRotateY_{},
	rotateEase_{},
	bubble_{},
	currentStage_{},
	maxStage_{},
	stageNumberTex_{},
	stageTenNumberTex_{}
{}

void StageSelect::Initialize() {
	camera_.farClip = 3000.0f;
	camera_.pos.z = -500.0f;
	currentStage_ = 1;
	maxStage_ = static_cast<decltype(maxStage_)>(sceneManager_->isClearStage_.size());

	moon_.LoadObj("./Resources/StageSelect/moon/moon.obj");
	moon_.light.ligDirection = { 0.0f, 0.0f, 1.0f };
	moon_.light.ligColor = Vector3::identity;
	moon_.light.ptRange = std::numeric_limits<float>::max();
	moon_.rotate.y = std::numbers::pi_v<float>;
	moon_.scale *= 300.0f;
	moon_.pos.y = -360.0f;
	moon_.rotate.z = std::numbers::pi_v<float> / 10.0f * static_cast<float>(maxStage_ - currentStage_);

	bubble_.LoadTexture("./Resources/StageSelect/hukidasi_stageSelect.png");
	bubble_.isSameTexSize = true;
	bubble_.texScalar = 0.48f;
	bubble_.pos.y = 110.0f;
	bubbleY_.first = bubble_.pos.y - 10.0f;
	bubbleY_.second = bubble_.pos.y + 10.0f;
	bubbleEase_.Start(true, 0.5f, Easeing::OutQuad);


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


	stageNumberTex_.LoadTexture("./Resources/Result/number.png");
	stageNumberTex_.uvSize.x = 0.1f;
	stageNumberTex_.color = 0xce591dff;
	stageNumberTex_.scale *= 70.0f;
	stageNumberTex_.pos.x = 36.0f;
	stageNumberTex_.pos.y = 77.0f;
	stageTenNumberTex_.LoadTexture("./Resources/Result/number.png");
	stageTenNumberTex_.uvSize.x = 0.1f;
	stageTenNumberTex_.color = 0xce591dff;
	stageTenNumberTex_.scale *= 70.0f;
	stageTenNumberTex_.pos.x = -36.0f;
	stageTenNumberTex_.pos.y = stageNumberTex_.pos.y;
	stageNumberTexY_ = { stageNumberTex_.pos.y - 10.0f, stageNumberTex_.pos.y + 10.0f };

	stageTex_.LoadTexture("./Resources/StageSelect/stageSelect_UI_stage.png");
	stageTex_.isSameTexSize = true;
	stageTex_.texScalar = 0.48f;
	stageTex_.pos.y = 130.0f;
	stageTexY_.first = stageTex_.pos.y - 10.0f;
	stageTexY_.second = stageTex_.pos.y + 10.0f;

	arrowRight_.LoadTexture("./Resources/StageSelect/arrow.png");
	arrowRight_.pos = Vector2{ 282.0f, 34.0f };
	arrowRight_.isSameTexSize = true;
	arrowRight_.texScalar = 0.41f;
	arrowRight_.rotate.z = 0.28f;
	arrowRightPos_ = 
	{ 
		Vector3{arrowRight_.pos.x-5.0f, arrowRight_.pos.y+5.0f, arrowRight_.pos.z },
		Vector3{arrowRight_.pos.x + 10.0f, arrowRight_.pos.y - 5.0f, arrowRight_.pos.z }
	};
	arrowLeft_.LoadTexture("./Resources/StageSelect/arrow.png");
	arrowLeft_.pos = Vector2{ -282.0f, 34.0f };
	arrowLeft_.isSameTexSize = true;
	arrowLeft_.texScalar = 0.41f;
	arrowLeft_.rotate.z = -0.28f - std::numbers::pi_v<float>;
	arrowLeftPos_ =
	{
		Vector3{arrowLeft_.pos.x + 5.0f, arrowLeft_.pos.y + 5.0f, arrowLeft_.pos.z },
		Vector3{arrowLeft_.pos.x - 10.0f, arrowLeft_.pos.y - 5.0f, arrowLeft_.pos.z }
	};

	arrowEase_.Start(true, 0.8f, Easeing::InOutSine);

	isClearTex_.LoadTexture("./Resources/StageSelect/stageSelect_UI_clear1.png");
	isClearTex_.isSameTexSize = true;
	isClearTex_.texScalar = 0.47f;
	isClearTex_.pos = Vector2{ 169.0f, 230.0f };
	isClearTex_.rotate.z = 0.43f;
	isClearTexY_ = {
		isClearTex_.pos.y - 10.0f,
		isClearTex_.pos.y + 10.0f
	};

	backGroundParticle_.LopadSettingDirectory("backGroundParticle");
	backGroundParticle_.ParticleStart();

	bgm_ = audioManager_->LoadWav("./Resources/Audio/BGM/BGM/stageSelect.wav", true);
	bgm_->Start(0.2f);
	choiceSE_ = audioManager_->LoadWav("./Resources/Audio/kouka/kouka/UI_sentaku.wav", false);
	decideSE_ = audioManager_->LoadWav("./Resources/Audio/kouka/kouka/UI_kettei.wav", false);
}

void StageSelect::Finalize() {
	bgm_->Stop();
}

void StageSelect::Update() {
	if (input_->GetKey()->Pushed(DIK_RIGHT) ||
		input_->GetKey()->Pushed(DIK_D) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::RIGHT) ||
		input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) > 0.3f
		) 
	{
		currentStage_++;
		rotateEase_.Start(false, 0.2f, Easeing::OutElastic);
		moonRotateY_.first = moon_.rotate.z;

		choiceSE_->Start(0.25f);
	}

	else if (input_->GetKey()->Pushed(DIK_LEFT) ||
		input_->GetKey()->Pushed(DIK_A) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::LEFT) ||
		input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) < -0.3f
		)
	{
		currentStage_--;
		rotateEase_.Start(false, 0.2f, Easeing::OutElastic);
		moonRotateY_.first = moon_.rotate.z;

		choiceSE_->Start(0.25f);
	}
	if (maxStage_ < currentStage_) {
		currentStage_ = 1;
	}
	else if (currentStage_ < 1) {
		currentStage_ = maxStage_;
	}
	moonRotateY_.second = std::numbers::pi_v<float> / 10.0f * static_cast<float>(maxStage_ - currentStage_);

	
	stageNumberTex_.uvPibot.x = static_cast<float>(currentStage_) * 0.1f;
	if (maxStage_ == currentStage_) {
		stageTenNumberTex_.uvPibot.x = 0.1f;
	}
	else {
		stageTenNumberTex_.uvPibot.x = 0.0f;
	}

	if (rotateEase_.ActiveEnter() || rotateEase_.ActiveStay()) {
		moon_.rotate.z = rotateEase_.Get(moonRotateY_.first, moonRotateY_.second);
	}

	for (auto& i : backGround_) {
		i.Update();
	}
	
	moon_.Update();
	
	bubble_.pos.y = bubbleEase_.Get(bubbleY_.first, bubbleY_.second);
	bubble_.Update();
	
	stageNumberTex_.pos.y = bubbleEase_.Get(stageNumberTexY_.first, stageNumberTexY_.second);
	stageNumberTex_.Update();
	stageTenNumberTex_.pos.y = bubbleEase_.Get(stageNumberTexY_.first, stageNumberTexY_.second);
	stageTenNumberTex_.Update();
	
	stageTex_.pos.y = bubbleEase_.Get(stageTexY_.first, stageTexY_.second);
	stageTex_.Update();

	arrowRight_.pos = arrowEase_.Get(arrowRightPos_.first, arrowRightPos_.second);
	arrowRight_.Update();
	arrowLeft_.pos = arrowEase_.Get(arrowLeftPos_.first, arrowLeftPos_.second);
	arrowLeft_.Update();


	isClearTex_.pos.y = bubbleEase_.Get(isClearTexY_.first, isClearTexY_.second);
	isClearTex_.Update();

	rotateEase_.Update();
	bubbleEase_.Update();
	arrowEase_.Update();

	backGroundParticle_.Update();

	if (input_->GetKey()->Pushed(DIK_SPACE) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::A)
		) {
		bgm_->Stop();
		decideSE_->Start(0.2f);
		auto gameScene = new GameScene;
		assert(gameScene);
		gameScene->SetStageNumber(currentStage_-1);

		sceneManager_->SceneChange(gameScene);
	}
}

void StageSelect::Draw() {
	camera_.Update();

	backGroundBlur_.PreDraw();
	for (auto& i : backGround_) {
		i.Draw(camera_.GetViewOthographics());
	}
	backGroundBlur_.Draw(Pipeline::None);
	backGroundParticle_.Draw(camera_.GetViewOthographics());

	bubble_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	
	stageTex_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	stageNumberTex_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	stageTenNumberTex_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);

	arrowRight_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	arrowLeft_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);

	if (sceneManager_->isClearStage_[currentStage_-1]) {
		isClearTex_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	}

	moon_.Draw(camera_.GetViewOthographics(), camera_.pos);
}