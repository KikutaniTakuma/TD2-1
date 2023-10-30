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
		if (isFixedDeltaTime_ || isDebugStopGame_) {
			return static_cast<float>(1.0 / fpsLimit_);
		}
		return static_cast<float>(deltaTime_);
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

	/// <summary>
	/// ゲームスピード取得
	/// </summary>
	/// <returns></returns>
	inline float GetGameSpeedScale() const {
		return static_cast<float>(gameSpeedSccale_);
	}

	void SetGameSpeedScale(float gameSpeedSccale);

	/// <summary>
	/// フレームの最初の時間を取得
	/// </summary>
	/// <returns>フレームの時間</returns>
	std::chrono::steady_clock::time_point GetThisFrameTime() const {
		return frameStartTime_;
	}

	/// <summary>
	/// デバッグ関数
	/// </summary>
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

	double gameSpeedSccale_;

#ifdef _DEBUG
	bool isDebugStopGame_;
	bool isOneFrameActive_;
	bool isFixedDeltaTime_;
#endif // _DEBUG

};