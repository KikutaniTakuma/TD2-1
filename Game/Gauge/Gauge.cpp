#include "Gauge.h"

#include <algorithm>

Gauge::Gauge() {

	globalVariables_ = std::make_unique<GlobalVariables>();

	for (int i = 0; i < static_cast<int>(TextureNames::kEnd); i++) {
		textures_.push_back(std::make_unique<Texture2D>());
		textures_[i]->LoadTexture("./Resources/white2x2.png");
	}

	textures_[static_cast<int>(TextureNames::kMostBack)]->color = 0x111111FF;
	textures_[static_cast<int>(TextureNames::kGaugeBack)]->color = 0x444444FF;
	textures_[static_cast<int>(TextureNames::kGaugeMain)]->color = 0x33FF99FF;

	num_ = 1;
	kMax_ = 1;

	SetGlobalVariable();
}

void Gauge::SetGlobalVariable()
{
	globalVariables_->CreateGroup(groupName_);

	globalVariables_->AddItem(groupName_, "kPos", textures_[static_cast<int>(TextureNames::kMostBack)]->pos);
	globalVariables_->AddItem(groupName_, "kAllScale", textures_[static_cast<int>(TextureNames::kMostBack)]->scale);
	globalVariables_->AddItem(groupName_, "kGaugeScale", textures_[static_cast<int>(TextureNames::kGaugeBack)]->scale);

	globalVariables_->LoadFile(groupName_);
	ApplyGlobalVariable();
}

void Gauge::ApplyGlobalVariable()
{
	textures_[static_cast<int>(TextureNames::kMostBack)]->pos = globalVariables_->GetVector3Value(groupName_, "kPos");
	textures_[static_cast<int>(TextureNames::kMostBack)]->scale = globalVariables_->GetVector2Value(groupName_, "kAllScale");
	textures_[static_cast<int>(TextureNames::kGaugeBack)]->scale = globalVariables_->GetVector2Value(groupName_, "kGaugeScale");

	textures_[static_cast<int>(TextureNames::kGaugeBack)]->pos = textures_[static_cast<int>(TextureNames::kMostBack)]->pos;

}

void Gauge::DamageUpdate(const int& damage)
{

	float t = 0.0f;

	if (damage > 0) {

		int num = 0;
		if (num_ < damage) {
			num = num_;
		}
		else {
			num = damage;
		}

		if (num_ == 0) {
			t = 1.0f;
		}
		else {
			t = float(num) / num_;
		}

		/*std::unique_ptr<Texture2D> damageTex = std::make_unique<Texture2D>();
		damageTex->pos = 
			damageTex->color = 0xFF4444FF;

		damageGaugeTextures_.push_back(damageTex);*/


	}

	t = float(num_) / kMax_;

	textures_[static_cast<int>(TextureNames::kGaugeMain)]->pos.x = t * textures_[static_cast<int>(TextureNames::kGaugeBack)]->pos.x +
		(1.0f - t) * (textures_[static_cast<int>(TextureNames::kGaugeBack)]->pos.x + textures_[static_cast<int>(TextureNames::kGaugeBack)]->scale.x / 2.0f);
	textures_[static_cast<int>(TextureNames::kGaugeMain)]->scale.x = t * textures_[static_cast<int>(TextureNames::kGaugeBack)]->scale.x;

	textures_[static_cast<int>(TextureNames::kGaugeMain)]->pos.y = textures_[static_cast<int>(TextureNames::kGaugeBack)]->pos.y;
	textures_[static_cast<int>(TextureNames::kGaugeMain)]->scale.y = textures_[static_cast<int>(TextureNames::kGaugeBack)]->scale.y;
}

void Gauge::Initialize(const int& num, const int& Max)
{
	num_ = num;
	kMax_ = Max;

	textures_[static_cast<int>(TextureNames::kGaugeBack)]->pos = textures_[static_cast<int>(TextureNames::kMostBack)]->pos;
	textures_[static_cast<int>(TextureNames::kGaugeMain)]->pos = textures_[static_cast<int>(TextureNames::kGaugeBack)]->pos;
	textures_[static_cast<int>(TextureNames::kGaugeMain)]->scale = textures_[static_cast<int>(TextureNames::kGaugeBack)]->scale;

}

void Gauge::Update(const int& num, const int& Max, const int& damage)
{
	globalVariables_->Update();

	ApplyGlobalVariable();

	num_ = num;
	kMax_ = Max;

	DamageUpdate(damage);

	for (std::unique_ptr<Texture2D>& tex : damageGaugeTextures_) {
		tex->Update();
	}

	for (std::unique_ptr<Texture2D>& tex : textures_) {
		tex->Update();
	}
}

void Gauge::Draw2D(const Mat4x4& viewProjection)
{
	for (std::unique_ptr<Texture2D>& tex : damageGaugeTextures_) {
		tex->Draw(viewProjection, Pipeline::Normal, false);
	}

	for (std::unique_ptr<Texture2D>& tex : textures_) {
		tex->Draw(viewProjection, Pipeline::Normal, false);
	}
}


