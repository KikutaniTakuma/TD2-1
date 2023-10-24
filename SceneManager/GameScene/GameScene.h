#pragma once
#include "SceneManager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "GlobalVariables/GlobalVariables.h"

#include "Game/Player/Player.h"
#include "Game/Enemy/Enemy.h"
#include "Game/ShockWave/ShockWave.h"
#include "Game/Layer/Layer.h"
#include "Game/Scaffolding/Scaffolding.h"
#include "Game/Background/Background.h"
#include "SceneManager/SubMenu/Pause.h"

class GameScene : public BaseScene {
public:
	GameScene();
	GameScene(const GameScene&) = delete;
	GameScene(GameScene&&) = delete;
	~GameScene() = default;

	GameScene& operator=(const GameScene&) = delete;
	GameScene& operator=(GameScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

public:
	/*std::vector<Model> models_;
	std::vector<Texture2D> texs_;
	std::vector<Particle> particles;
	GlobalVariables globalVariables_;*/

	///
	/// ここに必要なメンバ変数を追加
	/// 

public:

	// ImGuiで設定するエネミーのパラメータ
	enum class EnemyParameter {
		kType, // タイプ
		kPos, // ポジション
		kMoveVector, // 移動方向
		kIsHealer, // ヒーラーかどうか
		kMoveRadius, // 移動の半径
		kEnd, // 末尾。要素数を取り出すよう。
	};

	// ImGuiで設定するあしばのパラメータ
	enum class ScaffoldingParameter {
		kPos, // ポジション
		kScale, // スケール
		kEnd, // 末尾。要素数を取り出すよう。
	};

	enum class LayerParameter {
		kHP, // HP
		kEnd, // 末尾。要素数を取り出すよう。
	};

public:

	/// <summary>
	/// 衝撃波の作成
	/// </summary>
	/// <param name="pos">プレイヤーの座標</param>
	/// <param name="highest">プレイヤーの最大の高さ</param>
	/// <param name="y">層の高さ</param>
	void CreatShockWave(const Vector3& pos, float highest, float y);

private:

	/// <summary>
	/// jsonファイルへの書き込み
	/// </summary>
	void SetGlobalVariable();

	/// <summary>
	/// jsonファイルからの呼び出し
	/// </summary>
	void ApplyGlobalVariable();

	/// <summary>
	/// グローバル変数の初期化。ロードとかのまとめ
	/// </summary>
	void InitializeGlobalVariable();

	/// <summary>
	/// エネミーの生成
	/// </summary>
	void EnemyGeneration();

	/// <summary>
	/// エネミーのリストのクリア
	/// </summary>
	void EnemeiesClear();

	/// <summary>
	/// ImGuiで変えたエネミーの座標などをセットする。
	/// </summary>
	void SetEnemyParametar();

	/// <summary>
	/// 足場の生成
	/// </summary>
	void ScaffoldingGeneration();

	/// <summary>
	/// ImGuiで変えた足場の座標などをセットする。
	/// </summary>
	void SetScaffoldingParametar();

	/// <summary>
	/// 層の生成
	/// </summary>
	void CreateLayer();

	/// <summary>
	/// ImGuiで変えた層のパラメータのセット
	/// </summary>
	void SetLayerParametar();

	/// <summary>
	/// 衝撃波の削除
	/// </summary>
	void DeleteShockWave();

	/// <summary>
	/// プレイヤーとエネミーの当たり判定
	/// </summary>
	void Collision();

public:
	// ステージ番号のセッター
	void SetStageNumber(int32_t stage) {
		stage_ = stage;
		preStage_ = stage_;
	}

	void ShackUpdate();

private:

	// グローバル変数
	std::unique_ptr<GlobalVariables> globalVariables_;

	const std::string stageGruopName_ = "Stage";

	const std::string enemyGruoopName_ = "Enemy";

	const std::string enemyParameter[static_cast<uint16_t>(EnemyParameter::kEnd)] = {
		"Type", // タイプ
		"Pos", // 座標
		"MoveVector", // 移動方向
		"IsHealer", // ヒーラーかどうか
		"MoveRadius", // 移動の半分の距離 
	};

	const std::string scaffoldingGruoopName_ = "Scaffolding";

	const std::string scaffoldingParameter[static_cast<uint16_t>(ScaffoldingParameter::kEnd)] = {
		"Pos", // 座標
		"Scale", // スケール
	};
	const std::string layerGruoopName_ = "Layer";

	const std::string layerParameter[static_cast<uint16_t>(LayerParameter::kEnd)] = {
		"HP", // HP
	};

private:

	std::unique_ptr<Camera> camera2D_;

	Vector3 shackPos_;
	Vector3 preShack_;
	float shackMax_;
	bool isShack_ = false;
	float shackTime_;
	float shackCount_;

	Vector3 cameraLocalPos_;

	std::unique_ptr<Player> player_;

	std::unique_ptr<Background> background_;

	std::list<std::unique_ptr<Enemy>> enemies_;

	std::vector<std::vector<int>> enemyNums_;

	std::list<std::unique_ptr<Scaffolding>> scaffoldings_;

	std::vector<std::vector<int>> scaffoldingNums_;

	std::list<std::unique_ptr<ShockWave>> shockWaves_;

	std::unique_ptr<Layer> layer_;

	// 複数ステージの複数ある層のHP１つ１つが保存される配列
	std::vector<std::vector<int>> kLayerHitPoints_;

	// 複数ステージのある層の数
	std::vector<int> kLayerNums_;

	// 今のステージ。０が1ステージ目
	int stage_;

	int preStage_;

	// ステージ数
	int kMaxStageNum_;

	// フラグ用
	int preMaxStageNum_;

	// フラグ用
	std::vector<std::vector<int>> preEnemyNums_;

	// フラグ用
	std::vector<std::vector<int>> preScaffoldingNums_;

	// フラグ用
	std::vector<int> preLayerNums_;

	// ステージ毎の、それぞれのエネミーのポジション
	std::vector<std::vector<Vector3>> enemyPoses_;

	// ステージ毎の、それぞれのエネミーのタイプ
	std::vector<std::vector<int>> enemyType_;

	std::vector<std::vector<int>> enemyIsHealer_;

	std::vector<std::vector<int>> enemyMoveVector_;

	std::vector<std::vector<float>> enemyMoveRadius_;

	// ステージ毎の、それぞれの足場のポジション
	std::vector<std::vector<Vector3>> scaffoldingPoses_;

	// ステージ毎の、それぞれの足場のスケール
	std::vector<std::vector<Vector2>> scaffoldingScales_;


	// プレイ時間
	std::chrono::steady_clock::time_point startTime_;
	std::chrono::milliseconds playTime_;

	// 背景パーティクル
	Particle backGroundParticle_;


	// ポーズ画面
	Pause pause_;

	Audio* bgm_;

	// hud
	Texture2D aButtonHud_;
	Texture2D stickHud_;
	Texture2D puaseHud_;

	Texture2D spaceHud_;
	Texture2D rightKeyHud_;
	Texture2D leftKeyHud_;
	Texture2D puaseKeyHud_;


	///
	/// =============================================
	/// 
};