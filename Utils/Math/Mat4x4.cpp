#include "Mat4x4.h"
#include "Vector3.h"
#include <cmath>
#include <algorithm>
#include <Windows.h>
#include <immintrin.h>

Mat4x4::Mat4x4()
	:m()
{}

Mat4x4::Mat4x4(const Mat4x4& mat) {
	*this = mat;
}

Mat4x4::Mat4x4(Mat4x4&& mat) noexcept {
	*this = std::move(mat);
}

Mat4x4::Mat4x4(const std::array<Vector4, 4>& num) {
	m = num;
}

Mat4x4& Mat4x4::operator=(const Mat4x4& mat) {
	std::copy(mat.m.begin(), mat.m.end(), m.begin());

	return *this;
}

Mat4x4& Mat4x4::operator=(Mat4x4&& mat) noexcept {
	m = std::move(mat.m);

	return *this;
}

Mat4x4 Mat4x4::operator*(const Mat4x4& mat) const {
	Mat4x4 result;

	for (int y = 0; y < Mat4x4::HEIGHT; y++) {
		for (int x = 0; x < Mat4x4::WIDTH; x++) {
			for (int i = 0; i < Mat4x4::WIDTH; i++) {
				result.m[y][x] += this->m[y][i] * mat.m[i][x];
			}
		}
	}

	return result;
}

Mat4x4& Mat4x4::operator*=(const Mat4x4& mat) {
	*this = *this * mat;

	return *this;
}

Mat4x4 Mat4x4::operator+(const Mat4x4& mat) const {
	Mat4x4 tmp;

	for (int y = 0; y < Mat4x4::HEIGHT; y++) {
		for (int x = 0; x < Mat4x4::WIDTH; x++) {
			tmp[y][x] = this->m[y][x] + mat.m[y][x];
		}
	}

	return tmp;
}
Mat4x4& Mat4x4::operator+=(const Mat4x4& mat) {
	for (int y = 0; y < Mat4x4::HEIGHT; y++) {
		for (int x = 0; x < Mat4x4::WIDTH; x++) {
			this->m[y][x] += mat.m[y][x];
		}
	}

	return *this;
}
Mat4x4 Mat4x4::operator-(const Mat4x4& mat) const {
	Mat4x4 tmp;

	for (int y = 0; y < Mat4x4::HEIGHT; y++) {
		for (int x = 0; x < Mat4x4::WIDTH; x++) {
			tmp[y][x] = this->m[y][x] - mat.m[y][x];
		}
	}

	return tmp;
}
Mat4x4& Mat4x4::operator-=(const Mat4x4& mat) {
	for (int y = 0; y < Mat4x4::HEIGHT; y++) {
		for (int x = 0; x < Mat4x4::WIDTH; x++) {
			this->m[y][x] -= mat.m[y][x];
		}
	}

	return *this;
}

Vector4& Mat4x4::operator[](size_t index) {
	return m[index];
}

const Vector4& Mat4x4::operator[](size_t index) const {
	return m[index];
}

bool Mat4x4::operator==(const Mat4x4& mat) const {
	return m == mat.m;
}

bool Mat4x4::operator!=(const Mat4x4& mat) const {
	return m != mat.m;
}

void Mat4x4::Indentity() {
	m = {};

	for (int i = 0; i < WIDTH; i++) {
		m[i][i] = 1.0f;
	}
}

void Mat4x4::HoriTranslate(const Vector3& vec) {
	this->m = {};

	this->m[0][0] = 1.0f;
	this->m[1][1] = 1.0f;
	this->m[2][2] = 1.0f;
	this->m[3][3] = 1.0f;

	this->m[3][0] = vec.x;
	this->m[3][1] = vec.y;
	this->m[3][2] = vec.z;
}
void Mat4x4::VertTranslate(const Vector3& vec) {
	this->m = {};

	this->m[0][0] = 1.0f;
	this->m[1][1] = 1.0f;
	this->m[2][2] = 1.0f;
	this->m[3][3] = 1.0f;

	this->m[0][3] = vec.x;
	this->m[1][3] = vec.y;
	this->m[2][3] = vec.z;
}

void Mat4x4::Scalar(const Vector3& vec) {
	this->m = {};

	this->m[0][0] = vec.x;
	this->m[1][1] = vec.y;
	this->m[2][2] = vec.z;
	this->m[3][3] = 1.0f;
}

void Mat4x4::HoriRotateX(float rad) {
	this->m = {};
	this->m[0][0] = 1.0f;
	this->m[3][3] = 1.0f;

	this->m[1][1] = std::cos(rad);
	this->m[1][2] = std::sin(rad);
	this->m[2][1] = -std::sin(rad);
	this->m[2][2] = std::cos(rad);
}
void Mat4x4::VertRotateX(float rad) {
	this->m = {};
	this->m[0][0] = 1.0f;
	this->m[3][3] = 1.0f;

	this->m[1][1] = std::cos(rad);
	this->m[2][1] = std::sin(rad);
	this->m[1][2] = -std::sin(rad);
	this->m[2][2] = std::cos(rad);
}

void Mat4x4::HoriRotateY(float rad) {
	this->m = {};
	this->m[1][1] = 1.0f;
	this->m[3][3] = 1.0f;

	this->m[0][0] = std::cos(rad);
	this->m[0][2] = -std::sin(rad);
	this->m[2][0] = std::sin(rad);
	this->m[2][2] = std::cos(rad);
}
void Mat4x4::VertRotateY(float rad) {
	this->m = {};
	this->m[1][1] = 1.0f;
	this->m[3][3] = 1.0f;

	this->m[0][0] = std::cos(rad);
	this->m[2][0] = -std::sin(rad);
	this->m[0][2] = std::sin(rad);
	this->m[2][2] = std::cos(rad);
}

void Mat4x4::HoriRotateZ(float rad) {
	this->m = {};
	this->m[2][2] = 1.0f;
	this->m[3][3] = 1.0f;
	
	this->m[0][0] = std::cos(rad);
	this->m[0][1] = -std::sin(rad);
	this->m[1][0] = std::sin(rad);
	this->m[1][1] = std::cos(rad);
}
void Mat4x4::VertRotateZ(float rad) {
	this->m = {};
	this->m[2][2] = 1.0f;
	this->m[3][3] = 1.0f;

	this->m[0][0] = std::cos(rad);
	this->m[1][0] = std::sin(rad);
	this->m[0][1] = -std::sin(rad);
	this->m[1][1] = std::cos(rad);
}

void Mat4x4::HoriAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate) {
	Mat4x4 rotate = HoriMakeMatrixRotateX(rad.x) * HoriMakeMatrixRotateY(rad.y) * HoriMakeMatrixRotateZ(rad.z);

	*this = Mat4x4{ 
		std::array<Vector4, 4>{
		Vector4{scale.x * rotate.m[0][0], scale.x * rotate.m[0][1],scale.x * rotate.m[0][2], 0.0f},
		Vector4{scale.y * rotate.m[1][0], scale.y * rotate.m[1][1],scale.y * rotate.m[1][2], 0.0f },
		Vector4{scale.z * rotate.m[2][0], scale.z * rotate.m[2][1],scale.z * rotate.m[2][2], 0.0f},
		Vector4{translate.x, translate.y, translate.z, 1.0f}
		}
	};
}
void Mat4x4::VertAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate) {
	Mat4x4 rotate = VertMakeMatrixRotateZ(rad.z) * VertMakeMatrixRotateY(rad.y) * VertMakeMatrixRotateX(rad.x);

	*this = Mat4x4{
		std::array<Vector4, 4>{
		Vector4{ scale.x * rotate.m[0][0], scale.y * rotate.m[0][1], scale.z * rotate.m[0][2], translate.x },
		Vector4{ scale.x * rotate.m[1][0], scale.y * rotate.m[1][1], scale.z * rotate.m[1][2], translate.y },
		Vector4{ scale.x * rotate.m[2][0], scale.y * rotate.m[2][1], scale.z * rotate.m[2][2], translate.z },
		Vector4{ 0.0f, 0.0f, 0.0f, 1.0f }
		}
	};
}


void Mat4x4::Inverse() {
	Mat4x4 tmp = *this;

	Mat4x4 identity = MakeMatrixIndentity();

	float toOne = tmp[0][0];

	float tmpNum = 0.0f;

	for (int i = 0; i < Mat4x4::HEIGHT; i++) {
		if (tmp.m[i][i] == 0.0f && i < Mat4x4::HEIGHT) {
			int pibIndex = i;
			float pibot = fabsf(tmp.m[i][i]);

			for (int y = i + 1; y < Mat4x4::HEIGHT; y++) {
				if (tmp.m[y][i] != 0.0f && pibot < fabsf(tmp.m[y][i])) {
					pibot = fabsf(tmp.m[y][i]);
					pibIndex = y;
				}
			}

			if (pibot == 0.0f) {
				return;
			}

			tmp.m[i].m.swap(tmp.m[pibIndex].m);
			identity.m[i].m.swap(identity.m[pibIndex].m);
		}

		toOne = tmp.m[i][i];
		for (int x = 0; x < Mat4x4::HEIGHT; x++) {
			tmp.m[i][x] /= toOne;
			identity.m[i][x] /= toOne;
		}

		for (int y = 0; y < Mat4x4::HEIGHT; ++y) {
			if (i == y) {
				continue;
			}

			tmpNum = -tmp.m[y][i];
			for (int x = 0; x < Mat4x4::WIDTH; x++) {
				tmp.m[y][x] += tmpNum * tmp.m[i][x];
				identity.m[y][x] += tmpNum * identity.m[i][x];
			}
		}
	}

	if (tmp != MakeMatrixIndentity()) {
		return;
	}

	*this = identity;
}


void Mat4x4::Transepose() {
	std::swap(m[1][0], m[0][1]);
	std::swap(m[2][0], m[0][2]);
	std::swap(m[3][0], m[0][3]);
	std::swap(m[2][1], m[1][2]);
	std::swap(m[2][3], m[3][2]);
	std::swap(m[3][1], m[1][3]);
}

void Mat4x4::HoriPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
	m = {};

	m[0][0] = (1.0f / aspectRatio) * (1.0f / std::tan(fovY / 2.0f));
	m[1][1] = 1.0f / std::tan(fovY / 2.0f);
	m[2][2] = farClip / (farClip - nearClip);
	m[2][3] = 1.0f;
	m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
}
void Mat4x4::VertPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
	m = {};

	m[0][0] = (1.0f / aspectRatio) * (1.0f / std::tan(fovY / 2.0f));
	m[1][1] = 1.0f / std::tan(fovY / 2.0f);
	m[2][2] = farClip / (farClip - nearClip);
	m[3][2] = 1.0f;
	m[2][3] = (-nearClip * farClip) / (farClip - nearClip);
}

void Mat4x4::HoriOrthographic(float left, float top, float right, float bottom, float nearClip, float farClip) {
	m = {};

	m[0][0] = 2.0f / (right - left);
	m[1][1] = 2.0f / (top - bottom);
	m[2][2] = 1.0f / (farClip - nearClip);
	m[3][3] = 1.0f;

	m[3][0] = (left + right) / (left - right);
	m[3][1] = (top + bottom) / (bottom - top);
	m[3][2] = nearClip / (nearClip - farClip);
}
void Mat4x4::VertOrthographic(float left, float top, float right, float bottom, float nearClip, float farClip) {
	m = {};

	m[0][0] = 2.0f / (right - left);
	m[1][1] = 2.0f / (top - bottom);
	m[2][2] = 1.0f / (farClip - nearClip);
	m[3][3] = 1.0f;

	m[0][3] = (left + right) / (left - right);
	m[1][3] = (top + bottom) / (bottom - top);
	m[2][3] = nearClip / (nearClip - farClip);
}

void Mat4x4::HoriViewPort(float left, float top, float width, float height, float minDepth, float maxDepth) {
	m = {};

	m[0][0] = width / 2.0f;
	m[1][1] = height/ -2.0f;
	m[2][2] = maxDepth - minDepth;
	m[3][3] = 1.0f;

	m[3][0] = left + (width / 2.0f);
	m[3][1] = top + (height / 2.0f);
	m[3][2] = minDepth;
}
void Mat4x4::VertViewPort(float left, float top, float width, float height, float minDepth, float maxDepth) {
	m = {};

	m[0][0] = width / 2.0f;
	m[1][1] = height / -2.0f;
	m[2][2] = maxDepth - minDepth;
	m[3][3] = 1.0f;

	m[0][3] = left + (width / 2.0f);
	m[1][3] = top + (height / 2.0f);
	m[2][3] = minDepth;
}


Mat4x4 MakeMatrixIndentity() {
	Mat4x4 tmp;
	tmp.Indentity();
	return tmp;
}

Mat4x4 MakeMatrixInverse(Mat4x4 mat) {
	Mat4x4 tmp = mat;
	tmp.Inverse();
	return tmp;
}

Mat4x4 MakeMatrixTransepose(Mat4x4 mat) {
	Mat4x4 tmp = mat;
	tmp.Transepose();
	return tmp;
}

Mat4x4 HoriMakeMatrixTranslate(Vector3 vec) {
	Mat4x4 mat;

	mat.HoriTranslate(vec);

	return mat;
}
Mat4x4 VertMakeMatrixTranslate(Vector3 vec) {
	Mat4x4 mat;

	mat.VertTranslate(vec);

	return mat;
}

Mat4x4 MakeMatrixScalar(Vector3 vec) {
	Mat4x4 mat;

	mat.Scalar(vec);

	return mat;
}

Mat4x4 HoriMakeMatrixRotateX(float rad) {
	Mat4x4 tmp;

	tmp.HoriRotateX(rad);

	return tmp;
}
Mat4x4 VertMakeMatrixRotateX(float rad) {
	Mat4x4 tmp;

	tmp.VertRotateX(rad);

	return tmp;
}

Mat4x4 HoriMakeMatrixRotateY(float rad) {
	Mat4x4 tmp;

	tmp.HoriRotateY(rad);

	return tmp;
}
Mat4x4 VertMakeMatrixRotateY(float rad) {
	Mat4x4 tmp;

	tmp.VertRotateY(rad);

	return tmp;
}

Mat4x4 HoriMakeMatrixRotateZ(float rad) {
	Mat4x4 tmp;

	tmp.HoriRotateZ(rad);

	return tmp;
}
Mat4x4 VertMakeMatrixRotateZ(float rad) {
	Mat4x4 tmp;

	tmp.VertRotateZ(rad);

	return tmp;
}

Mat4x4 HoriMakeMatrixAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate) {
	Mat4x4 tmp;

	tmp.HoriAffin(scale, rad, translate);

	return tmp;
}
Mat4x4 VertMakeMatrixAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate) {
	Mat4x4 tmp;

	tmp.VertAffin(scale, rad, translate);

	return tmp;
}

Mat4x4 HoriMakeMatrixPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
	Mat4x4 tmp;

	tmp.HoriPerspectiveFov(fovY, aspectRatio, nearClip, farClip);

	return tmp;
}
Mat4x4 VertMakeMatrixPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
	Mat4x4 tmp;

	tmp.VertPerspectiveFov(fovY, aspectRatio, nearClip, farClip);

	return tmp;
}

Mat4x4 HoriMakeMatrixOrthographic(float left, float right, float top, float bottom, float nearClip, float farClip) {
	Mat4x4 tmp;

	tmp.HoriOrthographic(left, right, top, bottom, nearClip, farClip);

	return tmp;
}
Mat4x4 VertMakeMatrixOrthographic(float left, float right, float top, float bottom, float nearClip, float farClip) {
	Mat4x4 tmp;

	tmp.VertOrthographic(left, right, top, bottom, nearClip, farClip);

	return tmp;
}

Mat4x4 HoriMakeMatrixViewPort(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Mat4x4 tmp;

	tmp.HoriViewPort(left, top, width, height, minDepth, maxDepth);

	return tmp;
}
Mat4x4 VertMakeMatrixViewPort(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Mat4x4 tmp;

	tmp.VertViewPort(left, top, width, height, minDepth, maxDepth);

	return tmp;
}