#include "Player.h"

#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Game/ShockWave/ShockWave.h"
#include "Scenes/GameScene/GameScene.h"
#include "Utils/Camera/Camera.h"
#include <numbers>
#include "Game/Enemy/Enemy.h"
#include "imgui.h"
#include "AudioManager/AudioManager.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Utils/Random/Random.h"

Player::Player() {
	
	input_ = Input::GetInstance();

	tex_ = std::make_unique<Texture2D>();
	tex_->LoadTexture("./Resources/Player/usausa.png");

	models_.push_back(std::make_unique<Model>());
	models_[static_cast<uint16_t>(Parts::kMain)]->LoadObj("./Resources/Player/Player.obj");
	models_[static_cast<uint16_t>(Parts::kMain)]->light.ligDirection = { 0.0f,0.0f,1.0f };
	models_[static_cast<uint16_t>(Parts::kMain)]->light.ligColor = { 1.0f,1.0f,1.0f };
	models_[static_cast<uint16_t>(Parts::kMain)]->light.ptRange = 10000.0f;
	models_[static_cast<uint16_t>(Parts::kMain)]->rotate.y = std::numbers::pi_v<float>;

	globalVariables_ = std::make_unique<GlobalVariables>();

	tex_->pos = {};

	tex_->scale = scale_;

	// ジャンプ時の初速
	kJampInitialVelocity_ = 10.0f;

	// 移動スピード
	kMoveSpeed_ = 5.0f;

	// 重力加速度
	kGravity_ = -9.8f;

	// 降下中の重力加速度
	kFallingGravity_ = -20.0f;

	isFallingGravity_ = true;
	isHipdropJamp_ = true;

	auto textureManager = TextureManager::GetInstance();
	playerAnimationTex_.reserve(5);
	playerAnimationTex_.push_back(textureManager->LoadTexture("./Resources/Player/player_face.png"));
	playerAnimationTex_.push_back(textureManager->LoadTexture("./Resources/Player/player_face2.png"));
	playerAnimationTex_.push_back(textureManager->LoadTexture("./Resources/Player/player_face3.png"));
	playerAnimationTex_.push_back(textureManager->LoadTexture("./Resources/Player/player_face4.png"));
	playerAnimationTex_.push_back(textureManager->LoadTexture("./Resources/Player/player_face5.png"));
	nockBackFaceTexture_ = textureManager->LoadTexture("./Resources/Player/player_face_happy.png");

	playerAnimationDuration_ = std::chrono::milliseconds{ 33 };
	currentPlayerAnimation_ = 0;
	isPlayerAnimationTurnBack_ = false;
	playerAnimationStartTime_ = std::chrono::steady_clock::now();

	playerAnimationCoolTime_ = std::chrono::milliseconds{ 1600 };
	playerAnimationCoolTimeDuration_ = {
		800,
		1600
	};
	isPlayerAnimationCoolTime_ = true;
	playerAnimationCoolStartTime_ = playerAnimationStartTime_;
}

void Player::SetGlobalVariable() {
	
	globalVariables_->CreateGroup(groupName_);
	globalVariables_->AddItem(groupName_, "kReboundCoefficient", kReboundCoefficient_);
	globalVariables_->AddItem(groupName_, "kLayerReboundCoefficient", kLayerReboundCoefficient_);
	globalVariables_->AddItem(groupName_, "kJampInitialVelocity", kJampInitialVelocity_);
	globalVariables_->AddItem(groupName_, "kMoveSpeed", kMoveSpeed_);
	globalVariables_->AddItem(groupName_, "kGravity", kGravity_);
	globalVariables_->AddItem(groupName_, "kHipDropSpeed", kHipDropSpeed_);
	globalVariables_->AddItem(groupName_, "kFallingGravity", kFallingGravity_);
	globalVariables_->AddItem(groupName_, "kFallingGravity", kFallingGravity_);

	globalVariables_->LoadFile(groupName_);
	ApplyGlobalVariable();
}

void Player::ApplyGlobalVariable() {

	kReboundCoefficient_ = globalVariables_->GetFloatValue(groupName_, "kReboundCoefficient");
	kLayerReboundCoefficient_ = globalVariables_->GetFloatValue(groupName_, "kLayerReboundCoefficient");
	kJampInitialVelocity_ = globalVariables_->GetFloatValue(groupName_, "kJampInitialVelocity");
	kMoveSpeed_ = globalVariables_->GetFloatValue(groupName_, "kMoveSpeed");
	kGravity_ = globalVariables_->GetFloatValue(groupName_, "kGravity");
	kHipDropSpeed_ = globalVariables_->GetFloatValue(groupName_, "kHipDropSpeed");
	kFallingGravity_ = globalVariables_->GetFloatValue(groupName_, "kFallingGravity");
}

void Player::EnemyStep(bool step) {

	if (step) {
		if (!isSteped_) {
			isStep_ = true;
		}
	}
	else {
		isSteped_ = true;
		isFly_ = false;

		if (statusRequest_ == std::nullopt) {
			statusRequest_ = Status::kFalling;
		}
	}
}

void Player::Initialize() {

	SetGlobalVariable();

	velocity_ = {};

	// 空中にいるか
	isFly_ = false;

	// 敵を踏んだか
	isStep_ = false;

	// 敵に踏まれたか
	isSteped_ = false;

	jumpSE_ = AudioManager::GetInstance()->LoadWav("./Resources/Audio/kouka/kouka/jamp1.wav", false);
	fallSE_ = AudioManager::GetInstance()->LoadWav("./Resources/Audio/kouka/kouka/otiru.wav", false);

	damageSE_ = AudioManager::GetInstance()->LoadWav("./Resources/Audio/kouka/kouka/damage.wav", false);
	batinSE_ = AudioManager::GetInstance()->LoadWav("./Resources/Audio/kouka/kouka/batin.wav", false);
	humuSE_ = AudioManager::GetInstance()->LoadWav("./Resources/Audio/kouka/kouka/player_humu.wav", false);
}

void Player::Update(const float& y, const Camera* camera) {
	auto nowTime = std::chrono::steady_clock::now();

#ifdef _DEBUG
	ImGui::Begin("PlayerFlag");
	ImGui::Checkbox("ChangeGravityType", &isFallingGravity_);
	ImGui::Checkbox("ChangeIsHipdropJamp", &isHipdropJamp_);
	ImGui::End();
#endif // _DEBUG

	globalVariables_->Update();
	ApplyGlobalVariable();

	if (statusRequest_) {
		status_ = statusRequest_.value();

		switch (status_)
		{
		case Player::Status::kNormal:
			NormalInitialize(y);
			break;
		case Player::Status::kHipDrop:
			HipDropInitialize();
			break;
		case Player::Status::kOnScaffolding:
			OnScaffoldingInitialize();
			break;
		case Player::Status::kLanding:
			LandingInitialize(y);
			break;
		case Player::Status::kFalling:
			FallingInitialize(y);
			break;
		case Player::Status::kKnockBack:
			KnockBackInitilize();
			break;
		default:
			break;
		}

		statusRequest_ = std::nullopt;
	}

	switch (status_)
	{
	case Player::Status::kNormal:
		NormalUpdate(y);
		break;
	case Player::Status::kHipDrop:
		HipDropUpdate(y);
		break;
	case Player::Status::kOnScaffolding:
		OnScaffoldingUpdate();
		break;
	case Player::Status::kLanding:
		LandingUpdate(y);
		break;
	case Player::Status::kFalling:
		FallingUpdate(y);
		break;
	case Player::Status::kKnockBack:
		KnockBackUpdate(y);
		break;
	default:
		break;
	}


	if (status_ != Status::kLanding) {
		if (tex_->pos.x - tex_->scale.x < -640) {
			tex_->pos.x -= tex_->pos.x - tex_->scale.x + 640;
			velocity_.x *= -1;
		}
		else if (tex_->pos.x + tex_->scale.x > 640) {
			tex_->pos.x -= tex_->pos.x + tex_->scale.x - 640;
			velocity_.x *= -1;
		}
	}

	float ratio = WindowFactory::GetInstance()->GetClientSize().y /
		(std::tanf(camera->fov / 2) * (models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) * 2);

	float indication = 90.0f;

	models_[static_cast<uint16_t>(Parts::kMain)]->pos.x = tex_->pos.x / ratio + camera->pos.x - camera->pos.x / ratio;
	models_[static_cast<uint16_t>(Parts::kMain)]->pos.y = tex_->pos.y / ratio + camera->pos.y- camera->pos.y / ratio;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.x = tex_->scale.x / (indication * std::tanf(camera->fov / 2) * 2) *
		(models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) / indication;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.y = tex_->scale.y / (indication * std::tanf(camera->fov / 2) * 2) *
		(models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) / indication;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.z = models_[static_cast<uint16_t>(Parts::kMain)]->scale.y;
	models_[static_cast<uint16_t>(Parts::kMain)]->Update();
	tex_->Update();

	easeScale_.Update();
	isCollisionLayer_.Update();
	isCollisionEnemy_.Update();

	if(status_  == Status::kFalling || status_ == Status::kKnockBack){
		models_[0]->ChangeTexture("face", nockBackFaceTexture_);
	}
	else {
		if (isPlayerAnimationCoolTime_ && playerAnimationCoolTime_ < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - playerAnimationCoolStartTime_)) {
			isPlayerAnimationCoolTime_ = false;
		}

		if (!isPlayerAnimationCoolTime_ && playerAnimationDuration_ < std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - playerAnimationStartTime_)) {
			isPlayerAnimationTurnBack_ ? --currentPlayerAnimation_ : ++currentPlayerAnimation_;
			if (currentPlayerAnimation_ >= static_cast<int32_t>(playerAnimationTex_.size()) - 1) {
				isPlayerAnimationTurnBack_ = true;
				playerAnimationCoolTime_ = std::chrono::milliseconds{
					Lamb::Random(playerAnimationCoolTimeDuration_.first, playerAnimationCoolTimeDuration_.second)
				};
			}
			else if (currentPlayerAnimation_ <= 0) {
				isPlayerAnimationTurnBack_ = false;
				isPlayerAnimationCoolTime_ = true;
				playerAnimationCoolStartTime_ = nowTime;
			}
			currentPlayerAnimation_ = std::clamp(currentPlayerAnimation_, 0, static_cast<int32_t>(playerAnimationTex_.size()) - 1);
			models_[0]->ChangeTexture("face", playerAnimationTex_[currentPlayerAnimation_]);
			playerAnimationStartTime_ = nowTime;
		}
	}
	jumpSE_->Debug("jumpSE_");
	fallSE_->Debug("fallSE_");
}

void Player::NormalInitialize(const float& y) {

	//velocity_ = {};
	//highest_ = 0.0f;
	Collision(y);

	isStep_ = false;
	isSteped_ = false;
	models_[static_cast<uint16_t>(Parts::kMain)]->rotate.y = -std::numbers::pi_v<float>;
}

void Player::NormalUpdate(const float& y) {

	float deletaTime = FrameInfo::GetInstance()->GetDelta();

	// 移動の単位ベクトル
	Vector3 move = {};

	// 左右移動
	if (input_->GetKey()->LongPush(DIK_A) || input_->GetKey()->LongPush(DIK_LEFT) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::LEFT) ||
		input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) < -0.3f) {
		move.x--;
	}
	if (input_->GetKey()->LongPush(DIK_D) || input_->GetKey()->LongPush(DIK_RIGHT) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::RIGHT) ||
		input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) > 0.3f) {
		move.x++;
	}

	// 空中にいる時の処理。
	if (isFly_) {
		if (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)) {
			statusRequest_ = Status::kHipDrop;

			// 音を出す
			//fallSE_->Start(0.3f);
		}

		easeCount_ += deletaTime;

		if (easeCount_ > easeTime_) {
			easeCount_ = easeTime_;
		}

		tex_->scale = Vector2::Lerp(scaleStart_, scaleEnd_, easeCount_ / easeTime_);
	}

	// ジャンプ入力
	if (isStep_ || (!isFly_ && (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetGamepad()->Pushed(Gamepad::Button::A)))) {
		if (!isStep_) {
			// 音を出す
			jumpSE_->Start(0.4f);
		}
		else {
			humuSE_->Start(0.4f);
		}

		isFly_ = true;
		isStep_ = false;
		// 初速を与える
		velocity_.y = kJampInitialVelocity_;

		//easeScale_.Start(false, 0.3f, Easeing::InSine);
		scaleStart_ = scale_;
		scaleEnd_ = { scale_.x * 0.8f,scale_.y * 1.2f };
		easeCount_ = 0.0f;
		easeTime_ = 0.3f;
	}

	if (isFallingGravity_) {
		if (velocity_.y < 0) {
			velocity_.y += kFallingGravity_ * deletaTime;
		}
		else {
			velocity_.y += kGravity_ * deletaTime;
		}
	}
	else {
		velocity_.y += kGravity_ * deletaTime;
	}

	// 横の移動距離
	velocity_.x = move.x * kMoveSpeed_ * deletaTime;

	tex_->pos += velocity_;

	MemoHighest();

	// 地面との当たり判定。
	if (tex_->pos.y - tex_->scale.y / 2.0f <= y && isFly_) {

		Collision(y);
		velocity_.y = 0.0f;
		isFly_ = false;
		statusRequest_ = Status::kLanding;
	}
	else {
		Collision(y);
	}
}

void Player::HipDropInitialize() {

	velocity_ = {};
	hipdropCount_ = 0;
}

void Player::HipDropUpdate(const float& y) {

	float time = 0.3f;

	if (hipdropCount_ < time) {
		hipdropCount_ = std::clamp<float>(hipdropCount_ + FrameInfo::GetInstance()->GetDelta(), 0.0f, time);

		float t = hipdropCount_ / time;

		models_[static_cast<uint16_t>(Parts::kMain)]->rotate.y = (1.0f - t) * (-std::numbers::pi_v<float>) + t * std::numbers::pi_v<float>;

		if (hipdropCount_ >= time) {
			// 音を出す
			fallSE_->Start(0.3f);
		}

	}
	else {
		velocity_.y += kHipDropSpeed_ * FrameInfo::GetInstance()->GetDelta();
		tex_->pos += velocity_;

		if (isHipdropJamp_) {
			if (isStep_) {
				isFly_ = true;
				isStep_ = false;
				// 初速を与える
				velocity_.y = kJampInitialVelocity_;
				statusRequest_ = Status::kNormal;
			}
		}

		if (tex_->pos.y - tex_->scale.y / 2.0f <= y && isFly_) {

			Collision(y);
			velocity_.y = 0.0f;
			isFly_ = false;
			statusRequest_ = Status::kLanding;
		}
	}
}

void Player::OnScaffoldingInitialize()
{
	velocity_ = {};
	isFly_ = false;
	isSteped_ = false;
	isStep_ = false;
}

void Player::OnScaffoldingUpdate()
{
	float deletaTime = FrameInfo::GetInstance()->GetDelta();

	// 移動の単位ベクトル
	Vector3 move = {};

	// 左右移動
	if (input_->GetKey()->LongPush(DIK_A) || input_->GetKey()->LongPush(DIK_LEFT) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::LEFT) ||
		input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) < -0.3f) {
		move.x--;
	}
	if (input_->GetKey()->LongPush(DIK_D) || input_->GetKey()->LongPush(DIK_RIGHT) ||
		input_->GetGamepad()->Pushed(Gamepad::Button::RIGHT) ||
		input_->GetGamepad()->GetStick(Gamepad::Stick::LEFT_X) > 0.3f) {
		move.x++;
	}

	if ((!isFly_ && (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetKey()->Pushed(DIK_W) ||
		input_->GetKey()->Pushed(DIK_UP) || input_->GetGamepad()->Pushed(Gamepad::Button::A)))) {
		isFly_ = true;
		isStep_ = false;
		// 初速を与える
		velocity_.y = kJampInitialVelocity_;

		statusRequest_ = Status::kNormal;
	}

	velocity_.y += kGravity_ * deletaTime;

	// 横の移動距離
	velocity_.x = move.x * kMoveSpeed_;

	tex_->pos += velocity_ * deletaTime;

	MemoHighest();

}

void Player::LandingInitialize(const float& y) {

	velocity_ = { 0.0f,0.0f,0.0f };

	Collision(y);

	play_->CreatShockWave(tex_->pos, highest_, y);

	scaleStart_ = tex_->scale;

	if (highest_ >= ShockWave::GetHighCriteria(static_cast<int>(ShockWave::Size::kMiddle))) {
		scaleEnd_ = { scale_.x * 10.0f,scale_.y * 0.3f };
		easeTime_ = 0.7f;
		batinSE_->Start(0.4f);
	}
	else if (highest_ >= ShockWave::GetHighCriteria(static_cast<int>(ShockWave::Size::kSmall))) {
		scaleEnd_ = { scale_.x * 5.0f,scale_.y * 0.5f };
		easeTime_ = 0.5f;
		batinSE_->Start(0.2f);
	}
	else {
		scaleEnd_ = { scale_.x * 2.0f,scale_.y * 0.8f };
		easeTime_ = 0.3f;
		batinSE_->Start(0.1f);
	}

	easeCount_ = 0.0f;
	isEaseReturn_ = false;
}

void Player::LandingUpdate(const float& y) {

	velocity_.y += kFallingGravity_;
	tex_->pos += velocity_ * FrameInfo::GetInstance()->GetDelta();
	Collision(y);

	easeCount_ += FrameInfo::GetInstance()->GetDelta();

	if (easeCount_ >= easeTime_ / 2 && !isEaseReturn_) {
		isEaseReturn_ = true;
		easeCount_ -= easeTime_ / 2;
		scaleStart_ = tex_->scale;
		scaleEnd_ = scale_;
	}

	tex_->scale = Vector2::Lerp(scaleStart_, scaleEnd_, easeCount_ / easeTime_ * 2.0f);

	if (easeCount_ >= easeTime_ / 2 && isEaseReturn_) {
		statusRequest_ = Status::kNormal;
		tex_->scale = scale_;
		isEaseReturn_ = false;
		highest_ = 0.0f;
	}

}

void Player::FallingInitialize(const float& y) {

	Collision(y);
	//velocity_ = {};

	scaleStart_ = tex_->scale;

	scaleEnd_ = scale_;

	easeCount_ = 0.0f;
	easeTime_ = 0.5f;
	isEaseReturn_ = false;

}

void Player::FallingUpdate(const float& y) {

	float deletaTime = FrameInfo::GetInstance()->GetDelta();

	if (isFallingGravity_) {
		velocity_.y += kFallingGravity_;
	}
	else {
		velocity_.y += kGravity_;
	}

	easeCount_ += deletaTime;

	if (easeCount_ > easeTime_) {
		easeCount_ = easeTime_;
	}

	tex_->scale = Vector2::Lerp(scaleStart_, scaleEnd_, easeCount_ / easeTime_);

	tex_->pos += velocity_ * deletaTime;

	if (isCollisionLayer_.OnStay()) {

		rotateTimeCount_ += FrameInfo::GetInstance()->GetDelta();

		float t = std::clamp<float>(rotateTimeCount_, 0.0f, rotateTime_) / rotateTime_;

		models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z = std::lerp(startRotate_, endRotate_, t);
	}
	else {
		models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z += rotateAddAngle_ * FrameInfo::GetInstance()->GetDelta();
	}


	if (tex_->pos.y - tex_->scale.y / 2.0f <= y) {

		Vector3 vect = velocity_;

		Collision(y);

		if (isCollisionLayer_.OnStay()) {
			isCollisionLayer_ = false;
			Collision(y);
			velocity_.x = 0.0f;
			velocity_.y = 0.0f;
			isFly_ = false;
			statusRequest_ = Status::kNormal;
			models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z = 0.0f;
			highest_ = 0.0f;
		}
		else {

			isCollisionLayer_ = true;

			velocity_.y = std::fabsf(vect.y) * kLayerReboundCoefficient_;

			rotateTime_ = 2.0f * velocity_.y / std::fabsf(kFallingGravity_) * FrameInfo::GetInstance()->GetDelta();
			startRotate_ = models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z;

			float pi = std::numbers::pi_v<float>;
			float memo = startRotate_ / 2.0f / pi;
			float moveSpeed = 100.0f;
			if (startRotate_ >= 0) {
				endRotate_ = 2.0f * pi * (static_cast<int>(memo));
				float theta = endRotate_ - memo;

				if (std::sinf(theta) <= 0) {
					velocity_.x = std::fabsf(std::cosf(theta)) * moveSpeed;
				}
				else {
					velocity_.x = -std::fabsf(std::cosf(theta)) * moveSpeed;
				}
				velocity_.x = std::sinf(theta) * moveSpeed;
			}
			else {
				endRotate_ = -2.0f * pi * (static_cast<int>(memo));
				float theta = endRotate_ - memo;

				if (-std::sinf(theta) <= 0) {
					velocity_.x = std::fabsf(std::cosf(theta)) * moveSpeed;
				}
				else {
					velocity_.x = -std::fabsf(std::cosf(theta)) * moveSpeed;
				}
				velocity_.x = -std::sinf(theta) * moveSpeed;
			}

			rotateTimeCount_ = 0.0f;

		}
	}
}

void Player::KnockBackInitilize()
{


}

void Player::KnockBackUpdate(const float& y)
{
	velocity_.y += kGravity_;

	tex_->pos += velocity_ * FrameInfo::GetInstance()->GetDelta();

	rotateTimeCount_ += FrameInfo::GetInstance()->GetDelta();

	float t = std::clamp<float>(rotateTimeCount_, 0.0f, rotateTime_) / rotateTime_;

	models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z = std::lerp(0.0f, endRotate_, t);

	if (tex_->pos.y - tex_->scale.y / 2.0f <= y) {
		Collision(y);

		velocity_.y = 0.0f;
		velocity_.x = 0.0f;
		statusRequest_ = Status::kNormal;
		models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z = 0.0f;
	}
}

void Player::CollisionScaffolding(const Texture2D* tex)
{

	if (velocity_.y < 0 &&
		tex_->pos.y - velocity_.y - tex_->scale.y/2.0f >= tex->pos.y + tex->scale.y / 2.0f &&
		tex->pos.x - tex->scale.x / 2.0f <= tex_->pos.x + tex_->scale.x / 2.0f &&
		tex->pos.x + tex->scale.x / 2.0f >= tex_->pos.x - tex_->scale.x / 2.0f &&
		tex->pos.y + tex->scale.y / 2.0f >= tex_->pos.y - tex_->scale.y / 2.0f) {

		tex_->pos.y = tex->pos.y + tex->scale.y / 2.0f + tex_->scale.y / 2.0f + 0.1f;
		velocity_.y = 0.0f;

		tex_->Update();
		
		highest_ = tex_->pos.y;

		statusRequest_ = Status::kOnScaffolding;
	}
	else {
		if (status_ == Status::kOnScaffolding) {
			statusRequest_ = Status::kNormal;
			isFly_ = true;
		}
	}
}

void Player::KnockBack(const Vector3& pos, const Vector3& scale)
{
	damageSE_->Start(0.1f);
	Vector3 vector = tex_->pos - pos;
	vector.y = 0.0f;

	Vector3 normal = vector.Normalize();

	float theta = 1.0f;

	if (vector.x < 0) {
		theta *= -1;
		tex_->pos.x = pos.x - scale.x / 2 - tex_->scale.x / 2;
	}
	else {
		tex_->pos.x = pos.x + scale.x / 2 + tex_->scale.x / 2;
	}

	float speed = 200.0f;
	
	velocity_.x = normal.x * std::cosf(theta) - normal.y * std::sinf(theta);
	velocity_.y = normal.y * std::cosf(theta) + normal.x * std::sinf(theta);

	velocity_ *= speed;

	rotateTime_ = 2.0f * velocity_.y / (-kGravity_) * FrameInfo::GetInstance()->GetDelta();

	float pi = std::numbers::pi_v<float>;
	
	if (velocity_.x >= 0) {
		endRotate_ = pi * 2.0f;
	}
	else {
		endRotate_ = -pi * 2.0f;
	}

	rotateTimeCount_ = 0.0f;

	if (statusRequest_ == std::nullopt) {
		statusRequest_ = Status::kKnockBack;
	}

}

void Player::Steped(Enemy* enemy)
{
	damageSE_->Start(0.1f);
	float speed = velocity_.Length() * kReboundCoefficient_;

	Vector3 beforePos = tex_->pos - velocity_;
	Vector3 pos = tex_->pos;
	bool isFirstX = false;


	if (velocity_.x == 0.0f) {
		if (velocity_.y <= 0) {
			tex_->pos.y = enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2 + tex_->scale.y / 2 + 0.001f;
		}
		else {
			tex_->pos.y = enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2 - tex_->scale.y / 2 - 0.001f;
		}
	}
	else if (velocity_.y == 0.0f) {
		if (velocity_.x <= 0) {
			tex_->pos.x = enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2 + tex_->scale.x / 2 + 0.001f;
		}
		else {
			tex_->pos.x = enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2 - tex_->scale.x / 2 - 0.001f;
		}
	}
	else if (velocity_.x > 0) {
		if (velocity_.y > 0) {
			if (beforePos.x <= enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2 - tex_->scale.x / 2) {
				if (beforePos.y >= enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2 - tex_->scale.y / 2) {
					isFirstX = true;
				}
				else {
					if (Vector2{ (enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2) - (beforePos.x + tex_->scale.x / 2),
						(enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2) - (beforePos.y + tex_->scale.y / 2) }.Cross(
							{ velocity_.x,velocity_.y }) > 0.0f) {
						isFirstX = true;
					}
					else {
						isFirstX = false;
					}
				}
			}
			else {
				isFirstX = false;
			}

			if (isFirstX) {
				tex_->pos.x = enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2 - tex_->scale.x / 2 - 0.001f;
				float t = (tex_->pos.x - beforePos.x) / (pos.x - beforePos.x);
				tex_->pos.x -= velocity_.x * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
				//tex_->pos.y = beforePos.y + t * velocity_.y * FrameInfo::GetInstance()->GetDelta();
			}
			else {
				tex_->pos.y = enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2 - tex_->scale.y / 2 - 0.001f;
				float t = (tex_->pos.y - beforePos.y) / (pos.y - beforePos.y);
				tex_->pos.y -= velocity_.y * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
				//tex_->pos.x = beforePos.x + t * velocity_.x * FrameInfo::GetInstance()->GetDelta();
			}
		}
		else {
			if (beforePos.x <= enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2 - tex_->scale.x / 2) {
				if (beforePos.y <= enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2 + tex_->scale.y / 2) {
					isFirstX = true;
				}
				else {
					if (Vector2{ (enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2) - (beforePos.x + tex_->scale.x / 2),
						(enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2) - (beforePos.y - tex_->scale.y / 2) }.Cross(
							{ velocity_.x,velocity_.y }) > 0.0f) {
						isFirstX = false;
					}
					else {
						isFirstX = true;
					}
				}
			}
			else {
				isFirstX = false;
			}

			if (isFirstX) {
				tex_->pos.x = enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2 - tex_->scale.x / 2 - 0.001f;
				float t = (tex_->pos.x - beforePos.x) / (pos.x - beforePos.x);
				tex_->pos.x -= velocity_.x * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
				//tex_->pos.y = beforePos.y + t * velocity_.y * FrameInfo::GetInstance()->GetDelta();
			}
			else {
				tex_->pos.y = enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2 + tex_->scale.y / 2 + 0.001f;
				float t = (tex_->pos.y - beforePos.y) / (pos.y - beforePos.y);
				tex_->pos.y -= velocity_.y * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
				//tex_->pos.x = beforePos.x + t * velocity_.x * FrameInfo::GetInstance()->GetDelta();
			}

		}
	}
	else {
		if (velocity_.y > 0) {
			if (beforePos.x >= enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2 + tex_->scale.x / 2) {
				if (beforePos.y >= enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2 - tex_->scale.y / 2) {
					isFirstX = true;
				}
				else {
					if (Vector2{ (enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2) - (beforePos.x - tex_->scale.x / 2),
						(enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2) - (beforePos.y + tex_->scale.y / 2) }.Cross(
							{ velocity_.x,velocity_.y }) < 0.0f) {
						isFirstX = true;
					}
					else {
						isFirstX = false;
					}
				}
			}
			else {
				isFirstX = false;
			}

			if (isFirstX) {
				tex_->pos.x = enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2 + tex_->scale.x / 2 + 0.001f;
				float t = (tex_->pos.x - beforePos.x) / (pos.x - beforePos.x);
				tex_->pos.x -= velocity_.x * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
				//tex_->pos.y = beforePos.y + t * velocity_.y * FrameInfo::GetInstance()->GetDelta();
			}
			else {
				tex_->pos.y = enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2 - tex_->scale.y / 2 - 0.001f;
				float t = (tex_->pos.y - beforePos.y) / (pos.y - beforePos.y);
				tex_->pos.y -= velocity_.y * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
				//tex_->pos.x = beforePos.x + t * velocity_.x * FrameInfo::GetInstance()->GetDelta();
			}
		}
		else {
			if (beforePos.x >= enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2 + tex_->scale.x / 2) {
				if (beforePos.y <= enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2 + tex_->scale.y / 2) {
					isFirstX = true;
				}
				else {
					if (Vector2{ (enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2) - (beforePos.x - tex_->scale.x / 2),
						(enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2) - (beforePos.y - tex_->scale.y / 2) }.Cross(
							{ velocity_.x,velocity_.y }) < 0.0f) {
						isFirstX = false;
					}
					else {
						isFirstX = true;
					}
				}
			}
			else {
				isFirstX = false;
			}

			if (isFirstX) {
				tex_->pos.x = enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2 + tex_->scale.x / 2 + 0.001f;
				float t = (tex_->pos.x - beforePos.x) / (pos.x - beforePos.x);
				tex_->pos.x -= velocity_.x * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
				//tex_->pos.y = beforePos.y + t * velocity_.y * FrameInfo::GetInstance()->GetDelta();
			}
			else {
				tex_->pos.y = enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2 + tex_->scale.y / 2 + 0.001f;
				float t = (tex_->pos.y - beforePos.y) / (pos.y - beforePos.y);
				tex_->pos.y -= velocity_.y * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
				//tex_->pos.x = beforePos.x + t * velocity_.x * FrameInfo::GetInstance()->GetDelta();
			}

		}
	}


	Vector3 vector = enemy->GetTex()->pos - tex_->pos;

	if (vector.x == 0) {

		if (Lamb::Random(0, 1) == 0) {
			rotateAddAngle_ = 6.0f;
		}
		else {
			rotateAddAngle_ = -6.0f;
		}
	}
	else {
		float angle = std::numbers::pi_v<float> / 2 - std::atan2f(vector.y, vector.x);

		if (angle >= std::numbers::pi_v<float> / 2) {
			angle -= std::numbers::pi_v<float> / 2;
		}
		else if (angle <= -std::numbers::pi_v<float> / 2) {
			angle += std::numbers::pi_v<float> / 2;
		}
		rotateAddAngle_ = angle * 4;
	}

	isCollisionEnemy_ = true;
	velocity_ = vector.Normalize() * speed;

}

void Player::FallingCollision(Enemy* enemy)
{
	if (status_ == Status::kFalling) {
		if (tex_->Collision(*enemy->GetTex())) {
			if (status_ == Status::kFalling) {
				isCollisionEnemy_ = true;
				if (isCollisionEnemy_.OnEnter()) {


					Vector3 beforePos = tex_->pos - velocity_;
					Vector3 pos = tex_->pos;
					bool isFirstX = false;


					if (velocity_.x == 0.0f) {
						if (velocity_.y <= 0) {
							tex_->pos.y = enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2 + tex_->scale.y / 2 + 0.001f;
						}
						else {
							tex_->pos.y = enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2 - tex_->scale.y / 2 - 0.001f;
						}
					}
					else if (velocity_.y == 0.0f) {
						if (velocity_.x <= 0) {
							tex_->pos.x = enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2 + tex_->scale.x / 2 + 0.001f;
						}
						else {
							tex_->pos.x = enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2 - tex_->scale.x / 2 - 0.001f;
						}
					}
					else if (velocity_.x > 0) {
						if (velocity_.y > 0) {
							if (beforePos.x <= enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2 - tex_->scale.x / 2) {
								if (beforePos.y >= enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2 - tex_->scale.y / 2) {
									isFirstX = true;
								}
								else {
									if (Vector2{ (enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2) - (beforePos.x + tex_->scale.x / 2),
										(enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2) - (beforePos.y + tex_->scale.y / 2) }.Cross(
											{ velocity_.x,velocity_.y }) > 0.0f) {
										isFirstX = true;
									}
									else {
										isFirstX = false;
									}
								}
							}
							else {
								isFirstX = false;
							}

							if (isFirstX) {
								tex_->pos.x = enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2 - tex_->scale.x / 2 - 0.001f;
								float t = (tex_->pos.x - beforePos.x) / (pos.x - beforePos.x);
								tex_->pos.x -= velocity_.x * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
								//tex_->pos.y = beforePos.y + t * velocity_.y * FrameInfo::GetInstance()->GetDelta();
							}
							else {
								tex_->pos.y = enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2 - tex_->scale.y / 2 - 0.001f;
								float t = (tex_->pos.y - beforePos.y) / (pos.y - beforePos.y);
								tex_->pos.y -= velocity_.y * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
								//tex_->pos.x = beforePos.x + t * velocity_.x * FrameInfo::GetInstance()->GetDelta();
							}
						}
						else {
							if (beforePos.x <= enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2 - tex_->scale.x / 2) {
								if (beforePos.y <= enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2 + tex_->scale.y / 2) {
									isFirstX = true;
								}
								else {
									if (Vector2{ (enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2) - (beforePos.x + tex_->scale.x / 2),
										(enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2) - (beforePos.y - tex_->scale.y / 2) }.Cross(
											{ velocity_.x,velocity_.y }) > 0.0f) {
										isFirstX = false;
									}
									else {
										isFirstX = true;
									}
								}
							}
							else {
								isFirstX = false;
							}

							if (isFirstX) {
								tex_->pos.x = enemy->GetTex()->pos.x - enemy->GetTex()->scale.x / 2 - tex_->scale.x / 2 - 0.001f;
								float t = (tex_->pos.x - beforePos.x) / (pos.x - beforePos.x);
								tex_->pos.x -= velocity_.x * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
								//tex_->pos.y = beforePos.y + t * velocity_.y * FrameInfo::GetInstance()->GetDelta();
							}
							else {
								tex_->pos.y = enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2 + tex_->scale.y / 2 + 0.001f;
								float t = (tex_->pos.y - beforePos.y) / (pos.y - beforePos.y);
								tex_->pos.y -= velocity_.y * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
								//tex_->pos.x = beforePos.x + t * velocity_.x * FrameInfo::GetInstance()->GetDelta();
							}

						}
					}
					else {
						if (velocity_.y > 0) {
							if (beforePos.x >= enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2 + tex_->scale.x / 2) {
								if (beforePos.y >= enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2 - tex_->scale.y / 2) {
									isFirstX = true;
								}
								else {
									if (Vector2{ (enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2) - (beforePos.x - tex_->scale.x / 2),
										(enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2) - (beforePos.y + tex_->scale.y / 2) }.Cross(
											{ velocity_.x,velocity_.y }) < 0.0f) {
										isFirstX = true;
									}
									else {
										isFirstX = false;
									}
								}
							}
							else {
								isFirstX = false;
							}

							if (isFirstX) {
								tex_->pos.x = enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2 + tex_->scale.x / 2 + 0.001f;
								float t = (tex_->pos.x - beforePos.x) / (pos.x - beforePos.x);
								tex_->pos.x -= velocity_.x * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
								//tex_->pos.y = beforePos.y + t * velocity_.y * FrameInfo::GetInstance()->GetDelta();
							}
							else {
								tex_->pos.y = enemy->GetTex()->pos.y - enemy->GetTex()->scale.y / 2 - tex_->scale.y / 2 - 0.001f;
								float t = (tex_->pos.y - beforePos.y) / (pos.y - beforePos.y);
								tex_->pos.y -= velocity_.y * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
								//tex_->pos.x = beforePos.x + t * velocity_.x * FrameInfo::GetInstance()->GetDelta();
							}
						}
						else {
							if (beforePos.x >= enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2 + tex_->scale.x / 2) {
								if (beforePos.y <= enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2 + tex_->scale.y / 2) {
									isFirstX = true;
								}
								else {
									if (Vector2{ (enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2) - (beforePos.x - tex_->scale.x / 2),
										(enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2) - (beforePos.y - tex_->scale.y / 2) }.Cross(
											{ velocity_.x,velocity_.y }) < 0.0f) {
										isFirstX = false;
									}
									else {
										isFirstX = true;
									}
								}
							}
							else {
								isFirstX = false;
							}

							if (isFirstX) {
								tex_->pos.x = enemy->GetTex()->pos.x + enemy->GetTex()->scale.x / 2 + tex_->scale.x / 2 + 0.001f;
								float t = (tex_->pos.x - beforePos.x) / (pos.x - beforePos.x);
								tex_->pos.x -= velocity_.x * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
								//tex_->pos.y = beforePos.y + t * velocity_.y * FrameInfo::GetInstance()->GetDelta();
							}
							else {
								tex_->pos.y = enemy->GetTex()->pos.y + enemy->GetTex()->scale.y / 2 + tex_->scale.y / 2 + 0.001f;
								float t = (tex_->pos.y - beforePos.y) / (pos.y - beforePos.y);
								tex_->pos.y -= velocity_.y * (1.0f - t) * FrameInfo::GetInstance()->GetDelta() * kReboundCoefficient_;
								//tex_->pos.x = beforePos.x + t * velocity_.x * FrameInfo::GetInstance()->GetDelta();
							}

						}
					}

					Vector3 vector = tex_->pos - enemy->GetTex()->pos;

					float speed = velocity_.Length() * kReboundCoefficient_;

					if (vector.y <= 0 && velocity_.y <= 0) {
						velocity_.x = vector.Normalize().x * speed;
						velocity_.y = -vector.Normalize().y * speed;
					}
					else {
						velocity_ = vector.Normalize() * speed;
					}
					velocity_.z = 0;

					if (velocity_.x == 0) {

						velocity_.x = std::sinf(rotateAddAngle_) * Lamb::Random(2, 6);

						/*if (std::fabsf(rotateAddAngle_) <= 0.5f) {
							rotateAddAngle_ = rotateAddAngle_ * Lamb::Random(3, 6);
						}
						velocity_.x = -std::sinf(rotateAddAngle_) * velocity_.y;
						velocity_.y = std::cosf(rotateAddAngle_) * velocity_.y;*/
					}
					else {

						float angle = std::numbers::pi_v<float> / 2 - std::atan2f(vector.y, vector.x);

						if (angle >= std::numbers::pi_v<float> / 2) {
							angle -= std::numbers::pi_v<float> / 2;
						}
						else if (angle <= -std::numbers::pi_v<float> / 2) {
							angle += std::numbers::pi_v<float> / 2;
						}
						rotateAddAngle_ = angle * 4;
					}
					isCollisionLayer_ = false;
				}
			}
		}
		else {
			isCollisionEnemy_ = false;
		}
	}
}

void Player::ModelUpdate(const Camera* camera)
{
	float ratio = WindowFactory::GetInstance()->GetClientSize().y /
		(std::tanf(camera->fov / 2) * (models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) * 2);

	float indication = 90.0f;

	models_[static_cast<uint16_t>(Parts::kMain)]->pos.x = tex_->pos.x / ratio + camera->pos.x - camera->pos.x / ratio;
	models_[static_cast<uint16_t>(Parts::kMain)]->pos.y = tex_->pos.y / ratio + camera->pos.y - camera->pos.y / ratio;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.x = tex_->scale.x / (indication * std::tanf(camera->fov / 2) * 2) *
		(models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) / indication;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.y = tex_->scale.y / (indication * std::tanf(camera->fov / 2) * 2) *
		(models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) / indication;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.z = models_[static_cast<uint16_t>(Parts::kMain)]->scale.y;
	models_[static_cast<uint16_t>(Parts::kMain)]->Update();
	tex_->Update();
}

void Player::Collision(const float& y) {

	float posY = tex_->pos.y - tex_->scale.y / 2.0f;

	if (y > posY) {
		tex_->pos.y += y - posY;
		velocity_.y = 0;
	}

}

void Player::MemoHighest() {

	if (highest_ < tex_->pos.y) {
		highest_ = tex_->pos.y;
	}
}

void Player::Draw(const Mat4x4& viewProjection, const Vector3& cameraPos) {
	for (const std::unique_ptr<Model>& model : models_) {
		model->Draw(viewProjection, cameraPos);
	}
}

void Player::Draw2D(const Mat4x4& viewProjection) {

	tex_->Draw(viewProjection, Pipeline::Normal, false);

}
