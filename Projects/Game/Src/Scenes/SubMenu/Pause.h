#pragma once
#include "SubMenu.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/Easing/Easing.h"
#include <array>

class Pause final : public SubMenu {
public:
	Pause() = default;
	~Pause() = default;

	Pause(const Pause&) = delete;
	Pause(Pause&&) = delete;
	Pause& operator=(const Pause&) = delete;
	Pause& operator=(const Pause&&) = delete;

public:
	void Initialize() override;

	void Finalize() override;

	void Draw() override;

private:
	void Update() override;

public:
	void SceneChange(int32_t nowStage);

private:
	// 背景を少し暗くする
	std::unique_ptr<Texture2D> backGround_;
	std::unique_ptr<Texture2D> backGround2_;
	std::unique_ptr<Texture2D> backGround3_;

	std::unique_ptr<Texture2D> goToTitle_;
	std::unique_ptr<Texture2D> goToStageSelect_;
	std::unique_ptr<Texture2D> goToGame_;

	std::unique_ptr<Texture2D> pauseTex_;

	int32_t currentChoose_;

	std::unique_ptr<Texture2D> arrow_;
	// 今選択しているところ(0:nextStage, 1:stageSelect)
	int32_t nowChoose_;
	std::array<float, 3> arrowPosY_;
	// 矢印のイージング
	std::pair<float, float> arrowPosX_;
	Easing arrowEase_;
	
	class Input* input_;
	class SceneManager* sceneManager_;
	class AudioManager* audioManager_;

	std::array<class Audio*, 4> audios_;

	bool isStick_;
};