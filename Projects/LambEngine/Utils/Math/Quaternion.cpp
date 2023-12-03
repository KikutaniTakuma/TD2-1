#include "Quaternion.h"
#include "Mat4x4.h"
#include <cmath>

/// ========================================================================
/// 静的メンバ定数
/// ========================================================================
#pragma region Constant Number
const Quaternion Quaternion::identity = { 0.0f, 0.0f, 0.0f, 1.0f };
const Quaternion Quaternion::zero = { 0.0f, 0.0f, 0.0f, 0.0f };
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// コンストラクタ
/// ========================================================================
#pragma region Constructor
Quaternion::Quaternion():
	m_{0.0f}
{}

Quaternion::Quaternion(const Quaternion& right):
	Quaternion()
{
	*this = right;
}
Quaternion::Quaternion(Quaternion&& right) noexcept :
	Quaternion()
{
	*this = right;
}

Quaternion::Quaternion(const Vector4& right):
	Quaternion()
{
	*this = right;
}

Quaternion::Quaternion(const Vector3& right, float w) {
	vector_.vector3_ = right;
	vector_.w_ = w;
}

Quaternion::Quaternion(const std::array<float, 4>& right) {
	m_ = right;
}

Quaternion::Quaternion(float x, float y, float z, float w) {
	m_ = { x,y,z,w };
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// コピー演算子
/// ========================================================================
#pragma region Copy operator
Quaternion& Quaternion::operator=(const Quaternion& right) {
	m_ = right.m_;

	return *this;
}
Quaternion& Quaternion::operator=(Quaternion&& right)noexcept {
	m_ = std::move(right.m_);

	return *this;
}

Quaternion& Quaternion::operator=(const Vector4& right) {
	m_ = right.m;

	return *this;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// 単項演算子
/// ========================================================================
#pragma region Unary operator
Quaternion Quaternion::operator+() const {
	return *this;
}
Quaternion Quaternion::operator-() const {
	return Quaternion{ -m_[0],-m_[1],-m_[2],-m_[3] };
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// 乗算演算子
/// ========================================================================
#pragma region Multiplication operator
Quaternion Quaternion::operator*(const Quaternion& right) const {
	Quaternion result;

	result = Quaternion{
		vector_.vector3_.Cross(right.vector_.vector3_) + vector_.vector3_ * right.vector_.w_ + right.vector_.vector3_ * vector_.w_,
		vector_.w_ * right.vector_.w_ - vector_.vector3_.Dot(right.vector_.vector3_)
	};

	return result;
}
Quaternion& Quaternion::operator*=(const Quaternion& right) {
	*this = *this * right;

	return *this;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// 加算演算子
/// ========================================================================
#pragma region Add operator
Quaternion Quaternion::operator+(const Quaternion& right) const {
	Quaternion result;

	result = vector4_ + right.vector4_;

	return result;
}
Quaternion& Quaternion::operator+=(const Quaternion& right) {
	*this = *this + right;

	return *this;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// 減算演算子
/// ========================================================================
#pragma region Sub operator
Quaternion Quaternion::operator-(const Quaternion& right) const {
	Quaternion result;

	result = vector4_ - right.vector4_;

	return result;
}
Quaternion& Quaternion::operator-=(const Quaternion& right) {
	*this = *this - right;

	return *this;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// スカラー倍演算子
/// ========================================================================
#pragma region Scalar operator
Quaternion Quaternion::operator*(float right) const {
	Quaternion result{ *this };

	result.vector4_ *= right;

	return result;
}
Quaternion operator*(float right, const Quaternion& left) {
	return left * right;
}
Quaternion& Quaternion::operator*=(float right) {
	*this = *this * right;

	return *this;
}

Quaternion Quaternion::operator/(float right) const {
	return *this * (1.0f / right);
}
Quaternion operator/(float right, const Quaternion& left) {
	return left / right;
}
Quaternion& Quaternion::operator/=(float right) {
	*this = *this / right;

	return *this;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// 等値演算子
/// ========================================================================
#pragma region Equal operator
bool Quaternion::operator==(const Quaternion& right) const {
	return m_ == right.m_;
}
bool Quaternion::operator!=(const Quaternion& right) const {
	return m_ != right.m_;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================


/// ========================================================================
/// メンバ関数
/// ========================================================================
#pragma region Member function
Quaternion Quaternion::Conjugate() const {
	return Quaternion{ -vector_.vector3_, vector_.w_ };
}

float Quaternion::Dot(const Quaternion& other) const {
	return _mm_cvtss_f32(_mm_dp_ps(m128_, other.m128_, 0xff));
}

float Quaternion::Length() const {
	return std::sqrt(Dot(*this));
}

Quaternion Quaternion::Normalize() const {
	if (*this == Quaternion::zero) {
		return *this;
	}

	float nor = 1.0f / this->Length();

	return Quaternion{ *this } *nor;
}

Quaternion Quaternion::Inverce() const {
	return Conjugate() / std::pow(Length(), 2.0f);
}

Mat4x4 Quaternion::GetMatrix() const {
	Mat4x4 result = Mat4x4{
		std::array<Vector4, 4>{
			Vector4{
				std::pow(quaternion_.w_, 2.0f) + std::pow(quaternion_.x_, 2.0f) - std::pow(quaternion_.y_, 2.0f) - std::pow(quaternion_.z_, 2.0f),
				2.0f * (quaternion_.x_ * quaternion_.y_ + quaternion_.w_ * quaternion_.z_),
				2.0f * (quaternion_.x_ * quaternion_.z_ - quaternion_.w_ * quaternion_.y_),
				0.0f
			},
			Vector4{
				2.0f * (quaternion_.x_ * quaternion_.y_ - quaternion_.w_ * quaternion_.z_),
				std::pow(quaternion_.w_, 2.0f) - std::pow(quaternion_.x_, 2.0f) + std::pow(quaternion_.y_, 2.0f) - std::pow(quaternion_.z_, 2.0f),
				2.0f * (quaternion_.y_ * quaternion_.z_ + quaternion_.w_ * quaternion_.x_),
				0.0f
			},
			Vector4{
				2.0f * (quaternion_.x_ * quaternion_.z_ + quaternion_.w_ * quaternion_.y_),
				2.0f * (quaternion_.y_ * quaternion_.z_ - quaternion_.w_ * quaternion_.x_),
				std::pow(quaternion_.w_, 2.0f) - std::pow(quaternion_.x_, 2.0f) - std::pow(quaternion_.y_, 2.0f) + std::pow(quaternion_.z_, 2.0f),
				0.0f
			},
			Vector4::wIdy
		}
	};


	return result;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================

/// ========================================================================
/// 静的メンバ関数
/// ========================================================================
#pragma region Static member function
Quaternion Quaternion::MakeRotateAxisAngle(const Vector3& axis, float angle) {
	Quaternion result;
	result.vector_.w_ = std::cos(angle * 0.5f);
	result.vector_.vector3_ = axis * std::sin(angle * 0.5f);

	return result;
}
Quaternion Quaternion::MakeRotateXAxis(float eulerAngle) {
	return Quaternion{ std::sin(eulerAngle * 0.5f),0.0f,0.0f,std::cos(eulerAngle * 0.5f), };
}
Quaternion Quaternion::MakeRotateYAxis(float eulerAngle) {
	return Quaternion{ 0.0f, std::sin(eulerAngle * 0.5f),0.0f,std::cos(eulerAngle * 0.5f) };
}
Quaternion Quaternion::MakeRotateZAxis(float eulerAngle) {
	return Quaternion{ 0.0f, 0.0f, std::sin(eulerAngle * 0.5f),std::cos(eulerAngle * 0.5f) };
}

Quaternion Quaternion::Slerp(const Quaternion& start, const Quaternion& end, float t) {
	float dot = start.Dot(end);
	float theata = std::acos(dot);
	float sinTheata = 1.0f / std::sin(theata);

	static constexpr float kEpsilon = 0.0005f;

	Quaternion result;

	// sinθが0.0fになる場合またはそれに近くなる場合
	if (dot <= -1.0f + kEpsilon || 1.0f - kEpsilon <= dot || sinTheata == 0.0f) {
		result = (1.0f - t) * start + t * end;
	}
	// 近いほうで補完する
	else if (dot < 0.0f) {
		result = (std::sin(theata * (1.0f - t)) * sinTheata) * start + (std::sin(theata * t) * sinTheata) * -end;
	}
	else {
		result = (std::sin(theata * (1.0f - t)) * sinTheata) * start + (std::sin(theata * t) * sinTheata) * end;
	}

	return result;
}
#pragma endregion
/// ========================================================================
/// ========================================================================
/// ========================================================================