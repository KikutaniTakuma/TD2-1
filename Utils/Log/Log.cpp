#include "Log.h"
#include <fstream>
#include <filesystem>
#include <cassert>

namespace Log {
	bool AddLog(const std::string& text) {
		static const std::string fileName = "./Log/Log.txt";
		static bool isOpned = false;

		if (!std::filesystem::exists("./Log/")) {
			std::filesystem::create_directories("./Log/");
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
}