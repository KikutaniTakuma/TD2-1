#include "Engine/Engine.h"
#include "Engine/ErrorCheck/ErrorCheck.h"

#include "SceneManager/ResultScene/ResultScene.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// ライブラリ初期化
	if (!Engine::Initialize("DirectXGame", Engine::Resolution::HD)) {
		ErrorCheck::GetInstance()->ErrorTextBox(
			"InitializeDirect3D() : Engine::Initialize() Failed",
			"WinMain"
		);
		return -1;
	}
	// シーンマネージャー初期化
	auto* const sceneManager = SceneManager::GetInstace();
	auto scene = new ResultScene{};
	scene->SetClearTime(std::chrono::milliseconds{ 1000 });
	scene->SetStageNumber(1);
	
	sceneManager->Initialize(scene);

	// メインループ
	sceneManager->Game();

	// ライブラリ終了
	Engine::Finalize();

	return 0;
}