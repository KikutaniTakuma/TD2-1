#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Utils/Camera/Camera.h"
#include <vector>
#include "Fade/Fade.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include "Input/Input.h"

class BaseScene {
public:
	BaseScene() = default;
	BaseScene(const BaseScene&) = delete;
	BaseScene(BaseScene&&) = delete;
	virtual ~BaseScene() = default;

	BaseScene& operator=(const BaseScene&) = delete;
	BaseScene& operator=(BaseScene&&) = delete;

public:
	void SceneInitialize(class SceneManager* sceneManager);

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

	virtual void Update() = 0;
	virtual void Draw() = 0;

protected:
	class SceneManager* sceneManager_;

	MeshManager* meshManager_;

	AudioManager* audioManager_;

	TextureManager* textureManager_;

	FrameInfo* frameInfo_;

	Input* input_;

	static bool isPad_;

protected:
	Camera camera_;
};

class SceneManager final {
private:
	SceneManager();
	SceneManager(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	~SceneManager() = default;

	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

public:
	static SceneManager* const GetInstace();

public:
	void Initialize(BaseScene* firstScene);

	void SceneChange(BaseScene* next);

	void Update();

	void Draw();

private:
	std::unique_ptr<BaseScene> scene_;
	std::unique_ptr<BaseScene> next_;

	std::unique_ptr<Fade> fade_;

	Camera fadeCamera;
};