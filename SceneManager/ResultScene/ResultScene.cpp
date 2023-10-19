#include "ResultScene.h"

ResultScene::ResultScene():
	BaseScene(BaseScene::ID::Result),
	models_(),
	texs_(),
	particles_(),
	globalVariables_(),
	clearTime_(),
	hoge()
{

}

void ResultScene::Initialize() {
	camera_.farClip = 3000.0f;

	globalVariables_.LoadFile();
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

	hoge.Update();
}

void ResultScene::Draw() {
	camera_.Update();

	for (auto& model : models_) {
		model.Draw(camera_.GetViewProjection(), camera_.GetPos());
	}

	for (auto& tex : texs_) {
		tex.Draw(camera_.GetViewOthographics());
	}

	hoge.Draw(camera_.GetViewOthographics());
}