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

Enemy::Enemy(int type, const Vector3& pos, const float& layerY, int firstMoveVector, int isHealer, float scale) {

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

		tex_->pos = pos;
		tex_->scale *= scale;
		break;
	case Enemy::Type::kWalk:

		tex_->scale *= scale;
		firstPos_ = pos;
		firstPos_.y = layerY + tex_->scale.y / 2.0f;
		velocity_ = {};

		tex_->pos = pos;
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

	globalVariables_->LoadFile(groupName_);
	ApplyGlobalVariable();
}

void Enemy::ApplyGlobalVariable() {

	kFallingSpeed_ = globalVariables_->GetFloatValue(groupName_, "kFallingSpeed");

}

void Enemy::SetParametar(int type, const Vector3& pos, const float& y, int firstMoveVector, int isHealer) {

	if (type == static_cast<int>(Type::kFly)) {
		type_ = Type::kFly;
		firstPos_ = pos;
		
	}
	else if (type == static_cast<int>(Type::kWalk)) {
		type_ = Type::kWalk;
		firstPos_ = pos;
		firstPos_.y = y + tex_->scale.y / 2.0f;
	}
	else {
		type_ = Type::kFly;
		firstPos_ = pos;
		
	}

	InitializeFirstMove(firstMoveVector);

	InitializeIsHealer(isHealer);
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
			player->EnemyStep(false);
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
			GenerationInitialize();
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
		NormalUpdate();
		break;
	case Enemy::Status::kFalling:
		FallingUpdate(y);
		break;
	case Enemy::Status::kFaint:
		FaintUpdate(y);
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
	firstMoveVector_ = move;
	switch (move)
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

void Enemy::GenerationInitialize() {
	tex_->pos = firstPos_;
}

void Enemy::GenerationUpdate() {


	statusRequest_ = Status::kNormal;
}

void Enemy::NormalInitialize() {

	//tex_->pos = firstPos_;

}

void Enemy::NormalUpdate() {

#ifdef _DEBUG
	tex_->pos = firstPos_;
#endif // _DEBUG

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
				endRotate_ = -pi + 2.0f * pi * (static_cast<int>(memo));
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

}

void Enemy::FaintUpdate(const float& y) {

	velocity_.y += kFallingSpeed_;

	tex_->pos += velocity_ * FrameInfo::GetInstance()->GetDelta();

	if (tex_->pos.y - tex_->scale.y / 2.0f <= y) {

		Collision(y);
		velocity_.y = 0.0f;
		
	}

}

void Enemy::DeathInitialize(Layer* layer) {
	layer->AddDamage(1);
}

void Enemy::DeathUpdate() {

	statusRequest_ = Status::kGeneration;
}

void Enemy::Draw(const Mat4x4& viewProjection, const Vector3& cameraPos) {
	for (const std::unique_ptr<Model>& model : models_) {
		model->Draw(viewProjection, cameraPos);
	}
}

void Enemy::Draw2D(const Mat4x4& viewProjection) {

	tex_->Draw(viewProjection, Pipeline::Normal, false);
}
