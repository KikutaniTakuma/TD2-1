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

	/*particles.push_back(Particle{});
	particles.back().LopadSettingDirectory("break");*/

	texs_.push_back(Texture2D{});
	texs_.back().LoadTexture("./Resources/Watame.png");
	texs_.back().scale = texs_.back().GetTexSize();
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

	//particles.back().Debug("particle");
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