#include "CsvLoad.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>

std::vector<std::vector<int32_t>> CsvLoad(const std::string& fileName) {
	std::vector<std::vector<int32_t>> result;

	if (!(std::filesystem::path(fileName).extension() == ".csv")) {
		Log::ErrorLog("This file is not csv -> " + fileName, "CsvLoad()");
		return std::vector<std::vector<int32_t>>(0);
	}

	std::ifstream file{ fileName };
	if (file.fail()) {
		Log::ErrorLog("This file is not exist -> " + fileName, "CsvLoad()");
		return std::vector<std::vector<int32_t>>(0);
	}

	std::string line;
	while (std::getline(file, line))
	{
		std::stringstream sLine{ line };
		std::string stringBuf;

		result.push_back(std::vector<int32_t>());

		while (std::getline(sLine, stringBuf, ',')) {
			if (std::any_of(stringBuf.begin(), stringBuf.end(), isdigit)) {
				result.back().push_back(std::atoi(stringBuf.c_str()));
			}
		}
	}

	return result;
}