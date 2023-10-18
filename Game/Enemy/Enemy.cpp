#include "Enemy.h"

#include "Engine/FrameInfo/FrameInfo.h"
#include "Utils/Camera/Camera.h"
#include "Game/Layer/Layer.h"
#include <numbers>

std::unique_ptr<GlobalVariables> Enemy::globalVariables_ = std::make_unique<GlobalVariables>();

float Enemy::kFallingSpeed_ = -9.8f;

const std::string Enemy::groupName_ = "StaticEnemy";

Enemy::Enemy(int type, const Vector3& pos, const float& layerY, float scale) {

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
		tex_->rotate.z = 3.14f;

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

void Enemy::SetParametar(int type, const Vector3& pos, const float& y) {

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

	float ratio = static_cast<float>(Engine::GetInstance()->clientHeight) /
		(std::tanf(camera->fov / 2) * (models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) * 2);

	float indication = 90.0f;

	models_[static_cast<uint16_t>(Parts::kMain)]->pos.x = tex_->pos.x / ratio + camera->pos.x - camera->pos.x / ratio;
	models_[static_cast<uint16_t>(Parts::kMain)]->pos.y = tex_->pos.y / ratio + camera->pos.y - camera->pos.y / ratio;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.x = tex_->scale.x / (indication * std::tanf(camera->fov / 2) * 2) *
		(models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) / indication;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.y = tex_->scale.y / (indication * std::tanf(camera->fov / 2) * 2) *
		(models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) / indication;
	models_[static_cast<uint16_t>(Parts::kMain)]->Update();
	models_[static_cast<uint16_t>(Parts::kDoukasen)]->Update();
	tex_->Update();
}

void Enemy::Collision(const float& y) {
	float posY = tex_->pos.y - tex_->scale.y / 2.0f;

	if (y > posY) {
		tex_->pos.y += y - posY;
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

	velocity_.y += kFallingSpeed_ * FrameInfo::GetInstance()->GetDelta();

	tex_->pos += velocity_;

	if (tex_->pos.y - tex_->scale.y / 2.0f <= y) {

		Collision(y);
		velocity_.y = 0.0f;
		statusRequest_ = Status::kFaint;
	}
	
}

void Enemy::FaintInitialize() {

}

void Enemy::FaintUpdate(const float& y) {

	velocity_.y += kFallingSpeed_ * FrameInfo::GetInstance()->GetDelta();

	tex_->pos += velocity_;

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
