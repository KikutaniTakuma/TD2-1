#pragma once

#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Input/Input.h"

#include "GlobalVariables/GlobalVariables.h"

class Play;

class Player
{
public:

	// 状態
	enum class Status {
		kNormal, // 通常時
		kHipDrop, // ヒップドロップ
		kLanding, // 着地
		kFalling, // 落ちている
	};

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
	/// 状態のリクエスト
	/// </summary>
	/// <param name="status">したい状態</param>
	void StatusRequest(Status status) { status_ = status; }

	/// <summary>
	/// 今の状態の確認。あたり判定のフラグに使用。
	/// </summary>
	/// <returns>状態</returns>
	Status GetStatus() { return status_; }

	/// <summary>
	/// テクスチャの参照。あたり判定用。
	/// </summary>
	/// <returns>テクスチャのポインタ</returns>
	const Texture2D* GetTex() { return tex_.get(); }

	/// <summary>
	/// 速度の参照。あたり判定用。
	/// </summary>
	/// <returns>速度</returns>
	const Vector3& GetVelocity() { return velocity_; }

	/// <summary>
	/// エネミーを踏んだかどうか
	/// </summary>
	/// <param name="step">踏んでたらtrue,踏まれてたらfalse</param>
	void EnemyStep(bool step);

	/// <summary>
	/// プレイシーンのポインタのsetter
	/// </summary>
	/// <param name="play">プレイシーンのポインタ</param>
	void SetPlayScene(Play* play) { play_ = play; }

private:

	/// <summary>
	/// jsonファイルへの書き込み
	/// </summary>
	void SetGlobalVariable();

	/// <summary>
	/// jsonファイルからの呼び出し
	/// </summary>
	void ApplyGlobalVariable();

	/// <summary>
	/// 通常の初期化
	/// </summary>
	void NormalInitialize();
	/// <summary>
	/// 通常のアップデート
	/// </summary>
	void NormalUpdate();

	/// <summary>
	/// ヒップドロップ時の初期化
	/// </summary>
	void HipDropInitialize();
	/// <summary>
	/// ヒップドロップ時のアップデート
	/// </summary>
	void HipDropUpdate();

	/// <summary>
	/// 着地時の初期化
	/// </summary>
	void LandingInitialize();
	/// <summary>
	/// 着地時のアップデート
	/// </summary>
	void LandingUpdate();

	/// <summary>
	/// 落ちる時の初期化
	/// </summary>
	void FallingInitialize();
	/// <summary>
	/// 落ちる時のアップデート
	/// </summary>
	void FallingUpdate();

	/// <summary>
	/// 最も高い位置の数値の保存
	/// </summary>
	void MemoHighest();
	
private:

	Play* play_ = nullptr;

	// プレイヤーのテクスチャ
	std::unique_ptr<Texture2D> tex_;

	// 入力状況
	Input* input_ = nullptr;

	// グローバル変数
	std::unique_ptr<GlobalVariables> globalVariables_;

	// グローバル変数のグループネーム
	const std::string groupName_ = "Player";

	// 今の状態
	Status status_ = Status::kNormal;

	// 状態のリクエスト
	std::optional<Status> statusRequest_ = std::nullopt;

	// 速度
	Vector3 velocity_;

	// 一番高いときを保存
	float highest_;

	// ジャンプ時の初速
	float kJampInitialVelocity_;

	// 移動スピード
	float kMoveSpeed_;

	// 重力加速度
	float kGravity_ ;

	// ヒップドロップ中の加速度
	float kHipDropSpeed_;

	// 空中にいるか
	bool isFly_;

	// 敵を踏んだか
	bool isStep_;

	// 敵に踏まれたか
	bool isSteped_;
};
