#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "imgui.h"
#include "Scenes/ResultScene/ResultScene.h"
#include "Utils/Random/Random.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{

	/*models_(),
	texs_(),
	globalVariables_()*/

	//Enemy::GlobalVariablesLoad();
	//ShockWave::GlobalVariablesLoad();
	//Layer::GlobalVariablesLoad();

	camera2D_ = std::make_unique<Camera>();

	cameraLocalPos_ = { 0.0f,200.0f,0.0f };

	player_ = std::make_unique<Player>();
	player_->SetGameScene(this);

	stage_ = 0;
	preStage_ = stage_;

	kMaxStageNum_ = 1;

	enemyNums_.push_back(std::vector<int>());

	scaffoldingNums_.push_back(std::vector<int>());

	//ShockWave::SetGlobalVariable();

	background_ = std::make_unique<Background>();

	preMaxStageNum_ = kMaxStageNum_;
	preEnemyNums_ = enemyNums_;
	preScaffoldingNums_ = scaffoldingNums_;
	preLayerNums_ = kLayerNums_;

	globalVariables_ = std::make_unique<GlobalVariables>();

	InitializeGlobalVariable();

	SetGlobalVariable();

	//InitializeGlobalVariable();


	layer_ = std::make_unique<Layer>(kLayerNums_[stage_], kLayerHitPoints_[stage_]);

	playTime_ = std::chrono::milliseconds{ 0 };
}

void GameScene::Initialize() {
	aButtonHud_.reset(new Texture2D);
	stickHud_.reset(new Texture2D);
	puaseHud_.reset(new Texture2D);

	spaceHud_.reset(new Texture2D);
	rightKeyHud_.reset(new Texture2D);
	leftKeyHud_.reset(new Texture2D);
	puaseKeyHud_.reset(new Texture2D);

	backGroundParticle_.reset(new Particle{});


	textureManager_->LoadTexture("./Resources/ShockWave/player_wave.png");

	textureManager_->LoadTexture("./Resources/ShockWave/player_wave_gyaku.png");

	shockWaves_.clear();

	if (stage_ >= kMaxStageNum_) {
		stage_ = kMaxStageNum_ - 1;
	}

	layer_->Initialize(kLayerNums_[stage_], kLayerHitPoints_[stage_]);

	player_->Initialize();

	EnemeiesClear();

	EnemyGeneration();

	scaffoldings_.clear();

	ScaffoldingGeneration();

	startTime_ = std::chrono::steady_clock::now();

	backGroundParticle_->LoadSettingDirectory("backGroundParticle");
	backGroundParticle_->ParticleStart();

	pause_.reset(new Pause{});
	pause_->Initialize();

	bgm_ = audioManager_->LoadWav("./Resources/Audio/BGM/BGM/game.wav", true);
	bgm_->Start(0.1f);

	// コントローラーHUD
	aButtonHud_->LoadTexture("./Resources/HUD/controler_UI_A.png");
	aButtonHud_->scale = Vector2{ 70.0f, 70.0f };
	aButtonHud_->pos = Vector2{ 544.0f,291.0f };
	stickHud_->LoadTexture("./Resources/HUD/UI_stick.png");
	stickHud_->scale = Vector2{ 70.0f, 80.0f };
	stickHud_->pos = Vector2{ 453.0f,291.0f };
	puaseHud_->LoadTexture("./Resources/HUD/controler_UI_pose.png");
	puaseHud_->scale = Vector2{ 70.0f, 70.0f };
	puaseHud_->pos = Vector2{ -555.0f,291.0f };

	// キーボードHUD
	spaceHud_->LoadTexture("./Resources/HUD/keys_UI_space.png");
	spaceHud_->scale = Vector2{ 92.0f, 124.0f };
	spaceHud_->pos = Vector2{ 544.0f,298.0f };
	rightKeyHud_->LoadTexture("./Resources/HUD/keys_UI_right.png");
	rightKeyHud_->scale = Vector2{ 93.0f, 69.0f };
	rightKeyHud_->pos = Vector2{ 438.0f,291.0f };
	leftKeyHud_->LoadTexture("./Resources/HUD/keys_UI_left.png");
	leftKeyHud_->scale = Vector2{ 93.0f, 69.0f };
	leftKeyHud_->pos = Vector2{ 359.0f,291.0f };
	puaseKeyHud_->LoadTexture("./Resources/HUD/keys_UI_pose.png");
	puaseKeyHud_->scale = Vector2{ 115.0f, 113.0f };
	puaseKeyHud_->pos = Vector2{ -555.0f,291.0f };

	aButtonHud_->uvSize.x = 0.5f;
	spaceHud_->uvSize.x = 0.5f;
	stickHud_->uvSize.x = 1.0f / 3.0f;
	rightKeyHud_->uvSize.x = 0.5f;
	leftKeyHud_->uvSize.x = 0.5f;
	puaseHud_->uvSize.x = 0.5f;
	puaseKeyHud_->uvSize.x = 0.5f;
}

void GameScene::Finalize() {
	bgm_->Stop();
}

void GameScene::InitializeGlobalVariable() {

	globalVariables_->LoadFile("Game");

	camera2D_->pos = globalVariables_->GetVector3Value("Game", "Camera2DPos");

	kMaxStageNum_ = globalVariables_->GetIntValue("Game", "kMaxStageNum");

	for (int stageNum = 0; stageNum < kMaxStageNum_; stageNum++) {

		//const char stage = (char)stageNum;

		std::string stage = std::to_string(stageNum);

		std::string g = stageGruopName_ + stage;

		globalVariables_->LoadFile(g);

		if (kLayerNums_.size() <= stageNum) {
			kLayerNums_.push_back(1);
		}

		kLayerNums_[stageNum] = globalVariables_->GetIntValue(g, "LayerNum");

		if (kLayerHitPoints_.size() <= stageNum) {
			kLayerHitPoints_.push_back(std::vector<int>());
			kLayerHitPoints_[stageNum].push_back(1);
		}

		if (enemyNums_.size() <= stageNum) {
			enemyNums_.push_back(std::vector<int>());
			enemyNums_[stageNum].push_back(1);
		}
		if (scaffoldingNums_.size() <= stageNum) {
			scaffoldingNums_.push_back(std::vector<int>());
			scaffoldingNums_[stageNum].push_back(1);
		}

		for (int layerNum = 0; layerNum < kLayerNums_[stageNum]; layerNum++) {

			if (kLayerHitPoints_[stageNum].size() <= layerNum) {
				kLayerHitPoints_[stageNum].push_back(1);
			}

			std::string layer = std::to_string(layerNum);

			std::string i = layerGruoopName_ + layer + layerParameter[static_cast<uint16_t>(LayerParameter::kHP)];

			kLayerHitPoints_[stageNum][layerNum] = globalVariables_->GetIntValue(g, i);


			if (enemyNums_[stageNum].size() <= layerNum) {
				enemyNums_[stageNum].push_back(1);
			}
			if (scaffoldingNums_[stageNum].size() <= layerNum) {
				scaffoldingNums_[stageNum].push_back(1);
			}

			i = layerGruoopName_ + layer + std::string("EnemyNum");

			enemyNums_[stageNum][layerNum] = globalVariables_->GetIntValue(g, i);

			i = layerGruoopName_ + layer + std::string("ScaffoldingNum");

			scaffoldingNums_[stageNum][layerNum] = globalVariables_->GetIntValue(g, i);

			if (enemyPoses_.size() <= stageNum) {
				enemyPoses_.push_back(std::vector<Vector3>());
				enemyType_.push_back(std::vector<int>());
				enemyIsHealer_.push_back(std::vector<int>());
				enemyMoveVector_.push_back(std::vector<int>());
				enemyMoveRadius_.push_back(std::vector<float>());
			}

			if (scaffoldingPoses_.size() <= stageNum) {
				scaffoldingPoses_.push_back(std::vector<Vector3>());
				scaffoldingScales_.push_back(std::vector<Vector2>());
			}


			for (int enemyNum = 0; enemyNum < enemyNums_[stageNum][layerNum]; enemyNum++) {

				if (enemyPoses_[stageNum].size() <= enemyNum) {
					enemyPoses_[stageNum].push_back(Vector3{ -200.0f + 100.0f * enemyNum, 300.0f,0.0f });
					enemyType_[stageNum].push_back(0);
					enemyIsHealer_[stageNum].push_back(0);
					enemyMoveVector_[stageNum].push_back(0);
					enemyMoveRadius_[stageNum].push_back(0.0f);
				}

				std::string enemy = std::to_string(enemyNum);

				std::string item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kPos)];

				//globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
				enemyPoses_[stageNum][enemyNum] = globalVariables_->GetVector3Value(g, item);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kType)];

				enemyType_[stageNum][enemyNum] = globalVariables_->GetIntValue(g, item);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kMoveVector)];

				enemyMoveVector_[stageNum][enemyNum] = globalVariables_->GetIntValue(g, item);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kIsHealer)];

				enemyIsHealer_[stageNum][enemyNum] = globalVariables_->GetIntValue(g, item);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kMoveRadius)];

				enemyMoveRadius_[stageNum][enemyNum] = globalVariables_->GetFloatValue(g, item);
			}

			for (int scaffoldingNum = 0; scaffoldingNum < scaffoldingNums_[stageNum][layerNum]; scaffoldingNum++) {

				if (scaffoldingPoses_[stageNum].size() <= scaffoldingNum) {
					scaffoldingPoses_[stageNum].push_back(Vector3{ -200.0f + 100.0f * scaffoldingNum, 100.0f,0.0f });
					scaffoldingScales_[stageNum].push_back(Vector2{ 100.0f, 10.0f });
				}

				std::string scaffolding = std::to_string(scaffoldingNum);

				std::string item = scaffoldingGruoopName_ + scaffolding + scaffoldingParameter[static_cast<uint16_t>(ScaffoldingParameter::kPos)];

				scaffoldingPoses_[stageNum][scaffoldingNum] = globalVariables_->GetVector3Value(g, item);

				item = scaffoldingGruoopName_ + scaffolding + scaffoldingParameter[static_cast<uint16_t>(ScaffoldingParameter::kScale)];

				scaffoldingScales_[stageNum][scaffoldingNum] = globalVariables_->GetVector2Value(g, item);
			}

		}

	}

}

void GameScene::SetGlobalVariable() {

	globalVariables_->CreateGroup("Game");

	globalVariables_->AddItem("Game", "kMaxStageNum", kMaxStageNum_);

	globalVariables_->AddItem("Game", "Camera2DPos", camera2D_->pos);

	for (int stageNum = 0; stageNum < kMaxStageNum_; stageNum++) {

		std::string stage = std::to_string(stageNum);

		std::string g = stageGruopName_ + stage;

		globalVariables_->CreateGroup(g);

		if (kLayerNums_.size() <= stageNum) {
			kLayerNums_.push_back(1);
		}

		globalVariables_->AddItem(g, "LayerNum", kLayerNums_[stageNum]);

		if (kLayerHitPoints_.size() <= stageNum) {
			kLayerHitPoints_.push_back(std::vector<int>());
			kLayerHitPoints_[stageNum].push_back(1);
		}

		if (enemyNums_.size() <= stageNum) {
			enemyNums_.push_back(std::vector<int>());
			enemyNums_[stageNum].push_back(1);
		}
		if (scaffoldingNums_.size() <= stageNum) {
			scaffoldingNums_.push_back(std::vector<int>());
			scaffoldingNums_[stageNum].push_back(1);
		}

		for (int layerNum = 0; layerNum < kLayerNums_[stageNum]; layerNum++) {

			if (kLayerHitPoints_[stageNum].size() <= layerNum) {
				kLayerHitPoints_[stageNum].push_back(1);
			}

			std::string layer = std::to_string(layerNum);

			std::string i = layerGruoopName_ + layer + layerParameter[static_cast<uint16_t>(LayerParameter::kHP)];

			globalVariables_->AddItem(g, i, kLayerHitPoints_[stageNum][layerNum]);


			if (enemyNums_[stageNum].size() <= layerNum) {
				if (layerNum == 0) {
					enemyNums_[stageNum].push_back(1);
				}
				else {
					enemyNums_[stageNum].push_back(enemyNums_[stageNum][layerNum - 1]);
				}
			}
			if (scaffoldingNums_[stageNum].size() <= layerNum) {
				if (layerNum == 0) {
					scaffoldingNums_[stageNum].push_back(0);
				}
				else {
					scaffoldingNums_[stageNum].push_back(scaffoldingNums_[stageNum][layerNum - 1]);
				}
			}

			i = layerGruoopName_ + layer + std::string("EnemyNum");

			globalVariables_->AddItem(g, i, enemyNums_[stageNum][layerNum]);

			i = layerGruoopName_ + layer + std::string("ScaffoldingNum");

			globalVariables_->AddItem(g, i, scaffoldingNums_[stageNum][layerNum]);

			if (enemyPoses_.size() <= stageNum) {
				enemyPoses_.push_back(std::vector<Vector3>());
				enemyType_.push_back(std::vector<int>());
				enemyIsHealer_.push_back(std::vector<int>());
				enemyMoveVector_.push_back(std::vector<int>());
				enemyMoveRadius_.push_back(std::vector<float>());
			}

			if (scaffoldingPoses_.size() <= stageNum) {
				scaffoldingPoses_.push_back(std::vector<Vector3>());
				scaffoldingScales_.push_back(std::vector<Vector2>());
			}

			for (int enemyNum = 0; enemyNum < enemyNums_[stageNum][layerNum]; enemyNum++) {

				if (enemyPoses_[stageNum].size() <= enemyNum) {
					enemyPoses_[stageNum].push_back(Vector3{ -200.0f + 100.0f * enemyNum, 300.0f,0.0f });
					enemyType_[stageNum].push_back(0);
					enemyMoveVector_[stageNum].push_back(0);
					enemyIsHealer_[stageNum].push_back(0);
					enemyMoveRadius_[stageNum].push_back(0.0f);
				}

				std::string enemy = std::to_string(enemyNum);

				std::string item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kPos)];

				globalVariables_->AddItem(g, item, enemyPoses_[stageNum][enemyNum]);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kType)];

				globalVariables_->AddItem(g, item, enemyType_[stageNum][enemyNum]);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kMoveVector)];

				globalVariables_->AddItem(g, item, enemyMoveVector_[stageNum][enemyNum]);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kIsHealer)];

				globalVariables_->AddItem(g, item, enemyIsHealer_[stageNum][enemyNum]);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kMoveRadius)];

				globalVariables_->AddItem(g, item, enemyMoveRadius_[stageNum][enemyNum]);
			}

			for (int scaffoldingNum = 0; scaffoldingNum < scaffoldingNums_[stageNum][layerNum]; scaffoldingNum++) {

				if (scaffoldingPoses_[stageNum].size() <= scaffoldingNum) {
					scaffoldingPoses_[stageNum].push_back(Vector3{ -200.0f + 100.0f * scaffoldingNum, 100.0f,0.0f });
					scaffoldingScales_[stageNum].push_back(Vector2{ 100.0f, 10.0f });
				}

				std::string scaffolding = std::to_string(scaffoldingNum);

				std::string item = scaffoldingGruoopName_ + scaffolding + scaffoldingParameter[static_cast<uint16_t>(ScaffoldingParameter::kPos)];

				globalVariables_->AddItem(g, item, scaffoldingPoses_[stageNum][scaffoldingNum]);

				item = scaffoldingGruoopName_ + scaffolding + scaffoldingParameter[static_cast<uint16_t>(ScaffoldingParameter::kScale)];

				globalVariables_->AddItem(g, item, scaffoldingScales_[stageNum][scaffoldingNum]);
			}


		}


	}

	//globalVariables_->LoadFile(groupName_);m
	ApplyGlobalVariable();
}

void GameScene::ApplyGlobalVariable() {

	kMaxStageNum_ = globalVariables_->GetIntValue("Game", "kMaxStageNum");

	cameraLocalPos_ = globalVariables_->GetVector3Value("Game", "Camera2DPos");

	for (int stageNum = 0; stageNum < kMaxStageNum_; stageNum++) {

		std::string stage = std::to_string(stageNum);

		std::string g = stageGruopName_ + stage;

		if (kLayerNums_.size() <= stageNum) {
			kLayerNums_.push_back(1);
		}

		if (preMaxStageNum_ < kMaxStageNum_) {
			globalVariables_->AddItem(g, "LayerNum", kLayerNums_[stageNum]);
		}
		else if (kLayerNums_.size() > preLayerNums_.size()) {
			globalVariables_->AddItem(g, "LayerNum", kLayerNums_[stageNum]);
		}
		else if (kLayerNums_[stageNum] != preLayerNums_[stageNum]) {
			globalVariables_->AddItem(g, "LayerNum", kLayerNums_[stageNum]);
		}
		kLayerNums_[stageNum] = globalVariables_->GetIntValue(g, "LayerNum");

		if (kLayerHitPoints_.size() <= stageNum) {
			kLayerHitPoints_.push_back(std::vector<int>());
		}

		if (enemyNums_.size() <= stageNum) {
			enemyNums_.push_back(std::vector<int>());
			enemyNums_[stageNum].push_back(1);
		}

		if (enemyPoses_.size() <= stageNum) {
			enemyPoses_.push_back(std::vector<Vector3>());
			enemyType_.push_back(std::vector<int>());
			enemyMoveVector_.push_back(std::vector<int>());
			enemyIsHealer_.push_back(std::vector<int>());
			enemyMoveRadius_.push_back(std::vector<float>());
		}

		if (scaffoldingNums_.size() <= stageNum) {
			scaffoldingNums_.push_back(std::vector<int>());
			scaffoldingNums_[stageNum].push_back(1);
		}

		if (scaffoldingPoses_.size() <= stageNum) {
			scaffoldingPoses_.push_back(std::vector<Vector3>());
			scaffoldingScales_.push_back(std::vector<Vector2>());
		}

		for (int layerNum = 0; layerNum < kLayerNums_[stageNum]; layerNum++) {

			if (kLayerHitPoints_[stageNum].size() <= layerNum) {
				kLayerHitPoints_[stageNum].push_back(1);
			}

			std::string layer = std::to_string(layerNum);

			std::string i = layerGruoopName_ + layer + layerParameter[static_cast<uint16_t>(LayerParameter::kHP)];

			if (preMaxStageNum_ < kMaxStageNum_) {
				globalVariables_->AddItem(g, i, kLayerHitPoints_[stageNum][layerNum]);
			}
			else if (kLayerNums_.size() > preLayerNums_.size()) {
				globalVariables_->AddItem(g, i, kLayerHitPoints_[stageNum][layerNum]);
			}
			else if (kLayerNums_[stageNum] != preLayerNums_[stageNum]) {
				globalVariables_->AddItem(g, i, kLayerHitPoints_[stageNum][layerNum]);
			}

			kLayerHitPoints_[stageNum][layerNum] = globalVariables_->GetIntValue(g, i);

			i = layerGruoopName_ + layer + std::string("EnemyNum");

			if (enemyNums_[stageNum].size() <= layerNum) {
				enemyNums_[stageNum].push_back(enemyNums_[stageNum][layerNum - 1]);
			}

			if (preMaxStageNum_ < kMaxStageNum_) {
				globalVariables_->AddItem(g, i, enemyNums_[stageNum][layerNum]);
			}
			else if (kLayerNums_.size() > preLayerNums_.size()) {
				globalVariables_->AddItem(g, i, enemyNums_[stageNum][layerNum]);
			}
			else if (kLayerNums_[stageNum] != preLayerNums_[stageNum]) {
				globalVariables_->AddItem(g, i, enemyNums_[stageNum][layerNum]);
			}

			enemyNums_[stageNum][layerNum] = globalVariables_->GetIntValue(g, i);

			i = layerGruoopName_ + layer + std::string("ScaffoldingNum");

			if (scaffoldingNums_[stageNum].size() <= layerNum) {
				scaffoldingNums_[stageNum].push_back(scaffoldingNums_[stageNum][layerNum - 1]);
			}

			if (preMaxStageNum_ < kMaxStageNum_) {
				globalVariables_->AddItem(g, i, scaffoldingNums_[stageNum][layerNum]);
			}
			else if (kLayerNums_.size() > preLayerNums_.size()) {
				globalVariables_->AddItem(g, i, scaffoldingNums_[stageNum][layerNum]);
			}
			else if (kLayerNums_[stageNum] != preLayerNums_[stageNum]) {
				globalVariables_->AddItem(g, i, scaffoldingNums_[stageNum][layerNum]);
			}

			scaffoldingNums_[stageNum][layerNum] = globalVariables_->GetIntValue(g, i);


			for (int enemyNum = 0; enemyNum < enemyNums_[stageNum][layerNum]; enemyNum++) {

				if (enemyPoses_[stageNum].size() <= enemyNum) {
					enemyPoses_[stageNum].push_back(Vector3{ -200.0f + 100.0f * enemyNum, 300.0f,0.0f });
					enemyType_[stageNum].push_back(0);
					enemyMoveVector_[stageNum].push_back(0);
					enemyIsHealer_[stageNum].push_back(0);
					enemyMoveRadius_[stageNum].push_back(0.0f);
				}

				std::string enemy = std::to_string(enemyNum);

				std::string item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kPos)];

				if (preMaxStageNum_ < kMaxStageNum_) {
					globalVariables_->AddItem(g, item, enemyPoses_[stageNum][enemyNum]);
				}
				else if (enemyNums_.size() > preEnemyNums_.size()) {
					globalVariables_->AddItem(g, item, enemyPoses_[stageNum][enemyNum]);
				}
				else if (enemyNums_[stageNum] > preEnemyNums_[stageNum]) {
					globalVariables_->AddItem(g, item, enemyPoses_[stageNum][enemyNum]);
				}
				//globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
				enemyPoses_[stageNum][enemyNum] = globalVariables_->GetVector3Value(g, item);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kType)];

				if (preMaxStageNum_ < kMaxStageNum_) {
					globalVariables_->AddItem(g, item, enemyType_[stageNum][enemyNum]);
				}
				else if (enemyNums_.size() > preEnemyNums_.size()) {
					globalVariables_->AddItem(g, item, enemyType_[stageNum][enemyNum]);
				}
				else if (enemyNums_[stageNum] > preEnemyNums_[stageNum]) {
					globalVariables_->AddItem(g, item, enemyType_[stageNum][enemyNum]);
				}

				enemyType_[stageNum][enemyNum] = globalVariables_->GetIntValue(g, item);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kMoveVector)];

				if (preMaxStageNum_ < kMaxStageNum_) {
					globalVariables_->AddItem(g, item, enemyMoveVector_[stageNum][enemyNum]);
				}
				else if (enemyNums_.size() > preEnemyNums_.size()) {
					globalVariables_->AddItem(g, item, enemyMoveVector_[stageNum][enemyNum]);
				}
				else if (enemyNums_[stageNum] > preEnemyNums_[stageNum]) {
					globalVariables_->AddItem(g, item, enemyMoveVector_[stageNum][enemyNum]);
				}

				enemyMoveVector_[stageNum][enemyNum] = globalVariables_->GetIntValue(g, item);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kIsHealer)];

				if (preMaxStageNum_ < kMaxStageNum_) {
					globalVariables_->AddItem(g, item, enemyIsHealer_[stageNum][enemyNum]);
				}
				else if (enemyNums_.size() > preEnemyNums_.size()) {
					globalVariables_->AddItem(g, item, enemyIsHealer_[stageNum][enemyNum]);
				}
				else if (enemyNums_[stageNum] > preEnemyNums_[stageNum]) {
					globalVariables_->AddItem(g, item, enemyIsHealer_[stageNum][enemyNum]);
				}

				enemyIsHealer_[stageNum][enemyNum] = globalVariables_->GetIntValue(g, item);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kMoveRadius)];

				if (preMaxStageNum_ < kMaxStageNum_) {
					globalVariables_->AddItem(g, item, enemyMoveRadius_[stageNum][enemyNum]);
				}
				else if (enemyNums_.size() > preEnemyNums_.size()) {
					globalVariables_->AddItem(g, item, enemyMoveRadius_[stageNum][enemyNum]);
				}
				else if (enemyNums_[stageNum] > preEnemyNums_[stageNum]) {
					globalVariables_->AddItem(g, item, enemyMoveRadius_[stageNum][enemyNum]);
				}

				enemyMoveRadius_[stageNum][enemyNum] = globalVariables_->GetFloatValue(g, item);
			}

			for (int scaffoldingNum = 0; scaffoldingNum < scaffoldingNums_[stageNum][layerNum]; scaffoldingNum++) {

				if (scaffoldingPoses_[stageNum].size() <= scaffoldingNum) {
					scaffoldingPoses_[stageNum].push_back(Vector3{ -200.0f + 100.0f * scaffoldingNum, 100.0f,0.0f });
					scaffoldingScales_[stageNum].push_back(Vector2{ 100.0f, 10.0f });
				}

				std::string scaffolding = std::to_string(scaffoldingNum);

				std::string item = scaffoldingGruoopName_ + scaffolding + scaffoldingParameter[static_cast<uint16_t>(ScaffoldingParameter::kPos)];

				if (preMaxStageNum_ < kMaxStageNum_) {
					globalVariables_->AddItem(g, item, scaffoldingPoses_[stageNum][scaffoldingNum]);
				}
				else if (scaffoldingNums_.size() > preScaffoldingNums_.size()) {
					globalVariables_->AddItem(g, item, scaffoldingPoses_[stageNum][scaffoldingNum]);
				}
				else if (scaffoldingNums_[stageNum] != preScaffoldingNums_[stageNum]) {
					globalVariables_->AddItem(g, item, scaffoldingPoses_[stageNum][scaffoldingNum]);
				}
				scaffoldingPoses_[stageNum][scaffoldingNum] = globalVariables_->GetVector3Value(g, item);

				item = scaffoldingGruoopName_ + scaffolding + scaffoldingParameter[static_cast<uint16_t>(ScaffoldingParameter::kScale)];

				if (preMaxStageNum_ < kMaxStageNum_) {
					globalVariables_->AddItem(g, item, scaffoldingScales_[stageNum][scaffoldingNum]);
				}
				else if (scaffoldingNums_.size() > preScaffoldingNums_.size()) {
					globalVariables_->AddItem(g, item, scaffoldingScales_[stageNum][scaffoldingNum]);
				}
				else if (scaffoldingNums_[stageNum] != preScaffoldingNums_[stageNum]) {
					globalVariables_->AddItem(g, item, scaffoldingScales_[stageNum][scaffoldingNum]);
				}
				scaffoldingScales_[stageNum][scaffoldingNum] = globalVariables_->GetVector2Value(g, item);
			}


		}


	}

}

void GameScene::CreatShockWave(const Vector3& pos, float highest, float y) {
	shockWaves_.push_back(std::make_unique<ShockWave>(pos, highest, y));

	if (highest >= ShockWave::GetHighCriteria(static_cast<int>(ShockWave::Size::kMiddle))) {
		isShack_ = true;
		shackTime_ = 0.7f;
		shackMax_ = 30.0f;
		shackCount_ = 0.0f;
		shackPos_ = { 0.0f,0.0f,0.0f };
	}
	else if (highest >= ShockWave::GetHighCriteria(static_cast<int>(ShockWave::Size::kSmall))) {
		isShack_ = true;
		shackTime_ = 0.4f;
		shackMax_ = 18.0f;
		shackCount_ = 0.0f;
		shackPos_ = { 0.0f,0.0f,0.0f };
	}
}

void GameScene::EnemyGeneration() {

	if (enemies_.size() < enemyNums_[stage_][layer_->GetNowLayer()]) {

		int size = static_cast<int>(enemies_.size());

		for (int num = 0; num < enemyNums_[stage_][layer_->GetNowLayer()]; num++) {
			if (num >= size) {

				enemies_.push_back(std::make_unique<Enemy>(enemyType_[stage_][num], enemyPoses_[stage_][num], layer_->GetHighestPosY(),
					enemyMoveVector_[stage_][num],enemyIsHealer_[stage_][num],enemyMoveRadius_[stage_][num]));

			}
		}
	}
}

void GameScene::EnemeiesClear() {

	enemies_.clear();
}

void GameScene::SetEnemyParametar() {

	int i = 0;

	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		if (enemyNums_[stage_][layer_->GetNowLayer()] == i) {
			break;
		}
		enemy->SetParametar(enemyType_[stage_][i], enemyPoses_[stage_][i], layer_->GetHighestPosY(),
			enemyMoveVector_[stage_][i], enemyIsHealer_[stage_][i], enemyMoveRadius_[stage_][i]);
		i++;
	}
}

void GameScene::ScaffoldingGeneration() {
	if (scaffoldings_.size() < scaffoldingNums_[stage_][layer_->GetNowLayer()]) {

		int size = static_cast<int>(scaffoldings_.size());

		for (int num = 0; num < scaffoldingNums_[stage_][layer_->GetNowLayer()]; num++) {
			if (num >= size) {
				scaffoldings_.push_back(std::make_unique<Scaffolding>(scaffoldingPoses_[stage_][num], scaffoldingScales_[stage_][num]));
			}
		}
	}
}

void GameScene::SetScaffoldingParametar() {
	int i = 0;

	for (std::unique_ptr<Scaffolding>& scaffolding : scaffoldings_) {
		if (scaffoldingNums_[stage_][layer_->GetNowLayer()] == i) {
			break;
		}
		scaffolding->SetParametar(scaffoldingPoses_[stage_][i], scaffoldingScales_[stage_][i], camera2D_.get());
		i++;
	}
}

void GameScene::CreateLayer() {

	if (stage_ != preStage_ || kLayerNums_[stage_] != preLayerNums_[stage_]) {

		layer_->Initialize(kLayerNums_[stage_], kLayerHitPoints_[stage_]);
	}

}

void GameScene::SetLayerParametar() {

	layer_->SetParametar(kLayerHitPoints_[stage_]);

}

void GameScene::DeleteShockWave() {
	shockWaves_.remove_if([](const std::unique_ptr<ShockWave>& shockWave) {
		return shockWave->GetDeleteFlag();
		});
}

void GameScene::Collision() {

	int i = 0;

	for (std::unique_ptr<Enemy>& enemy : enemies_) {

		if (enemyNums_[stage_][layer_->GetNowLayer()] == i) {
			break;
		}
		i++;

		const Texture2D* enemyTex = enemy->GetTex();

		player_->FallingCollision(enemy.get());

		enemy->CollisionPlayer(player_.get());


		if (shockWaves_.size() != 0 && enemy->GetStatus() != Enemy::Status::kDeath && enemy->GetStatus() != Enemy::Status::kGeneration) {

			if (enemy->GetStatus() == Enemy::Status::kNormal && enemy->GetType() == Enemy::Type::kFly) {

			}
			else {
				for (std::unique_ptr<ShockWave>& shockWave : shockWaves_) {
					const std::list<std::unique_ptr<Texture2D>>& shockWaveTextures = shockWave->GetTextures();

					for (const std::unique_ptr<Texture2D>& shockWaveTex : shockWaveTextures) {

						if (shockWaveTex->Collision(*enemyTex)) {
							enemy->StatusRequest(Enemy::Status::kDeath);
						}
					}
				}
			}
		}

		int j = 0;

		for (std::unique_ptr<Enemy>& enemy2 : enemies_) {
			if (enemyNums_[stage_][layer_->GetNowLayer()] == j) {
				break;
			}
			j++;

			if (i != j) {
				enemy->CollisionEnemy(enemy2.get());
			}
		}
	}

	i = 0;

	for (const std::unique_ptr<Scaffolding>& scaff : scaffoldings_) {
		if (scaffoldingNums_[stage_][layer_->GetNowLayer()] == i) {
			break;
		}
		i++;

		player_->CollisionScaffolding(scaff->GetTex());
	}
}

void GameScene::ShackUpdate()
{
	if (isShack_) {

		shackCount_ += FrameInfo::GetInstance()->GetDelta();

		float length = shackMax_ * (1.0f - shackCount_ / shackTime_);

		shackPos_.x = Lamb::Random(-length / 2.0f, length / 2.0f);
		shackPos_.y = Lamb::Random(-length / 2.0f, length / 2.0f);

		if (shackCount_ >= shackTime_) {
			shackCount_ = shackTime_;
			isShack_ = false;
			shackPos_ = { 0.0f,0.0f,0.0f };
		}
	}

}

void GameScene::Update() {
	auto nowTime = std::chrono::steady_clock::now();

	if (!pause_->isActive_) {
		/*float y = player_->GetTex()->pos.y;
		if (cameraLocalPos_.y + layer_->GetHighestPosY() > player_->GetTex()->pos.y) {
			y = cameraLocalPos_.y + layer_->GetHighestPosY();
		}
		if (player_->GetVelocity().y < 0) {
			camera2D_->pos.y = camera2D_->pos.y + (y - camera2D_->pos.y) * 0.1f;
		}
		else {
			camera2D_->pos.y = camera2D_->pos.y + (y - camera2D_->pos.y) * 0.05f;
		}*/
		if (cameraLocalPos_.y + layer_->GetHighestPosY() <= player_->GetTex()->pos.y) {
			camera2D_->pos.y = player_->GetTex()->pos.y;
		}
		else {
			camera2D_->pos.y = cameraLocalPos_.y + layer_->GetHighestPosY();
		}
		if (isShack_) {
			camera2D_->pos += shackPos_;
		}
		else {
			camera2D_->pos.x = 0.0f;
		}

		camera2D_->Update();

#ifdef _DEBUG

		preStage_ = stage_;
		preMaxStageNum_ = kMaxStageNum_;
		preEnemyNums_ = enemyNums_;
		preScaffoldingNums_ = scaffoldingNums_;
		preLayerNums_ = kLayerNums_;

		ImGui::Begin("PlayScene");
		ImGui::SliderInt("NowStage 0 = 1stage", &stage_, 0, kMaxStageNum_ - 1);
		ImGui::End();

		globalVariables_->Update();

		//SetGlobalVariable();
		Enemy::GlobalVariablesUpdate();
		ShockWave::GlobalVariablesUpdate();
		ShockWave::ApplyGlobalVariable();
		Layer::GlobalVariablesUpdate();

#endif // _DEBUG

		ApplyGlobalVariable();

		if (kMaxStageNum_ <= 0) {
			kMaxStageNum_ = 1;
		}

		EnemyGeneration();
		ScaffoldingGeneration();

#ifdef _DEBUG

		SetEnemyParametar();
		SetScaffoldingParametar();
		CreateLayer();
		SetLayerParametar();

#endif // _DEBUG

		background_->Update();

		player_->Update(layer_->GetHighestPosY(), camera2D_.get());

		int i = 0;

		for (std::unique_ptr<Enemy>& enemy : enemies_) {
			if (enemyNums_[stage_][layer_->GetNowLayer()] == i) {
				break;
			}
			enemy->Update(layer_.get(), layer_->GetHighestPosY(), camera2D_.get());
			i++;
		}
		for (std::unique_ptr<ShockWave>& shockWave : shockWaves_) {
			shockWave->Update();
		}

		DeleteShockWave();

		Collision();

		ShackUpdate();

		layer_->Update(camera2D_.get());

		if (layer_->GetClearFlag().OnEnter()) {
			bgm_->Stop();
			playTime_ += std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime_);

			auto result = new ResultScene{};
			assert(result);
			result->SetClearTime(playTime_);
			result->SetStageNumber(stage_ + 1);
			sceneManager_->SceneChange(result);
		}
	}

	if (input_->GetGamepad()->GetButton(Gamepad::Button::A)) {
		aButtonHud_->uvPibot.x = 0.5f;
	}
	else {
		aButtonHud_->uvPibot.x = 0.0f;
	}
	if (input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) < -0.3f ||
		input_->GetGamepad()->GetButton(Gamepad::Button::LEFT)) {
		stickHud_->uvPibot.x = 1.0f / 3.0f;
	}
	else if (input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) > 0.3f||
		input_->GetGamepad()->GetButton(Gamepad::Button::RIGHT)) {
		stickHud_->uvPibot.x = 2.0f / 3.0f;
	}
	else {
		stickHud_->uvPibot.x = 0.0f;
	}

	if (input_->GetGamepad()->GetButton(Gamepad::Button::START)) {
		puaseHud_->uvPibot.x = 0.5f;
	}
	else {
		puaseHud_->uvPibot.x = 0.0f;
	}

	if (input_->GetKey()->GetKey(DIK_SPACE)) {
		spaceHud_->uvPibot.x = 0.5f;
	}
	else {
		spaceHud_->uvPibot.x = 0.0f;
	}
	if (input_->GetKey()->GetKey(DIK_RIGHT)||
		input_->GetKey()->GetKey(DIK_D)
		) {
		rightKeyHud_->uvPibot.x = 0.5f;
	}
	else {
		rightKeyHud_->uvPibot.x = 0.0f;
	}
	if (input_->GetKey()->GetKey(DIK_LEFT)||
		input_->GetKey()->GetKey(DIK_A)) {
		leftKeyHud_->uvPibot.x = 0.5f;
	}
	else {
		leftKeyHud_->uvPibot.x = 0.0f;
	}
	if (input_->GetKey()->GetKey(DIK_TAB)) {
		puaseKeyHud_->uvPibot.x = 0.5f;
	}
	else {
		puaseKeyHud_->uvPibot.x = 0.0f;
	}

	aButtonHud_->Update();
	stickHud_->Update();
	puaseHud_->Update();

	spaceHud_->Update();
	rightKeyHud_->Update();
	leftKeyHud_->Update();
	puaseKeyHud_->Update();



	if (!WindowFactory::GetInstance()->IsThisWindowaActive()) {
		pause_->isActive_ = true;
	}

	pause_->ActiveUpdate();
	pause_->SceneChange(stage_ + 1);
	if (input_->GetKey()->Pushed(DIK_TAB) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::START)
		) 
	{
		pause_->isActive_ = !pause_->isActive_;
		if (pause_->isActive_) {
			bgm_->SetAudio(0.025f);
			playTime_ += std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime_);
		}
		else {
			bgm_->SetAudio(0.1f);
			startTime_ = nowTime;
		}
	}

	backGroundParticle_->Update();
}

void GameScene::Draw() {
	camera_->Update();

	background_->Draw2D(camera2D_->GetViewOthographics());

	backGroundParticle_->Draw(Vector3::kZero, camera2D_->GetViewOthographics());

	//layer_->Draw2DFar(camera2D_->GetViewOthographics());

	player_->Draw(camera2D_->GetViewProjection(), camera2D_->GetPos());

	layer_->Draw(camera2D_->GetViewProjection(), camera2D_->GetPos());
	layer_->ParticleDraw(camera2D_->GetViewOthographics());

	int i = 0;

	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		if (enemyNums_[stage_][layer_->GetNowLayer()] == i) {
			break;
		}
		enemy->Draw(camera2D_->GetViewProjection(), camera2D_->GetPos());
		enemy->DrawParticle(camera2D_->GetViewOthographics());

		//enemy->Draw2D(camera2D_->GetViewOthographics());
		i++;
	}

	i = 0;
	for (std::unique_ptr<Scaffolding>& scaffolding : scaffoldings_) {
		if (scaffoldingNums_[stage_][layer_->GetNowLayer()] == i) {
			break;
		}
		scaffolding->Draw(camera2D_->GetViewProjection(), camera2D_->GetPos());
		//scaffolding->Draw2D(camera2D_->GetViewOthographics());
		i++;
	}
	for (std::unique_ptr<ShockWave>& shockWave : shockWaves_) {
		shockWave->Draw2D(camera2D_->GetViewOthographics());
	}

	layer_->Draw2DNear(camera2D_->GetViewOthographics());

	//player_->Draw2D(camera2D_->GetViewOthographics());

	if (sceneManager_->GetIsPad()) {
		aButtonHud_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
		stickHud_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
		puaseHud_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
	}
	else {
		spaceHud_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
		rightKeyHud_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
		leftKeyHud_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
		puaseKeyHud_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false);
	}
	pause_->Draw();
}