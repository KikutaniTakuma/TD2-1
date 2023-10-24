#pragma once
#include "SceneManager/SceneManager.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/Easeing/Easeing.h"
#include "Drawers/Particle/Particle.h"

#include "SceneManager/SubMenu/Pause.h"

class TitleScene : public BaseScene {
public:
	TitleScene();
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	~TitleScene() = default;

	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;


/// <summary>
/// メンバ変数
/// </summary>
private:
	// 背景
	std::array<Texture2D, 3> backGround_;
	// 背景に軽く縦平均化ブラー
	PeraRender backGroundBlur_;

	Model player_;
	std::pair<Vector3, Vector3> playerScale_;
	std::pair<Vector3, Vector3> playerPos_;
	Easeing playerScaleEaseing_;
	Easeing playerPosEaseing_;

	// プレイヤーアニメーション
	std::vector<Texture*> playerAnimationTex_;
	std::chrono::milliseconds playerAnimationDuration_;
	std::chrono::steady_clock::time_point playerAnimationStartTime_;
	int32_t currentPlayerAnimation_;
	bool isPlayerAnimationTurnBack_;
	std::chrono::milliseconds playerAnimationCoolTime_;
	std::pair<int32_t, int32_t> playerAnimationCoolTimeDuration_;
	std::chrono::steady_clock::time_point playerAnimationCoolStartTime_;
	bool isPlayerAnimationCoolTime_;

	// パーティクル
	Particle smoke_;

	// タイトル
	Texture2D titleTex_;
	std::pair<float, float> titleScaleDuration_;
	Easeing titleEase_;

	// hud
	Texture2D aButtonHud_;
	Texture2D spaceHud_;
	Texture2D startHud_;
	Easeing hudAlphaEase_;

	// 床
	Texture2D floor_;

	// 背景パーティクル
	Particle backGroundParticle_;

	Audio* bgm_;
	Audio* decideSE_;
};