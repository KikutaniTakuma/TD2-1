#pragma once

#include <list>

#include "Utils/Camera/Camera.h"
#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"

#include "Game/Player/Player.h"
#include "Game/Enemy/Enemy.h"
#include "Game/ShockWave/ShockWave.h"

#include "GlobalVariables/GlobalVariables.h"

class Play
{
public:

	// ImGuiで設定するエネミーのパラメータ
	enum class EnemyParameter {
		kPos, // ポジション
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
	void CreatShockWave(const Vector3& pos, float highest);

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

	const char* stageGruopName_ = "Stage";

	const char* enemyGruoopName_ = "Enemy";

	const char* enemyParameter[static_cast<uint16_t>(EnemyParameter::kEnd)] = {
		"Pos", // 座標
	};

private:

	std::unique_ptr<Camera> camera2D_;
	std::unique_ptr<Camera> camera3D_;

	std::unique_ptr<Player> player_;

	std::list<std::shared_ptr<Enemy>> enemies_;

	std::vector<int> enemyNums_;

	std::list<std::shared_ptr<ShockWave>> shockWaves_;

	// 今のステージ。０が1ステージ目
	int stageNum_;

	// ステージ数
	int kMaxStageNum_;

	// フラグ用
	int preMaxStageNum_;

	bool isFile_;

	// フラグ用
	std::vector<int> preEnemyNums_;


	std::vector<std::vector<Vector3>> enemyPoses_;
};
