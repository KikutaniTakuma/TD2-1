#pragma once

#include <list>

#include "Utils/Camera/Camera.h"
#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"

#include "Game/Player/Player.h"
#include "Game/Enemy/Enemy.h"

class Play
{
public:
	Play();
	~Play() = default;

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

private:

	/// <summary>
	/// エネミーの生成
	/// </summary>
	void EnemyGeneration();

	/// <summary>
	/// エネミーのリストのクリア
	/// </summary>
	void EnemeiesClear();

	/// <summary>
	/// プレイヤーとエネミーの当たり判定
	/// </summary>
	void Collision();

private:

	std::unique_ptr<Camera> camera2D_;
	std::unique_ptr<Camera> camera3D_;

	std::unique_ptr<Player> player_;

	std::list<std::shared_ptr<Enemy>> enemies_;

};
