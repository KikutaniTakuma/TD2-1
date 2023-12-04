#pragma once
#include "Scenes/Manager/BaseScene/BaseScene.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Utils/Easing/Easing.h"

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

public:
	void SetStartStage(int32_t startStageNumber);

	
private:
	// 背景
	std::array<std::unique_ptr<Texture2D>, 3> backGround_;
	// 背景に軽く縦平均化ブラー
	std::unique_ptr<PeraRender> backGroundBlur_;

	std::unique_ptr<Model> moon_;
	std::pair<float, float> moonRotateY_;
	std::unique_ptr<Easing> rotateEase_;

	std::unique_ptr<Model> player_;
	std::array<std::pair<Vector2, Vector2>, 3> playerScaleEaseDuration_;
	std::array<std::unique_ptr<Easing>, 3> playerEase_;
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

	std::unique_ptr<Texture2D> bubble_;
	std::pair<float, float> bubbleY_;
	std::unique_ptr<Easing> bubbleEase_;

	int32_t currentStage_;

	int32_t maxStage_;

	std::unique_ptr<Texture2D> stageNumberTex_;
	std::unique_ptr<Texture2D> stageTenNumberTex_;
	std::pair<float, float> stageNumberTexY_;

	std::unique_ptr<Texture2D> stageTex_;
	std::pair<float, float> stageTexY_;


	std::unique_ptr<Texture2D> arrowRight_;
	std::pair<Vector3, Vector3> arrowRightPos_;
	std::unique_ptr<Texture2D> arrowLeft_;
	std::pair<Vector3, Vector3> arrowLeftPos_;
	std::unique_ptr<Easing> arrowEase_;

	std::unique_ptr<Texture2D> isClearTex_;
	std::pair<float, float> isClearTexY_;

	// 背景パーティクル
	std::unique_ptr<Particle> backGroundParticle_;

	class Audio* bgm_;
	class Audio* choiceSE_;
	class Audio* decideSE_;
	class Audio* backSE_;

	// Hud
	std::unique_ptr<Texture2D> aButtonHud_;
	std::unique_ptr<Texture2D> spaceHud_;
	std::unique_ptr<Easing> hudAlphaEase_;
	std::unique_ptr<Texture2D> keyEscHud_;
	std::unique_ptr<Texture2D> padStartHud_;
	std::unique_ptr<Texture2D> backToHud_;

	std::unique_ptr<Camera> staticCamera_;
};