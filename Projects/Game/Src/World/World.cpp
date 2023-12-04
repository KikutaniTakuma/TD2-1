#include "World.h"
#include "Scenes/Manager/SceneManager.h"
#include "Editor/ParticleEditor/ParticleEditor.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"

World::World():
	sceneManager_{},
	particleEditor_{},
	inactiveSprite_{},
	staticCameraMatrix_{}
{}

void World::Initialize() {
	Framework::Initialize();

	sceneManager_ = SceneManager::GetInstance();

	sceneManager_->Initialize(BaseScene::ID::Title, BaseScene::ID::Title);

	ParticleEditor::Initialize();
	particleEditor_ = ParticleEditor::GetInstance();

	inactiveSprite_.reset(new Texture2D{});
	inactiveSprite_->scale = WindowFactory::GetInstance()->GetClientSize();

	inactiveSprite_->color = 130;
	inactiveSprite_->Update();

	staticCameraMatrix_.reset(new Mat4x4{});

	Camera camera;
	camera.Update();
	*staticCameraMatrix_ = camera.GetViewOthographics();
}

void World::Finalize() {
	if (sceneManager_) {
		sceneManager_->Finalize();
	}

	inactiveSprite_.reset();
	staticCameraMatrix_.reset();

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
		if (!WindowFactory::GetInstance()->IsThisWindowaActive()) {
			inactiveSprite_->Draw(*staticCameraMatrix_, Pipeline::Normal, false);
		}
	}
	else {
		isEnd_ = true;
	}
}