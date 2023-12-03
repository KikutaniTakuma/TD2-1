#include "World.h"
#include "Scenes/Manager/SceneManager.h"
#include "Editor/ParticleEditor/ParticleEditor.h"

void World::Initialize() {
	Framework::Initialize();

	sceneManager_ = SceneManager::GetInstance();

	sceneManager_->Initialize(BaseScene::ID::Title, BaseScene::ID::Title);

	ParticleEditor::Initialize();
	particleEditor_ = ParticleEditor::GetInstance();
}

void World::Finalize() {
	if (sceneManager_) {
		sceneManager_->Finalize();
	}

	ParticleEditor::Finalize();

	Framework::Finalize();
}

void World::Update() {
	if (sceneManager_) {
		sceneManager_->Update();
		particleEditor_->Editor();
		isEnd_ = sceneManager_->IsEnd();
	}
	else {
		isEnd_ = true;
	}
}

void World::Draw() {
	if (sceneManager_) {
		sceneManager_->Draw();
		particleEditor_->Draw();
	}
	else {
		isEnd_ = true;
	}
}