#pragma once

#include "Utils/Camera/Camera.h"
#include "Drawers/Texture2D/Texture2D.h"

#include "Game/GameScene/Title/Title.h"
#include "Game/GameScene/Play/Play.h"
#include "Game/GameScene/Result/Result.h"

class GameScene
{
public:
	enum class Scene {
		kTitle, // タイトル
		kPlay, // プレイ
		kResult // リザルト
	};

	// シングルトンインスタンス
	/// <summary>
	/// インスタンスの取得
	/// </summary>
	static GameScene* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

public:

	void SceneRequest(Scene scene) { sceneRequest_ = scene; }

private:
	GameScene() = default;
	~GameScene() = default;
	GameScene(const GameScene& gameScene) = delete;
	GameScene& operator=(const GameScene& gameScene) = delete;

private:

	// タイトルシーン
	std::unique_ptr<Title> title_;
	// プレイシーン
	std::unique_ptr<Play> play_;
	// リザルトシーン
	std::unique_ptr<Result> result_;

	// 現在のシーン。今はプレイ確認用のためプレイにしている。
	Scene scene_ = Scene::kPlay;
	// シーンのリクエスト
	std::optional<Scene> sceneRequest_ = std::nullopt;

};