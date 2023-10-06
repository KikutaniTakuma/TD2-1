#include "Camera.h"
#include "Engine/Engine.h"
#include "Input/KeyInput/KeyInput.h"
#include "Input/Mouse/Mouse.h"
#include "Input/Gamepad/Gamepad.h"
#include "externals/imgui/imgui.h"
#include "Engine/WinApp/WinApp.h"
#include <numbers>
#include <cmath>

Camera::Camera() noexcept :
	type(Type::Projecction),
	isDebug(false),
	pos(),
	scale(Vector3::identity),
	rotate(),
	drawScale(1.0f),
	moveVec(),
	moveSpd(1.65f),
	moveRotateSpd(std::numbers::pi_v<float> / 720.0f),
	gazePointRotate(),
	gazePointRotateSpd(std::numbers::pi_v<float> / 90.0f),
	farClip(1000.0f),
	fov(0.45f),
	view(),
	projection(),
	othograohics()
{}

Camera::Camera(Camera::Type mode) noexcept :
	type(mode),
	isDebug(false),
	pos(),
	scale(Vector3::identity),
	rotate(),
	drawScale(1.0f),
	moveVec(),
	moveSpd(0.02f),
	moveRotateSpd(std::numbers::pi_v<float> / 720.0f),
	gazePointRotate(),
	gazePointRotateSpd(std::numbers::pi_v<float> / 9.0f),
	farClip(1000.0f),
	fov(0.45f),
	view(),
	projection(),
	othograohics()
{}

Camera::Camera(const Camera& right) noexcept
{
	*this = right;
}

Camera::Camera(Camera&& right) noexcept
{
	*this = std::move(right);
}

void Camera::Update(const Vector3& gazePoint) {
	moveVec = Vector3();


	view.VertAffin(scale, rotate, pos + gazePoint);
	view = VertMakeMatrixAffin(Vector3::identity, Vector3(gazePointRotate.y, gazePointRotate.x, 0.0f), pos + gazePoint) * view;
	worldPos = { view[0][3],view[1][3], view[2][3] };
	view.Inverse();

	static auto engine = Engine::GetInstance();
	static const float aspect = static_cast<float>(engine->clientWidth) / static_cast<float>(engine->clientHeight);

	const auto&& windowSize = WinApp::GetInstance()->GetWindowSize();

	switch (type)
	{
	case Camera::Type::Projecction:
	default:
		fov = std::clamp(fov, 0.0f, 1.0f);
		projection.VertPerspectiveFov(fov, aspect, kNearClip, farClip);
		viewProjecction = projection * view;

		viewProjecctionVp = VertMakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f) * viewProjecction;

	/*	billViewProjecction = VertMakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f) * 
			projection * ;*/
		break;

	case Camera::Type::Othographic:
		othograohics.VertOrthographic(
			-static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
			static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
			static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
			-static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
			kNearClip, farClip);
		viewOthograohics = othograohics * view;


		viewOthograohicsVp = VertMakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f) * viewOthograohics;
		break;
	}
}

void Camera::Update(const Mat4x4& worldMat) {
	view.VertAffin(scale, rotate, pos);
	view = worldMat * view;
	worldPos = { view[0][3],view[1][3], view[2][3] };
	view.Inverse();

	static auto engine = Engine::GetInstance();
	static const float aspect = static_cast<float>(engine->clientWidth) / static_cast<float>(engine->clientHeight);

	const auto&& windowSize = WinApp::GetInstance()->GetWindowSize();

	switch (type)
	{
	case Camera::Type::Projecction:
	default:
		fov = std::clamp(fov, 0.0f, 1.0f);
		projection.VertPerspectiveFov(fov, aspect, kNearClip, farClip);
		viewProjecction = projection * view;

		viewProjecctionVp = VertMakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f) * viewProjecction;
		break;

	case Camera::Type::Othographic:
		othograohics.VertOrthographic(
			-static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
			static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
			static_cast<float>(engine->clientWidth) * 0.5f * drawScale,
			-static_cast<float>(engine->clientHeight) * 0.5f * drawScale,
			kNearClip, farClip);
		viewOthograohics = othograohics * view;


		viewOthograohicsVp = VertMakeMatrixViewPort(0.0f, 0.0f, windowSize.x, windowSize.y, 0.0f, 1.0f) * viewOthograohics;
		break;
	}
}