#include "ResultScene.h"

void ResultScene::Initialize() {
	camera_.farClip = 3000.0f;

	globalVariables_.LoadFile();

	texs_.push_back(Texture2D());
	texs_.back().LoadTexture("./Resources/Watame.png");
	texs_.back().scale = texs_.back().GetTexSize();
}

void ResultScene::Finalize() {

}

void ResultScene::Update() {
	for (auto& model : models_) {
		model.Update();
	}

	for (auto& tex : texs_) {
		tex.Update();
	}
}

void ResultScene::Draw() {
	camera_.Update();

	for (auto& model : models_) {
		model.Draw(camera_.GetViewProjection(), camera_.GetPos());
	}

	for (auto& tex : texs_) {
		tex.Draw(camera_.GetViewOthographics());
	}
}