#pragma once

#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"

#include "GlobalVariables/GlobalVariables.h"

class Layer;

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
	/// <param name="layerY">層の上のY座標</param>
	/// <param name="scale">スケール</param>
	Enemy(const Vector3& pos, const float& layerY, float scale = 40.0f);
	~Enemy() = default;
	

	/// <summary>
	/// 初期化
	/// </summary>
	//void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Layer* layer, const float& y);

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
	void StatusRequest(Status status) { statusRequest_ = status; }

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
	/// グローバル変数のロード
	/// </summary>
	static void GlobalVariablesLoad() { globalVariables_->LoadFile(groupName_); }

	/// <summary>
	/// 静的メンバ定数のImGui用
	/// </summary>
	static void GlobalVariablesUpdate(){ globalVariables_->Update(); }

	/// <summary>
	/// 初期座標などのパラメーターをいれる
	/// </summary>
	void SetParametar(const Vector3& pos, const float& y = 0);

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
	void FallingUpdate(const float& y);

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
	void DeathInitialize(Layer* layer);
	/// <summary>
	/// 死んだときのアップデート
	/// </summary>
	void DeathUpdate();

	void Collision(const float& y);

private:

	// 落下スピード
	static float kFallingSpeed_;

	// グローバル変数
	static std::unique_ptr<GlobalVariables> globalVariables_;

	// 静的メンバ定数のグローバル変数のグループネーム
	static const std::string groupName_;

private:

	// Enemyのテクスチャ
	std::unique_ptr<Texture2D> tex_;

	// 初期座標の保存用。倒した敵を生成するのに使うイメージ。
	Vector3 firstPos_;

	// 今のエネミーの状態
	Status status_ = Status::kNormal;

	// エネミーの状態のリクエスト
	std::optional<Status> statusRequest_ = std::nullopt;

	// 速度
	Vector3 velocity_;


};
