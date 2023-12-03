#include "ErrorCheck.h"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <format>
#include <cassert>
#include <Windows.h>
#include "Engine/Core/WindowFactory/WindowFactory.h"
#include "Utils/ExecutionLog/ExecutionLog.h"

ErrorCheck* const ErrorCheck::GetInstance() {
	static ErrorCheck instance;
	return &instance;
}

ErrorCheck::ErrorCheck() :
	isError_(false)
{
}

ErrorCheck::~ErrorCheck() {
}

void ErrorCheck::ErrorTextBox(const std::string& text, const std::string& boxName) {
	ErrorLog(text, boxName);

	if (boxName == "Error") {
		MessageBoxA(
			WindowFactory::GetInstance()->GetHwnd(), 
			text.c_str(), boxName.c_str(), 
			MB_OK | MB_SYSTEMMODAL | MB_ICONERROR
		);
	}
	else {
		MessageBoxA(
			WindowFactory::GetInstance()->GetHwnd(), 
			text.c_str(), ("Error : " + boxName).c_str(),
			MB_OK | MB_SYSTEMMODAL| MB_ICONERROR
		);
	}
	isError_ = true;
}

void ErrorCheck::ErrorLog(const std::string& text, const std::string& boxName) {
	std::filesystem::path directoryPath = "./Log/";
	if (!std::filesystem::exists(directoryPath)) {
		std::filesystem::create_directory(directoryPath);
	}

	std::ofstream file(directoryPath.string() + "Error.log", std::ios::app);
	assert(file);

	file << Log::NowTime() << ":"  << std::format("{} / {}", boxName, text) << std::endl;
	file.close();
}