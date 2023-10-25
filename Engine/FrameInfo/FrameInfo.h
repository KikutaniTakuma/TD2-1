#pragma once
#include <chrono>

class FrameInfo {
/// <summary>
/// コンストラクタ
/// </summary>
private:
	FrameInfo();
	FrameInfo(const FrameInfo&) = delete;
	FrameInfo(FrameInfo&&) = delete;
	~FrameInfo();

	FrameInfo& operator=(const FrameInfo&) = delete;
	FrameInfo& operator=(FrameInfo&&) = delete;

/// <summary>
/// シングルトン
/// </summary>
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns>インスタンスのポインタ(deleteしてはいけない)</returns>
	static FrameInfo* const GetInstance();

/// <summary>
/// メンバ関数
/// </summary>
public:
	/// <summary>
	/// フレーム開始時関数
	/// </summary>
	void Start();

	/// <summary>
	/// フレーム終了時関数
	/// </summary>
	void End();

/// <summary>
/// getter
/// </summary>
public:
	/// <summary>
	/// デルタタイム取得
	/// </summary>
	/// <returns>デルタタイム</returns>
	inline float GetDelta() const {
#ifdef _DEBUG
		return static_cast<float>(1.0 / fpsLimit_);
#else
		return static_cast<float>(deltaTime_);
#endif
	}

	/// <summary>
	/// fps取得
	/// </summary>
	/// <returns>fps</returns>
	inline double GetFps() const {
		return fps_;
	}

	/// <summary>
	/// フレームカウント取得
	/// </summary>
	/// <returns>フレームカウント</returns>
	inline size_t GetFrameCount() const {
		return frameCount_;
	}

	void SetFpsLimit(double fpsLimit);

	void Debug();

#ifdef _DEBUG
	bool GetIsDebugStop() const {
		return isDebugStopGame_;
	}

	bool GetIsOneFrameActive() const {
		return isOneFrameActive_;
	}

	void SetIsOneFrameActive(bool isOneFramActive) {
		if (isDebugStopGame_) {
			isOneFrameActive_ = isOneFramActive;
		}
	}

#endif // _DEBUG

/// <summary>
/// メンバ変数
/// </summary>
private:
	std::chrono::steady_clock::time_point frameStartTime_;
	double deltaTime_;
	double fps_;
	double maxFps_;
	double minFps_;
	size_t frameCount_;

	std::chrono::steady_clock::time_point gameStartTime_;

	std::chrono::steady_clock::time_point reference_;
	double fpsLimit_;
	double maxFpsLimit_;

	std::chrono::microseconds minTime;
	std::chrono::microseconds minCheckTime;

#ifdef _DEBUG
	bool isDebugStopGame_;
	bool isOneFrameActive_;
#endif // _DEBUG

};