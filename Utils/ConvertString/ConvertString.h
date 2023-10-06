#pragma once
#include <string>
#include <Windows.h>

/// 
/// string to wstring
/// 
std::wstring ConvertString(const std::string& msg);

/// 
/// wstring to string
/// 
std::string ConvertString(const std::wstring& msg);