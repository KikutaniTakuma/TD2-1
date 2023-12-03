#include "BaseScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Input/Input.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"

BaseScene::BaseScene(BaseScene::ID sceneID) :
	sceneManager_(nullptr),
	meshManager_(nullptr),
	audioManager_(nullptr),
	textureManager_(nullptr),
	frameInfo_(nullptr),
	input_(nullptr),
	stringOutPutManager_(nullptr),
	sceneID_(sceneID),
	camera_()
{}

void BaseScene::SceneInitialize(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;

	meshManager_ = MeshManager::GetInstance();

	audioManager_ = AudioManager::GetInstance();

	textureManager_ = TextureManager::GetInstance();

	frameInfo_ = FrameInfo::GetInstance();

	stringOutPutManager_ = StringOutPutManager::GetInstance();

	input_ = Input::GetInstance();
}