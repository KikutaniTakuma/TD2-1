#pragma once
#include "SubMenu.h"
#include "Drawers/Texture2D/Texture2D.h"

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
	// メニューバー
	Texture2D menuBar_;
	// 背景を少し暗くする
	Texture2D backGround_;

	Texture2D goToTitle_;
	Texture2D goToStageSelect_;
	Texture2D goToGame_;

	int32_t currentChoose_;
};