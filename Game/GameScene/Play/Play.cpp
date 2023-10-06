#include "Play.h"

Play::Play() {

	camera2D_ = std::make_unique<Camera>(Camera::Type::Othographic);
	camera3D_ = std::make_unique<Camera>(Camera::Type::Projecction);

	player_ = std::make_unique<Player>();
}

void Play::Initialize() {

	player_->Initialize();

}


void Play::Update() {

	player_->Update();

	camera2D_->Update();
	camera3D_->Update();
}

void Play::Draw() {

	player_->Draw2D(camera2D_->GetViewOthographics());
}


