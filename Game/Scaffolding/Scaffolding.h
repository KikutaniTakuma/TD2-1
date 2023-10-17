#pragma once

#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"

class Scaffolding {
public:

	Scaffolding(const Vector3& pos, const Vector2& scale);
	~Scaffolding() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	//void Initialize(const int& num, const int& Max);

	/// <summary>
	/// 更新
	/// </summary>
	//void Update(const int& num, const int& Max, const int& damage);

	/// <summary>
	/// 3DモデルのDraw仮
	/// </summary>
	/// <param name="viewProjection">カメラのマトリックス</param>
	//void Draw(const Mat4x4& viewProjection);

	/// <summary>
	/// 2DテクスチャのDraw
	/// </summary>
	/// <param name="viewProjection">カメラのマトリックス</param>
	void Draw2D(const Mat4x4& viewProjection);

public:

	const Texture2D* GetTex() { return tex_.get(); }

	const float GetHighestPosY() { return (tex_->pos.y + tex_->scale.y / 2.0f); }

	const Vector3 GetVelocity() { return velocity; }

	/// <summary>
	/// 初期座標などのパラメーターをいれる
	/// </summary>
	void SetParametar(const Vector3& pos, const Vector2& scale);

private:

	// テクスチャ
	std::unique_ptr<Texture2D> tex_;

	Vector3 velocity;
	
};
