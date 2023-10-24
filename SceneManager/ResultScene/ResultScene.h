#pragma once
#include "SceneManager/SceneManager.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/Particle/Particle.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Result/Star/Star.h"

class ResultScene : public BaseScene {
/// <summary>
/// コンストラクタ
/// </summary>
public:
	/// <summary>
	/// SceneIDを代入するため定義しないとダメ
	/// </summary>
	ResultScene();
	~ResultScene() = default;

	// コピーコンストラクタは削除
	ResultScene(const ResultScene&) = delete;
	ResultScene(ResultScene&&) = delete;
	ResultScene& operator=(const ResultScene&) = delete;
	ResultScene& operator=(ResultScene&&) = delete;

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


/// <summary>
/// メンバ関数
/// </summary>
public:




/// <summary>
/// セッター
/// </summary>
public:
	void SetClearTime(std::chrono::milliseconds clearTime);
	void SetStageNumber(int32_t stageNumber);



/// <summary>
/// ゲッター
/// </summary>
public:





/// <summary>
/// メンバ変数
/// </summary>
private:
	std::vector<Model> models_;
	std::vector<Texture2D> texs_;
	std::vector<Particle> particles_;
	GlobalVariables globalVariables_;

	///
	/// ここに必要なメンバ変数を追加
	/// 
	
	// 星
	std::array<Star,3> stars_;

	// 背景の型
	std::array<Star,3> starsGray_;

	// グレースケール化
	PeraRender grayPera_;

	// 星のアニメーション間隔
	std::chrono::milliseconds starEffectDuration_;
	std::chrono::steady_clock::time_point startTime_;
	size_t currentStar_;

	// 吹き出し
	Texture2D speechBubble_;

	// 背景
	std::array<Texture2D, 3> backGround_;
	// 背景に軽く縦平均化ブラー
	PeraRender backGroundBlur_;

	// クリア時間
	std::chrono::milliseconds clearTime_;

	std::array<std::chrono::milliseconds, 3> clearTimeBasis_;

	int32_t score_;

	// アップデートが始まる時間
	std::chrono::milliseconds updateStartTime_;
	bool isUpdate_;

	// プレイヤーのモデル
	Model player_;
	std::pair<Vector3, Vector3> playerScale_;
	Easeing playerScaleEase_;

	// 星獲得時のアニメーション
	std::pair<Vector3, Vector3> playerScaleGetStar_;
	std::pair<Vector3, Vector3> playerScaleGetStar2_;
	Easeing playerScaleGetStarEase_;
	Easeing playerScaleGetStarEase2_;

	// 星3つの時のモデルアニメーション
	std::pair<float, float> playerRotateSpecial_;
	Easeing playerSpecialEase_;

	Texture* speciaclPlayerTex_;

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


	// タイマー
	Texture2D timer_;
	Texture2D timerUI_;

	// ステージセレクトUI
	Texture2D nextStageMassage_;
	Texture2D stageSelectMassage_;
	Texture2D arrow_;
	// 今選択しているところ(0:nextStage, 1:stageSelect)
	int32_t nowChoose_;
	std::pair<float, float> arrowPosY_;
	// 矢印のイージング
	std::pair<float, float> arrowPosX_;
	Easeing arrowEase_;

	// 次のステージかステージセレクトを選択できるか
	UtilsLib::Flg isCanSelect_;

	// 時間表示
	Texture2D tenMinutes_;
	Texture2D minutes_;
	Texture2D tenSeconds_;
	Texture2D seconds_;
	Texture2D colon1_;
	Texture2D colon2_;

	// resultUI
	Texture2D resultUI_;
	Texture2D stageNumberTex_;
	Texture2D stageTenNumberTex_;
	int32_t stageNumber_;

	// 背景パーティクル
	Particle backGroundParticle_;


	// BGM
	Audio* bgm_;
	Audio* choiceSE_;
	Audio* decideSE_;
	Audio* starSE_;
	Audio* specialStarSE_;

	bool isStick_;

	///
	/// =============================================
	/// 
};