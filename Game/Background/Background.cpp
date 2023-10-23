#include "Background.h"

#include "Game/ShockWave/ShockWave.h"

Background::Background() {

	for (int i = 0; i < kTexNum_; i++) {
		textures_.push_back(std::make_unique<Texture2D>());
		textures_[i]->scale = { 1300,1000 };
	}

	textures_[0]->LoadTexture("./Resources/Background/backGra_3.png");
	textures_[1]->LoadTexture("./Resources/Background/backGra_2.png");
	textures_[2]->LoadTexture("./Resources/Background/backGra_1.png");

	textures_[2]->pos.y = ShockWave::GetHighCriteria(static_cast<int>(ShockWave::Size::kSmall)) - textures_[2]->scale.y / 2.0f;
	textures_[1]->pos.y = ShockWave::GetHighCriteria(static_cast<int>(ShockWave::Size::kMiddle)) - textures_[1]->scale.y / 2.0f;
	textures_[0]->pos.y = ShockWave::GetHighCriteria(static_cast<int>(ShockWave::Size::kMiddle)) + textures_[2]->scale.y / 2.0f - 2.0f;

	for (std::unique_ptr<Texture2D>& tex : textures_) {
		tex->Update();
	}
}

void Background::Initialize() {

}

void Background::Update() {

	textures_[2]->pos.y = ShockWave::GetHighCriteria(static_cast<int>(ShockWave::Size::kSmall)) - textures_[2]->scale.y / 2.0f;
	textures_[1]->pos.y = ShockWave::GetHighCriteria(static_cast<int>(ShockWave::Size::kMiddle)) - textures_[1]->scale.y / 2.0f;
	textures_[0]->pos.y = ShockWave::GetHighCriteria(static_cast<int>(ShockWave::Size::kMiddle)) + textures_[2]->scale.y / 2.0f - 2.0f;

	for (std::unique_ptr<Texture2D>& tex : textures_) {
		tex->Update();
	}
}

void Background::Draw2D(const Mat4x4& viewProjection) {

	for (std::unique_ptr<Texture2D>& tex : textures_) {
		tex->Draw(viewProjection, Pipeline::Normal, false);
	}

}