#include "Engine/Engine.h"
#include "externals/imgui/imgui.h"

#include "Engine/WinApp/WinApp.h"

#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Engine/FrameInfo/FrameInfo.h"

#include "Input/Input.h"

#include "Utils/Camera/Camera.h"
#include "Drawers/Texture2D/Texture2D.h"

#include "Game/GameScene/GameScene.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリ初期化
	if (!Engine::Initialize("DirectXGame", Engine::Resolution::HD)) {
		ErrorCheck::GetInstance()->ErrorTextBox(
			"InitializeDirect3D() : Engine::Initialize() Failed",
			"WinMain"
		);
		return -1;
	}

	// フォントロード
	Engine::LoadFont("./Resources/Font/fonttest.spritefont");

	static auto* const frameInfo = FrameInfo::GetInstance();

	auto input = Input::GetInstance();

	GameScene* gameScene = GameScene::GetInstance();
	gameScene->Initialize();

	Camera camera{ Camera::Type::Othographic };

	Texture2D tex;
	tex.LoadTexture("./Resources/uvChecker.png");
	tex.scale *= 512.0f;

	/// 
	/// メインループ
	/// 
	while (Engine::WindowMassage()) {
		// 描画開始処理
		Engine::FrameStart();

		// fps
		ImGui::Begin("fps");
		ImGui::Text("Frame rate: %3.0lf fps", frameInfo->GetFps());
		ImGui::Text("Delta Time: %.4lf", frameInfo->GetDelta());
		ImGui::Text("Frame Count: %llu", frameInfo->GetFrameCount());
		ImGui::End();

		// 入力処理
		input->InputStart();


		/// 
		/// 更新処理
		/// 

		tex.Update();
		gameScene->Update();

		///
		/// 更新処理ここまで
		/// 

		///
		/// 描画処理
		/// 
		camera.Update();

		//tex.Draw(camera.GetViewOthographics(), Pipeline::Normal, false);

		gameScene->Draw();

		///
		/// 描画処理ここまで
		/// 


		// フレーム終了処理
		Engine::FrameEnd();

		// Escapeが押されたら終了
		if (input->GetKey()->Pushed(DIK_ESCAPE) || input->GetGamepad()->Pushed(Gamepad::Button::BACK)) {
			break;
		}
	}

	// ライブラリ終了
	Engine::Finalize();

	return 0;
}