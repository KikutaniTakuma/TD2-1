#pragma once
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/Easing/Easing.h"

class Star final {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	Star();
	Star(const Star&) = default;
	Star(Star&&) = default;
	~Star() = default;

	Star& operator=(const Star&) = default;
	Star& operator=(Star&&) = default;

public:
	/// <summary>
	/// 演出スタート関数
	/// </summary>
	void Start();

	/// <summary>
	/// 通常時の演出スタート
	/// </summary>
	void NormalStart();

	/// <summary>
	/// デバッグ(調整関数)
	/// </summary>
	/// <param name="guiName">デバッグ時の名前</param>
	void Debug(const std::string& guiName);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画関数
	/// </summary>
	/// <param name="viewProjection">カメラ行列</param>
	void Draw(const Mat4x4& viewProjection);

	const Lamb::Flg& GetEndFlg() const {
		return isEnd_;
	}

	const Lamb::Flg& GetStartFlg() const {
		return isStart_;
	}

	void SetDefaultScale(float magnification);

public:
	// ポジション
	Vector3 pos_;
	// スケール
	Vector2 scale_;
	// 回転
	Vector3 rotate_;

	// 特別演出か
	Lamb::Flg isSpecial_;

private:
	/// <summary>
	/// 板ポリ
	/// </summary>
	Texture2D tex_;


	// イージングの範囲

	/// <summary>
	/// 演出
	/// </summary>
	std::pair<Vector2, Vector2> scaleDuration_;

	/// <summary>
	/// スター演出終わったあとのスケール
	/// </summary>
	std::pair<Vector2, Vector2> scaleNormalDuration_;

	/// <summary>
	/// スター演出が終って特別演出がある場合のrotate
	/// </summary>
	std::pair<Vector3, Vector3> rotateDuration_;

	// =============================================

	// イージングの時間

	float scaleEaseTime_;
	float specialScaleEaseTime_;
	float rotateEaseTime_;

	// =============================================


	/// <summary>
	/// スター演出
	/// </summary>
	Easing ease_;

	/// <summary>
	/// 通常のアニメーション(どくどくさせる)
	/// </summary>
	Easing scaleEase_;

	/// <summary>
	/// 3つ目の星の演出(回転する)
	/// </summary>
	Easing rotateEase_;

	/// <summary>
	/// スターの演出が行われてるか否か
	/// </summary>
	Lamb::Flg isStart_;

	/// <summary>
	/// スターの演出が終わっているか
	/// </summary>
	Lamb::Flg isEnd_;
};