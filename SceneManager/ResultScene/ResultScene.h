#pragma once
#include "SceneManager/SceneManager.h"
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
	void SetClearTime(std::chrono::milliseconds clearTime) {
		clearTime_ = clearTime;
	}




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
	std::chrono::milliseconds clearTime_;

	///
	/// ここに必要なメンバ変数を追加
	/// 
	
	Star hoge;



	///
	/// =============================================
	/// 
};