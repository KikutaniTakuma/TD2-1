#include "Vector3.h"
#include <cmath>
#include "Mat4x4.h"
#include "Vector2.h"
#include <cassert>
#include "Engine/ErrorCheck/ErrorCheck.h"

Vector3::Vector3() noexcept :
	x(0.0f),
	y(0.0f),
	z(0.0f)
{}

Vector3::Vector3(float x, float y, float z) noexcept :
	x(x),
	y(y),
	z(z)
{}

Vector3::Vector3(const Vector3& right) noexcept
{
	*this = right;
}
Vector3::Vector3(const Vector2& right) noexcept {
	x = right.x;
	y = right.y;
	z = 0.0f;
}

Vector3::Vector3(Vector3&& right) noexcept
{
	*this = std::move(right);
}

const Vector3 Vector3::identity = { 1.0f,1.0f,1.0f };
const Vector3 Vector3::zero = { 0.0f, 0.0f,0.0f };
const Vector3 Vector3::xIdy = { 1.0f,0.0f,0.0f };
const Vector3 Vector3::yIdy = { 0.0f,1.0f,0.0f };
const Vector3 Vector3::zIdy = { 0.0f,0.0f,1.0f };

Vector3 Vector3::operator+() const noexcept {
	return *this;
}
Vector3 Vector3::operator-() const noexcept {
	return Vector3(-x, -y, -z);
}

Vector3& Vector3::operator=(const Vector3& right) noexcept {
	x = right.x;
	y = right.y;
	z = right.z;

	return *this;
}

Vector3& Vector3::operator=(Vector3&& right) noexcept {
	x = std::move(right.x);
	y = std::move(right.y);
	z = std::move(right.z);

	return *this;
}

Vector3 Vector3::operator+(const Vector3& right) const noexcept {
	Vector3 tmp(x + right.x, y + right.y, z + right.z);

	return tmp;
}
Vector3 Vector3::operator-(const Vector3& right) const noexcept {
	Vector3 tmp(x - right.x, y - right.y, z - right.z);

	return tmp;
}
Vector3& Vector3::operator+=(const Vector3& right) noexcept {
	*this = *this + right;

	return *this;
}
Vector3& Vector3::operator-=(const Vector3& right) noexcept {
	*this = *this - right;

	return *this;
}

Vector3 Vector3::operator*(float scalar) const noexcept {
	Vector3 tmp(x * scalar, y * scalar, z * scalar);

	return tmp;
}
Vector3& Vector3::operator*=(float scalar) noexcept {
	*this = *this * scalar;

	return *this;
}

Vector3 Vector3::operator/(float scalar) const noexcept {
	Vector3 tmp(x / scalar, y / scalar, z / scalar);

	return tmp;
}
Vector3& Vector3::operator/=(float scalar) noexcept {
	*this = *this / scalar;

	return *this;
}

Vector3 Vector3::operator*(const Mat4x4& mat) const noexcept {
	Vector3 result;
	Vector4 vec = { *this,1.0f };
	result.x = x * mat[0][0] + y * mat[1][0] + z * mat[2][0] + 1.0f * mat[3][0];
	result.y = x * mat[0][1] + y * mat[1][1] + z * mat[2][1] + 1.0f * mat[3][1];
	result.z = x * mat[0][2] + y * mat[1][2] + z * mat[2][2] + 1.0f * mat[3][2];
	float&& w = x * mat[0][3] + y * mat[1][3] + z * mat[2][3] + 1.0f * mat[3][3];
	assert(w != 0.0f);
	if (w == 0.0f) {
		ErrorCheck::GetInstance()->ErrorTextBox("Vector3 * Matrix4x4 : w = 0.0f", "Vector3");
	}
	w = 1.0f / w;
	result.x *= w;
	result.y *= w;
	result.z *= w;

	return result;
}

Vector3 operator*(const Mat4x4& left, const Vector3& right) {
	Vector3 result;
	Vector4 vec = { right,1.0f };

	result.x = left[0].Dot(vec);
	result.y = left[1].Dot(vec);
	result.z = left[2].Dot(vec);
	float&& w = left[3].Dot(vec);
	assert(w != 0.0f);
	if (w == 0.0f) {
		ErrorCheck::GetInstance()->ErrorTextBox("Vector3 * Matrix4x4 : w = 0.0f", "Vector3");
	}

	w = 1.0f / w;
	result.x *= w;
	result.y *= w;
	result.z *= w;

	return result;
}

Vector3& Vector3::operator*=(const Mat4x4& mat) noexcept {
	*this = *this * mat;

	return *this;
}

Vector3& Vector3::operator=(const Vector2& right) noexcept {
	x = right.x;
	y = right.y;

	return *this;
}

bool Vector3::operator==(const Vector3& right) const noexcept {
	return x == right.x && y == right.y && z == right.z;
}
bool Vector3::operator!=(const Vector3& right) const noexcept {
	return x != right.x || y != right.y || z != right.z;
}

float Vector3::Length() const noexcept {
	return std::sqrt(x*x + y*y + z*z);
}

float Vector3::Dot(const Vector3& right) const noexcept {
	return x * right.x + y * right.y + z * right.z;
}

Vector3 Vector3::Cross(const Vector3& right) const noexcept {
	Vector3 result;

	result.x = y * right.z - z * right.y;
	result.y = z * right.x - x * right.z;
	result.z = x * right.y - y * right.x;

	return result;
}

Vector3 Vector3::Normalize() const noexcept {
	if (*this == Vector3::zero) {
		return Vector3::zero;
	}

	return *this / this->Length();
}

Vector3 Vector3::GetRad() const noexcept {
	if (*this == Vector3::zero) {
		return Vector3::zero;
	}

	Vector3 result;
	result.z = std::atan2(y, x);
	result.y = std::atan2(x, z);
	result.x = std::atan2(y, z);

	return result;
}

Vector3 Vector3::Lerp(const Vector3& start, const Vector3& end, float t) {
	Vector3 result;

	result.x = std::lerp(start.x, end.x, t);
	result.y = std::lerp(start.y, end.y, t);
	result.z = std::lerp(start.z, end.z, t);

	return result;
}