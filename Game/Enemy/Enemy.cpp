#include "Enemy.h"

#include "Engine/FrameInfo/FrameInfo.h"

#include "Game/Layer/Layer.h"

std::unique_ptr<GlobalVariables> Enemy::globalVariables_ = std::make_unique<GlobalVariables>();

float Enemy::kFallingSpeed_ = -9.8f;

const std::string Enemy::groupName_ = "StaticEnemy";

Enemy::Enemy(const Vector3& pos, const float& layerY, float scale) {

	tex_ = std::make_unique<Texture2D>();
	tex_->LoadTexture("./Resources/Enemy/usabom.png");

	firstPos_ = pos;
	firstPos_.y += layerY;
	velocity_ = {};

	tex_->pos = pos;
	tex_->scale *= scale;

	status_ = Status::kNormal;

	statusRequest_ = std::nullopt;
	
	SetGlobalVariable();

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

void Enemy::SetParametar(const Vector3& pos, const float& y) {

	firstPos_ = pos;
	firstPos_.y += y;
}



//void Enemy::Initialize() {
//
//}

void Enemy::Update(Layer* layer, const float& y) {

	ApplyGlobalVariable();

	if (statusRequest_) {
		status_ = statusRequest_.value();

		switch (status_)
		{
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
	case Enemy::Status::kNormal:
		NormalUpdate();
		break;
	case Enemy::Status::kFalling:
		FallingUpdate(y);
		break;
	case Enemy::Status::kFaint:
		FaintUpdate();
		break;
	case Enemy::Status::kDeath:
		DeathUpdate();
		break;
	default:
		break;
	}

	tex_->Update();
}

void Enemy::Collision(const float& y) {
	float posY = tex_->pos.y - tex_->scale.y / 2.0f;

	if (y > posY) {
		tex_->pos.y += y - posY;
	}
}

void Enemy::NormalInitialize() {

	tex_->pos = firstPos_;

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

void Enemy::FaintUpdate() {

	// 仮で落ちたら初期値に戻るようにしている
	//statusRequest_ = Status::kNormal;

}

void Enemy::DeathInitialize(Layer* layer) {
	layer->AddDamage(1);
}

void Enemy::DeathUpdate() {

	statusRequest_ = Status::kNormal;
}

//void Enemy::Draw(const Mat4x4& viewProjection) {
//
//}

void Enemy::Draw2D(const Mat4x4& viewProjection) {

	tex_->Draw(viewProjection, Pipeline::Normal, false);
}
