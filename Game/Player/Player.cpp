#include "Player.h"

#include "Engine/FrameInfo/FrameInfo.h"
#include "Game/ShockWave/ShockWave.h"
#include "SceneManager/GameScene/GameScene.h"
#include "Utils/Camera/Camera.h"
#include <numbers>

#include "externals/imgui/imgui.h"

Player::Player() {
	
	input_ = Input::GetInstance();

	tex_ = std::make_unique<Texture2D>();
	tex_->LoadTexture("./Resources/Player/usausa.png");

	models_.push_back(std::make_unique<Model>());
	models_[static_cast<uint16_t>(Parts::kMain)]->LoadObj("./Resources/Player/Player.obj");
	models_[static_cast<uint16_t>(Parts::kMain)]->light.ligDirection = { 0.0f,0.0f,1.0f };
	models_[static_cast<uint16_t>(Parts::kMain)]->light.ligColor = { 1.0f,1.0f,1.0f };
	models_[static_cast<uint16_t>(Parts::kMain)]->light.ptRange = 10000.0f;
	models_[static_cast<uint16_t>(Parts::kMain)]->rotate.y = std::numbers::pi_v<float>;

	globalVariables_ = std::make_unique<GlobalVariables>();

	tex_->pos = {};

	tex_->scale *= 50.0f;

	// ジャンプ時の初速
	kJampInitialVelocity_ = 10.0f;

	// 移動スピード
	kMoveSpeed_ = 5.0f;

	// 重力加速度
	kGravity_ = -9.8f;

	// 降下中の重力加速度
	kFallingGravity_ = -20.0f;

	isFallingGravity_ = true;
	isHipdropJamp_ = true;
}

void Player::SetGlobalVariable() {
	
	globalVariables_->CreateGroup(groupName_);

	globalVariables_->AddItem(groupName_, "kJampInitialVelocity", kJampInitialVelocity_);
	globalVariables_->AddItem(groupName_, "kMoveSpeed", kMoveSpeed_);
	globalVariables_->AddItem(groupName_, "kGravity", kGravity_);
	globalVariables_->AddItem(groupName_, "kHipDropSpeed", kHipDropSpeed_);
	globalVariables_->AddItem(groupName_, "kFallingGravity", kFallingGravity_);

	globalVariables_->LoadFile(groupName_);
	ApplyGlobalVariable();
}

void Player::ApplyGlobalVariable() {

	kJampInitialVelocity_ = globalVariables_->GetFloatValue(groupName_, "kJampInitialVelocity");
	kMoveSpeed_ = globalVariables_->GetFloatValue(groupName_, "kMoveSpeed");
	kGravity_ = globalVariables_->GetFloatValue(groupName_, "kGravity");
	kHipDropSpeed_ = globalVariables_->GetFloatValue(groupName_, "kHipDropSpeed");
	kFallingGravity_ = globalVariables_->GetFloatValue(groupName_, "kFallingGravity");
}

void Player::EnemyStep(bool step) {

	if (step) {
		if (!isSteped_) {
			isStep_ = true;
		}
	}
	else {
		isSteped_ = true;
		isFly_ = false;
		statusRequest_ = Status::kFalling;
	}
}

void Player::Initialize() {

	SetGlobalVariable();

	velocity_ = {};

	// 空中にいるか
	isFly_ = false;

	// 敵を踏んだか
	isStep_ = false;

	// 敵に踏まれたか
	isSteped_ = false;
}

void Player::Update(const float& y, const Camera* camera) {

#ifdef _DEBUG
	ImGui::Begin("PlayerFlag");
	ImGui::Checkbox("ChangeGravityType", &isFallingGravity_);
	ImGui::Checkbox("ChangeIsHipdropJamp", &isHipdropJamp_);
	ImGui::End();
#endif // _DEBUG

	globalVariables_->Update();
	ApplyGlobalVariable();

	if (statusRequest_) {
		status_ = statusRequest_.value();

		switch (status_)
		{
		case Player::Status::kNormal:
			NormalInitialize(y);
			break;
		case Player::Status::kHipDrop:
			HipDropInitialize();
			break;
		case Player::Status::kOnScaffolding:
			OnScaffoldingInitialize();
			break;
		case Player::Status::kLanding:
			LandingInitialize(y);
			break;
		case Player::Status::kFalling:
			FallingInitialize(y);
			break;
		default:
			break;
		}

		statusRequest_ = std::nullopt;
	}

	switch (status_)
	{
	case Player::Status::kNormal:
		NormalUpdate(y);
		break;
	case Player::Status::kHipDrop:
		HipDropUpdate(y);
		break;
	case Player::Status::kOnScaffolding:
		OnScaffoldingUpdate();
		break;
	case Player::Status::kLanding:
		LandingUpdate(y);
		break;
	case Player::Status::kFalling:
		FallingUpdate(y);
		break;
	default:
		break;
	}

	float ratio = static_cast<float>(Engine::GetInstance()->clientHeight) /
		(std::tanf(camera->fov / 2) * (models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) * 2);

	float indication = 90.0f;

	models_[static_cast<uint16_t>(Parts::kMain)]->pos.x = tex_->pos.x / ratio + camera->pos.x - camera->pos.x / ratio;
	models_[static_cast<uint16_t>(Parts::kMain)]->pos.y = tex_->pos.y / ratio + camera->pos.y- camera->pos.y / ratio;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.x = tex_->scale.x / (indication * std::tanf(camera->fov / 2) * 2) *
		(models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) / indication;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.y = tex_->scale.y / (indication * std::tanf(camera->fov / 2) * 2) *
		(models_[static_cast<uint16_t>(Parts::kMain)]->pos.z - camera->pos.z) / indication;
	models_[static_cast<uint16_t>(Parts::kMain)]->scale.z = models_[static_cast<uint16_t>(Parts::kMain)]->scale.y;
	models_[static_cast<uint16_t>(Parts::kMain)]->Update();
	tex_->Update();
}

void Player::NormalInitialize(const float& y) {

	//velocity_ = {};
	highest_ = 0.0f;
	Collision(y);
	
	isStep_ = false;
	isSteped_ = false;
	
}

void Player::NormalUpdate(const float& y) {

	float deletaTime = FrameInfo::GetInstance()->GetDelta();

	// 移動の単位ベクトル
	Vector3 move = {};

	// 左右移動
	if (input_->GetKey()->LongPush(DIK_A) || input_->GetKey()->LongPush(DIK_LEFT)) {
		move.x--;
	}
	if (input_->GetKey()->LongPush(DIK_D) || input_->GetKey()->LongPush(DIK_RIGHT)) {
		move.x++;
	}

	// 空中にいる時の処理。
	if (isFly_) {
		if (input_->GetKey()->Pushed(DIK_SPACE)) {
			statusRequest_ = Status::kHipDrop;
		}
		/*else {
			velocity_.y += kGravity_ * deletaTime;
		}*/
	}

	// ジャンプ入力
	if (isStep_ || (!isFly_ && (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetKey()->Pushed(DIK_W) || input_->GetKey()->Pushed(DIK_UP)))) {
		isFly_ = true;
		isStep_ = false;
		// 初速を与える
		velocity_.y = kJampInitialVelocity_;
	}

	if (isFallingGravity_) {
		if (velocity_.y < 0) {
			velocity_.y += kFallingGravity_ * deletaTime;
		}
		else {
			velocity_.y += kGravity_ * deletaTime;
		}
	}
	else {
		velocity_.y += kGravity_ * deletaTime;
	}

	// 横の移動距離
	velocity_.x = move.x * kMoveSpeed_ * deletaTime;

	tex_->pos += velocity_;

	MemoHighest();

	// 地面との当たり判定。
	if (tex_->pos.y - tex_->scale.y / 2.0f <= y && isFly_) {

		Collision(y);
		velocity_.y = 0.0f;
		isFly_ = false;
		statusRequest_ = Status::kLanding;
	}
	else {
		Collision(y);
	}
}

void Player::HipDropInitialize() {

	velocity_ = {};
}

void Player::HipDropUpdate(const float& y) {

	velocity_.y += kHipDropSpeed_ * FrameInfo::GetInstance()->GetDelta();
	tex_->pos += velocity_;

	if (isHipdropJamp_) {
		if (isStep_) {
			isFly_ = true;
			isStep_ = false;
			// 初速を与える
			velocity_.y = kJampInitialVelocity_;
			statusRequest_ = Status::kNormal;
		}
	}

	if (tex_->pos.y - tex_->scale.y / 2.0f <= y && isFly_) {

		Collision(y);
		velocity_.y = 0.0f;
		isFly_ = false;
		statusRequest_ = Status::kLanding;
	}

}

void Player::OnScaffoldingInitialize()
{
	velocity_ = {};
	isFly_ = false;
	isSteped_ = false;
	isStep_ = false;
}

void Player::OnScaffoldingUpdate()
{
	float deletaTime = FrameInfo::GetInstance()->GetDelta();

	// 移動の単位ベクトル
	Vector3 move = {};

	// 左右移動
	if (input_->GetKey()->LongPush(DIK_A) || input_->GetKey()->LongPush(DIK_LEFT)) {
		move.x--;
	}
	if (input_->GetKey()->LongPush(DIK_D) || input_->GetKey()->LongPush(DIK_RIGHT)) {
		move.x++;
	}

	if ((!isFly_ && (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetKey()->Pushed(DIK_W) || input_->GetKey()->Pushed(DIK_UP)))) {
		isFly_ = true;
		isStep_ = false;
		// 初速を与える
		velocity_.y = kJampInitialVelocity_;

		statusRequest_ = Status::kNormal;
	}

	velocity_.y += kGravity_ * deletaTime;

	// 横の移動距離
	velocity_.x = move.x * kMoveSpeed_ * deletaTime;

	tex_->pos += velocity_;

	MemoHighest();

}

void Player::LandingInitialize(const float& y) {

	velocity_ = {};

	Collision(y);

	play_->CreatShockWave(tex_->pos, highest_, y);

	/*if (highest_ > ShockWave::kHighCriteria_[static_cast<uint16_t>(ShockWave::Size::kSmall)] - y) {
		play_->CreatShockWave(tex_->pos, highest_, y);
	}*/
}

void Player::LandingUpdate(const float& y) {

	Collision(y);
	statusRequest_ = Status::kNormal;

}

void Player::FallingInitialize(const float& y) {

	Collision(y);
	velocity_ = {};

}

void Player::FallingUpdate(const float& y) {

	float deletaTime = FrameInfo::GetInstance()->GetDelta();

	if (isFallingGravity_) {
		velocity_.y += kFallingGravity_ * deletaTime;
	}
	else {
		velocity_.y += kGravity_ * deletaTime;
	}
	tex_->pos += velocity_;

	// 地面との当たり判定。
	if (tex_->pos.y - tex_->scale.y / 2.0f <= y) {

		Collision(y);
		velocity_.y = 0.0f;
		isFly_ = false;
		statusRequest_ = Status::kNormal;
	}
}

void Player::CollisionScaffolding(const Texture2D* tex)
{

	if (velocity_.y < 0 &&
		tex_->pos.y - velocity_.y - tex_->scale.y/2.0f >= tex->pos.y + tex->scale.y / 2.0f &&
		tex->pos.x - tex->scale.x / 2.0f <= tex_->pos.x + tex_->scale.x / 2.0f &&
		tex->pos.x + tex->scale.x / 2.0f >= tex_->pos.x - tex_->scale.x / 2.0f &&
		tex->pos.y + tex->scale.y / 2.0f >= tex_->pos.y - tex_->scale.y / 2.0f) {

		tex_->pos.y = tex->pos.y + tex->scale.y / 2.0f + tex_->scale.y / 2.0f + 0.1f;
		velocity_.y = 0.0f;

		tex_->Update();
		
		highest_ = tex_->pos.y;

		statusRequest_ = Status::kOnScaffolding;
	}
	else {
		if (status_ == Status::kOnScaffolding) {
			statusRequest_ = Status::kNormal;
			isFly_ = true;
		}
	}
}

void Player::KnockBack(const Vector3& pos)
{
	Vector3 s = pos;
	s = pos;

}

void Player::Collision(const float& y) {

	float posY = tex_->pos.y - tex_->scale.y / 2.0f;

	if (y > posY) {
		tex_->pos.y += y - posY;
		velocity_.y = 0;
	}

}

void Player::MemoHighest() {

	if (highest_ < tex_->pos.y) {
		highest_ = tex_->pos.y;
	}
}

void Player::Draw(const Mat4x4& viewProjection, const Vector3& cameraPos) {
	for (const std::unique_ptr<Model>& model : models_) {
		model->Draw(viewProjection, cameraPos);
	}
}

void Player::Draw2D(const Mat4x4& viewProjection) {

	tex_->Draw(viewProjection, Pipeline::Normal, false);

}
