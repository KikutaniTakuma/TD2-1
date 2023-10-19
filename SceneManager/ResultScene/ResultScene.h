#pragma once
#include "SceneManager/SceneManager.h"
#include "Drawers/Model/Model.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Drawers/Particle/Particle.h"
#include "GlobalVariables/GlobalVariables.h"

class ResultScene : public BaseScene {
public:
	ResultScene();
	ResultScene(const ResultScene&) = delete;
	ResultScene(ResultScene&&) = delete;
	~ResultScene() = default;

	ResultScene& operator=(const ResultScene&) = delete;
	ResultScene& operator=(ResultScene&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

private:
	std::vector<Model> models_;
	std::vector<Texture2D> texs_;
	std::vector<Particle> particles_;
	GlobalVariables globalVariables_;

	///
	/// ここに必要なメンバ変数を追加
	/// 
	




	///
	/// =============================================
	/// 
};