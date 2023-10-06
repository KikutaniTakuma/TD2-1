#pragma once

#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"

class Result
{
public:
	Result();
	~Result() = default;

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
	/// <param name="blend">ブレンドモード</param>
	/// <param name="isDepth">深度バッファの有効か否か(trueなら有効)</param>
	void Draw(const Mat4x4& viewProjection, Pipeline::Blend blend, bool isDepth);

	/// <summary>
	/// 2DテクスチャのDraw
	/// </summary>
	/// <param name="viewProjection">カメラのマトリックス</param>
	/// <param name="blend">ブレンドモード</param>
	/// <param name="isDepth">深度バッファの有効か否か(trueなら有効)</param>
	void Draw2D(const Mat4x4& viewProjection, Pipeline::Blend blend, bool isDepth);

};
