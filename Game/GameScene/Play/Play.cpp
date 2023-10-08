#include "Play.h"

Play::Play() {

	camera2D_ = std::make_unique<Camera>(Camera::Type::Othographic);
	camera3D_ = std::make_unique<Camera>(Camera::Type::Projecction);

	player_ = std::make_unique<Player>();
	player_->SetPlayScene(this);
}

void Play::Initialize() {

	EnemeiesClear();

	EnemyGeneration();

	player_->Initialize();

}

void Play::CreatShockWave(const Vector3& pos, float highest) {
	shockWaves_.push_back(std::make_shared<ShockWave>(pos, highest));
}

void Play::EnemyGeneration() {

	enemies_.push_back(std::make_shared<Enemy>(Vector3(100.0f, 100.0f, 0.0f), 40.0f));

	enemies_.push_back(std::make_shared<Enemy>(Vector3(-80.0f, 160.0f, 0.0f), 40.0f));

}

void Play::EnemeiesClear() {

	enemies_.remove_if([](std::shared_ptr<Enemy> enemy) {
		enemy.reset();
		return true;
	});
}

void Play::DeleteShockWave() {

	shockWaves_.remove_if([](std::shared_ptr<ShockWave> shockWave) {
		if (shockWave->GetDeleteFlag()) {
			shockWave->Finalize();
			shockWave.reset();
			return true;
		}
		return false;
	});
}

void Play::Collision() {

	const Texture2D* playerTex = player_->GetTex();
	const Vector3& playerVelocity = player_->GetVelocity();

	for (std::shared_ptr<Enemy> enemy : enemies_) {
		
		const Texture2D* enemyTex = enemy->GetTex();
		
		// エネミーが通常時の時、プレイヤーが通常時ヒップドロップ時の時のみ
		if (enemy->GetStatus() == Enemy::Status::kNormal && 
			(player_->GetStatus() == Player::Status::kNormal || player_->GetStatus() == Player::Status::kHipDrop)) {

			if (playerTex->Colision(*enemyTex)) {

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
			for (std::shared_ptr<ShockWave> shockWave : shockWaves_) {
				const std::list<std::shared_ptr<Texture2D>>& shockWaveTextures = shockWave->GetTextures();

				for (std::shared_ptr<Texture2D> shockWaveTex : shockWaveTextures) {

					if (shockWaveTex->Colision(*enemyTex)) {
						enemy->StatusRequest(Enemy::Status::kDeath);
					}
				}
			}
		}
	}
}


void Play::Update() {

	Enemy::GlobalVariablesUpdate();
	ShockWave::GlobalVariablesUpdate();

	player_->Update();
	for (std::shared_ptr<Enemy> enemy : enemies_) {
		enemy->Update();
	}
	for (std::shared_ptr<ShockWave> shockWave : shockWaves_) {
		shockWave->Update();
	}

	// これと押すとメモリリーク起きる
	// DeleteShockWave();

	Collision();

	camera2D_->Update();
	camera3D_->Update();
}

void Play::Draw() {

	for (std::shared_ptr<Enemy> enemy : enemies_) {
		enemy->Draw2D(camera2D_->GetViewOthographics());
	}
	for (std::shared_ptr<ShockWave> shockWave : shockWaves_) {
		shockWave->Draw2D(camera2D_->GetViewOthographics());
	}

	player_->Draw2D(camera2D_->GetViewOthographics());
}


