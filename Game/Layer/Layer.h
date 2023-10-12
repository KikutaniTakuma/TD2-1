#pragma once

#include "Utils/Math/Mat4x4.h"
#include "Drawers/Texture2D/Texture2D.h"

#include "GlobalVariables/GlobalVariables.h"

class Layer
{
public:

	Layer(int kMaxLayerNum, const std::vector<int>& kMaxHitPoints);
	~Layer() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="kMaxLayerNum">層の数</param>
	/// <param name="kMaxHitPoints">層毎のHP</param>
	void Initialize(int kMaxLayerNum, const std::vector<int>& kMaxHitPoints);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 3DモデルのDraw仮
	/// </summary>
	/// <param name="viewProjection">カメラのマトリックス</param>
	void Draw(const Mat4x4& viewProjection);

	/// <summary>
	/// 2DテクスチャのDraw
	/// </summary>
	/// <param name="viewProjection">カメラのマトリックス</param>
	void Draw2D(const Mat4x4& viewProjection);

public:

	/// <summary>
	/// グローバル変数のロード
	/// </summary>
	static void GlobalVariablesLoad() { globalVariables_->LoadFile(groupName_); }

	/// <summary>
	/// 静的メンバ定数のImGui用
	/// </summary>
	static void GlobalVariablesUpdate() { globalVariables_->Update(); }

public:

	/// <summary>
	/// HPなどのパラメーターをいれる
	/// </summary>
	void SetParametar(std::vector<int> kMaxHitPoints);

	const Texture2D* GetHighestTex() { return tex_[nowLayer_].get(); }

	const float GetHighestPosY() { return (tex_[nowLayer_]->pos.y + tex_[nowLayer_]->scale.y / 2.0f); }

	const int GetNowLayer() { return nowLayer_; }

	const bool GetChangeLayerFlag() { return isChangeLayer_; }

	const bool GetClearFlag() { return isClear_; }

	/// <summary>
	/// ダメージの加算
	/// </summary>
	void AddDamage(int damage) { damage_ += damage; }

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
	/// 初期化
	/// </summary>
	void Reset();

private:

	// グローバル変数
	static std::unique_ptr<GlobalVariables> globalVariables_;

	// 静的メンバ定数のグローバル変数のグループネーム
	static const std::string groupName_;

	static Vector2 kLayer2DScale_;

	static float kFirstLayerCenterPosY_;

	static float highestPosY_;

private:

	// テクスチャ
	std::vector<std::unique_ptr<Texture2D>> tex_;

	// HP
	std::vector<int> hitPoints_;

	// HPの最大値
	std::vector<int> kMaxHitPoints_;

	int damage_;

	// 層の数
	int kMaxLayerNum_;

	// 今の層
	int nowLayer_;

	bool isChangeLayer_;

	bool isClear_;

};