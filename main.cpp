#include "Engine/Engine.h"
#include "Engine/ErrorCheck/ErrorCheck.h"

#include "SceneManager/SceneManager.h"
#include "SceneManager/GameScene/GameScene.h"
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
	sceneManager->Initialize(new ResultScene{});

	// メインループ
	sceneManager->Game();

	// ライブラリ終了
	Engine::Finalize();

	return 0;
}