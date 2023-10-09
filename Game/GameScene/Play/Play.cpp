#include "Play.h"

#include "externals/imgui/imgui.h"
#include <filesystem>
#include <fstream>

Play::Play() {

	camera2D_ = std::make_unique<Camera>(Camera::Type::Othographic);
	camera3D_ = std::make_unique<Camera>(Camera::Type::Projecction);

	player_ = std::make_unique<Player>();
	player_->SetPlayScene(this);

	stageNum_ = 0;

	kMaxStageNum_ = 1;

	enemyNums_.push_back(1);

	preEnemyNums_ = enemyNums_;
	preMaxStageNum_ = kMaxStageNum_;

	isFile_ = true;

	globalVariables_ = std::make_unique<GlobalVariables>();

	SetGlobalVariable();

	InitializeGlobalVariable();
}

void Play::Initialize() {

	EnemeiesClear();

	EnemyGeneration();

	player_->Initialize();

}

void Play::InitializeGlobalVariable() {

	globalVariables_->LoadFile("Game");

	kMaxStageNum_ = globalVariables_->GetIntValue("Game", "kMaxStageNum");

	for (int stageNum = 0; stageNum < kMaxStageNum_; stageNum++) {

		isFile_ = true;

		//const char stage = (char)stageNum;

		std::string stage = std::to_string(stageNum);

		std::string g = std::string(stageGruopName_) + stage;

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

			std::string i = std::string(enemyGruoopName_) + enemy + std::string(enemyParameter[static_cast<uint16_t>(EnemyParameter::kPos)]);

			//globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
			enemyPoses_[stageNum][enemyNum] = globalVariables_->GetVector3Value(g, i);
		}
	}

}

void Play::SetGlobalVariable() {

	globalVariables_->CreateGroup("Game");

	globalVariables_->AddItem("Game", "kMaxStageNum", kMaxStageNum_);

	for (int stageNum = 0; stageNum < kMaxStageNum_; stageNum++) {

		std::string stage = std::to_string(stageNum);

		std::string g = std::string(stageGruopName_) + stage;

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

			std::string i = std::string(enemyGruoopName_) + enemy + std::string(enemyParameter[static_cast<uint16_t>(EnemyParameter::kPos)]);

			globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
		}
	}

	//globalVariables_->LoadFile(groupName_);m
	ApplyGlobalVariable();
}

void Play::ApplyGlobalVariable() {

	kMaxStageNum_= globalVariables_->GetIntValue("Game", "kMaxStageNum");

	for (int stageNum = 0; stageNum < kMaxStageNum_; stageNum++) {

		std::string stage = std::to_string(stageNum);

		std::string g = std::string(stageGruopName_) + stage;

		if (enemyNums_.size() <= stageNum) {
			enemyNums_.push_back(1);
		}

		if (preMaxStageNum_ < kMaxStageNum_) {
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

			std::string i = std::string(enemyGruoopName_) + enemy + std::string(enemyParameter[static_cast<uint16_t>(EnemyParameter::kPos)]);

			if (preMaxStageNum_ < kMaxStageNum_) {
				globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
			}
			else if (enemyNums_[stageNum] > preEnemyNums_[stageNum]) {
				globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
			}
			//globalVariables_->AddItem(g, i, enemyPoses_[stageNum][enemyNum]);
			enemyPoses_[stageNum][enemyNum] = globalVariables_->GetVector3Value(g, i);
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

	preEnemyNums_ = enemyNums_;
	preMaxStageNum_ = kMaxStageNum_;

	ImGui::Begin("PlayScene");
	ImGui::SliderInt("NowStage 0 = 1stage", &stageNum_, 0, kMaxStageNum_ - 1);
	ImGui::End();

	globalVariables_->Update();

	//SetGlobalVariable();
	Enemy::GlobalVariablesUpdate();
	ShockWave::GlobalVariablesUpdate();

#endif // _DEBUG

	ApplyGlobalVariable();

	if (kMaxStageNum_ <= 0) {
		kMaxStageNum_ = 1;
	}

#ifdef _DEBUG
	EnemyGeneration();
	SetEnemyParametar();

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

	// これと押すとメモリリーク起きる
	DeleteShockWave();

	Collision();

	camera2D_->Update();
	camera3D_->Update();
}

void Play::Draw() {

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


