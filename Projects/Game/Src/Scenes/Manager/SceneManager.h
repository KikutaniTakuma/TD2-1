#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <bitset>
#include <optional>
#include "Fade/Fade.h"
#include "Input/Input.h"
#include "BaseScene/BaseScene.h"

class SceneManager final {
private:
	SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	~SceneManager() = default;

	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

public:
	static SceneManager* const GetInstance();

public:
	void Initialize(std::optional<BaseScene::ID> firstScene, std::optional<BaseScene::ID> finishID);
	void Finalize();

	void SceneChange(std::optional<BaseScene::ID> next);

	void SceneChange(BaseScene* next);

	void Update();

	void Draw();

public:
	inline bool GetIsPad() const {
		return isPad_;
	}

	bool IsEnd() const;

	void SetClearMilliSecond(size_t stageNumber, const std::chrono::milliseconds& cleatTime) {
		clearData_[stageNumber].push_back(cleatTime);
		clearCountData_[stageNumber]++;
	}

private:
	std::unique_ptr<BaseScene> scene_;
	std::unique_ptr<BaseScene> next_;

	std::unique_ptr<Fade> fade_;

	Camera fadeCamera_;

	FrameInfo* frameInfo_ = nullptr;
	Input* input_ = nullptr;

	bool isPad_ = false;

	std::optional<BaseScene::ID> finishID_;

	std::array<std::vector<std::chrono::milliseconds>, 10> clearData_;
	std::array<uint32_t, 10> clearCountData_;

public:
	std::bitset<10> isClearStage_;
};