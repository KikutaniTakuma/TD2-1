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
	camera_.pos.z -= 10.0f;
	
	globalVariables_.LoadFile();
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

	for (auto& particle : particles) {
		particle.Update();
	}

#ifdef _DEBUG
	/*if (input_->GetKey()->Pushed(DIK_SPACE)) {
		models_.back().ChangeTexture("Material", "./Resources/uvChecker.png");
	}*/
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

	for (auto& particle : particles) {
		particle.Draw(camera_.GetViewOthographics());
	}
}