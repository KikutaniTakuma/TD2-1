#pragma once

#include <array>
#include "Vector4.h"

class Vector3;

class Mat4x4 final {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	Mat4x4();
	Mat4x4(const Mat4x4& mat);
	Mat4x4(Mat4x4&& mat) noexcept;
	Mat4x4(const std::array<Vector4, 4>& num);
public:
	~Mat4x4() = default;

/// <summary>
/// 演算子のオーバーロード
/// </summary>
public:
	Mat4x4& operator=(const Mat4x4& mat);
	Mat4x4& operator=(Mat4x4&& mat) noexcept;
	Mat4x4 operator*(const Mat4x4& mat) const;
	Mat4x4& operator*=(const Mat4x4& mat);

	Mat4x4 operator+(const Mat4x4& mat) const;
	Mat4x4& operator+=(const Mat4x4& mat);
	Mat4x4 operator-(const Mat4x4& mat) const;
	Mat4x4& operator-=(const Mat4x4& mat);

	Vector4& operator[](size_t index);
	const Vector4& operator[](size_t index) const;

	bool operator==(const Mat4x4& mat) const;
	bool operator!=(const Mat4x4& mat) const;

/// <summary>
/// メンバ関数
/// </summary>
public:
	void Indentity();

	/// <summary>
	/// 横ベクトル用平衡移動行列
	/// </summary>
	/// <param name="vec">移動</param>
	void HoriTranslate(const Vector3& vec);
	/// <summary>
	/// 縦ベクトル用平衡移動行列
	/// </summary>
	/// <param name="vec">移動</param>
	void VertTranslate(const Vector3& vec);

	/// <summary>
	/// スカラー倍行列
	/// </summary>
	/// <param name="vec">スカラー</param>
	void Scalar(const Vector3& vec);

	/// <summary>
	/// 横ベクトル用X軸回転行列
	/// </summary>
	/// <param name="rad">ラジアン</param>
	void HoriRotateX(float rad);
	/// <summary>
	/// 縦ベクトル用X軸回転行列
	/// </summary>
	/// <param name="rad">ラジアン</param>
	void VertRotateX(float rad);

	/// <summary>
	/// 横ベクトル用Y軸回転行列
	/// </summary>
	/// <param name="rad">ラジアン</param>
	void HoriRotateY(float rad);
	void VertRotateY(float rad);

	/// <summary>
	/// 横ベクトル用Z軸回転行列
	/// </summary>
	/// <param name="rad">ラジアン</param>
	void HoriRotateZ(float rad);
	/// <summary>
	/// 縦ベクトル用Z軸回転行列
	/// </summary>
	/// <param name="rad">ラジアン</param>
	void VertRotateZ(float rad);

	/// <summary>
	/// 横ベクトル用アフィン行列
	/// </summary>
	/// <param name="scale">スカラー</param>
	/// <param name="rad">ラジアン</param>
	/// <param name="translate">移動</param>
	void HoriAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate);
	/// <summary>
	/// 縦ベクトル用アフィン行列
	/// </summary>
	/// <param name="scale">スカラー</param>
	/// <param name="rad">ラジアン</param>
	/// <param name="translate">移動</param>
	void VertAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate);

	/// <summary>
	/// 逆行列
	/// </summary>
	void Inverse();

	/// <summary>
	///  転置行列
	/// </summary>
	void Transepose();

	/// <summary>
	/// 横ベクトル用透視投影行列
	/// </summary>
	/// <param name="fovY">視野角</param>
	/// <param name="aspectRatio">アスペクト比</param>
	/// <param name="nearClip">ニアクリップ</param>
	/// <param name="farClip">ファークリップ</param>
	void HoriPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);
	/// <summary>
	/// 縦ベクトル用透視投影行列
	/// </summary>
	/// <param name="fovY">視野角</param>
	/// <param name="aspectRatio">アスペクト比</param>
	/// <param name="nearClip">ニアクリップ</param>
	/// <param name="farClip">ファークリップ</param>
	void VertPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);

	/// <summary>
	/// 横ベクトル用平衡投影行列
	/// </summary>
	/// <param name="left">左の範囲</param>
	/// <param name="right">右の範囲</param>
	/// <param name="top">上の範囲</param>
	/// <param name="bottom">下の範囲</param>
	/// <param name="nearClip">ニアクリップ</param>
	/// <param name="farClip">ファークリップ</param>
	void HoriOrthographic(float left, float right, float top, float bottom, float nearClip, float farClip);
	/// <summary>
	/// 縦ベクトル用平衡投影行列
	/// </summary>
	/// <param name="left">左の範囲</param>
	/// <param name="right">右の範囲</param>
	/// <param name="top">上の範囲</param>
	/// <param name="bottom">下の範囲</param>
	/// <param name="nearClip">ニアクリップ</param>
	/// <param name="farClip">ファークリップ</param>
	void VertOrthographic(float left, float right, float top, float bottom, float nearClip, float farClip);

	/// <summary>
	/// 横ベクトル用ビューポート行列
	/// </summary>
	/// <param name="left">映し出したViewPortの位置横</param>
	/// <param name="top">映し出したViewPortの位置縦</param>
	/// <param name="width">映し出す範囲横</param>
	/// <param name="height">映し出す範囲縦</param>
	/// <param name="minDepth">深度バッファーの最低値</param>
	/// <param name="maxDepth">深度バッファーの最大値</param>
	void HoriViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);
	void VertViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);

/// <summary>
/// 静的定数
/// </summary>
private:
	static constexpr int32_t HEIGHT = 4;
	static constexpr int32_t WIDTH = 4;

/// <summary>
/// メンバ変数
/// </summary>
private:
	std::array<Vector4, 4> m;
};


Mat4x4 MakeMatrixIndentity();

Mat4x4 HoriMakeMatrixTranslate(Vector3 vec);
Mat4x4 VertMakeMatrixTranslate(Vector3 vec);

Mat4x4 MakeMatrixScalar(Vector3 vec);

Mat4x4 HoriMakeMatrixRotateX(float rad);
Mat4x4 VertMakeMatrixRotateX(float rad);

Mat4x4 HoriMakeMatrixRotateY(float rad);
Mat4x4 VertMakeMatrixRotateY(float rad);

Mat4x4 HoriMakeMatrixRotateZ(float rad);
Mat4x4 VertMakeMatrixRotateZ(float rad);

Mat4x4 HoriMakeMatrixAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate);
Mat4x4 VertMakeMatrixAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate);

Mat4x4 MakeMatrixInverse(Mat4x4 mat);

Mat4x4 MakeMatrixTransepose(Mat4x4 mat);

Mat4x4 HoriMakeMatrixPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);
Mat4x4 VertMakeMatrixPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip);

Mat4x4 HoriMakeMatrixOrthographic(float left, float top, float right, float bottom, float nearClip, float farClip);
Mat4x4 VertMakeMatrixOrthographic(float left, float top, float right, float bottom, float nearClip, float farClip);

Mat4x4 HoriMakeMatrixViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);
Mat4x4 VertMakeMatrixViewPort(float left, float top, float width, float height, float minDepth, float maxDepth);