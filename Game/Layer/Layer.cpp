#include "Layer.h"

std::unique_ptr<GlobalVariables> Layer::globalVariables_ = std::make_unique<GlobalVariables>();

Vector2 Layer::kLayer2DScale_ = { 1300.0f,80.0f };

float Layer::kFirstLayerCenterPosY_ = -40.0f;

const std::string Layer::groupName_ = "staticLayer";

Layer::Layer(int kMaxLayerNum, const std::vector<int>& kMaxHitPoints) {

	kMaxLayerNum_ = kMaxLayerNum;
	kMaxHitPoints_ = kMaxHitPoints;
	hitPoints_ = kMaxHitPoints_;
	nowLayer_ = 0;

	for (int i = 0; i < kMaxLayerNum_; i++) {
		tex_.push_back(std::make_unique<Texture2D>());
		tex_[i]->scale = kLayer2DScale_;
		tex_[i]->pos = { 0.0f, kFirstLayerCenterPosY_ + (i * (-kLayer2DScale_.y)) };
		tex_[i]->LoadTexture("./Resources/uvChecker.png");

		tex_[i]->Update();
	}

	isChangeLayer_ = false;
	isClear_ = false;

	SetGlobalVariable();
}

void Layer::SetParametar(std::vector<int> kMaxHitPoints) {
	kMaxHitPoints_ = kMaxHitPoints;
}

void Layer::SetGlobalVariable() {

	globalVariables_->CreateGroup(groupName_);

	globalVariables_->AddItem(groupName_, "kLayer2DScale", kLayer2DScale_);

	globalVariables_->AddItem(groupName_, "kFirstLayerCenterPosY", kFirstLayerCenterPosY_);

	globalVariables_->LoadFile(groupName_);
	ApplyGlobalVariable();

}

void Layer::ApplyGlobalVariable() {

	kLayer2DScale_ = globalVariables_->GetVector2Value(groupName_, "kLayer2DScale");

	kFirstLayerCenterPosY_ = globalVariables_->GetFloatValue(groupName_, "kFirstLayerCenterPosY");

	if (static_cast<int>(tex_.size()) != kMaxLayerNum_) {
		tex_.clear();
		for (int i = 0; i < kMaxLayerNum_; i++) {
			tex_.push_back(std::make_unique<Texture2D>());
			tex_[i]->scale = kLayer2DScale_;
			tex_[i]->pos = { 0.0f, kFirstLayerCenterPosY_ + (i * (-kLayer2DScale_.y)) };
			tex_[i]->LoadTexture("./Resources/uvChecker.png");

			tex_[i]->Update();
		}
	}
	else {
		for (int i = 0; i < kMaxLayerNum_; i++) {

			tex_[i]->scale = kLayer2DScale_;
			tex_[i]->pos = { 0.0f, kFirstLayerCenterPosY_ + (i * (-kLayer2DScale_.y)) };

			tex_[i]->Update();
		}
	}
}

//void Layer::Initialize() {
//
//	hitPoints_ = kMaxHitPoints_;
//	nowLayer_ = 0;
//
//	for (int i = 0; i < kMaxLayerNum_; i++) {
//
//		tex_[i]->scale = kLayer2DScale_;
//		tex_[i]->pos = { 0.0f, kFirstLayerCenterPosY_ + (i * (-kLayer2DScale_.y)) };
//		tex_[i]->Update();
//	}
//}

void Layer::Initialize(int kMaxLayerNum, const std::vector<int>& kMaxHitPoints) {

	kMaxLayerNum_ = kMaxLayerNum;
	kMaxHitPoints_ = kMaxHitPoints;

	hitPoints_ = kMaxHitPoints_;
	nowLayer_ = 0;

	tex_.clear();

	for (int i = 0; i < kMaxLayerNum_; i++) {
		tex_.push_back(std::make_unique<Texture2D>());
		tex_[i]->scale = kLayer2DScale_;
		tex_[i]->pos = { 0.0f, kFirstLayerCenterPosY_ + (i * (-kLayer2DScale_.y)) };
		tex_[i]->LoadTexture("./Resources/uvChecker.png");

		tex_[i]->Update();
	}
}

void Layer::Update() {

	ApplyGlobalVariable();

	for (int i = 0; i < kMaxLayerNum_; i++) {
		tex_[i]->Update();
	}
}

//void Layer::Draw(const Mat4x4& viewProjection) {
//
//}

void Layer::Draw2D(const Mat4x4& viewProjection) {
	
	for (int i = 0; i < kMaxLayerNum_; i++) {
		if (hitPoints_[i] != 0) {
			tex_[i]->Draw(viewProjection, Pipeline::Normal, false);
		}
	}
}
