#include "Layer.h"

#include <numbers>
#include "Utils/Camera/Camera.h"

#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"

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
		tex_[i]->LoadTexture("./Resources/Layer/layer_front0.png");
		if (i % 3 == 0) {
			tex_[i]->color = 0xFFFFFFFF;
		}
		else if (i % 3 == 1) {
			tex_[i]->color = 0xFFFFFFFF;
		}
		else {
			tex_[i]->color = 0xFFFFFFFF;
		}
		tex_[i]->Update();

		models_.push_back(std::vector<std::unique_ptr<Model>>());

		for (int j = 0; j < static_cast<int>(Parts::kEnd); j++) {
			models_[i].push_back(std::make_unique<Model>());
		}
		models_[i][static_cast<uint16_t>(Parts::kMain)]->LoadObj("./Resources/Layer/layer.obj");
		models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ligDirection = Vector3{ 0.0f,-0.5f,1.0f }.Normalize();
		models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ligColor = { 1.0f,1.0f,1.0f };
		models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ptRange = 10000.0f;
		models_[i][static_cast<uint16_t>(Parts::kMain)]->rotate.y = std::numbers::pi_v<float>;
	}

	isChangeLayer_ = false;
	isClear_ = false;

	gauge_ = std::make_unique<Gauge>();
	gauge_->Initialize(nowLayer_, kMaxHitPoints_[nowLayer_]);

	SetGlobalVariable();

	colorSet_ = {
		std::numeric_limits<uint32_t>::max(),
		Vector4ToUint(Vector4{0.75f,0.75f,0.75f,1.0f }),
		Vector4ToUint(Vector4{0.5f,0.5f,0.5f,1.0f })
	};

	clearUnitlTime_ = std::chrono::milliseconds{ 500 };
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
			tex_[i]->LoadTexture("./Resources/Layer/layer_front0.png");
			/*if (i % 3 == 0) {
				tex_[i]->color = 0xFFFFFFFF;
			}
			else if (i % 3 == 1) {
				tex_[i]->color = 0xFFFFFFFF;
			}
			else {
				tex_[i]->color = 0xFFFFFFFF;
			}*/

			models_.push_back(std::vector<std::unique_ptr<Model>>());

			for (int j = 0; j < static_cast<int>(Parts::kEnd); j++) {
				models_[i].push_back(std::make_unique<Model>());
			}
			models_[i][static_cast<uint16_t>(Parts::kMain)]->LoadObj("./Resources/Layer/layer.obj");
			models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ligDirection = Vector3{ 0.0f,-0.5f,1.0f }.Normalize();
			models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ligColor = { 1.0f,1.0f,1.0f };
			models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ptRange = 10000.0f;
			models_[i][static_cast<uint16_t>(Parts::kMain)]->rotate.y = std::numbers::pi_v<float>;

			tex_[i]->Update();
		}

		auto colorSetItr = colorSet_.begin();

		for (auto& i : tex_) {
			i->color = *colorSetItr;
			colorSetItr++;
			if (colorSetItr == colorSet_.end()) {
				colorSetItr = colorSet_.begin();
			}
		}

		colorSetItr = colorSet_.begin();
		for (auto& i : models_) {
			i[static_cast<uint16_t>(Parts::kMain)]->color = *colorSetItr;
			colorSetItr++;
			if (colorSetItr == colorSet_.end()) {
				colorSetItr = colorSet_.begin();
			}
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
				tex_[i]->color = 0xFFFFFFFF;
			}
			else {
				tex_[i]->color = 0xFFFFFFFF;
			}


			tex_[i]->Update();
		}

		auto colorSetItr = colorSet_.begin();

		for (auto& i : tex_) {
			i->color = *colorSetItr;
			colorSetItr++;
			if (colorSetItr == colorSet_.end()) {
				colorSetItr = colorSet_.begin();
			}
		}

		colorSetItr = colorSet_.begin();
		for (auto& i : models_) {
			i[static_cast<uint16_t>(Parts::kMain)]->color = *colorSetItr;
			colorSetItr++;
			if (colorSetItr == colorSet_.end()) {
				colorSetItr = colorSet_.begin();
			}
		}
	}
}

void Layer::Reset() {

	hitPoints_ = kMaxHitPoints_;
	nowLayer_ = 0;

	auto colorSetItr = colorSet_.begin();

	for (auto& i : tex_) {
		i->color = *colorSetItr;
		colorSetItr++;
		if (colorSetItr == colorSet_.end()) {
			colorSetItr = colorSet_.begin();
		}
	}

	colorSetItr = colorSet_.begin();
	for (auto& i : models_) {
		i[static_cast<uint16_t>(Parts::kMain)]->color = *colorSetItr;
		colorSetItr++;
		if (colorSetItr == colorSet_.end()) {
			colorSetItr = colorSet_.begin();
		}
	}
	for (int i = 0; i < kMaxLayerNum_; i++) {

		/*if (i % 3 == 0) {
			tex_[i]->color = 0xFFFFFFFF;
		}
		else if (i % 3 == 1) {
			tex_[i]->color = 0xFFFFFFFF;
		}
		else {
			tex_[i]->color = 0xFFFFFFFF;
		}*/
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
		/*if (i % 3 == 0) {
			tex_[i]->color = 0xFFFFFFFF;
		}
		else if (i % 3 == 1) {
			tex_[i]->color = 0xFF0000FF;
		}
		else {
			tex_[i]->color = 0xFFFF00FF;
		}*/
		tex_[i]->LoadTexture("./Resources/Layer/layer_front0.png");

		models_.push_back(std::vector<std::unique_ptr<Model>>());

		for (int j = 0; j < static_cast<int>(Parts::kEnd); j++) {
			models_[i].push_back(std::make_unique<Model>());
		}
		models_[i][static_cast<uint16_t>(Parts::kMain)]->LoadObj("./Resources/Layer/layer.obj");
		models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ligDirection = Vector3{ 0.0f,-0.5f,1.0f }.Normalize();;
		models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ligColor = { 1.0f,1.0f,1.0f };
		models_[i][static_cast<uint16_t>(Parts::kMain)]->light.ptRange = 10000.0f;
		models_[i][static_cast<uint16_t>(Parts::kMain)]->rotate.y = std::numbers::pi_v<float>;

		tex_[i]->Update();
	}

	auto colorSetItr = colorSet_.begin();

	for (auto& i : tex_) {
		i->color = *colorSetItr;
		colorSetItr++;
		if (colorSetItr == colorSet_.end()) {
			colorSetItr = colorSet_.begin();
		}
	}

	colorSetItr = colorSet_.begin();
	for (auto& i : models_) {
		i[static_cast<uint16_t>(Parts::kMain)]->color = *colorSetItr;
		colorSetItr++;
		if (colorSetItr == colorSet_.end()) {
			colorSetItr = colorSet_.begin();
		}
	}

	gauge_->Initialize(hitPoints_[nowLayer_], kMaxHitPoints_[nowLayer_]);

	TimerStart();

	gamePlayTime_ = std::chrono::milliseconds(0);

	breakEffect_.LoadSettingDirectory("break");
	healEffect_.LoadSettingDirectory("layer-heal");
}

void Layer::Update(const Camera* camera) {
	isClear_.Update();
	isClearStop_.Update();

	[[maybe_unused]]auto nowTime = std::chrono::steady_clock::now();

	ApplyGlobalVariable();

	gauge_->Update(hitPoints_[nowLayer_], kMaxHitPoints_[nowLayer_], damage_, GetHighestPosY());

	hitPoints_[nowLayer_] -= damage_;

	if (hitPoints_[nowLayer_] <= 0) {
		hitPoints_[nowLayer_] = 0;

		if (nowLayer_ == kMaxLayerNum_ - 1) {
		    // ここ
			if (!isClearStop_ && !breakEffect_.GetIsParticleStart()) {
				isClearStop_ = true;
				startClearUnitlTime_ = FrameInfo::GetInstance()->GetThisFrameTime();
				FrameInfo::GetInstance()->HitStop(500);
			}

			if (breakEffect_.GetIsParticleStart().OnExit()) {
				// クリア処理
				isClear_ = true;

				// クリアした時間を計算して代入
				if (isClear_.OnEnter()) {
					TimerStop();
				}

				//// 今は仮で層のリセット
				//Reset();
			}
		}
		else {
			nowLayer_++;
			hitPoints_[nowLayer_] = kMaxHitPoints_[nowLayer_];
			// ここ
			if (!isClearStop_ && !breakEffect_.GetIsParticleStart()) {
				isClearStop_ = true;
				startClearUnitlTime_ = FrameInfo::GetInstance()->GetThisFrameTime();
				FrameInfo::GetInstance()->HitStop(500);
			}
		}
	}

	if (isClearStop_) {
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startClearUnitlTime_);
		if (clearUnitlTime_ < duration) {
			breakEffect_.ParticleStart();
			isClearStop_ = false;
		}
	}

	damage_ = 0;

	for (int i = 0; i < kMaxLayerNum_; i++) {

		float ratio = WindowFactory::GetInstance()->GetClientSize().y /
			(std::tanf(camera->fov / 2) * (models_[i][static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) * 2);

		float indication = 185.0f;

		models_[i][static_cast<uint16_t>(Parts::kMain)]->pos.x = tex_[i]->pos.x / ratio + camera->pos.x - camera->pos.x / ratio;
		models_[i][static_cast<uint16_t>(Parts::kMain)]->pos.y = tex_[i]->pos.y / ratio + camera->pos.y - camera->pos.y / ratio;
		models_[i][static_cast<uint16_t>(Parts::kMain)]->scale.y = tex_[i]->scale.y / (indication * std::tanf(camera->fov / 2) * 2) *
			(models_[i][static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) / indication;
		models_[i][static_cast<uint16_t>(Parts::kMain)]->scale.z = models_[i][static_cast<uint16_t>(Parts::kMain)]->scale.y * 10;
		models_[i][static_cast<uint16_t>(Parts::kMain)]->Update();

		if (kMaxHitPoints_[i] / 3.0f <= hitPoints_[i] && hitPoints_[i] < kMaxHitPoints_[i] / 3.0f * 2.0f) {
			tex_[i]->LoadTexture("./Resources/Layer/layer_front1.png");
		}
		else if (hitPoints_[i] < kMaxHitPoints_[i] / 3.0f) {
			tex_[i]->LoadTexture("./Resources/Layer/layer_front2.png");
		}
		else if(hitPoints_[i] >= kMaxHitPoints_[i] / 3.0f * 2.0f){
			tex_[i]->LoadTexture("./Resources/Layer/layer_front0.png");
		}

		float texYPos = tex_[i]->pos.y;
		tex_[i]->pos.y -= tex_[i]->scale.y * 0.5f;
		tex_[i]->Update();
		tex_[i]->pos.y = texYPos;
	}


	breakEffect_.emitterPos.y = tex_[nowLayer_]->pos.y;
	breakEffect_.Update();
	healEffect_.emitterPos.y = tex_[nowLayer_]->pos.y;
	healEffect_.Update();
}

void Layer::Heal() {
	if (hitPoints_[nowLayer_] != kMaxHitPoints_[nowLayer_]) {
		hitPoints_[nowLayer_]++;
		healEffect_.ParticleStart();
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

void Layer::ParticleDraw(const Mat4x4& viewProjections) {
	breakEffect_.Draw(Vector3::kZero, viewProjections);
	healEffect_.Draw(Vector3::kZero, viewProjections);
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
	gauge_->Draw2D(viewProjection);
}

void Layer::TimerStart() {
	playStartTime_ = std::chrono::steady_clock::now();
}

void Layer::TimerStop() {
	auto nowTime = std::chrono::steady_clock::now();
	gamePlayTime_ += std::chrono::duration_cast<decltype(gamePlayTime_)>(nowTime - playStartTime_);
}
