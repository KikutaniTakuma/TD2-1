#include "Star.h"
#include "externals/imgui/imgui.h"

Star::Star() :
	tex_{},
	ease_{},
	scaleEase_{},
	rotateEase_{},
	isStart_{false},
	isEnd_{false},
	scaleEaseTime_{1.2f},
	rotateEaseTime_{0.8f},
	specialScaleEaseTime_{ 1.0f }
{
	scaleDuration_.first = Vector2::zero;
	scaleDuration_.second = { 200.0f, 200.0f };

	scaleNormalDuration_.first = scaleDuration_.second;
	scaleNormalDuration_.second = scaleDuration_.second * 1.1f;

	rotateDuration_.second.z = -6.28f;

	tex_.LoadTexture("./Resources/Result/Star.png");
}

void Star::Start() {
	isStart_ = true;
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

	if (ImGui::Button("start")) {
		this->Start();
	}
	ImGui::End();
#endif // _DEBUG
}

void Star::Update() {
	// 1.スターの演出開始
	if (isStart_.OnEnter()) {
		// 星3つの演出
		if (isSpecial_) {
			rotateEase_.Start(false, rotateEaseTime_, Easeing::InOutCirc);
			ease_.Start(false, scaleEaseTime_, Easeing::OutBack);
		}
		else {
			ease_.Start(false, specialScaleEaseTime_, Easeing::OutElastic);
		}
	}

	// 3.スターの演出終了
	if (ease_.ActiveExit()) {
		isStart_ = false;
		tex_.rotate = rotate_;
		isEnd_ = true;

		scaleEase_.Start(true, 0.2f, Easeing::InExpo);
	}

	// 2.スターの演出実行時
	if (isStart_) {
		// 特別演出
		if (isSpecial_) {
			// 回転
			tex_.rotate = rotate_ + rotateEase_.Get(rotateDuration_.first, rotateDuration_.second);
		}
		tex_.scale = ease_.Get(scaleDuration_.first, scaleDuration_.second);
	}

	// 4.スター演出が終わった
	if (isEnd_) {
		// 大きさ
		tex_.scale = scaleEase_.Get(scaleNormalDuration_.first, scaleNormalDuration_.second);
	}


	
	// 板ポリアップデート
	tex_.Update();

	// 各種イージングアップデート
	ease_.Update();
	scaleEase_.Update();
	rotateEase_.Update();

	//各フラグアップデート
	isStart_.Update();
	isEnd_.Update();
	isSpecial_.Update();
}

void Star::Draw(const Mat4x4& viewProjection) {
	tex_.Draw(viewProjection, Pipeline::Normal, false);
}