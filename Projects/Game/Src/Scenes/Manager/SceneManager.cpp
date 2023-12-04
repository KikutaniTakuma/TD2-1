#include "SceneManager.h"
#include "Engine/Engine.h"
#include "Utils/UtilsLib/UtilsLib.h"
#include <filesystem>
#include <fstream>
#include <format>
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Input/Input.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"
#include "SceneFactory/SceneFactory.h"

#include "Game/Enemy/Enemy.h"
#include "Game/ShockWave/ShockWave.h"
#include "Game/Layer/Layer.h"

SceneManager* const SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::Initialize(std::optional<BaseScene::ID> firstScene, std::optional<BaseScene::ID> finishID) {
	/*const std::filesystem::path fileName_ = "./ExecutionLog/PlayLog.txt";
	if (!std::filesystem::exists(fileName_.parent_path())) {
		std::filesystem::create_directories(fileName_.parent_path());
	}

	std::ofstream playLogFile;
	playLogFile.open(fileName_, std::ios::app);
	if (!playLogFile.fail()) {
		auto now = std::chrono::system_clock::now();
		auto nowSec = std::chrono::floor<std::chrono::seconds>(now);
		std::chrono::zoned_time zt{ "Asia/Tokyo", nowSec };

		playLogFile << std::endl
			<< "play start : " << std::format("{:%Y/%m/%d %H:%M:%S}", zt) << std::endl;
	}*/

	fade_ = std::make_unique<Fade>();
	fadeCamera_.Update();

	frameInfo_ = FrameInfo::GetInstance();
	input_ = Input::GetInstance();

	SceneFactory* const sceneFactory = SceneFactory::GetInstance();

	scene_.reset(sceneFactory->CreateBaseScene(firstScene));
	scene_->SceneInitialize(this);
	scene_->Initialize();

	StringOutPutManager::GetInstance()->LoadFont("./Resources/Font/default.spritefont");

	finishID_ = finishID;

	Enemy::GlobalVariablesLoad();
	ShockWave::GlobalVariablesLoad();
	Layer::GlobalVariablesLoad();
	ShockWave::SetGlobalVariable();

	frameInfo_->SetFpsLimit(60.0);
}

void SceneManager::SceneChange(std::optional<BaseScene::ID> next) {
	if (next_) {
		return;
	}
	SceneFactory* const sceneFactory = SceneFactory::GetInstance();

	next_.reset(sceneFactory->CreateBaseScene(next));
	next_->SceneInitialize(this);

	fade_->OutStart();
}

void SceneManager::SceneChange(BaseScene* next) {
	if (next_) {
		return;
	}
	next_.reset(next);
	next_->SceneInitialize(this);

	fade_->OutStart();
}

void SceneManager::Update() {
	if (input_->GetGamepad()->PushAnyKey()) {
		isPad_ = true;
	}
	else if (input_->GetMouse()->PushAnyKey() || input_->GetKey()->PushAnyKey()) {
		isPad_ = false;
	}

	if (scene_ && !next_ && !fade_->InStay()) {
		scene_->Update();
	}
	else {
		fade_->Update();
	}

	if (fade_->OutEnd()) {
		fade_->InStart();
		scene_->Finalize();
		scene_.reset(next_.release());
		next_.reset();
		scene_->Initialize();
		scene_->Update();
		fade_->Update();
	}
	else if (fade_->InEnd()) {
		fade_->Update();
	}
}

void SceneManager::Draw() {
	if (scene_) {
		scene_->Draw();
	}

	fade_->Draw(fadeCamera_.GetViewOthographics());
}

bool SceneManager::IsEnd() const {
	if (!scene_) {
		return true;
	}

	return scene_->GetID() == finishID_ && 
	(input_->GetKey()->Pushed(DIK_ESCAPE) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::START));
}

void SceneManager::Finalize() {
	fade_.reset();
	if (scene_) {
		scene_->Finalize();
	}
	scene_.reset();
	if (next_) {
		next_->Finalize();
	}
	next_.reset();


	/*const std::filesystem::path fileName_ = "./ExecutionLog/PlayLog.txt";

	if (!std::filesystem::exists(fileName_.parent_path())) {
		std::filesystem::create_directories(fileName_.parent_path());
	}

	std::ofstream playLogFile;
	playLogFile.open(fileName_, std::ios::app);
	if (!playLogFile.fail()) {
		auto now = std::chrono::system_clock::now();
		auto nowSec = std::chrono::floor<std::chrono::seconds>(now);
		std::chrono::zoned_time zt{ "Asia/Tokyo", nowSec };

		for (size_t i = 0; i < clearData_.size(); i++) {
			playLogFile << "stage " << std::to_string(i + 1) << " : clear count " << clearCountData_[i] << std::endl;
			std::chrono::milliseconds averageClearTime{};
			for (auto& time : clearData_[i]) {
				averageClearTime += time;
			}
			if (!clearData_[i].empty()) {
				averageClearTime /= clearData_[i].size();
			}
			std::chrono::seconds averageClearTimeSecond = std::chrono::duration_cast<std::chrono::seconds>(averageClearTime);

			playLogFile << "           average " << averageClearTimeSecond << std::endl;
		}

		playLogFile
			<< "play end : " << std::format("{:%Y/%m/%d %H:%M:%S}", zt) << std::endl;
	}*/
}
