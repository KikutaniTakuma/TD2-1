#include "StageTimer.h"

StageTimer::StageTimer()
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

void StageTimer::Init()
{
	time_ = 0;

	SetNumTeces();
	SetSpriteSize();
	SetSpritePos();
	TecesUpdate();
}

void StageTimer::Update()
{
	globalVariables_->Update();

	ApplyGlobalVariable();

	SetNumTeces();
	SetSpritePos();
	SetSpriteSize();

	TecesUpdate();
}

void StageTimer::Draw2D(const Mat4x4& viewProjection)
{
	for (const std::unique_ptr<Texture2D>& tex : teces_) {
		tex->Draw(viewProjection, Pipeline::Normal, false);
	}

	for (const std::unique_ptr<Texture2D>& tex : numTeces_) {
		tex->Draw(viewProjection, Pipeline::Normal, false);
	}
}

void StageTimer::SetGlobalVariable()
{
	globalVariables_->CreateGroup(groupName_);

	for (int i = 0; i < V2ItemNames::kV2ItemCount; i++) {
		globalVariables_->AddItem(groupName_, v2ItemNames_[i], v2Info_[i]);
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		globalVariables_->AddItem(groupName_, fInfoNames_[i], fInfo_[i]);
	}

	globalVariables_->LoadFile(groupName_);
	ApplyGlobalVariable();
}

void StageTimer::ApplyGlobalVariable()
{
	for (int i = 0; i < V2ItemNames::kV2ItemCount; i++) {
		v2Info_[i] = globalVariables_->GetVector2Value(groupName_, v2ItemNames_[i]);
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		fInfo_[i] = globalVariables_->GetFloatValue(groupName_, fInfoNames_[i]);
	}
}

void StageTimer::SetNumTeces()
{
	if (playTime_) {
		time_ = static_cast<int>(*playTime_);
	}

	int num = time_;
	int drawNum = 0;

	drawNum = num / 60;

	if (drawNum >= 10) {
		numTeces_[0]->uvPibot.x = 0.9f;
		numTeces_[1]->uvPibot.x = 0.5f;
		numTeces_[2]->uvPibot.x = 0.9f;

		return;
	}

	num = num % 60;

	numTeces_[0]->uvPibot.x = static_cast<float>(drawNum) / 10.0f;

	for (int i = 0; i < MaxDigits; i++) {
		drawNum = num / int(pow(10, MaxDigits - 1 - i));
		num = num % int(pow(10, MaxDigits - 1 - i));

		numTeces_[i + 1]->uvPibot.x = static_cast<float>(drawNum) / 10.0f;
	}
}

void StageTimer::SetSpriteSize()
{
	for (const std::unique_ptr<Texture2D>& tex : teces_) {
		tex->scale = { fInfo_[FInfoNames::kTexScale],fInfo_[FInfoNames::kTexScale] };
	}

	for (const std::unique_ptr<Texture2D>& tex : numTeces_) {
		tex->scale = { fInfo_[FInfoNames::kNumScale],fInfo_[FInfoNames::kNumScale] };
	}
}

void StageTimer::SetSpritePos()
{
	teces_[0]->pos = v2Info_[V2ItemNames::kTexPos];
	teces_[1]->pos = v2Info_[V2ItemNames::kTexPos];
	teces_[0]->pos.y += fInfo_[FInfoNames::kTecesInterval];
	teces_[1]->pos.y -= fInfo_[FInfoNames::kTecesInterval];

	numTeces_[0]->pos = { fInfo_[FInfoNames::kMinutesPosX],fInfo_[FInfoNames::kNumPosY] };
	numTeces_[1]->pos = { fInfo_[FInfoNames::kSecondPosX] - fInfo_[FInfoNames::kSecondInterval],fInfo_[FInfoNames::kNumPosY] };
	numTeces_[2]->pos = { fInfo_[FInfoNames::kSecondPosX] + fInfo_[FInfoNames::kSecondInterval],fInfo_[FInfoNames::kNumPosY] };
}

void StageTimer::TecesUpdate()
{
	for (const std::unique_ptr<Texture2D>& tex : teces_) {
		tex->Update();
	}

	for (const std::unique_ptr<Texture2D>& tex : numTeces_) {
		tex->Update();
	}
}
