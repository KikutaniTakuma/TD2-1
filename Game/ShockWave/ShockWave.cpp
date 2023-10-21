#include "ShockWave.h"

#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>

std::unique_ptr<GlobalVariables> ShockWave::globalVariables_ = std::make_unique<GlobalVariables>();

float ShockWave::kSpeed_[static_cast<uint16_t>(Size::kEnd)] = {};

Vector2 ShockWave::kSize_[static_cast<uint16_t>(Size::kEnd)] = {};

float ShockWave::kHighCriteria_[static_cast<uint16_t>(Size::kEnd)] = {};

int ShockWave::kDeleteFrame_[static_cast<uint16_t>(Size::kEnd)] = {};

const std::string ShockWave::typeNames_[static_cast<uint16_t>(Size::kEnd)] = {
	"Small",
	"Middle",
	"Major"
};

const std::string ShockWave::groupName_ = "StaticShockWave";

ShockWave::ShockWave(const Vector3& pos, float highest, float layerY) {

	//SetGlobalVariable();

	for (int i = 0; i < kWaveNum_; i++) {
		textures_.push_back(std::make_unique<Texture2D>());
	}

	if (highest < kHighCriteria_[static_cast<uint16_t>(Size::kSmall)]) {
		type_ = Size::kSmall;
	}
	else if (highest < kHighCriteria_[static_cast<uint16_t>(Size::kMiddle)]) {
		type_ = Size::kMiddle;
	}
	else {
		type_ = Size::kMajor;
	}

	/*if (highest >= kHighCriteria_[static_cast<uint16_t>(Size::kSmall)] - layerY && highest < kHighCriteria_[static_cast<uint16_t>(Size::kMiddle)] - layerY) {
		type_ = Size::kSmall;
	}
	else if (highest >= kHighCriteria_[static_cast<uint16_t>(Size::kMiddle)] - layerY && highest < kHighCriteria_[static_cast<uint16_t>(Size::kMajor)] - layerY) {
		type_ = Size::kMiddle;
	}
	else if (highest >= kHighCriteria_[static_cast<uint16_t>(Size::kMajor)] - layerY) {
		type_ = Size::kMajor;
	}
	else {
		type_ = Size::kSmall;
	}*/

	int i = 0;

	for (std::unique_ptr<Texture2D>& tex : textures_) {
		tex->LoadTexture("./Resources/ShockWave/player_syogekiha.png");
		tex->scale = kSize_[static_cast<uint16_t>(type_)];
		tex->pos = pos;
		if (i % 2 == 1) {
			tex->rotate.z = std::numbers::pi_v<float>;
		}
		tex->Update();
		i++;
	}

	Collision(layerY);

	deleteCount_ = 0;
	isDelete_ = false;

}

ShockWave::~ShockWave() {
	textures_.clear();
}

void ShockWave::Finalize() {

	textures_.clear();
}

void ShockWave::Collision(const float& y) {

	for (std::unique_ptr<Texture2D>& tex : textures_) {
		float posY = tex->pos.y - tex->scale.y / 2.0f;

		tex->pos.y += y - posY;
	}
}

void ShockWave::SetGlobalVariable() {

	globalVariables_->CreateGroup(groupName_);

	for (int i = 0; i < static_cast<int>(Size::kEnd); i++) {

		std::string item = "kSpeed";
		item = item + typeNames_[i];
		globalVariables_->AddItem(groupName_, item, kSpeed_[i]);

		item = "kSize";
		item = item + typeNames_[i];
		globalVariables_->AddItem(groupName_, item, kSize_[i]);

		item = "kHighCriteria";
		item = item + typeNames_[i];
		globalVariables_->AddItem(groupName_, item, kHighCriteria_[i]);

		item = "kDeleteFrame";
		item = item + typeNames_[i];
		globalVariables_->AddItem(groupName_, item, kDeleteFrame_[i]);
	}

	/*globalVariables_->AddItem(groupName_, "kSpeedSmall", kSpeed_[static_cast<uint16_t>(Size::kSmall)]);
	globalVariables_->AddItem(groupName_, "kSpeedMiddle", kSpeed_[static_cast<uint16_t>(Size::kMiddle)]);
	globalVariables_->AddItem(groupName_, "kSpeedMajor", kSpeed_[static_cast<uint16_t>(Size::kMajor)]);

	globalVariables_->AddItem(groupName_, "kSizeSmall", kSize_[static_cast<uint16_t>(Size::kSmall)]);
	globalVariables_->AddItem(groupName_, "kSizeMiddle", kSize_[static_cast<uint16_t>(Size::kMiddle)]);
	globalVariables_->AddItem(groupName_, "kSizeMajor", kSize_[static_cast<uint16_t>(Size::kMajor)]);

	globalVariables_->AddItem(groupName_, "kHighCriteriaSmall", kHighCriteria_[static_cast<uint16_t>(Size::kSmall)]);
	globalVariables_->AddItem(groupName_, "kHighCriteriaMiddle", kHighCriteria_[static_cast<uint16_t>(Size::kMiddle)]);
	globalVariables_->AddItem(groupName_, "kHighCriteriaMajor", kHighCriteria_[static_cast<uint16_t>(Size::kMajor)]);

	globalVariables_->AddItem(groupName_, "kDeleteFrame", kDeleteFrame_);*/

	globalVariables_->LoadFile(groupName_);
	ApplyGlobalVariable();
}

void ShockWave::ApplyGlobalVariable() {

	for (int i = 0; i < static_cast<int>(Size::kEnd); i++) {

		std::string item = "kSpeed";
		item = item + typeNames_[i];
		kSpeed_[i] = globalVariables_->GetFloatValue(groupName_, item);

		item = "kSize";
		item = item + typeNames_[i];
		kSize_[i] = globalVariables_->GetVector2Value(groupName_, item);

		item = "kHighCriteria";
		item = item + typeNames_[i];
		kHighCriteria_[i] = globalVariables_->GetFloatValue(groupName_, item);

		item = "kDeleteFrame";
		item = item + typeNames_[i];
		kDeleteFrame_[i] = globalVariables_->GetIntValue(groupName_, item);
	}

	/*kSpeed_ = globalVariables_->GetFloatValue(groupName_, "kSpeed");

	kDeleteFrame_ = globalVariables_->GetIntValue(groupName_, "kDeleteFrame");

	kSize_[static_cast<uint16_t>(Size::kSmall)] = globalVariables_->GetFloatValue(groupName_, "kSizeSmall");
	kSize_[static_cast<uint16_t>(Size::kMiddle)] = globalVariables_->GetFloatValue(groupName_, "kSizeMiddle");
	kSize_[static_cast<uint16_t>(Size::kMajor)] = globalVariables_->GetFloatValue(groupName_, "kSizeMajor");

	kHighCriteria_[static_cast<uint16_t>(Size::kSmall)] = globalVariables_->GetFloatValue(groupName_, "kHighCriteriaSmall");
	kHighCriteria_[static_cast<uint16_t>(Size::kMiddle)] = globalVariables_->GetFloatValue(groupName_, "kHighCriteriaMiddle");
	kHighCriteria_[static_cast<uint16_t>(Size::kMajor)] = globalVariables_->GetFloatValue(groupName_, "kHighCriteriaMajor");*/

}

void ShockWave::Update() {

	//ApplyGlobalVariable();

	deleteCount_++;

	if (deleteCount_ == kDeleteFrame_[static_cast<int>(type_)]) {
		textures_.clear();
		isDelete_ = true;
	}

	// 左右方向に動かす
	int i = 0;
	for (std::unique_ptr<Texture2D>& tex : textures_) {
		if (i == 0) {
			tex->pos.x += kSpeed_[static_cast<int>(type_)] * FrameInfo::GetInstance()->GetDelta();
		}
		else {
			tex->pos.x -= kSpeed_[static_cast<int>(type_)] * FrameInfo::GetInstance()->GetDelta();
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
