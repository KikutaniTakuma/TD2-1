#include "Player.h"

#include "Engine/FrameInfo/FrameInfo.h"
#include "Game/GameScene/Play/Play.h"
#include "Game/ShockWave/ShockWave.h"

Player::Player() {
	
	input_ = Input::GetInstance();

	tex_ = std::make_shared<Texture2D>();
	tex_->LoadTexture("./Resources/uvChecker.png");

	globalVariables_ = std::make_unique<GlobalVariables>();

	tex_->scale *= 50.0f;

	// ジャンプ時の初速
	kJampInitialVelocity_ = 10.0f;

	// 移動スピード
	kMoveSpeed_ = 5.0f;

	// 重力加速度
	kGravity_ = -9.8f;


}

void Player::SetGlobalVariable() {
	
	globalVariables_->CreateGroup(groupName_);

	globalVariables_->AddItem(groupName_, "kJampInitialVelocity", kJampInitialVelocity_);
	globalVariables_->AddItem(groupName_, "kMoveSpeed", kMoveSpeed_);
	globalVariables_->AddItem(groupName_, "kGravity", kGravity_);
	globalVariables_->AddItem(groupName_, "kHipDropSpeed", kHipDropSpeed_);

	globalVariables_->LoadFile(groupName_);
	ApplyGlobalVariable();
}

void Player::ApplyGlobalVariable() {

	kJampInitialVelocity_ = globalVariables_->GetFloatValue(groupName_, "kJampInitialVelocity");
	kMoveSpeed_ = globalVariables_->GetFloatValue(groupName_, "kMoveSpeed");
	kGravity_ = globalVariables_->GetFloatValue(groupName_, "kGravity");
	kHipDropSpeed_ = globalVariables_->GetFloatValue(groupName_, "kHipDropSpeed");

}

void Player::EnemyStep(bool step) {

	if (step) {
		if (!isSteped_) {
			isStep_ = true;
		}
	}
	else {
		isSteped_ = true;
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

void Player::Update() {

	globalVariables_->Update();
	ApplyGlobalVariable();

	if (statusRequest_) {
		status_ = statusRequest_.value();

		switch (status_)
		{
		case Player::Status::kNormal:
			NormalInitialize();
			break;
		case Player::Status::kHipDrop:
			HipDropInitialize();
			break;
		case Player::Status::kLanding:
			LandingInitialize();
			break;
		case Player::Status::kFalling:
			FallingInitialize();
			break;
		default:
			break;
		}

		statusRequest_ = std::nullopt;
	}

	switch (status_)
	{
	case Player::Status::kNormal:
		NormalUpdate();
		break;
	case Player::Status::kHipDrop:
		HipDropUpdate();
		break;
	case Player::Status::kLanding:
		LandingUpdate();
		break;
	case Player::Status::kFalling:
		FallingUpdate();
		break;
	default:
		break;
	}

	tex_->Update();
}

void Player::NormalInitialize() {

	velocity_ = {};
	highest_ = 0.0f;
	
	isStep_ = false;
	isSteped_ = false;
	
}

void Player::NormalUpdate() {

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
		else {
			velocity_.y += kGravity_ * deletaTime;
		}
	}

	// ジャンプ入力
	if (isStep_ || (!isFly_ && (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetKey()->Pushed(DIK_W) || input_->GetKey()->Pushed(DIK_UP)))) {
		isFly_ = true;
		isStep_ = false;
		// 初速を与える
		velocity_.y = kJampInitialVelocity_;
	}

	// 横の移動距離
	velocity_.x = move.x * kMoveSpeed_ * deletaTime;

	tex_->pos += velocity_;

	MemoHighest();

	// 地面との当たり判定。
	if (tex_->pos.y <= 0.0f && isFly_) {
		tex_->pos.y = 0.0f;
		velocity_.y = 0.0f;
		isFly_ = false;
		statusRequest_ = Status::kLanding;
	}
}

void Player::HipDropInitialize() {

	velocity_ = {};
}

void Player::HipDropUpdate() {

	velocity_.y += kHipDropSpeed_ * FrameInfo::GetInstance()->GetDelta();
	tex_->pos += velocity_;

	if (tex_->pos.y <= 0.0f) {
		tex_->pos.y = 0.0f;
		velocity_.y = 0.0f;

		statusRequest_ = Status::kLanding;
	}

}

void Player::LandingInitialize() {

	velocity_ = {};

	if (highest_ >= ShockWave::kHighCriteria_[static_cast<uint16_t>(ShockWave::Size::kSmall)]) {
		play_->CreatShockWave(tex_->pos, highest_);
	}
}

void Player::LandingUpdate() {

	statusRequest_ = Status::kNormal;

}

void Player::FallingInitialize() {

	velocity_ = {};

}

void Player::FallingUpdate() {

	velocity_.y += kGravity_ * FrameInfo::GetInstance()->GetDelta();
	tex_->pos += velocity_;

	if (tex_->pos.y <= 0.0f) {
		tex_->pos.y = 0.0f;
		velocity_.y = 0.0f;

		statusRequest_ = Status::kNormal;
	}
}

void Player::MemoHighest() {

	if (highest_ < tex_->pos.y) {
		highest_ = tex_->pos.y;
	}
}

//void Player::Draw(const Mat4x4& viewProjection) {
//
//}

void Player::Draw2D(const Mat4x4& viewProjection) {

	tex_->Draw(viewProjection, Pipeline::Normal, false);
}
