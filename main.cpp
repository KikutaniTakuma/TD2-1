#include "Engine/Engine.h"
#include "externals/imgui/imgui.h"

#include "Engine/WinApp/WinApp.h"

#include "Engine/ErrorCheck/ErrorCheck.h"
#include "Engine/FrameInfo/FrameInfo.h"

#include "Input/Input.h"

#include "Utils/Camera/Camera.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Particle/Particle.h"

#include "SceneManager/Fade/Fade.h"

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

	/*Particle hoge;
	hoge.LopadSettingDirectory("smoke");*/

	Camera camera;
	camera.pos.z = -3.0f;

	Fade fade;

	/// 
	/// メインループ
	/// 
	while (Engine::WindowMassage()) {
		// 描画開始処理
		Engine::FrameStart();

		// fps
		frameInfo->Debug();

		// 入力処理
		input->InputStart();


		/// 
		/// 更新処理
		/// 

		/*hoge.Update();
		hoge.Debug("particle test");*/

		fade.Update();

		///
		/// 更新処理ここまで
		/// 

		///
		/// 描画処理
		/// 
		camera.Update();

		fade.Draw(camera.GetViewOthographics());

		//hoge.Draw(camera.GetViewOthographics());

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