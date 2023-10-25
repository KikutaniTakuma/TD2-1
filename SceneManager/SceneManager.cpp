#include "SceneManager.h"
#include "Engine/Engine.h"
#include "Utils/UtilsLib/UtilsLib.h"

BaseScene::BaseScene(BaseScene::ID sceneID):
	sceneManager_(nullptr),
	meshManager_(nullptr),
	audioManager_(nullptr),
	textureManager_(nullptr),
	frameInfo_(nullptr),
	input_(nullptr),
	sceneID_(sceneID),
	camera_()
{}

void BaseScene::SceneInitialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;

	meshManager_ = MeshManager::GetInstance();

	audioManager_ = AudioManager::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	frameInfo_ = FrameInfo::GetInstance();

	input_ = Input::GetInstance();
}

SceneManager* const SceneManager::GetInstace() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::Initialize(BaseScene* firstScene) {
	fade_ = std::make_unique<Fade>();
	fadeCamera_.Update();

	frameInfo_ = FrameInfo::GetInstance();
	input_ = Input::GetInstance();

	auto textureManager = TextureManager::GetInstance();

	auto texNames = UtilsLib::GetFilePathFormDir("./Resources/", ".png");
	for (auto& i : texNames) {
		textureManager->LoadTexture(i.string());
	}

	auto meshManager = MeshManager::GetInstance();

	auto meshNames = UtilsLib::GetFilePathFormDir("./Resources/", ".obj");

	for (auto& i : meshNames) {
		meshManager->LoadObj(i.string());
	}

	auto audioManager = AudioManager::GetInstance();

	auto seNames = UtilsLib::GetFilePathFormDir("./Resources/Audio/kouka/", ".wav");

	for (auto& i : seNames) {
		audioManager->LoadWav(i.string(), false);
	}

	auto bgmNames = UtilsLib::GetFilePathFormDir("./Resources/Audio/BGM/", ".wav");

	for (auto& i : bgmNames) {
		audioManager->LoadWav(i.string(), true);
	}

	assert(firstScene != nullptr);
	scene_.reset(firstScene);
	scene_->SceneInitialize(this);
	scene_->Initialize();
}

void SceneManager::SceneChange(BaseScene* next) {
	assert(next != nullptr);
	if (next_) {
		return;
	}
	next_.reset(next);
	next_->SceneInitialize(this);

	fade_->OutStart();
}

void SceneManager::Update() {
	if (input_->GetGamepad()->PushAnyKey()) {
		isPad_ = true;
	}
	else if (input_->GetMouse()->PushAnyKey() || input_->GetKey()->PushAnyKey()) {
		isPad_ = false;
	}

	if (scene_ && !next_ && !fade_->InStay()) {
		scene_->Update();
	}
	else {
		fade_->Update();
	}

	if (fade_->OutEnd()) {
		fade_->InStart();
		scene_->Finalize();
		scene_.reset(next_.release());
		next_.reset();
		scene_->Initialize();
		scene_->Update();
		fade_->Update();
	}
	else if (fade_->InEnd()) {
		fade_->Update();
	}
}

void SceneManager::Draw() {
	if (scene_) {
		scene_->Draw();
	}

	fade_->Draw(fadeCamera_.GetViewOthographics());
}

void SceneManager::Game() {
	/// 
	/// メインループ
	/// 
	while (Engine::WindowMassage()) {
		// 描画開始処理
		Engine::FrameStart();

		// fps
		frameInfo_->Debug();

		// 入力処理
		input_->InputStart();

#ifdef _DEBUG		
		if (frameInfo_->GetIsDebugStop() && frameInfo_->GetIsOneFrameActive()) {
			this->Update();
			frameInfo_->SetIsOneFrameActive(false);
		}
		else if(!frameInfo_->GetIsDebugStop()){
			// 更新処理
			this->Update();
		}
#else
		// 更新処理
		this->Update();
#endif

		// 描画処理
		this->Draw();

		// フレーム終了処理
		Engine::FrameEnd();

		// Escapeが押されたら終了
		if (scene_->GetID() == BaseScene::ID::Title) {
			if (input_->GetKey()->Pushed(DIK_ESCAPE) || input_->GetGamepad()->Pushed(Gamepad::Button::START)) {
				break;
			}
		}
	}
}
