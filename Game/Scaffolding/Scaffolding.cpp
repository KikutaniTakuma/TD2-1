#include "Scaffolding.h"

Scaffolding::Scaffolding(const Vector3& pos, const Vector2& scale)
{
	tex_ = std::make_unique<Texture2D>();
	tex_->LoadTexture("./Resources/white2x2.png");

	tex_->pos = pos;
	tex_->scale = scale;

	tex_->Update();
}

void Scaffolding::SetParametar(const Vector3& pos, const Vector2& scale)
{
	tex_->pos = pos;
	tex_->scale = scale;

	tex_->Update();
}

void Scaffolding::Draw2D(const Mat4x4& viewProjection)
{
	tex_->Draw(viewProjection, Pipeline::Normal, false);
}

