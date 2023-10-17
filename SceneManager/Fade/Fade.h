#pragma once
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include "Utils/Easeing/Easeing.h"

class Fade {
public:
	Fade();
	Fade(const Fade&) = default;
	Fade(Fade&&) = default;
	~Fade() = default;

	Fade& operator=(const Fade&) = default;
	Fade& operator=(Fade&&) = default;

public:
	/// <summary>
	/// フェードアウトをスタートさせる
	/// </summary>
	void OutStart();

	/// <summary>
	/// フェードインをスタートさせる
	/// </summary>
	void InStart();

	/// <summary>
	/// フェードアウトが終わった瞬間を取得
	/// </summary>
	/// <returns>終わった瞬間true</returns>
	bool OutEnd();

	/// <summary>
	/// フェードインが終わった瞬間を取得
	/// </summary>
	/// <returns>終わった瞬間</returns>
	bool InEnd();

	/// <summary>
	/// 更新関数
	/// </summary>
	void Update();

	/// <summary>
	/// 描画関数
	/// </summary>
	/// <param name="viewProjection">カメラ行列</param>
	void Draw(const Mat4x4& viewProjection);

private:
	/// <summary>
	/// フェードインしてるか
	/// </summary>
	UtilsLib::Flg isInStart_;

	/// <summary>
	/// フェードアウトしてるか
	/// </summary>
	UtilsLib::Flg isOutStart_;

	Texture2D tex_;

	Easeing ease_;
	/// <summary>
	/// seconds
	/// </summary>
	float fadeTime_;
};