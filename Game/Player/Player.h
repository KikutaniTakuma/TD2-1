#pragma once

#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Input/Input.h"

#include "GlobalVariables/GlobalVariables.h"

class Player
{
public:
	Player();
	~Player() = default;

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
	/// <param name="viewProjection">カメラのマトリックス</param>
	void Draw(const Mat4x4& viewProjection);

	/// <summary>
	/// 2DテクスチャのDraw
	/// </summary>
	/// <param name="viewProjection">カメラのマトリックス</param>
	void Draw2D(const Mat4x4& viewProjection);

private:

	void SetGlobalVariable();

	void ApplyGlobalVariable();
	
private:

	// プレイヤーのテクスチャ
	Texture2D tex_;

	// 入力状況
	Input* input_ = nullptr;

	// グローバル変数
	std::unique_ptr<GlobalVariables> globalVariables_;

	// グローバル変数のグループネーム
	const char* groupName_ = "Player";

	// 速度
	//Vector3 velocity_ = {};

	// ジャンプ時の初速
	float kFlyInitialVelocity_ = 30.0f;

	// 移動スピード
	float kMoveSpeed_ = 5.0f;

	// 重力加速度
	float kGravity_ = -9.8f;

	// 空中にいるか
	bool isFly = false;

};
