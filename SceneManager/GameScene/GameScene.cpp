#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include "externals/imgui/imgui.h"
#include "SceneManager/ResultScene/ResultScene.h"

GameScene::GameScene():
	models_(),
	texs_(),
	globalVariables_()
{}

void GameScene::Initialize() {
	camera_.farClip = 3000.0f;

	globalVariables_.LoadFile();

	texs_.push_back(Texture2D());
	texs_.back().scale *= 512.0f;
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	for (auto& model : models_) {
		model.Update();
	}

	for (auto& tex : texs_) {
		tex.Update();
	}

#ifdef _DEBUG
	if (input_->GetKey()->Pushed(DIK_SPACE)) {
		sceneManager_->SceneChange(new ResultScene{});
	}
#endif // _DEBUG

}

void GameScene::Draw() {
	camera_.Update();
	for (auto& model : models_) {
		model.Draw(camera_.GetViewProjection(), camera_.GetPos());
	}

	for (auto& tex : texs_) {
		tex.Draw(camera_.GetViewOthographics());
	}
}