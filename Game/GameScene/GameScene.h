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
	/// 3DモデルのDraw仮
	/// </summary>
	void Draw();

	/// <summary>
	/// 2DテクスチャのDraw
	/// </summary>
	void Draw2D();

private:
	GameScene() = default;
	~GameScene() = default;
	GameScene(const GameScene& gameScene) = delete;
	GameScene& operator=(const GameScene& gameScene) = delete;

private:

	// タイトルシーンインスタンス
	std::unique_ptr<Title> title_;
	// プレイシーンインスタンス
	std::unique_ptr<Play> play_;
	// リザルトシーンインスタンス
	std::unique_ptr<Result> result_;



};