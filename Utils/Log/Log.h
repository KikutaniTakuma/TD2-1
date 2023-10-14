#pragma once
#include <string>

namespace Log {
	bool AddLog(const std::string& text);

	void DebugLog(const std::string& text);
}