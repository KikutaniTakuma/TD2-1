#pragma once
#include <string>
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector4.h"
#include "Utils/ConvertString/ConvertString.h"


class StringOut {
public:
	StringOut();
	StringOut(const StringOut& right);
	StringOut(StringOut&& right) noexcept;
	StringOut(const std::string& formatName);
	StringOut(const std::wstring& formatName);
	~StringOut() = default;

public:
	StringOut& operator=(const StringOut& right);
	StringOut& operator=(StringOut&& right) noexcept;

public:
	void Draw();

#ifdef _DEBUG
	void Debug(const std::string& debugName);
#endif // _DEBUG


private:
	std::string format;
public:
	std::wstring str;

	Vector2 pos;
	float rotation;
	Vector2 scale;
	uint32_t color;
	bool isHorizontal;

public:
	inline void SetFormat(const std::string& formatName) {
		format = formatName;
	}
	inline void SetFormat(const std::wstring& formatName) {
		format = ConvertString(formatName);
	}
};