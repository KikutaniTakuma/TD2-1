#pragma once

#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"

#include "GlobalVariables/GlobalVariables.h"

class Enemy
{
public:

	// 状態
	enum class Status {
		kNormal, // 通常時
		kFalling, // 落ちている
		kFaint, // 気絶
		kDeath, // 死亡
	};


	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="pos">初期座標</param>
	/// /// <param name="scale">スケール</param>
	Enemy(const Vector3& pos, float scale);
	~Enemy() = default;
	

	/// <summary>
	/// 初期化
	/// </summary>
	//void Initialize();

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
	/// テクスチャの参照。あたり判定用。
	/// </summary>
	const Texture2D& GetTex() { return tex_; }

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
	/// 落ちる時の初期化
	/// </summary>
	void FallingInitialize();
	/// <summary>
	/// 落ちる時のアップデート
	/// </summary>
	void FallingUpdate();

	/// <summary>
	/// 落ち切った時の初期化
	/// </summary>
	void FaintInitialize();
	/// <summary>
	/// 落ち切った時のアップデート
	/// </summary>
	void FaintUpdate();

	/// <summary>
	/// 死んだときの初期化
	/// </summary>
	void DeathInitialize();
	/// <summary>
	/// 死んだときのアップデート
	/// </summary>
	void DeathUpdate();

private:

	// 落下スピード
	static float kFallingSpeed_;

	// グローバル変数
	static std::unique_ptr<GlobalVariables> globalVariables_;

	// 静的メンバ定数のグローバル変数のグループネーム
	static const char* groupName_;

private:

	// Enemyのテクスチャ
	Texture2D tex_;

	// 初期座標の保存用。倒した敵を生成するのに使うイメージ。
	Vector3 firstPos_;

	// 今のエネミーの状態
	Status status_ = Status::kNormal;

	// エネミーの状態のリクエスト
	std::optional<Status> statusRequest_ = std::nullopt;

	// 速度
	Vector3 velocity_;


};
