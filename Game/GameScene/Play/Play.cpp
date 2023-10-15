#include "Play.h"

#include "externals/imgui/imgui.h"
#include <filesystem>
#include <fstream>

Play::Play() {

	Enemy::GlobalVariablesLoad();
	ShockWave::GlobalVariablesLoad();
	Layer::GlobalVariablesLoad();

	camera2D_ = std::make_unique<Camera>(Camera::Type::Othographic);
	camera3D_ = std::make_unique<Camera>(Camera::Type::Projecction);

	player_ = std::make_unique<Player>();
	player_->SetPlayScene(this);

	stage_ = 0;
	preStage_ = stage_;

	kMaxStageNum_ = 1;

	enemyNums_.push_back(std::vector<int>());

	scaffoldingNums_.push_back(std::vector<int>());

	ShockWave::SetGlobalVariable();

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


}

void Play::Initialize() {

	shockWaves_.clear();

	layer_->Initialize(kLayerNums_[stage_], kLayerHitPoints_[stage_]);

	player_->Initialize();

	EnemeiesClear();

	EnemyGeneration();

	scaffoldings_.clear();

	ScaffoldingGeneration();

}

void Play::InitializeGlobalVariable() {

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
			}

			if (scaffoldingPoses_.size() <= stageNum) {
				scaffoldingPoses_.push_back(std::vector<Vector3>());
				scaffoldingScales_.push_back(std::vector<Vector2>());
			}


			for (int enemyNum = 0; enemyNum < enemyNums_[stageNum][layerNum]; enemyNum++) {

				if (enemyPoses_[stageNum].size() <= enemyNum) {
					enemyPoses_[stageNum].push_back(Vector3{ -200.0f + 100.0f * enemyNum, 300.0f,0.0f });
					enemyType_[stageNum].push_back(0);
				}

				std::string enemy = std::to_string(enemyNum);

				std::string item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kPos)];

				//globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
				enemyPoses_[stageNum][enemyNum] = globalVariables_->GetVector3Value(g, item);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kType)];

				enemyType_[stageNum][enemyNum] = globalVariables_->GetIntValue(g, item);
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

void Play::SetGlobalVariable() {

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
			}

			if (scaffoldingPoses_.size() <= stageNum) {
				scaffoldingPoses_.push_back(std::vector<Vector3>());
				scaffoldingScales_.push_back(std::vector<Vector2>());
			}

			for (int enemyNum = 0; enemyNum < enemyNums_[stageNum][layerNum]; enemyNum++) {

				if (enemyPoses_[stageNum].size() <= enemyNum) {
					enemyPoses_[stageNum].push_back(Vector3{ -200.0f + 100.0f * enemyNum, 300.0f,0.0f });
					enemyType_[stageNum].push_back(0);
				}

				std::string enemy = std::to_string(enemyNum);

				std::string item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kPos)];

				globalVariables_->AddItem(g, item, enemyPoses_[stageNum][enemyNum]);

				item = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kType)];

				globalVariables_->AddItem(g, item, enemyType_[stageNum][enemyNum]);
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

void Play::ApplyGlobalVariable() {

	kMaxStageNum_ = globalVariables_->GetIntValue("Game", "kMaxStageNum");

	camera2D_->pos = globalVariables_->GetVector3Value("Game", "Camera2DPos");

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

void Play::CreatShockWave(const Vector3& pos, float highest, float y) {
	shockWaves_.push_back(std::make_unique<ShockWave>(pos, highest, y));
}

void Play::EnemyGeneration() {

	if (enemies_.size() < enemyNums_[stage_][layer_->GetNowLayer()]) {

		int size = static_cast<int>(enemies_.size());

		for (int num = 0; num < enemyNums_[stage_][layer_->GetNowLayer()]; num++) {
			if (num >= size) {

				enemies_.push_back(std::make_unique<Enemy>(enemyType_[stage_][num], enemyPoses_[stage_][num], layer_->GetHighestPosY()));

			}
		}
	}
}

void Play::EnemeiesClear() {

	enemies_.clear();
}

void Play::SetEnemyParametar() {

	int i = 0;

	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		if (enemyNums_[stage_][layer_->GetNowLayer()] == i) {
			break;
		}
		enemy->SetParametar(enemyType_[stage_][i], enemyPoses_[stage_][i], layer_->GetHighestPosY());
		i++;
	}
}

void Play::ScaffoldingGeneration() {
	if (scaffoldings_.size() < scaffoldingNums_[stage_][layer_->GetNowLayer()]) {

		int size = static_cast<int>(scaffoldings_.size());

		for (int num = 0; num < scaffoldingNums_[stage_][layer_->GetNowLayer()]; num++) {
			if (num >= size) {
				scaffoldings_.push_back(std::make_unique<Scaffolding>(scaffoldingPoses_[stage_][num], scaffoldingScales_[stage_][num]));
			}
		}
	}
}

void Play::SetScaffoldingParametar() {
	int i = 0;

	for (std::unique_ptr<Scaffolding>& scaffolding : scaffoldings_) {
		if (scaffoldingNums_[stage_][layer_->GetNowLayer()] == i) {
			break;
		}
		scaffolding->SetParametar(scaffoldingPoses_[stage_][i], scaffoldingScales_[stage_][i]);
		i++;
	}
}

void Play::CreateLayer() {

	if (stage_ != preStage_ || kLayerNums_[stage_] != preLayerNums_[stage_]) {

		layer_->Initialize(kLayerNums_[stage_], kLayerHitPoints_[stage_]);
	}

}

void Play::SetLayerParametar() {

	layer_->SetParametar(kLayerHitPoints_[stage_]);

}

void Play::DeleteShockWave() {
	shockWaves_.remove_if([](const std::unique_ptr<ShockWave>& shockWave) {
		return shockWave->GetDeleteFlag();
	});
}

void Play::Collision() {

	const Texture2D* playerTex = player_->GetTex();
	const Vector3& playerVelocity = player_->GetVelocity();

	int i = 0;

	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		
		if (enemyNums_[stage_][layer_->GetNowLayer()] == i) {
			break;
		}
		i++;

		const Texture2D* enemyTex = enemy->GetTex();
		
		// エネミーが通常時の時、プレイヤーが通常時ヒップドロップ時の時のみ
		if (enemy->GetStatus() == Enemy::Status::kNormal && 
			(player_->GetStatus() == Player::Status::kNormal || player_->GetStatus() == Player::Status::kHipDrop)) {

			if (playerTex->Collision(*enemyTex)) {

				// 雑だけど速度が0以下でプレイヤーが踏んでる判定にしている
				if (playerVelocity.y < 0.0f) {
					// 敵を踏んだ時
					enemy->StatusRequest(Enemy::Status::kFalling);
					player_->EnemyStep(true);
				}
				else {
					// 敵に踏まれた時
					player_->EnemyStep(false);
				}
			}
		}
		else if (enemy->GetStatus() == Enemy::Status::kFaint && shockWaves_.size() != 0) {
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

	i = 0;

	for (const std::unique_ptr<Scaffolding>& scaff : scaffoldings_) {
		if (scaffoldingNums_[stage_][layer_->GetNowLayer()] == i) {
			break;
		}
		i++;

		player_->CollisionScaffolding(scaff->GetTex());
	}
}


void Play::Update() {

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

	player_->Update(layer_->GetHighestPosY());

	int i = 0;

	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		if (enemyNums_[stage_][layer_->GetNowLayer()] == i) {
			break;
		}
		enemy->Update(layer_.get(), layer_->GetHighestPosY());
		i++;
	}
	for (std::unique_ptr<ShockWave>& shockWave : shockWaves_) {
		shockWave->Update();
	}

	DeleteShockWave();

	Collision();

	layer_->Update();

	camera2D_->Update();
	camera3D_->Update();
}

void Play::Draw() {

	background_->Draw2D(camera2D_->GetViewOthographics());

	layer_->Draw2D(camera2D_->GetViewOthographics());

	int i = 0;

	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		if (enemyNums_[stage_][layer_->GetNowLayer()] == i) {
			break;
		}
		enemy->Draw2D(camera2D_->GetViewOthographics());
		i++;
	}
	i = 0;
	for (std::unique_ptr<Scaffolding>& scaffolding : scaffoldings_) {
		if (scaffoldingNums_[stage_][layer_->GetNowLayer()] == i) {
			break;
		}
		scaffolding->Draw2D(camera2D_->GetViewOthographics());
		i++;
	}
	for (std::unique_ptr<ShockWave>& shockWave : shockWaves_) {
		shockWave->Draw2D(camera2D_->GetViewOthographics());
	}

	player_->Draw2D(camera2D_->GetViewOthographics());
}

