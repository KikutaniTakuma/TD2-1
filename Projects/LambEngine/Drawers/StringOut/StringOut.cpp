#include "StringOut.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"
#include "Engine/Core/StringOutPutManager/StringOutPutManager.h"
#include "Engine/Engine.h"
#include "imgui.h"
#include "Utils/ConvertString/ConvertString.h"
#include <format>


StringOut::StringOut():
#ifdef _DEBUG
	debugStr_{},
#endif // _DEBUG

	format_(),
	str_(),
	pos_(),
	rotation_(0.0f),
	scale_(Vector2::identity),
	color_(0xffffffff),
	isHorizontal_(false)
{
	str_.reserve(0x40);
}

StringOut::StringOut(const std::string& formatName) :
	format_(formatName),
	str_(),
	pos_(),
	rotation_(0.0f),
	scale_(Vector2::identity),
	color_(0xffffffff),
	isHorizontal_(false)
{
	str_.reserve(0x40);
}

StringOut::StringOut(const std::wstring& formatName) :
	format_(ConvertString(formatName)),
	str_(),
	pos_(),
	rotation_(0.0f),
	scale_(Vector2::identity),
	color_(0xffffffff),
	isHorizontal_(false)
{
	str_.reserve(0x40);
}

StringOut& StringOut::operator=(const std::string& right) {
	str_ = ConvertString(right);

	return *this;
}

StringOut& StringOut::operator=(const std::wstring& right) {
	str_ = right;

	return *this;
}

StringOut& StringOut::operator+=(const std::string& right) {
	str_ += ConvertString(right);

	return *this;
}
StringOut& StringOut::operator+=(const std::wstring& right) {
	str_ += right;

	return *this;
}

StringOut& StringOut::operator<<(const std::string& right) {
	str_ += ConvertString(right);

	return *this;
}
StringOut& StringOut::operator<<(const std::wstring& right) {
	str_ += right;

	return *this;
}

StringOut& StringOut::operator<<(const Vector2& right) {
	str_ += std::format(L"{:6.3f}, {:6.3f}", right.x, right.y);

	return *this;
}
StringOut& StringOut::operator<<(const Vector3& right) {
	str_ += std::format(L"{:6.3f}, {:6.3f}, {:6.3f}", right.x, right.y, right.z);

	return *this;
}
StringOut& StringOut::operator<<(const Vector4& right) {
	str_ += std::format(L"{:6.3f}, {:6.3f}, {:6.3f}, {:6.3f}", 
		right.m[0], right.m[1], right.m[2], right.m[3]
	);

	return *this;
}
StringOut& StringOut::operator<<(const Quaternion& right) {
	*this << right.vector4_;

	return *this;
}

StringOut& StringOut::operator>>(std::wstring& right) {
	right = str_;

	Clear();

	return *this;
}

StringOut& StringOut::operator>>(std::string& right) {
	right = ConvertString(str_);

	Clear();

	return *this;
}

void StringOut::Draw() {
	if (str_.empty()) {
		return;
	}

	ID3D12GraphicsCommandList* commandList = DirectXCommand::GetInstance()->GetCommandList();
	auto  stringOutPutManager = StringOutPutManager::GetInstance();
	
	auto batch = stringOutPutManager->GetBatch(format_);

	batch->Begin(commandList);

	stringOutPutManager->GetFont(format_)->DrawString(
		stringOutPutManager->GetBatch(format_),
		str_.c_str(),
		DirectX::XMFLOAT2(pos_.x, pos_.y),
		UintToVector4(color_).m128,
		rotation_,
		DirectX::XMFLOAT2(0.0f, 0.0f),
		DirectX::XMFLOAT2(scale_.x, scale_.y)
	);
	batch->End();
}

void StringOut::Clear() {
	str_.clear();
}

void StringOut::Debug([[maybe_unused]]const std::string& debugName) {
#ifdef _DEBUG
	static Vector4 debugColor;
	debugColor = UintToVector4(color_);
	debugStr_.resize(64);
	debugStr_ = ConvertString(str_);

	ImGui::Begin(debugName.c_str());
	ImGui::InputText("text", debugStr_.data(), debugStr_.size());
	ImGui::DragFloat2("pos", &pos_.x);
	ImGui::DragFloat("rotation", &rotation_, 0.01f);
	ImGui::DragFloat2("scale", &scale_.x, 0.01f);
	ImGui::ColorEdit4("SphereColor", &debugColor.color.r);
	ImGui::Checkbox("isHorizontal", &isHorizontal_);
	ImGui::End();

	str_ = ConvertString(debugStr_);

	color_ = Vector4ToUint(debugColor);
#endif // _DEBUG
}

void StringOut::SetFormat(const std::string& formatName) {
	format_ = formatName;
}
void StringOut::SetFormat(const std::wstring& formatName) {
	format_ = ConvertString(formatName);
}