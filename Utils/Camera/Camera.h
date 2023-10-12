#pragma once
#include "Utils/Math/Vector3.h"
#include "Utils/Math/Mat4x4.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"

class Camera {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	Camera() noexcept;
	Camera(const Camera& right) noexcept;
	Camera(Camera&& right) noexcept;
	~Camera() noexcept = default;


/// <summary>
///  代入演算子
/// </summary>
public:
	Camera& operator=(const Camera& right) noexcept = default;
	Camera& operator=(Camera&& right) noexcept = default;


/// <summary>
/// メンバ関数
/// </summary>
public:
	/// <summary>
	/// 行列更新
	/// </summary>
	/// <param name="gazePoint">デバッグ時の起点</param>
	void Update(const Vector3& gazePoint = {});
	/// <summary>
	/// 行列更新
	/// </summary>
	/// <param name="worldMat">カメラの親のワールド行列</param>
	void Update(const Mat4x4& worldMat);

	/// <summary>
	/// 透視投影行列を取得(TypeがOthographicの場合ゼロ行列が返る)
	/// </summary>
	/// <returns>透視投影行列</returns>
	inline const Mat4x4& GetViewProjection() noexcept {
		return viewProjecction;
	}
	
	/// <summary>
	/// 平衡投影行列を取得(TypeがProjecctionの場合ゼロ行列が返る)
	/// </summary>
	/// <returns></returns>
	inline const Mat4x4& GetViewOthographics() noexcept {
		return viewOthograohics;
	}

	/// <summary>
	/// カメラのポジションを取得
	/// </summary>
	/// <returns>カメラのポジション</returns>
	const Vector3& GetPos() const {
		return worldPos;
	}

	/// <summary>
	/// 透視投影行列にビューポートをかけたものを取得(TypeがProjecctionの場合ゼロ行列が返る)
	/// </summary>
	/// <returns></returns>
	inline const Mat4x4& GetViewProjectionVp() noexcept {
		return viewProjecctionVp;
	}
	/// <summary>
	/// 平衡投影行列ビューポートをかけたものを取得(TypeがProjecctionの場合ゼロ行列が返る)
	/// </summary>
	/// <returns></returns>
	inline const Mat4x4& GetViewOthographicsVp() noexcept {
		return viewOthograohicsVp;
	}


/// <summary>
/// メンバ変数
/// </summary>
public:
	bool isDebug;

public:
	Vector3 pos;
	Vector3 scale;
	Vector3 rotate;

	float drawScale;

	Vector3 moveVec;
	float moveSpd;
	float moveRotateSpd;
	Vector2 gazePointRotate;
	float gazePointRotateSpd;

protected:
	Vector3 worldPos;

protected:
	static constexpr float kNearClip = 0.01f;

public:
	float farClip;
	float fov;

protected:
	Mat4x4 view;
	Mat4x4 projection;
	Mat4x4 othograohics;

	Mat4x4 viewProjecction;
	Mat4x4 viewOthograohics;

	Mat4x4 viewProjecctionVp;
	Mat4x4 viewOthograohicsVp;
};