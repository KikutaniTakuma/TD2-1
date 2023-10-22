#include "Enemy.h"

#include "Engine/FrameInfo/FrameInfo.h"
#include "Utils/Camera/Camera.h"
#include "Game/Player/Player.h"
#include "Game/Layer/Layer.h"
#include <numbers>
#include <cmath>
#include <algorithm>

std::unique_ptr<GlobalVariables> Enemy::globalVariables_ = std::make_unique<GlobalVariables>();

float Enemy::kFallingSpeed_ = -9.8f;

const std::string Enemy::groupName_ = "StaticEnemy";

float Enemy::kReboundCoefficient_ = 0.9f;

float Enemy::kLayerReboundCoefficient_ = 0.3f;

float Enemy::kMoveSpeed_ = 5.0f;

float Enemy::enemyScale_ = 40.0f;

float Enemy::kGenerationTime_ = 2.0f;

float Enemy::kLeaveTime_ = 2.0f;

float Enemy::kFaintTime_ = 7.0f;

float Enemy::kDeathTime_ = 5.0f;

Enemy::Enemy(int type, const Vector3& pos, const float& layerY, int firstMoveVector, int isHealer, float moveRadius) {

	tex_ = std::make_unique<Texture2D>();
	tex_->LoadTexture("./Resources/Enemy/usabom.png");

	for (int i = 0; i < static_cast<int>(Parts::kEnd); i++) {
		models_.push_back(std::make_unique<Model>());
	}
	models_[static_cast<uint16_t>(Parts::kMain)]->LoadObj("./Resources/Enemy/enemy.obj");
	models_[static_cast<uint16_t>(Parts::kMain)]->light.ligDirection = { 0.0f,0.0f,1.0f };
	models_[static_cast<uint16_t>(Parts::kMain)]->light.ligColor = { 1.0f,1.0f,1.0f };
	models_[static_cast<uint16_t>(Parts::kMain)]->light.ptRange = 10000.0f;
	models_[static_cast<uint16_t>(Parts::kMain)]->rotate.y = std::numbers::pi_v<float>;
	models_[static_cast<uint16_t>(Parts::kDoukasen)]->LoadObj("./Resources/Enemy/enemy_doukasen.obj");
	models_[static_cast<uint16_t>(Parts::kDoukasen)]->light.ligDirection = { 0.0f,0.0f,1.0f };
	models_[static_cast<uint16_t>(Parts::kDoukasen)]->light.ligColor = { 1.0f,1.0f,1.0f };
	models_[static_cast<uint16_t>(Parts::kDoukasen)]->light.ptRange = 10000.0f;
	models_[static_cast<uint16_t>(Parts::kDoukasen)]->rotate.y = std::numbers::pi_v<float>;

	models_[static_cast<uint16_t>(Parts::kDoukasen)]->parent = models_[static_cast<uint16_t>(Parts::kMain)].get();

	InitializeFirstMove(firstMoveVector);
	InitializeIsHealer(isHealer);
	InitializeMoveRadius(moveRadius);
	
	if (type == static_cast<int>(Type::kFly)) {
		type_ = Type::kFly;
	}
	else if (type == static_cast<int>(Type::kWalk)) {
		type_ = Type::kWalk;
	}
	else {
		type_ = Type::kFly;
	}

	status_ = Status::kNormal;

	statusRequest_ = std::nullopt;
	
	SetGlobalVariable();

	switch (type_)
	{
	case Enemy::Type::kFly:

		firstPos_ = pos;
		
		velocity_ = {};

		tex_->pos = firstPos_;
		tex_->scale *= enemyScale_;
		break;
	case Enemy::Type::kWalk:

		tex_->scale *= enemyScale_;
		firstPos_ = pos;
		firstPos_.y = layerY + tex_->scale.y / 2.0f;
		velocity_ = {};

		tex_->pos = firstPos_;
		tex_->rotate.z = std::numbers::pi_v<float>;
		models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z = std::numbers::pi_v<float>;

		break;
	case Enemy::Type::kEnd:
		break;
	default:
		break;
	}

	tex_->Update();
}

void Enemy::SetGlobalVariable() {

	globalVariables_->CreateGroup(groupName_);

	globalVariables_->AddItem(groupName_, "kFallingSpeed", kFallingSpeed_);

	globalVariables_->AddItem(groupName_, "kMoveSpeed", kMoveSpeed_);

	globalVariables_->AddItem(groupName_, "kEnemyScale", enemyScale_);

	globalVariables_->AddItem(groupName_, "kGenerationTime", kGenerationTime_);

	globalVariables_->AddItem(groupName_, "kLeaveTime", kLeaveTime_);

	globalVariables_->AddItem(groupName_, "kFaintTime", kFaintTime_);

	globalVariables_->AddItem(groupName_, "kDeathTime", kDeathTime_);

	globalVariables_->LoadFile(groupName_);
	ApplyGlobalVariable();
}

void Enemy::ApplyGlobalVariable() {

	kFallingSpeed_ = globalVariables_->GetFloatValue(groupName_, "kFallingSpeed");

	kMoveSpeed_ = globalVariables_->GetFloatValue(groupName_, "kMoveSpeed");

	enemyScale_ = globalVariables_->GetFloatValue(groupName_, "kEnemyScale");

	kGenerationTime_ = globalVariables_->GetFloatValue(groupName_, "kGenerationTime");

	kLeaveTime_ = globalVariables_->GetFloatValue(groupName_, "kLeaveTime");

	kFaintTime_ = globalVariables_->GetFloatValue(groupName_, "kFaintTime");

	kDeathTime_ = globalVariables_->GetFloatValue(groupName_, "kDeathTime");
}

void Enemy::SetParametar(int type, const Vector3& pos, const float& y, int firstMoveVector, int isHealer, float moveRadius) {


	if (type == static_cast<int>(Type::kFly)) {
		type_ = Type::kFly;

		if (firstPos_ != pos) {
			tex_->pos = pos;
			models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z = 0.0f;
		}
		firstPos_ = pos;
		
	}
	else if (type == static_cast<int>(Type::kWalk)) {
		if (firstPos_.x != pos.x || type != static_cast<int>(type_)) {
			isChange_ = true;
		}
		type_ = Type::kWalk;
		firstPos_ = pos;
		firstPos_.y = y + enemyScale_ / 2.0f;
		models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z = std::numbers::pi_v<float>;
		if (isChange_) {
			tex_->pos = firstPos_;
			isChange_ = false;
		}
	}
	else {
		type_ = Type::kFly;
		if (firstPos_ != pos) {
			tex_->pos = pos;
			models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z = 0.0f;
		}
		firstPos_ = pos;
		
	}

	InitializeFirstMove(firstMoveVector);

	InitializeIsHealer(isHealer);

	InitializeMoveRadius(moveRadius);
}

void Enemy::CollisionEnemy(Enemy* enemy)
{

	if (tex_->Collision(*enemy->GetTex())) {
		if (status_ == Status::kFalling) {
			isCollisionEnemy_ = true;
			if (isCollisionEnemy_.OnEnter()) {

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

				fallingSpeed_ = kFallingSpeed_;

				if (velocity_.x == 0) {

					velocity_.x = std::sinf(rotateAddAngle_) * UtilsLib::Random(2, 6);

					/*if (std::fabsf(rotateAddAngle_) <= 0.5f) {
						rotateAddAngle_ = rotateAddAngle_ * UtilsLib::Random(3, 6);
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
					rotateAddAngle_ = angle * 6;
				}
				isCollisionLayer_ = false;
			}
		}
		else if (type_ == Type::kWalk && status_ == Status::kNormal && enemy->GetType() == Type::kWalk && enemy->GetStatus() == Status::kNormal) {

		}
	}
	else {
		isCollisionEnemy_ = false;
	}
}

void Enemy::CollisionPlayer(Player* player) {

	if (player->GetStatus() != Player::Status::kFalling && (status_ == Status::kLeave || status_ == Status::kNormal)) {
		if (tex_->Collision(*player->GetTex())) {

			if (player->GetVelocity().y < 0.0f) {
				StatusRequest(Enemy::Status::kFalling);
				fallingSpeed_ = kFallingSpeed_ + player->GetVelocity().y;

				Vector3 vector = player->GetTex()->pos - tex_->pos;

				if (vector.x == 0) {

					if (UtilsLib::Random(0, 1) == 0) {
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
					rotateAddAngle_ = angle * 6;
				}

				player->EnemyStep(true);
			}
			else {

				if (type_ == Type::kWalk) {
					moveVector_ *= -1;
					player->KnockBack(tex_->pos);
				}
				else if (type_ == Type::kFly) {
					if (status_ == Status::kNormal) {
						player->EnemyStep(false);
						player->Steped(tex_->pos);
					}
				}
			}
		}
	}
}

//void Enemy::Initialize() {
//
//}

void Enemy::Update(Layer* layer, const float& y, const Camera* camera) {

	ApplyGlobalVariable();

	if (statusRequest_) {
		status_ = statusRequest_.value();

		switch (status_)
		{
		case Enemy::Status::kGeneration:
			GenerationInitialize(y);
			break;
		case Enemy::Status::kNormal:
			NormalInitialize();
			break;
		case Enemy::Status::kFalling:
			FallingInitialize();
			break;
		case Enemy::Status::kFaint:
			FaintInitialize();
			break;
		case Enemy::Status::kLeave:
			LeaveInitialize();
			break;
		case Enemy::Status::kDeath:
			DeathInitialize(layer);
			break;
		default:
			break;
		}

		statusRequest_ = std::nullopt;
	}

	switch (status_)
	{
	case Enemy::Status::kGeneration:
		GenerationUpdate();
		break;
	case Enemy::Status::kNormal:
		NormalUpdate(y);
		break;
	case Enemy::Status::kFalling:
		FallingUpdate(y);
		break;
	case Enemy::Status::kFaint:
		FaintUpdate(y);
		break;
	case Enemy::Status::kLeave:
		LeaveUpdate(y);
		break;
	case Enemy::Status::kDeath:
		DeathUpdate();
		break;
	default:
		break;
	}

	ModelUpdate(camera);
	tex_->Update();

	isCollisionEnemy_.Update();
	isCollisionLayer_.Update();
}

void Enemy::Collision(const float& y) {
	float posY = tex_->pos.y - tex_->scale.y / 2.0f;

	if (y > posY) {
		tex_->pos.y += y - posY;
		if (type_ == Type::kWalk && status_ == Status::kNormal) {
			normalFallingSpeed_ = 0.0f;
		}
	}
}

void Enemy::ModelUpdate(const Camera* camera)
{

	float ratio = static_cast<float>(Engine::GetInstance()->clientHeight) /
		(std::tanf(camera->fov / 2) * (models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) * 2);

	float indication = 90.0f;

	//models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z = tex_->rotate.z;

	models_[static_cast<uint16_t>(Parts::kMain)]->pos.x = tex_->pos.x / ratio + camera->pos.x - camera->pos.x / ratio;
	models_[static_cast<uint16_t>(Parts::kMain)]->pos.y = tex_->pos.y / ratio + camera->pos.y - camera->pos.y / ratio;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.x = tex_->scale.x / (indication * std::tanf(camera->fov / 2) * 2) *
		(models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) / indication;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.y = tex_->scale.y / (indication * std::tanf(camera->fov / 2) * 2) *
		(models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) / indication;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.z = models_[static_cast<uint16_t>(Parts::kMain)]->scale.y;
	models_[static_cast<uint16_t>(Parts::kMain)]->Update();
	models_[static_cast<uint16_t>(Parts::kDoukasen)]->Update();

}

void Enemy::InitializeFirstMove(int move)
{
	if (firstMoveVector_ != move) {
		firstMoveVector_ = move;
		switch (firstMoveVector_)
		{
		case 0:
			moveVector_ = { 0.0f,0.0f,0.0f };
			break;
		case 1:
			moveVector_ = { -1.0f,0.0f,0.0f };
			break;
		case 2:
			moveVector_ = { 1.0f,0.0f,0.0f };
			break;
		case 3:
			moveVector_ = { 0.0f,1.0f,0.0f };
			break;
		case 4:
			moveVector_ = { 0.0f,-1.0f,0.0f };
			break;
		default:
			firstMoveVector_ = 0;
			moveVector_ = { 0.0f,0.0f,0.0f };
			break;
		}
		tex_->pos = firstPos_;
	}
}

void Enemy::InitializeFirstMove() {
	switch (firstMoveVector_)
	{
	case 0:
		moveVector_ = { 0.0f,0.0f,0.0f };
		break;
	case 1:
		moveVector_ = { -1.0f,0.0f,0.0f };
		break;
	case 2:
		moveVector_ = { 1.0f,0.0f,0.0f };
		break;
	case 3:
		moveVector_ = { 0.0f,1.0f,0.0f };
		break;
	case 4:
		moveVector_ = { 0.0f,-1.0f,0.0f };
		break;
	default:
		firstMoveVector_ = 0;
		moveVector_ = { 0.0f,0.0f,0.0f };
		break;
	}
}

void Enemy::InitializeIsHealer(int isHealer)
{
	if (isHealer == 0) {
		isHealer_ = false;
	}
	else {
		isHealer_ = true;
	}
}

void Enemy::InitializeMoveRadius(float radius)
{
	if (radius < 5) {
		moveRadius_ = 1280.0f;
	}
	else {
		if (moveRadius_ != radius) {
			moveRadius_ = radius;
			tex_->pos = firstPos_;
		}
	}
}

void Enemy::GenerationInitialize(const float y) {

	tex_->pos = firstPos_;
	Collision(y);

	switch (type_)
	{
	case Enemy::Type::kFly:
		models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z = 0.0f;
		break;
	case Enemy::Type::kWalk:
		models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z = std::numbers::pi_v<float>;
		break;
	case Enemy::Type::kEnd:
		break;
	default:
		break;
	}

	timeCount_ = 0;

	InitializeFirstMove();
	
}

void Enemy::GenerationUpdate() {

	timeCount_ += FrameInfo::GetInstance()->GetDelta();

	float t = timeCount_ / kGenerationTime_;

	tex_->scale = Vector2(enemyScale_, enemyScale_) * t + Vector2(0.0f, 0.0f) * (1.0f - t);

	if (timeCount_ >= kGenerationTime_) {
		statusRequest_ = Status::kNormal;
		timeCount_ = 0;
		tex_->scale = { enemyScale_, enemyScale_ };
	}
}

void Enemy::NormalInitialize() {

	normalFallingSpeed_ = 0.0f;
}

void Enemy::NormalUpdate(const float y) {



	velocity_ = moveVector_ * kMoveSpeed_;

	if (type_ == Type::kWalk) {
		normalFallingSpeed_ += kFallingSpeed_;
		velocity_.y = normalFallingSpeed_;
	}

	tex_->pos += velocity_ * FrameInfo::GetInstance()->GetDelta();

	Collision(y);

	if (firstMoveVector_ == 1 || firstMoveVector_ == 2) {
		if (tex_->pos.x <= firstPos_.x - moveRadius_ || tex_->pos.x >= firstPos_.x + moveRadius_) {
			moveVector_ *= -1;
		}
		else if (tex_->pos.x - tex_->scale.x <= -640 || tex_->pos.x + tex_->scale.x >= 640) {
			moveVector_ *= -1;
		}
	}
	else if (firstMoveVector_ == 3 || firstMoveVector_ == 4) {
		if (tex_->pos.y <= firstPos_.y - moveRadius_ || tex_->pos.y >= firstPos_.y + moveRadius_) {
			moveVector_ *= -1;
		}
	}

}

void Enemy::FallingInitialize() {

}

void Enemy::FallingUpdate(const float& y) {
	
	velocity_.y += fallingSpeed_;

	tex_->pos += velocity_ * FrameInfo::GetInstance()->GetDelta();

	if (isCollisionLayer_.OnStay()) {

		rotateTimeCount_ += FrameInfo::GetInstance()->GetDelta();

		float t = std::clamp<float>(rotateTimeCount_, 0.0f, rotateTime_) / rotateTime_;

		models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z = std::lerp(startRotate_, endRotate_, t);
	}
	else {
		models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z += rotateAddAngle_ * FrameInfo::GetInstance()->GetDelta();
	}


	if (tex_->pos.y - tex_->scale.y / 2.0f <= y) {

		Collision(y);

		if (isCollisionLayer_.OnStay()) {
			velocity_.y = 0.0f;
			velocity_.x = 0.0f;
			statusRequest_ = Status::kFaint;
			isCollisionLayer_ = false;
			models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z = std::numbers::pi_v<float>;
		}
		else {
			
			isCollisionLayer_ = true;

			fallingSpeed_ = kFallingSpeed_;
			velocity_.y = std::fabsf(velocity_.y) * kLayerReboundCoefficient_;

			rotateTime_ = 2.0f * velocity_.y / (-fallingSpeed_) * FrameInfo::GetInstance()->GetDelta();
			startRotate_ = models_[static_cast<uint16_t>(Parts::kMain)]->rotate.z;

			float pi = std::numbers::pi_v<float>;
			float memo = startRotate_ / 2.0f / pi;
			float moveSpeed = 100.0f;
			if (startRotate_ >= 0) {
				endRotate_ = pi + 2.0f * pi * (static_cast<int>(memo));
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
				endRotate_ = -pi - 2.0f * pi * (static_cast<int>(memo));
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

void Enemy::FaintInitialize() {

	timeCount_ = 0.0f;
}

void Enemy::FaintUpdate(const float& y) {

	velocity_.y += kFallingSpeed_;

	tex_->pos += velocity_ * FrameInfo::GetInstance()->GetDelta();

	if (tex_->pos.y - tex_->scale.y / 2.0f <= y) {
		Collision(y);
		velocity_.y = 0.0f;
	}

	timeCount_ += FrameInfo::GetInstance()->GetDelta();

	if (timeCount_ >= kFaintTime_) {
		if (type_ == Type::kFly) {
			statusRequest_ = Status::kLeave;
		}
		else if (type_ == Type::kWalk) {
			statusRequest_ = Status::kNormal;
		}
	}

}

void Enemy::LeaveInitialize() {
	timeCount_ = 0;
}

void Enemy::LeaveUpdate(const float& y) {
	velocity_.y += kFallingSpeed_;

	tex_->pos += velocity_ * FrameInfo::GetInstance()->GetDelta();

	if (tex_->pos.y - tex_->scale.y / 2.0f <= y) {
		Collision(y);
		velocity_.y = 0.0f;
	}

	timeCount_ += FrameInfo::GetInstance()->GetDelta();

	if (timeCount_ >= kLeaveTime_) {
		statusRequest_ = Status::kGeneration;
		timeCount_ = 0.0f;
	}
}

void Enemy::DeathInitialize(Layer* layer) {
	layer->AddDamage(1);
}

void Enemy::DeathUpdate() {

	timeCount_ += FrameInfo::GetInstance()->GetDelta();

	float t = timeCount_ / kDeathTime_;

	tex_->scale = Vector2(enemyScale_, enemyScale_) * (1.0f - t) + Vector2(0.0f, 0.0f) * t;

	if (timeCount_ >= kDeathTime_) {
		statusRequest_ = Status::kGeneration;
		tex_->scale = {};

	}
}

void Enemy::Draw(const Mat4x4& viewProjection, const Vector3& cameraPos) {
	for (const std::unique_ptr<Model>& model : models_) {
		model->Draw(viewProjection, cameraPos);
	}
}

void Enemy::Draw2D(const Mat4x4& viewProjection) {

	tex_->Draw(viewProjection, Pipeline::Normal, false);
}
