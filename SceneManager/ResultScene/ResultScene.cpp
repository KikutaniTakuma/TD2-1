#include "ResultScene.h"
#include "Engine/WinApp/WinApp.h"
#include "externals/imgui/imgui.h"
#include "SceneManager/GameScene/GameScene.h"
#include "SceneManager/StageSelect/StageSelect.h"

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
	isUpdate_(false),
	timer_(),
	timerUI_(),
	nextStageMassage_(),
	stageSelectMassage_(),
	arrow_(),
	nowChoose_(),
	isCanSelect_(false),
	stageNumber_()
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
	speechBubble_.LoadTexture("./Resources/Result/speechBubble.png");
	speechBubble_.scale = speechBubble_.GetTexSize() * 0.85f;
	speechBubble_.pos = { 218.0f,26.0f,0.01f };


	// 星の位置設定
	for (size_t i = 0; i < stars_.size();i++) {
		stars_[i].pos_.x = 55.0f + (178.0f * static_cast<float>(i));
		stars_[i].pos_.y = -55.0f;
	}
	stars_[1].pos_.y += 16.0f;
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

	// プレイヤーモデルのロード
	player_.LoadObj("./Resources/Player/player.obj");
	player_.light.ligDirection = { 0.0f, 0.0f, 1.0f };
	player_.light.ligColor = Vector3::identity;
	player_.light.ptRange = std::numeric_limits<float>::max();
	player_.rotate = { -0.1f, 2.962f, -0.12f };
	player_.pos = { -357.0f, -33.7f,-400.0f };
	player_.scale *= 170.0f;
	playerScale_ = { Vector3{190.0f, 150.0f, 150.0f}, Vector3{ 150.0f,190.0f,190.0f  } };
	playerScaleEase_.Start(
		true,
		0.75f,
		Easeing::GetFunction(24)
	);

	playerAnimationTex_.reserve(5);
	playerAnimationTex_.push_back(textureManager_->LoadTexture("./Resources/Player/player_face.png"));
	playerAnimationTex_.push_back(textureManager_->LoadTexture("./Resources/Player/player_face2.png"));
	playerAnimationTex_.push_back(textureManager_->LoadTexture("./Resources/Player/player_face3.png"));
	playerAnimationTex_.push_back(textureManager_->LoadTexture("./Resources/Player/player_face4.png"));
	playerAnimationTex_.push_back(textureManager_->LoadTexture("./Resources/Player/player_face5.png"));

	playerAnimationDuration_ = std::chrono::milliseconds{ 66 };
	currentPlayerAnimation_ = 0;
	isPlayerAnimationTurnBack_ = false;
	playerAnimationStartTime_ = std::chrono::steady_clock::now();

	playerAnimationCoolTime_ = std::chrono::milliseconds{ 800 };
	isPlayerAnimationCoolTime_ = true;
	playerAnimationCoolStartTime_ = playerAnimationStartTime_;


	// タイマーテクスチャ
	timer_.LoadTexture("./Resources/Result/result_UI_time.png");
	timer_.texScalar = 0.59f;
	timer_.isSameTexSize = true;
	timer_.pos = { 49.0f, 141.0f, 0.01f };
	timerUI_.LoadTexture("./Resources/Result/result_UI_scoreLine.png");
	timerUI_.texScalar = 0.58f;
	timerUI_.isSameTexSize = true;
	timerUI_.pos = { 243.0f, 107.0f, 0.01f };

	// ステージセレクトUI
	nextStageMassage_.LoadTexture("./Resources/Result/result_UI_next.png");
	nextStageMassage_.texScalar = 0.37f;
	nextStageMassage_.isSameTexSize = true;
	nextStageMassage_.pos = { 447.0f, -208.0f, 0.01f };
	stageSelectMassage_.LoadTexture("./Resources/Result/result_UI_stageSele.png");
	stageSelectMassage_.texScalar = 0.35f;
	stageSelectMassage_.isSameTexSize = true;
	stageSelectMassage_.pos = { 457.0f, -290.0f, 0.01f };
	
	arrow_.LoadTexture("./Resources/Result/arrow.png");
	arrow_.texScalar = 0.23f;
	arrow_.isSameTexSize = true;
	arrow_.pos = { 232.0f, -212.0f, 0.01f };

	arrowPosY_ = { -212.0f, -293.0f };
	arrowPosX_ = { arrow_.pos.x - 10.0f, arrow_.pos.x + 10.0f };

	if (score_ <= 0) {
		isCanSelect_ = true;
		isUpdate_ = true;
	}

	arrowEase_.Start(true, 0.5f, Easeing::InOutQuad);


	// 時間表示
	float clearSecond = (static_cast<float>(clearTime_.count()) / 1000.0f);
	clearSecond = std::clamp(clearSecond, 0.0f, 5999.0f);
	float minutes = std::floor(clearSecond / 60.0f);
	float tenMinutes = std::floor(minutes / 10.0f);
	float tenSeconds = std::floor((clearSecond - (minutes * 60.0f)) * 0.1f);
	float seconds = std::floor(clearSecond - (minutes * 60.0f) - (tenSeconds * 10.0f));
	tenMinutes_.LoadTexture("./Resources/Result/number.png");
	tenMinutes_.uvSize.x = 0.1f;
	tenMinutes_.uvPibot.x = tenMinutes * 0.1f;
	tenMinutes_.pos = Vector2{ 166.0f, 143.0f };
	tenMinutes_.scale *= 70.0f;
	tenMinutes_.color = 0x0B0B0BFF;
	minutes_.LoadTexture("./Resources/Result/number.png");
	minutes_.uvSize.x = 0.1f;
	minutes_.uvPibot.x = minutes * 0.1f;
	minutes_.pos = Vector2{ 239.0f, 143.0f };
	minutes_.scale *= 70.0f;
	minutes_.color = 0x0B0B0BFF;
	tenSeconds_.LoadTexture("./Resources/Result/number.png");
	tenSeconds_.uvSize.x = 0.1f;
	tenSeconds_.uvPibot.x = tenSeconds * 0.1f;
	tenSeconds_.pos = Vector2{ 358.0f, 143.0f };
	tenSeconds_.scale *= 70.0f;
	tenSeconds_.color = 0x0B0B0BFF;
	seconds_.LoadTexture("./Resources/Result/number.png");
	seconds_.uvSize.x = 0.1f;
	seconds_.uvPibot.x = seconds * 0.1f;
	seconds_.pos = Vector2{ 431.0f, 143.0f };
	seconds_.scale *= 70.0f;
	seconds_.color = 0x0B0B0BFF;

	colon1_.LoadTexture("./Resources/ball.png");
	colon1_.isSameTexSize = true;
	colon1_.texScalar = 0.13f;
	colon1_.pos = { 300.0f, 160.0f, 0.01f };
	colon1_.color = 0x0B0B0BFF;
	colon2_.LoadTexture("./Resources/ball.png");
	colon2_.isSameTexSize = true;
	colon2_.texScalar = 0.13f;
	colon2_.pos = { 300.0f, 126.0f, 0.01f };
	colon2_.color = 0x0B0B0BFF;

	resultUI_.LoadTexture("./Resources/Result/result_UI_stage.png");
	resultUI_.isSameTexSize = true;
	resultUI_.texScalar = 0.38f;
	resultUI_.pos = Vector2{ 218.0f, 283.0f };

	float stageTenNumber = std::floor(static_cast<float>(stageNumber_) / 10.0f);
	float stageNumber = static_cast<float>(stageNumber_) - stageTenNumber * 10.0f;
	stageTenNumberTex_.LoadTexture("./Resources/Result/number.png");
	stageTenNumberTex_.uvSize.x = 0.1f;
	stageTenNumberTex_.uvPibot.x = stageTenNumber * 0.1f;
	stageTenNumberTex_.color = 0xce591dff;
	stageTenNumberTex_.pos = Vector2{ 318, 260 };
	stageTenNumberTex_.scale *= 70.0f;
	stageNumberTex_.LoadTexture("./Resources/Result/number.png");
	stageNumberTex_.uvSize.x = 0.1f;
	stageNumberTex_.uvPibot.x = stageNumber * 0.1f;
	stageNumberTex_.color = 0xce591dff;
	if (stageNumber_ < 10) {
		stageNumberTex_.pos = stageTenNumberTex_.pos;
	}
	else {
		stageNumberTex_.pos = Vector2{ 380, 260 };
	}
	stageNumberTex_.scale *= 70.0f;

	sceneManager_->isClearStage_[stageNumber_-1] = true;


	backGroundParticle_.LopadSettingDirectory("backGroundParticle");
	backGroundParticle_.ParticleStart();
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
	if (score_ <= 0) {
		isCanSelect_ = true;
		isUpdate_ = true;
	}
}

void ResultScene::SetStageNumber(int32_t stageNumber) {
	stageNumber_ = std::clamp(stageNumber, 0, 99);
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
		if (currentStar_ < score_ && currentStar_ < stars_.size()&&
			starEffectDuration_ < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime_))
		{
			stars_[currentStar_].Start();
			startTime_ = nowTime;
			currentStar_++;
		}

		if (score_ && stars_[score_-1].GetEndFlg().OnEnter()) {
			for (size_t i = 0; i < stars_.size(); i++) {
				stars_[i].NormalStart();
			}
			isCanSelect_ = true;
		}

		for (size_t i = 0; i < stars_.size(); i++) {
			stars_[i].Update();
		}

		for (auto& i : starsGray_) {
			i.Update();
		}

		if (isCanSelect_) {
			if (input_->GetKey()->Pushed(DIK_W)||
				input_->GetKey()->Pushed(DIK_UP)||
				input_->GetGamepad()->Pushed(Gamepad::Button::UP)||
				input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) > 0.3f
				)
			{
				nowChoose_--;
			}
			else if (
				input_->GetKey()->Pushed(DIK_S) ||
				input_->GetKey()->Pushed(DIK_DOWN) || 
				input_->GetGamepad()->Pushed(Gamepad::Button::DOWN) ||
				input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_Y) < -0.3f
				)
			{
				nowChoose_++;
			}
			nowChoose_ = std::clamp(nowChoose_, 0, 1);

			nextStageMassage_.Update();
			stageSelectMassage_.Update();

			if (nowChoose_ == 0) {
				arrow_.pos.y = arrowPosY_.first;
			}
			else {
				arrow_.pos.y = arrowPosY_.second;
			}

			arrow_.pos.x = arrowEase_.Get(arrowPosX_.first, arrowPosX_.second);

			arrow_.Update();
			arrowEase_.Update();
		}
	}

	backGroundParticle_.Update();

	tenMinutes_.Update();
	minutes_.Update();
	tenSeconds_.Update();
	seconds_.Update();
	colon1_.Update();
	colon2_.pos.x = colon1_.pos.x;
	colon2_.Update();

	resultUI_.Update();
	stageNumberTex_.Update();
	stageTenNumberTex_.Update();

	//playerScaleEase_.Debug("playerScaleEase_");
	player_.scale = playerScaleEase_.Get(playerScale_.first, playerScale_.second);
	if (isPlayerAnimationCoolTime_ && playerAnimationCoolTime_ < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - playerAnimationCoolStartTime_)) {
		isPlayerAnimationCoolTime_ = false;
	}
	
	if (!isPlayerAnimationCoolTime_ && playerAnimationDuration_ < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - playerAnimationStartTime_)) {
		isPlayerAnimationTurnBack_ ? --currentPlayerAnimation_ : ++currentPlayerAnimation_;
		if (currentPlayerAnimation_ >= static_cast<int32_t>(playerAnimationTex_.size())-1) {
			isPlayerAnimationTurnBack_ = true;
		}
		else if (currentPlayerAnimation_ <= 0) {
			isPlayerAnimationTurnBack_ = false;
			isPlayerAnimationCoolTime_ = true;
			playerAnimationCoolStartTime_ = nowTime;
		}
		currentPlayerAnimation_ = std::clamp(currentPlayerAnimation_, 0, static_cast<int32_t>(playerAnimationTex_.size())-1);
		player_.ChangeTexture("face", playerAnimationTex_[currentPlayerAnimation_]->GetFileName());
		playerAnimationStartTime_ = nowTime;
	}

	player_.Update();
	playerScaleEase_.Update();

	timer_.Update();
	timerUI_.Update();

	if (input_->GetKey()->Pushed(DIK_SPACE) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::A)
		) {
		if (nowChoose_ == 0) {
			auto gameScene = new GameScene;
			assert(gameScene);
			gameScene->SetStageNumber(stageNumber_);

			sceneManager_->SceneChange(gameScene);
		}
		// ここはタイトルになる予定
		else {
			auto stageSelect = new StageSelect;
			assert(stageSelect);

			sceneManager_->SceneChange(stageSelect);
		}
	}
}

void ResultScene::Draw() {
	camera_.Update();

	backGroundBlur_.PreDraw();
	for (auto& i : backGround_) {
		i.Draw(camera_.GetViewOthographics());
	}
	backGroundBlur_.Draw(Pipeline::None);

	backGroundParticle_.Draw(camera_.GetViewOthographics(), Pipeline::Normal);

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

	timer_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	timerUI_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);

	if (isCanSelect_) {
		nextStageMassage_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
		stageSelectMassage_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
		arrow_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	}

	tenMinutes_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	minutes_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	tenSeconds_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	seconds_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	colon1_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	colon2_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	resultUI_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	stageNumberTex_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	if (static_cast<int32_t>(sceneManager_->isClearStage_.size()) <= stageNumber_) {
		stageTenNumberTex_.Draw(camera_.GetViewOthographics(), Pipeline::Normal, false);
	}
	player_.Draw(camera_.GetViewOthographics(), camera_.pos);
}