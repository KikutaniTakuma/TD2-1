#pragma once
#include "SubMenu.h"
#include "Drawers/Texture2D/Texture2D.h"
#include "Utils/Easeing/Easeing.h"
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

private:
	// 背景を少し暗くする
	Texture2D backGround_;
	Texture2D backGround2_;
	Texture2D backGround3_;

	Texture2D goToTitle_;
	Texture2D goToStageSelect_;
	Texture2D goToGame_;

	Texture2D pauseTex_;

	int32_t currentChoose_;

	Texture2D arrow_;
	// 今選択しているところ(0:nextStage, 1:stageSelect)
	int32_t nowChoose_;
	std::array<float, 3> arrowPosY_;
	// 矢印のイージング
	std::pair<float, float> arrowPosX_;
	Easeing arrowEase_;
	
	class Input* input_;
	class SceneManager* sceneManager_;
	class AudioManager* audioManager_;

	std::array<class Audio*, 4> audios_;

	bool isStick_;
};