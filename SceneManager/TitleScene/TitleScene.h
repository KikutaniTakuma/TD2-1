#pragma once
#include "SceneManager/SceneManager.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/Easeing/Easeing.h"
#include "Drawers/Particle/Particle.h"

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
	std::array<Texture2D, 3> backGround_;
	// 背景に軽く縦平均化ブラー
	PeraRender backGroundBlur_;

	Model player_;
	std::pair<Vector3, Vector3> playerScale_;
	std::pair<Vector3, Vector3> playerPos_;
	Easeing playerScaleEaseing_;
	Easeing playerPosEaseing_;

	// タイトル
	Texture2D titleTex_;

	// 床
	Texture2D floor_;

	// 背景パーティクル
	Particle backGroundParticle_;
};