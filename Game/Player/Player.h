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

public:

	/// <summary>
	/// テクスチャの参照。あたり判定用。
	/// </summary>
	const Texture2D& GetTex() { return tex_; }

	/// <summary>
	/// 速度の参照。あたり判定用。
	/// </summary>
	const Vector3& GetVelocity() { return velocity_; }

	/// <summary>
	/// エネミーを踏んだかどうか
	/// </summary>
	/// <param name="step">踏んでたらtrue,踏まれてたらfalse</param>
	void EnemyStep(bool step);

private:

	/// <summary>
	/// jsonファイルへの書き込み
	/// </summary>
	void SetGlobalVariable();

	/// <summary>
	/// jsonファイルからの呼び出し
	/// </summary>
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
	Vector3 velocity_ = {};

	// ジャンプ時の初速
	float kJampInitialVelocity_;

	// 移動スピード
	float kMoveSpeed_;

	// 重力加速度
	float kGravity_ ;

	// 空中にいるか
	bool isFly_;

	// 敵を踏んだか
	bool isStep_;

	// 敵に踏まれたか
	bool isSteped_;
};
