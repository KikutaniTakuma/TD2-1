#include "Framework.h"
#include "Engine/Engine.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Utils/ExecutionLog/ExecutionLog.h"

#include "Input/Input.h"

#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Line/Line.h"
#include "Drawers/Particle/Particle.h"

#include "Utils/Math/Vector2.h"

void Framework::Initialize() {
	// ライブラリ初期化
	if (!Engine::Initialize("DirectXGame", Vector2{ 1280.0f, 720.0f })) {
		Lamb::ErrorLog(
			"somthing error",
			"Engine::Initialize()",
			"Framework"
		);
		return;
	}

	// 入力処理初期化
	Input::Initialize();

	// 各種パイプライン生成
	Texture2D::Initialize();
	Mesh::Initialize();
	Model::Initialize();
	Line::Initialize();
	Particle::Initialize();
}

void Framework::Finalize() {
	// インデックスリソース解放
	Particle::Finalize();
	Texture2D::Finalize();

	// 入力関連解放
	Input::Finalize();

	// ライブラリ終了
	Engine::Finalize();
}

void Framework::Execution() {
	this->Initialize();

	FrameInfo* const frameInfo = FrameInfo::GetInstance();
	Input* const input = Input::GetInstance();
	WindowFactory* const window = WindowFactory::GetInstance();

	while (window->WindowMassage()) {
		// 描画開始処理
		Engine::FrameStart();

		// fps
		frameInfo->Debug();

		// 入力処理
		input->InputStart();

		// フルスクリーン化
		window->Fullscreen();

#ifdef _DEBUG		
		if (frameInfo->GetIsDebugStop() && frameInfo->GetIsOneFrameActive()) {
			this->Update();
			frameInfo->SetIsOneFrameActive(false);
		}
		else if (!frameInfo->GetIsDebugStop()) {
			// 更新処理
			this->Update();
		}
#else
		// 更新処理
		this->Update();
#endif
		// 終了リクエストが来たら終わり
		if (this->isEnd_) {
			break;
		}

		// 描画処理
		this->Draw();

		// フレーム終了処理
		Engine::FrameEnd();
	}

	this->Finalize();
}