#pragma once
#include "Scenes/Manager/SceneManager.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/Easing/Easing.h"
#include "Drawers/Particle/Particle.h"

#include "Scenes/SubMenu/Pause.h"

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
	std::array<std::unique_ptr<Texture2D>, 3> backGround_;
	// 背景に軽く縦平均化ブラー
	std::unique_ptr<PeraRender> backGroundBlur_;

	std::unique_ptr<Model> player_;
	std::pair<Vector3, Vector3> playerScale_;
	std::pair<Vector3, Vector3> playerPos_;
	std::unique_ptr<Easing> playerScaleEaseing_;
	std::unique_ptr<Easing> playerPosEaseing_;

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
	std::unique_ptr<Particle> smoke_;

	// タイトル
	std::unique_ptr<Texture2D> titleTex_;
	std::pair<float, float> titleScaleDuration_;
	std::unique_ptr<Easing> titleEase_;

	// hud
	std::unique_ptr<Texture2D> aButtonHud_;
	std::unique_ptr<Texture2D> spaceHud_;
	std::unique_ptr<Texture2D> startHud_;
	std::unique_ptr<Easing> hudAlphaEase_;

	// 床
	std::unique_ptr<Texture2D> floor_;

	// 背景パーティクル
	std::unique_ptr<Particle> backGroundParticle_;

	Audio* bgm_;
	Audio* decideSE_;

	std::unique_ptr<Texture2D> keyEscHud_;
	std::unique_ptr<Texture2D> padStartHud_;
	std::unique_ptr<Texture2D> backToHud_;

	std::unique_ptr<Camera> staticCamera_;
};