#include "Layer.h"

#include <numbers>
#include "Utils/Camera/Camera.h"

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
		tex_[i]->LoadTexture("./Resources/white2x2.png");
		if (i % 3 == 0) {
			tex_[i]->color = 0xFFFFFFFF;
		}
		else if (i % 3 == 1) {
			tex_[i]->color = 0xFF0000FF;
		}
		else {
			tex_[i]->color = 0xFFFF00FF;
		}
		tex_[i]->Update();

		models_.push_back(std::vector<std::unique_ptr<Model>>());

		for (int j = 0; j < static_cast<int>(Parts::kEnd); j++) {
			models_[i].push_back(std::make_unique<Model>());
		}
		models_[i][static_cast<uint16_t>(Parts::kMain)]->LoadObj("./Resources/Layer/layer.obj");
		models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ligDirection = { 0.0f,0.0f,1.0f };
		models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ligColor = { 1.0f,1.0f,1.0f };
		models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ptRange = 10000.0f;
		models_[i][static_cast<uint16_t>(Parts::kMain)]->rotate.y = std::numbers::pi_v<float>;
	}

	isChangeLayer_ = false;
	isClear_ = false;

	gauge_ = std::make_unique<Gauge>();
	gauge_->Initialize(nowLayer_, kMaxHitPoints_[nowLayer_]);

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
		models_.clear();
		for (int i = 0; i < kMaxLayerNum_; i++) {
			tex_.push_back(std::make_unique<Texture2D>());
			tex_[i]->scale = kLayer2DScale_;
			tex_[i]->pos = { 0.0f, kFirstLayerCenterPosY_ + (i * (-kLayer2DScale_.y)) };
			tex_[i]->LoadTexture("./Resources/white2x2.png");
			if (i % 3 == 0) {
				tex_[i]->color = 0xFFFFFFFF;
			}
			else if (i % 3 == 1) {
				tex_[i]->color = 0xFF0000FF;
			}
			else {
				tex_[i]->color = 0xFFFF00FF;
			}

			models_.push_back(std::vector<std::unique_ptr<Model>>());

			for (int j = 0; j < static_cast<int>(Parts::kEnd); j++) {
				models_[i].push_back(std::make_unique<Model>());
			}
			models_[i][static_cast<uint16_t>(Parts::kMain)]->LoadObj("./Resources/Layer/layer.obj");
			models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ligDirection = { 0.0f,0.0f,1.0f };
			models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ligColor = { 1.0f,1.0f,1.0f };
			models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ptRange = 10000.0f;
			models_[i][static_cast<uint16_t>(Parts::kMain)]->rotate.y = std::numbers::pi_v<float>;

			tex_[i]->Update();
		}
	}
	else {
		for (int i = 0; i < kMaxLayerNum_; i++) {

			tex_[i]->scale = kLayer2DScale_;
			tex_[i]->pos = { 0.0f, kFirstLayerCenterPosY_ + (i * (-kLayer2DScale_.y)) };
			if (i % 3 == 0) {
				tex_[i]->color = 0xFFFFFFFF;
			}
			else if (i % 3 == 1) {
				tex_[i]->color = 0xFF0000FF;
			}
			else {
				tex_[i]->color = 0xFFFF00FF;
			}


			tex_[i]->Update();
		}
	}
}

void Layer::Reset() {

	hitPoints_ = kMaxHitPoints_;
	nowLayer_ = 0;

	for (int i = 0; i < kMaxLayerNum_; i++) {

		if (i % 3 == 0) {
			tex_[i]->color = 0xFFFFFFFF;
		}
		else if (i % 3 == 1) {
			tex_[i]->color = 0xFF0000FF;
		}
		else {
			tex_[i]->color = 0xFFFF00FF;
		}
		tex_[i]->scale = kLayer2DScale_;
		tex_[i]->pos = { 0.0f, kFirstLayerCenterPosY_ + (i * (-kLayer2DScale_.y)) };
		tex_[i]->Update();
	}
}

void Layer::Initialize(int kMaxLayerNum, const std::vector<int>& kMaxHitPoints) {

	kMaxLayerNum_ = kMaxLayerNum;
	kMaxHitPoints_ = kMaxHitPoints;

	hitPoints_ = kMaxHitPoints_;
	nowLayer_ = 0;

	tex_.clear();
	models_.clear();

	for (int i = 0; i < kMaxLayerNum_; i++) {
		tex_.push_back(std::make_unique<Texture2D>());
		tex_[i]->scale = kLayer2DScale_;
		tex_[i]->pos = { 0.0f, kFirstLayerCenterPosY_ + (i * (-kLayer2DScale_.y)) };
		if (i % 3 == 0) {
			tex_[i]->color = 0xFFFFFFFF;
		}
		else if (i % 3 == 1) {
			tex_[i]->color = 0xFF0000FF;
		}
		else {
			tex_[i]->color = 0xFFFF00FF;
		}
		tex_[i]->LoadTexture("./Resources/white2x2.png");

		models_.push_back(std::vector<std::unique_ptr<Model>>());

		for (int j = 0; j < static_cast<int>(Parts::kEnd); j++) {
			models_[i].push_back(std::make_unique<Model>());
		}
		models_[i][static_cast<uint16_t>(Parts::kMain)]->LoadObj("./Resources/Layer/layer.obj");
		models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ligDirection = { 0.0f,0.0f,1.0f };
		models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ligColor = { 1.0f,1.0f,1.0f };
		models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ptRange = 10000.0f;
		models_[i][static_cast<uint16_t>(Parts::kMain)]->rotate.y = std::numbers::pi_v<float>;

		tex_[i]->Update();
	}

	gauge_->Initialize(hitPoints_[nowLayer_], kMaxHitPoints_[nowLayer_]);

	TimerStart();

	gamePlayTime_ = std::chrono::milliseconds(0);
}

void Layer::Update(const Camera* camera) {
	isClear_.Update();

	[[maybe_unused]]auto nowTime = std::chrono::steady_clock::now();

	ApplyGlobalVariable();

	gauge_->Update(hitPoints_[nowLayer_], kMaxHitPoints_[nowLayer_], damage_);

	hitPoints_[nowLayer_] -= damage_;

	if (hitPoints_[nowLayer_] <= 0) {
		hitPoints_[nowLayer_] = 0;

		if (nowLayer_ == kMaxLayerNum_ - 1) {

			// クリア処理
			isClear_ = true;

			// クリアした時間を計算して代入
			if (isClear_.OnEnter()) {
				TimerStop();
			}

			// 今は仮で層のリセット
			Reset();

		}
		else {
			nowLayer_++;
			hitPoints_[nowLayer_] = kMaxHitPoints_[nowLayer_];
		}
	}

	damage_ = 0;

	for (int i = 0; i < kMaxLayerNum_; i++) {

		float ratio = static_cast<float>(Engine::GetInstance()->clientHeight) /
			(std::tanf(camera->fov / 2) * (models_[i][static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) * 2);

		float indication = 200.0f;

		models_[i][static_cast<uint16_t>(Parts::kMain)]->pos.x = tex_[i]->pos.x / ratio + camera->pos.x - camera->pos.x / ratio;
		models_[i][static_cast<uint16_t>(Parts::kMain)]->pos.y = tex_[i]->pos.y / ratio + camera->pos.y - camera->pos.y / ratio;
		models_[i][static_cast<uint16_t>(Parts::kMain)]->scale.y = tex_[i]->scale.y / (indication * std::tanf(camera->fov / 2) * 2) *
			(models_[i][static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) / indication;
		models_[i][static_cast<uint16_t>(Parts::kMain)]->scale.z = models_[i][static_cast<uint16_t>(Parts::kMain)]->scale.y;
		models_[i][static_cast<uint16_t>(Parts::kMain)]->Update();

		tex_[i]->Update();
	}
}

void Layer::Draw(const Mat4x4& viewProjection, const Vector3& cameraPos) {
	for (int i = 0; i < kMaxLayerNum_; i++) {
		if (hitPoints_[i] != 0) {
			for (const std::unique_ptr<Model>& model : models_[i]) {
				model->Draw(viewProjection, cameraPos);
			}
		}
	}
}

void Layer::Draw2DFar(const Mat4x4& viewProjection) {

	gauge_->Draw2D(viewProjection);
}

void Layer::Draw2DNear(const Mat4x4& viewProjection) {

	for (int i = 0; i < kMaxLayerNum_; i++) {
		if (hitPoints_[i] != 0) {
			tex_[i]->Draw(viewProjection, Pipeline::Normal, false);
		}
	}
}

void Layer::TimerStart() {
	playStartTime_ = std::chrono::steady_clock::now();
}

void Layer::TimerStop() {
	auto nowTime = std::chrono::steady_clock::now();
	gamePlayTime_ += std::chrono::duration_cast<decltype(gamePlayTime_)>(nowTime - playStartTime_);
}
