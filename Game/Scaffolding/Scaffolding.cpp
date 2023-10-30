#include "Scaffolding.h"
#include "Utils/Camera/Camera.h"
#include "Engine/Engine.h"

Scaffolding::Scaffolding(const Vector3& pos, const Vector2& scale)
{
	tex_ = std::make_unique<Texture2D>();
	tex_->LoadTexture("./Resources/white2x2.png");

	model_ = std::make_unique<Model>();
	model_->LoadObj("./Resources/Layer/layer.obj");
	model_->light.ligDirection = { 0.0f,0.0f,1.0f };
	model_->light.ligColor = { 1.0f,1.0f,1.0f };
	model_->light.ptRange = 10000.0f;

	tex_->pos = pos;
	tex_->scale = scale;

	tex_->Update();
}

void Scaffolding::SetParametar(const Vector3& pos, const Vector2& scale, const Camera* camera)
{
	tex_->pos = pos;
	tex_->scale = scale;

	float ratio = static_cast<float>(Engine::GetInstance()->clientHeight) /
		(std::tanf(camera->fov / 2) * (model_->pos.z - camera->pos.z) * 2);


	float indication = 1030.0f;
	float indicationY = 200.0f;

	model_->pos.x = tex_->pos.x / ratio + camera->pos.x - camera->pos.x / ratio;
	model_->pos.y = tex_->pos.y / ratio + camera->pos.y - camera->pos.y / ratio;
	model_->scale.x = tex_->scale.x / (indication * std::tanf(camera->fov / 2) * 2) *
		(model_->pos.z - camera->pos.z) / indication;
	model_->scale.y = tex_->scale.y / (indicationY * std::tanf(camera->fov / 2) * 2) *
		(model_->pos.z - camera->pos.z) / indicationY;
	model_->scale.z = model_->scale.y * 40;
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

