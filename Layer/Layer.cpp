#include "Layer.h"

Layer::Layer() {
	tex_.LoadTexture("./Resources/uvChecker.png");
	kMaxHitPoint_ = 5;
}

void Layer::Initialize() {

	hitPoint_ = kMaxHitPoint_;

}

void Layer::Update() {


	tex_.Update();
}

//void Layer::Draw(const Mat4x4& viewProjection) {
//
//}

void Layer::Draw2D(const Mat4x4& viewProjection) {

	tex_.Draw(viewProjection, Pipeline::Normal, false);
}