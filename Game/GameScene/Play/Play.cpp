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

	stageNum_ = 0;

	kMaxStageNum_ = 1;

	enemyNums_.push_back(1);

	preMaxStageNum_ = kMaxStageNum_;
	preEnemyNums_ = enemyNums_;
	preLayerNums_ = kLayerNums_;

	globalVariables_ = std::make_unique<GlobalVariables>();

	InitializeGlobalVariable();

	SetGlobalVariable();

	InitializeGlobalVariable();


	layer_ = std::make_unique<Layer>(kLayerNums_[stageNum_], kLayerHitPoints_[stageNum_]);


}

void Play::Initialize() {

	shockWaves_.clear();

	layer_->Initialize(kLayerNums_[stageNum_], kLayerHitPoints_[stageNum_]);

	player_->Initialize();

	EnemeiesClear();

	EnemyGeneration();

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

		if (enemyNums_.size() <= stageNum) {
			enemyNums_.push_back(1);
		}

		//globalVariables_->AddItem(g, "EnemyNum", enemyNums_[stageNum]);
		enemyNums_[stageNum] = globalVariables_->GetIntValue(g, "EnemyNum");

		if (enemyPoses_.size() <= stageNum) {
			enemyPoses_.push_back(std::vector<Vector3>());
		}

		for (int enemyNum = 0; enemyNum < enemyNums_[stageNum]; enemyNum++) {

			if (enemyPoses_[stageNum].size() <= enemyNum) {
				enemyPoses_[stageNum].push_back(Vector3{ -200.0f + 100.0f * enemyNum, 300.0f,0.0f });
			}

			std::string enemy = std::to_string(enemyNum);

			std::string i = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kPos)];

			//globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
			enemyPoses_[stageNum][enemyNum] = globalVariables_->GetVector3Value(g, i);
		}


		if (kLayerNums_.size() <= stageNum) {
			kLayerNums_.push_back(1);
		}

		kLayerNums_[stageNum] = globalVariables_->GetIntValue(g, "LayerNum");

		if (kLayerHitPoints_.size() <= stageNum) {
			kLayerHitPoints_.push_back(std::vector<int>());
		}

		for (int layerNum = 0; layerNum < kLayerNums_[stageNum]; layerNum++) {

			if (kLayerHitPoints_[stageNum].size() <= layerNum) {
				kLayerHitPoints_[stageNum].push_back(1);
			}

			std::string layer = std::to_string(layerNum);

			std::string i = layerGruoopName_ + layer + layerParameter[static_cast<uint16_t>(LayerParameter::kHP)];

			kLayerHitPoints_[stageNum][layerNum] = globalVariables_->GetIntValue(g, i);
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

		if (enemyNums_.size() <= stageNum) {
			enemyNums_.push_back(1);
		}

		globalVariables_->AddItem(g, "EnemyNum", enemyNums_[stageNum]);

		if (enemyPoses_.size() <= stageNum) {
			enemyPoses_.push_back(std::vector<Vector3>());
		}

		for (int enemyNum = 0; enemyNum < enemyNums_[stageNum]; enemyNum++) {

			if (enemyPoses_[stageNum].size() <= enemyNum) {
				enemyPoses_[stageNum].push_back(Vector3{ -200.0f + 100.0f * enemyNum, 300.0f,0.0f });
			}

			std::string enemy = std::to_string(enemyNum);

			std::string i = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kPos)];

			globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
		}

		if (kLayerNums_.size() <= stageNum) {
			kLayerNums_.push_back(1);
		}

		globalVariables_->AddItem(g, "LayerNum", kLayerNums_[stageNum]);

		if (kLayerHitPoints_.size() <= stageNum) {
			kLayerHitPoints_.push_back(std::vector<int>());
		}

		for (int layerNum = 0; layerNum < kLayerNums_[stageNum]; layerNum++) {

			if (kLayerHitPoints_[stageNum].size() <= layerNum) {
				kLayerHitPoints_[stageNum].push_back(1);
			}

			std::string layer = std::to_string(layerNum);

			std::string i = layerGruoopName_ + layer + layerParameter[static_cast<uint16_t>(LayerParameter::kHP)];

			globalVariables_->AddItem(g, i, kLayerHitPoints_[stageNum][layerNum]);
		}
	}

	//globalVariables_->LoadFile(groupName_);m
	ApplyGlobalVariable();
}

void Play::ApplyGlobalVariable() {

	kMaxStageNum_= globalVariables_->GetIntValue("Game", "kMaxStageNum");

	camera2D_->pos = globalVariables_->GetVector3Value("Game", "Camera2DPos");

	for (int stageNum = 0; stageNum < kMaxStageNum_; stageNum++) {

		std::string stage = std::to_string(stageNum);

		std::string g = stageGruopName_ + stage;

		if (enemyNums_.size() <= stageNum) {
			enemyNums_.push_back(1);
		}

		if (preMaxStageNum_ < kMaxStageNum_) {
			globalVariables_->AddItem(g, "EnemyNum", enemyNums_[stageNum]);
		}
		else if (enemyNums_.size() > preEnemyNums_.size()) {
			globalVariables_->AddItem(g, "EnemyNum", enemyNums_[stageNum]);
		}
		else if (enemyNums_[stageNum] != preEnemyNums_[stageNum]) {
			globalVariables_->AddItem(g, "EnemyNum", enemyNums_[stageNum]);
		}
		//globalVariables_->AddItem(g, "EnemyNum", enemyNums_[stageNum]);
		enemyNums_[stageNum] = globalVariables_->GetIntValue(g, "EnemyNum");

		if (enemyPoses_.size() <= stageNum) {
			enemyPoses_.push_back(std::vector<Vector3>());
		}

		for (int enemyNum = 0; enemyNum < enemyNums_[stageNum]; enemyNum++) {

			if (enemyPoses_[stageNum].size() <= enemyNum) {
				enemyPoses_[stageNum].push_back(Vector3{ -200.0f + 100.0f * enemyNum, 300.0f,0.0f });
			}

			std::string enemy = std::to_string(enemyNum);

			std::string i = enemyGruoopName_ + enemy + enemyParameter[static_cast<uint16_t>(EnemyParameter::kPos)];

			if (preMaxStageNum_ < kMaxStageNum_) {
				globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
			}
			else if (enemyNums_.size() > preEnemyNums_.size()) {
				globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
			}
			else if (enemyNums_[stageNum] > preEnemyNums_[stageNum]) {
				globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
			}
			//globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
			enemyPoses_[stageNum][enemyNum] = globalVariables_->GetVector3Value(g, i);
		}

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
		}
	}

}

void Play::CreatShockWave(const Vector3& pos, float highest) {
	shockWaves_.push_back(std::make_unique<ShockWave>(pos, highest));
}

void Play::EnemyGeneration() {

	if (enemies_.size() < enemyNums_[stageNum_]) {

		int size = static_cast<int>(enemies_.size());

		for (int num = 0; num < enemyNums_[stageNum_]; num++) {
			if (num >= size) {

				enemies_.push_back(std::make_unique<Enemy>(enemyPoses_[stageNum_][num]));

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
		if (enemyNums_[stageNum_] == i) {
			break;
		}
		enemy->SetParametar(enemyPoses_[stageNum_][i]);
		i++;
	}
}

void Play::CreateLayer() {

	if (kLayerNums_[stageNum_] != preLayerNums_[stageNum_]) {

		layer_->Initialize(kLayerNums_[stageNum_], kLayerHitPoints_[stageNum_]);
	}

}

void Play::SetLayerParametar() {

	layer_->SetParametar(kLayerHitPoints_[stageNum_]);

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
		
		if (enemyNums_[stageNum_] == i) {
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
}


void Play::Update() {

#ifdef _DEBUG

	preMaxStageNum_ = kMaxStageNum_;
	preEnemyNums_ = enemyNums_;
	preLayerNums_ = kLayerNums_;

	ImGui::Begin("PlayScene");
	ImGui::SliderInt("NowStage 0 = 1stage", &stageNum_, 0, kMaxStageNum_ - 1);
	ImGui::End();

	globalVariables_->Update();

	//SetGlobalVariable();
	Enemy::GlobalVariablesUpdate();
	ShockWave::GlobalVariablesUpdate();
	Layer::GlobalVariablesUpdate();

#endif // _DEBUG

	ApplyGlobalVariable();

	if (kMaxStageNum_ <= 0) {
		kMaxStageNum_ = 1;
	}

#ifdef _DEBUG

	EnemyGeneration();
	SetEnemyParametar();
	CreateLayer();
	SetLayerParametar();

#endif // _DEBUG
	player_->Update();

	int i = 0;

	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		if (enemyNums_[stageNum_] == i) {
			break;
		}
		enemy->Update();
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

	layer_->Draw2D(camera2D_->GetViewOthographics());

	int i = 0;

	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		if (enemyNums_[stageNum_] == i) {
			break;
		}
		enemy->Draw2D(camera2D_->GetViewOthographics());
		i++;
	}
	for (std::unique_ptr<ShockWave>& shockWave : shockWaves_) {
		shockWave->Draw2D(camera2D_->GetViewOthographics());
	}

	player_->Draw2D(camera2D_->GetViewOthographics());
}


