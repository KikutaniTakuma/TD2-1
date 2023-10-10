#include "ShockWave.h"

#include "Engine/FrameInfo/FrameInfo.h"

std::unique_ptr<GlobalVariables> ShockWave::globalVariables_ = std::make_unique<GlobalVariables>();

float ShockWave::kSpeed_ = 50.0f;

float ShockWave::kSize_[static_cast<uint16_t>(Size::kEnd)];

float ShockWave::kHighCriteria_[static_cast<uint16_t>(Size::kEnd)];

int ShockWave::kDeleteFrame_ = 300;

const std::string ShockWave::groupName_ = "StaticShockWave";

ShockWave::ShockWave(const Vector3& pos, float highest) {

	SetGlobalVariable();

	for (int i = 0; i < kWaveNum_; i++) {
		textures_.push_back(std::make_unique<Texture2D>());
	}
	for (std::unique_ptr<Texture2D>& tex : textures_) {
		tex->LoadTexture("./Resources/uvChecker.png");
		tex->pos = pos;
		if (highest >= kHighCriteria_[static_cast<uint16_t>(Size::kSmall)] && highest < kHighCriteria_[static_cast<uint16_t>(Size::kMiddle)]) {
			tex->scale *= kSize_[static_cast<uint16_t>(Size::kSmall)];
		}
		else if (highest >= kHighCriteria_[static_cast<uint16_t>(Size::kMiddle)] && highest < kHighCriteria_[static_cast<uint16_t>(Size::kMajor)]) {
			tex->scale *= kSize_[static_cast<uint16_t>(Size::kMiddle)];
		}
		else if (highest >= kHighCriteria_[static_cast<uint16_t>(Size::kMajor)]) {
			tex->scale *= kSize_[static_cast<uint16_t>(Size::kMajor)];
		}
		tex->Update();
	}

	deleteCount_ = 0;
	isDelete_ = false;

}

ShockWave::~ShockWave() {
	textures_.clear();
}

void ShockWave::Finalize() {

	textures_.clear();
}

void ShockWave::SetGlobalVariable() {

	globalVariables_->CreateGroup(groupName_);

	globalVariables_->AddItem(groupName_, "kSpeed", kSpeed_);

	globalVariables_->AddItem(groupName_, "kSizeSmall", kSize_[static_cast<uint16_t>(Size::kSmall)]);
	globalVariables_->AddItem(groupName_, "kSizeMiddle", kSize_[static_cast<uint16_t>(Size::kMiddle)]);
	globalVariables_->AddItem(groupName_, "kSizeMajor", kSize_[static_cast<uint16_t>(Size::kMajor)]);

	globalVariables_->AddItem(groupName_, "kHighCriteriaSmall", kHighCriteria_[static_cast<uint16_t>(Size::kSmall)]);
	globalVariables_->AddItem(groupName_, "kHighCriteriaMiddle", kHighCriteria_[static_cast<uint16_t>(Size::kMiddle)]);
	globalVariables_->AddItem(groupName_, "kHighCriteriaMajor", kHighCriteria_[static_cast<uint16_t>(Size::kMajor)]);

	globalVariables_->AddItem(groupName_, "kDeleteFrame", kDeleteFrame_);

	globalVariables_->LoadFile(groupName_);
	ApplyGlobalVariable();
}

void ShockWave::ApplyGlobalVariable() {

	kSpeed_ = globalVariables_->GetFloatValue(groupName_, "kSpeed");

	kDeleteFrame_ = globalVariables_->GetIntValue(groupName_, "kDeleteFrame");

	kSize_[static_cast<uint16_t>(Size::kSmall)] = globalVariables_->GetFloatValue(groupName_, "kSizeSmall");
	kSize_[static_cast<uint16_t>(Size::kMiddle)] = globalVariables_->GetFloatValue(groupName_, "kSizeMiddle");
	kSize_[static_cast<uint16_t>(Size::kMajor)] = globalVariables_->GetFloatValue(groupName_, "kSizeMajor");

	kHighCriteria_[static_cast<uint16_t>(Size::kSmall)] = globalVariables_->GetFloatValue(groupName_, "kHighCriteriaSmall");
	kHighCriteria_[static_cast<uint16_t>(Size::kMiddle)] = globalVariables_->GetFloatValue(groupName_, "kHighCriteriaMiddle");
	kHighCriteria_[static_cast<uint16_t>(Size::kMajor)] = globalVariables_->GetFloatValue(groupName_, "kHighCriteriaMajor");

}

void ShockWave::Update() {

	ApplyGlobalVariable();

	deleteCount_++;

	if (deleteCount_ == kDeleteFrame_) {
		textures_.clear();
		isDelete_ = true;
	}

	// 左右方向に動かす
	int i = 0;
	for (std::unique_ptr<Texture2D>& tex : textures_) {
		if (i == 0) {
			tex->pos.x += kSpeed_ * FrameInfo::GetInstance()->GetDelta();
		}
		else {
			tex->pos.x -= kSpeed_ * FrameInfo::GetInstance()->GetDelta();
		}
		i++;
	}

	for (std::unique_ptr<Texture2D>& tex : textures_) {
		tex->Update();
	}
}

//void Player::Draw(const Mat4x4& viewProjection) {
//
//}

void ShockWave::Draw2D(const Mat4x4& viewProjection) {

	for (std::unique_ptr<Texture2D>& tex : textures_) {
		tex->Draw(viewProjection, Pipeline::Normal, false);
	}
}
