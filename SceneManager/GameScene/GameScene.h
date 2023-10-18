#pragma once
#include "SceneManager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/Particle/Particle.h"
#include "Drawers/PeraRender/PeraRender.h"
#include "GlobalVariables/GlobalVariables.h"

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
	std::vector<Model> models_;
	std::vector<Texture2D> texs_;
	std::vector<Particle> particles;
	GlobalVariables globalVariables_;

	///
	/// ここに必要なメンバ変数を追加
	/// 





	///
	/// =============================================
	/// 
};