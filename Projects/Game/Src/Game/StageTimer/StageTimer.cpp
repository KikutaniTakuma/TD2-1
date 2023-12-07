#include "StageTimer.h"

Timer::Timer()
{
	globalVariables_ = std::make_unique<GlobalVariables>();

	for (std::unique_ptr<Texture2D>& tex : teces_) {
		tex = std::make_unique<Texture2D>();
		tex->LoadTexture("./Resources/ball.png");
		tex->scale = tex->GetTexSize();
	}

	for (std::unique_ptr<Texture2D>& tex : numTeces_) {
		tex = std::make_unique<Texture2D>();
		tex->LoadTexture("./Resources/Result/number.png");
		tex->scale = tex->GetTexSize();
		tex->scale.x /= 10;
		tex->uvSize.x /= 10;
	}

	SetGlobalVariable();
}

void Timer::Init()
{
	time_ = 0;

	SetNumTeces();
	SetSpriteSize();
	TecesUpdate();
}

void Timer::Update()
{
	ApplyGlobalVariable();

	SetNumTeces();
	SetSpriteSize();

	TecesUpdate();
}

void Timer::Draw2D(const Mat4x4& viewProjection)
{
	for (const std::unique_ptr<Texture2D>& tex : teces_) {
		tex->Draw(viewProjection, Pipeline::Normal, false);
	}

	for (const std::unique_ptr<Texture2D>& tex : numTeces_) {
		tex->Draw(viewProjection, Pipeline::Normal, false);
	}
}

void Timer::SetGlobalVariable()
{
	globalVariables_->CreateGroup(groupName_);

	for (int i = 0; i < V2ItemNames::kV2ItemCount; i++) {
		globalVariables_->AddItem(groupName_, v2ItemNames_[i], v2Info_[i]);
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		globalVariables_->AddItem(groupName_, fInfoNames_[i], fInfo_[i]);
	}

	ApplyGlobalVariable();
}

void Timer::ApplyGlobalVariable()
{
	for (int i = 0; i < V2ItemNames::kV2ItemCount; i++) {
		v2Info_[i] = globalVariables_->GetVector2Value(groupName_, v2ItemNames_[i]);
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		fInfo_[i] = globalVariables_->GetFloatValue(groupName_, fInfoNames_[i]);
	}
}

void Timer::SetNumTeces()
{
	if (playTime_) {
		time_ = static_cast<int>(*playTime_);
	}


}

void Timer::SetSpriteSize()
{
}

void Timer::TecesUpdate()
{
	for (const std::unique_ptr<Texture2D>& tex : teces_) {
		tex->Update();
	}

	for (const std::unique_ptr<Texture2D>& tex : numTeces_) {
		tex->Update();
	}
}
