#pragma once
#include "SceneManager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Utils/Easeing/Easeing.h"

class StageSelect : public BaseScene {
public:
	StageSelect();
	~StageSelect() = default;

	StageSelect(const StageSelect&) = delete;
	StageSelect(StageSelect&&) = delete;
	StageSelect& operator=(const StageSelect&) = delete;
	StageSelect& operator=(StageSelect&&) = delete;

/// <summary>
/// オーバーライド関数
/// </summary>
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;
	
private:
	// 背景
	std::array<Texture2D, 3> backGround_;
	// 背景に軽く縦平均化ブラー
	PeraRender backGroundBlur_;

	Model moon_;
	std::pair<float, float> moonRotateY_;
	Easeing rotateEase_;

	Model player_;
	std::array<std::pair<Vector2, Vector2>, 3> playerScaleEaseDuration_;
	std::array<Easeing, 3> playerEase_;
	int32_t currentPlayerEaseing_;
	std::pair<float, float> playerNormalMove_;
	

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

	bool isStick_;

	Texture2D bubble_;
	std::pair<float, float> bubbleY_;
	Easeing bubbleEase_;

	int32_t currentStage_;

	int32_t maxStage_;

	Texture2D stageNumberTex_;
	Texture2D stageTenNumberTex_;
	std::pair<float, float> stageNumberTexY_;

	Texture2D stageTex_;
	std::pair<float, float> stageTexY_;


	Texture2D arrowRight_;
	std::pair<Vector3, Vector3> arrowRightPos_;
	Texture2D arrowLeft_;
	std::pair<Vector3, Vector3> arrowLeftPos_;
	Easeing arrowEase_;

	Texture2D isClearTex_;
	std::pair<float, float> isClearTexY_;

	// 背景パーティクル
	Particle backGroundParticle_;

	Audio* bgm_;
	Audio* choiceSE_;
	Audio* decideSE_;
	Audio* backSE_;

	// Hud
	Texture2D aButtonHud_;
	Texture2D spaceHud_;
	Easeing hudAlphaEase_;
	Texture2D keyEscHud_;
	Texture2D padStartHud_;
	Texture2D backToHud_;
};