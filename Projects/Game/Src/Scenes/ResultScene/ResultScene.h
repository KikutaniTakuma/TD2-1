#pragma once
#include "Scenes/Manager/SceneManager.h"
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
	std::unique_ptr<GlobalVariables> globalVariables_;

	///
	/// ここに必要なメンバ変数を追加
	/// 
	
	// 星
	std::array<std::unique_ptr<Star>,3> stars_;

	// 背景の型
	std::array<std::unique_ptr<Star>,3> starsGray_;

	// グレースケール化
	std::unique_ptr<PeraRender> grayPera_;

	// 星のアニメーション間隔
	std::chrono::milliseconds starEffectDuration_;
	std::chrono::steady_clock::time_point startTime_;
	size_t currentStar_;

	// 吹き出し
	std::unique_ptr<Texture2D> speechBubble_;

	// 背景
	std::array<std::unique_ptr<Texture2D>, 3> backGround_;
	// 背景に軽く縦平均化ブラー
	std::unique_ptr<PeraRender> backGroundBlur_;

	// クリア時間
	std::chrono::milliseconds clearTime_;

	std::array<std::chrono::milliseconds, 3> clearTimeBasis_;

	int32_t score_;

	// アップデートが始まる時間
	std::chrono::milliseconds updateStartTime_;
	bool isUpdate_;

	// プレイヤーのモデル
	std::unique_ptr<Model> player_;
	std::pair<Vector3, Vector3> playerScale_;
	std::unique_ptr<Easing> playerScaleEase_;

	// 星獲得時のアニメーション
	std::pair<Vector3, Vector3> playerScaleGetStar_;
	std::pair<Vector3, Vector3> playerScaleGetStar2_;
	std::unique_ptr<Easing> playerScaleGetStarEase_;
	std::unique_ptr<Easing> playerScaleGetStarEase2_;

	// 星3つの時のモデルアニメーション
	std::pair<float, float> playerRotateSpecial_;
	std::unique_ptr<Easing> playerSpecialEase_;

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
	std::unique_ptr<Texture2D> timer_;
	std::unique_ptr<Texture2D> timerUI_;

	// ステージセレクトUI
	std::unique_ptr<Texture2D> nextStageMassage_;
	std::unique_ptr<Texture2D> stageSelectMassage_;
	std::unique_ptr<Texture2D> arrow_;
	// 今選択しているところ(0:nextStage, 1:stageSelect)
	int32_t nowChoose_;
	std::pair<float, float> arrowPosY_;
	// 矢印のイージング
	std::pair<float, float> arrowPosX_;
	std::unique_ptr<Easing> arrowEase_;

	// 次のステージかステージセレクトを選択できるか
	Lamb::Flg isCanSelect_;

	// 時間表示
	std::unique_ptr<Texture2D> tenMinutes_;
	std::unique_ptr<Texture2D> minutes_;
	std::unique_ptr<Texture2D> tenSeconds_;
	std::unique_ptr<Texture2D> seconds_;
	std::unique_ptr<Texture2D> colon1_;
	std::unique_ptr<Texture2D> colon2_;

	// resultUI
	std::unique_ptr<Texture2D> resultUI_;
	std::unique_ptr<Texture2D> stageNumberTex_;
	std::unique_ptr<Texture2D> stageTenNumberTex_;
	int32_t stageNumber_;

	// 背景パーティクル
	std::unique_ptr<Particle> backGroundParticle_;


	// BGM
	class Audio* bgm_;
	class Audio* choiceSE_;
	class Audio* decideSE_;
	class Audio* starSE_;
	class Audio* specialStarSE_;

	bool isStick_;

	///
	/// =============================================
	/// 
};