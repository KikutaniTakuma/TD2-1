#include "SceneManager.h"

bool BaseScene::isPad_ = false;

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

SceneManager::SceneManager() {
	fade_ = std::make_unique<Fade>();
	fadeCamera.Update();

	assert(fade_);
}

void SceneManager::Initialize(BaseScene* firstScene) {
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
	if (scene_ && !next_ && !fade_->InStay()) {
		scene_->Update();
	}
	else {
		fade_->Update();
	}

	if (fade_->OutEnd()) {
		fade_->InStart();
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

	fade_->Draw(fadeCamera.GetViewOthographics());
}
