#pragma once
#include <string>

class ErrorCheck {
private:
	ErrorCheck();
	ErrorCheck(const ErrorCheck&) = delete;
	ErrorCheck(ErrorCheck&&) noexcept = delete;
	~ErrorCheck();

	ErrorCheck& operator=(const ErrorCheck&) = delete;
	ErrorCheck& operator=(ErrorCheck&&) noexcept = delete;

public:
	/// <summary>
	/// シングルトン
	/// </summary>
	/// <returns>インスタンス</returns>
	static ErrorCheck* GetInstance();

public:
	/// <summary>
	/// エラーログ
	/// </summary>
	/// <param name="text">エラーメッセージ</param>
	/// <param name="boxName">エラーウィンドウの名前</param>
	void ErrorTextBox(const std::string& text, const std::string& boxName = "Error");

	/// <summary>
	/// エラーが起きているかの判定
	/// </summary>
	/// <returns></returns>
	inline bool GetError() const {
		return isError;
	}

private:
	void ErrorLog(const std::string& text, const std::string& boxName = "Error");

private:
	bool isError;
};