#pragma once

#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"

class Background {

public:

	enum class TextureNames {
		kMostBack, // 一番後ろの矩形
		kGaugeBack, // ゲージが減ったときに見える部分
		kGaugeMain, // ゲージの部分
		kEnd, // 末尾
	};

	Background();
	~Background() = default;

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

	

private:

	// テクスチャ
	std::vector<std::unique_ptr<Texture2D>> textures_;

	// 背景の描画する数。3段階分だから3つ。
	const int kTexNum_ = 3;

};
