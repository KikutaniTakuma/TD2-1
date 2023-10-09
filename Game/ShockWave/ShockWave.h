#pragma once

#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"

#include "GlobalVariables/GlobalVariables.h"

class ShockWave
{
public:

	// 波のサイズのenum
	enum class Size {
		kSmall, // 小
		kMiddle, // 中
		kMajor, // 大
		kEnd, // 末尾。要素数を取り出すよう
	};

	/// <param name="pos">プレイヤーの座標</param>
	/// <param name="highest">プレイヤーの最大の高さ</param>
	ShockWave(const Vector3& pos, float highest);
	~ShockWave() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	//void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 最期
	/// </summary>
	void Finalize();

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
	/// <returns>テクスチャのlist配列</returns>
	const std::list<std::shared_ptr<Texture2D>>& GetTextures() { return textures_; }

	/// <summary>
	/// 消すかどうかのフラグを取得
	/// </summary>
	/// <returns>trueなら消す</returns>
	bool GetDeleteFlag() const { return isDelete_; }

	/// <summary>
	/// 静的メンバ定数のImGui用
	/// </summary>
	static void GlobalVariablesUpdate() { globalVariables_->Update(); }

private:

	/// <summary>
	/// jsonファイルへの書き込み
	/// </summary>
	void SetGlobalVariable();

	/// <summary>
	/// jsonファイルからの呼び出し
	/// </summary>
	void ApplyGlobalVariable();

public:

	// 高さの基準
	static float kHighCriteria_[static_cast<uint16_t>(Size::kEnd)];

private:

	// 衝撃波の数。左右に1つずつ。
	static const int kWaveNum_ = 2;

	// グローバル変数
	static std::unique_ptr<GlobalVariables> globalVariables_;

	// 静的メンバ定数のグローバル変数のグループネーム
	static const char* groupName_;

	// サイズ(スケール)
	static float kSize_[static_cast<uint16_t>(Size::kEnd)];

	// 移動スピード
	static float kSpeed_;

	// 消えるまでのフレーム数
	static int kDeleteFrame_;

private:

	// テクスチャ
	std::list<std::shared_ptr<Texture2D>> textures_;

	// 消えるまでのカウント
	int deleteCount_;

	// 消すかどうかのフラグ
	bool isDelete_;

};
