#include "Player.h"

#include "Engine/FrameInfo/FrameInfo.h"

Player::Player() {
	
	input_ = Input::GetInstance();

	tex_.LoadTexture("./Resources/uvChecker.png");

	globalVariables_ = std::make_unique<GlobalVariables>();

	tex_.scale *= 50.0f;

	// ジャンプ時の初速
	kFlyInitialVelocity_ = 10.0f;

	// 移動スピード
	kMoveSpeed_ = 5.0f;

	// 重力加速度
	kGravity_ = -9.8f;
}

void Player::SetGlobalVariable() {
	
	globalVariables_->CreateGroup(groupName_);

	globalVariables_->AddItem(groupName_, "kFlyInitialVelocity", kFlyInitialVelocity_);
	globalVariables_->AddItem(groupName_, "kMoveSpeed", kMoveSpeed_);
	globalVariables_->AddItem(groupName_, "kGravity", kGravity_);

	//globalVariables_->LoadFile(groupName_);
	ApplyGlobalVariable();
}

void Player::ApplyGlobalVariable() {

	kFlyInitialVelocity_ = globalVariables_->GetFloatValue(groupName_, "kFlyInitialVelocity");
	kMoveSpeed_ = globalVariables_->GetFloatValue(groupName_, "kMoveSpeed");
	kGravity_ = globalVariables_->GetFloatValue(groupName_, "kGravity");

}

void Player::Initialize() {

	SetGlobalVariable();

	// 空中にいるか
	isFly = false;
}

void Player::Update() {

	globalVariables_->Update();
	ApplyGlobalVariable();

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
	if (!isFly && (input_->GetKey()->Pushed(DIK_SPACE) || input_->GetKey()->Pushed(DIK_W) || input_->GetKey()->Pushed(DIK_UP))) {
		isFly = true;
		// 初速を与える
		move.y = kFlyInitialVelocity_;
	}

	float deletaTime = FrameInfo::GetInstance()->GetDelta();

	// 空中にいる時の処理。主に重力計算
	if (isFly) {
		move.y += kGravity_ * deletaTime;
	}

	// 横の移動距離
	move.x = move.x * kMoveSpeed_ * deletaTime;

	tex_.pos += move;
	
	// 地面との当たり判定。
	if (tex_.pos.y <= 0.0f && isFly) {
		tex_.pos.y = 0.0f;
		isFly = false;
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
