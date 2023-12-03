#include "Scaffolding.h"
#include "Utils/Camera/Camera.h"
#include "Engine/Core/WindowFactory/WindowFactory.h"

Scaffolding::Scaffolding(const Vector3& pos, const Vector2& scale)
{
	tex_ = std::make_unique<Texture2D>();
	tex_->LoadTexture("./Resources/white2x2.png");

	model_ = std::make_unique<Model>();
	model_->LoadObj("./Resources/Layer/layer.obj");
	model_->light_.ligDirection = { 0.0f,0.0f,1.0f };
	model_->light_.ligColor = { 1.0f,1.0f,1.0f };
	model_->light_.ptRange = 10000.0f;

	tex_->pos_ = pos;
	tex_->scale_ = scale;

	tex_->Update();
}

void Scaffolding::SetParametar(const Vector3& pos, const Vector2& scale, const Camera* camera)
{
	tex_->pos_ = pos;
	tex_->scale_ = scale;

	float ratio = WindowFactory::GetInstance()->GetClientSize().y /
		(std::tanf(camera->fov / 2) * (model_->pos_.z - camera->pos.z) * 2);


	float indication = 1030.0f;
	float indicationY = 200.0f;

	model_->pos_.x = tex_->pos_.x / ratio + camera->pos.x - camera->pos.x / ratio;
	model_->pos_.y = tex_->pos_.y / ratio + camera->pos.y - camera->pos.y / ratio;
	model_->scale_.x = tex_->scale_.x / (indication * std::tanf(camera->fov / 2) * 2) *
		(model_->pos_.z - camera->pos.z) / indication;
	model_->scale_.y = tex_->scale_.y / (indicationY * std::tanf(camera->fov / 2) * 2) *
		(model_->pos_.z - camera->pos.z) / indicationY;
	model_->scale_.z = model_->scale_.y * 40;
	model_->Update();

	tex_->Update();
}

void Scaffolding::Draw(const Mat4x4& viewProjection, const Vector3& cameraPos)
{
	model_->Draw(viewProjection, cameraPos);
}

void Scaffolding::Draw2D(const Mat4x4& viewProjection)
{
	tex_->Draw(viewProjection, Pipeline::Normal, false);
}

