#include "Player.h"

#include "Engine/FrameInfo/FrameInfo.h"

Player::Player() {
	
	input_ = Input::GetInstance();

	tex_.LoadTexture("./Resources/uvChecker.png");

	globalVariables_ = std::make_unique<GlobalVariables>();

	tex_.scale *= 50.0f;

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

	globalVariables_->LoadFile(groupName_);
	ApplyGlobalVariable();
}

void Player::ApplyGlobalVariable() {

	kJampInitialVelocity_ = globalVariables_->GetFloatValue(groupName_, "kJampInitialVelocity");
	kMoveSpeed_ = globalVariables_->GetFloatValue(groupName_, "kMoveSpeed");
	kGravity_ = globalVariables_->GetFloatValue(groupName_, "kGravity");

}

void Player::EnemyStep(bool step) {

	if (step) {
		isStep_ = true;
	}
	else {
		isSteped_ = true;
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

	float deletaTime = FrameInfo::GetInstance()->GetDelta();

	if (!isSteped_) {
		// 移動の単位ベクトル
		Vector3 move = {};

		// 左右移動
		if (input_->GetKey()->LongPush(DIK_A) || input_->GetKey()->LongPush(DIK_LEFT)) {
			move.x--;
		}
		if (input_->GetKey()->LongPush(DIK_D) || input_->GetKey()->LongPush(DIK_RIGHT)) {
			move.x++;
		}

		// ジャンプ入力
		if (isStep_ || (!isFly_ && (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetKey()->Pushed(DIK_W) || input_->GetKey()->Pushed(DIK_UP)))) {
			isFly_ = true;
			isStep_ = false;
			// 初速を与える
			velocity_.y = kJampInitialVelocity_;
		}

		// 空中にいる時の処理。主に重力計算
		if (isFly_) {
			velocity_.y += kGravity_ * deletaTime;
		}

		// 横の移動距離
		velocity_.x = move.x * kMoveSpeed_ * deletaTime;

		tex_.pos += velocity_;
	}
	else {
		velocity_.y += kGravity_ * deletaTime;
	}
	
	
	// 地面との当たり判定。
	if (tex_.pos.y <= 0.0f && isFly_) {
		tex_.pos.y = 0.0f;
		velocity_.y = 0.0f;
		isFly_ = false;
		isSteped_ = false;
		// 衝撃波フラグ用意
	}

	tex_.Update();
}

//void Player::Draw(const Mat4x4& viewProjection) {
//
//}

void Player::Draw2D(const Mat4x4& viewProjection) {

	tex_.Draw(viewProjection, Pipeline::Normal, false);
}
