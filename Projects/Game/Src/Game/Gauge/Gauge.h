#pragma once

#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"

#include "GlobalVariables/GlobalVariables.h"

class Gauge {

public:

	enum class TextureNames {
		kMostBack, // 一番後ろの矩形
		kGaugeBack, // ゲージが減ったときに見える部分
		kGaugeMain, // ゲージの部分
		kEnd, // 末尾
	};

	Gauge();
	~Gauge() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const int& num, const int& Max);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const int& num, const int& Max, const int& damage, const float& y);

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

	/// <summary>
	/// jsonファイルへの書き込み
	/// </summary>
	void SetGlobalVariable();

	/// <summary>
	/// jsonファイルからの呼び出し
	/// </summary>
	void ApplyGlobalVariable(const float& y = 0.0f);

	void DamageUpdate(const int& damage);

private:

	// テクスチャ
	std::vector<std::unique_ptr<Texture2D>> textures_;

	std::list<std::unique_ptr<Texture2D>> damageGaugeTextures_;

	// グローバル変数
	std::unique_ptr<GlobalVariables> globalVariables_;

	// グローバル変数のグループネーム
	const std::string groupName_ = "Gauge";

	int num_;

	int kMax_;

};
