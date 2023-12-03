#include "Star.h"
#include "imgui.h"

Star::Star() :
	tex_{},
	ease_{},
	scaleEase_{},
	rotateEase_{},
	isStart_{false},
	isEnd_{false},
	scaleEaseTime_{1.0f},
	rotateEaseTime_{1.0f},
	specialScaleEaseTime_{ 1.4f }
{
	scale_ = Vector2::zero;
		
	scaleDuration_.first = Vector2::zero;
	scaleDuration_.second = Vector2{ 200.0f, 200.0f } *0.8f;

	scaleNormalDuration_.first = scaleDuration_.second;
	scaleNormalDuration_.second = scaleDuration_.second * 1.1f;

	rotateDuration_.first.z = -6.28f;

	tex_.LoadTexture("./Resources/Result/Star.png");
}

void Star::Start() {
	isStart_ = true;
}

void Star::NormalStart() {
	scaleEase_.Start(true, 0.2f, Easing::InExpo);
}

void Star::SetDefaultScale(float magnification) {
	scale_ = scaleDuration_.second;
	scale_*= magnification;
	tex_.scale_ = scale_;
}

void Star::Debug([[maybe_unused]]const std::string& guiName) {
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	ImGui::DragFloat4("effect star", &scaleDuration_.first.x);
	ImGui::DragFloat4("normal star", &scaleNormalDuration_.first.x);
	ImGui::DragFloat("scale star ease spd", &scaleEaseTime_, 0.01f);
	ImGui::DragFloat("rotate star ease spd", &rotateEaseTime_, 0.01f);

	ImGui::DragFloat3("rotate normal star first", &rotateDuration_.first.x, 0.01f);
	ImGui::DragFloat3("rotate normal star second", &rotateDuration_.second.x, 0.01f);

	ImGui::Checkbox("is special", isSpecial_.Data());

	ImGui::DragFloat2("pos", &pos_.x);

	if (ImGui::Button("start")) {
		this->Start();
	}
	ImGui::End();
#endif // _DEBUG
}

void Star::Update() {
	isEnd_.Update();

	// 1.スターの演出開始
	if (isStart_.OnEnter()) {
		// 星3つの演出
		if (isSpecial_) {
			rotateEase_.Start(false, rotateEaseTime_, Easing::InOutCirc);
			ease_.Start(false, specialScaleEaseTime_, Easing::OutBack);
		}
		// 通常の演出
		else {
			ease_.Start(false, scaleEaseTime_, Easing::OutElastic);
		}
	}

	// 2.スターの演出実行時
	if (isStart_) {
		// 特別演出
		if (isSpecial_) {
			// 回転
			tex_.rotate_ = rotate_ + rotateEase_.Get(rotateDuration_.first, rotateDuration_.second);
		}
		scale_ = ease_.Get(scaleDuration_.first, scaleDuration_.second);
	}

	// 3.スターの演出終了
	if (ease_.ActiveExit()) {
		isStart_ = false;
		tex_.rotate_ = rotate_;
		isEnd_ = true;
	}


	// 4.スター演出が終わった
	if (!isStart_ && isEnd_) {
		// 大きさ
		scale_ = scaleEase_.Get(scaleNormalDuration_.first, scaleNormalDuration_.second);
	}

	tex_.scale_ = scale_;
	tex_.pos_ = pos_;
	
	// 板ポリアップデート
	tex_.Update();

	// 各種イージングアップデート
	ease_.Update();
	scaleEase_.Update();
	rotateEase_.Update();

	//各フラグアップデート
	isStart_.Update();
	isSpecial_.Update();
}

void Star::Draw(const Mat4x4& viewProjection) {
	tex_.Draw(viewProjection, Pipeline::Normal, false);
}