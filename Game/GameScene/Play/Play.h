#pragma once

#include <list>

#include "Utils/Camera/Camera.h"
#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"

#include "Game/Player/Player.h"
#include "Game/Enemy/Enemy.h"
#include "Game/ShockWave/ShockWave.h"
#include "Game/Layer/Layer.h"
#include "Game/Scaffolding/Scaffolding.h"

#include "GlobalVariables/GlobalVariables.h"

class Play
{
public:

	// ImGuiで設定するエネミーのパラメータ
	enum class EnemyParameter {
		kPos, // ポジション
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

	Play();
	~Play() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

public:

	/// <summary>
	/// 衝撃波の作成
	/// </summary>
	/// <param name="highest">プレイヤーの座標</param>
	/// <param name="highest">プレイヤーの最大の高さ</param>
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

private:

	// グローバル変数
	std::unique_ptr<GlobalVariables> globalVariables_;

	const std::string stageGruopName_ = "Stage";

	const std::string enemyGruoopName_ = "Enemy";

	const std::string enemyParameter[static_cast<uint16_t>(EnemyParameter::kEnd)] = {
		"Pos", // 座標
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
	std::unique_ptr<Camera> camera3D_;

	std::unique_ptr<Player> player_;

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

	// ステージ毎の、それぞれの足場のポジション
	std::vector<std::vector<Vector3>> scaffoldingPoses_;

	// ステージ毎の、それぞれの足場のスケール
	std::vector<std::vector<Vector2>> scaffoldingScales_;
};
