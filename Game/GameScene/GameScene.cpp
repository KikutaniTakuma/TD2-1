#include "GameScene.h"

GameScene* GameScene::GetInstance() {
	static GameScene gameScene;
	return &gameScene;
}

void GameScene::Initialize() {
	
	title_ = std::make_unique<Title>();
	play_ = std::make_unique<Play>();
	result_ = std::make_unique<Result>();

	switch (scene_)
	{
	case GameScene::Scene::kTitle:

		title_->Initialize();
		break;
	case GameScene::Scene::kPlay:

		play_->Initialize();
		break;
	case GameScene::Scene::kResult:

		result_->Initialize();
		break;
	default:
		break;
	}
}

void GameScene::Update() {

	if (sceneRequest_) {

		scene_ = sceneRequest_.value();

		switch (scene_)
		{
		case GameScene::Scene::kTitle:

			title_->Initialize();
			break;
		case GameScene::Scene::kPlay:

			play_->Initialize();
			break;
		case GameScene::Scene::kResult:

			result_->Initialize();
			break;
		default:
			break;
		}
	}

	switch (scene_)
	{
	case GameScene::Scene::kTitle:

		title_->Update();
		break;
	case GameScene::Scene::kPlay:

		play_->Update();
		break;
	case GameScene::Scene::kResult:

		result_->Update();
		break;
	default:
		break;
	}

}

void GameScene::Draw() {

	switch (scene_)
	{
	case GameScene::Scene::kTitle:

		title_->Draw();
		break;
	case GameScene::Scene::kPlay:

		play_->Draw();
		break;
	case GameScene::Scene::kResult:

		result_->Draw();
		break;
	default:
		break;
	}
}


