#include "Log.h"
#include <fstream>
#include <filesystem>
#include <cassert>
#include <Windows.h>
#undef max
#undef min

namespace Log {
	bool AddLog(const std::string& text) {
		static const std::filesystem::path fileName = "./ExecutionLog/ExecutionLog.txt";
		static bool isOpned = false;

		if (!std::filesystem::exists(fileName.parent_path())) {
			std::filesystem::create_directories(fileName.parent_path());
		}

		std::ofstream file;
		if (isOpned) {
			file.open(fileName, std::ios::app);
		}
		else {
			file.open(fileName);
		}
		if (file.fail()) {
			return false;
		}

		file << text;

		isOpned = true;

		return true;
	}

	void DebugLog(const std::string& text) {
		OutputDebugStringA(text.c_str());
	}
}