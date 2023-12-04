#pragma once
#include <functional>
#include <chrono>

class Frame {
public:
	Frame();
	~Frame() = default;
	Frame(const Frame&) = default;
	Frame(Frame&&) = default;

	Frame& operator=(const Frame&) = default;
	Frame& operator=(Frame&&) = default;

public:
	void Start();
	void Puase();
	void Stop();

	void Update();

	inline std::chrono::milliseconds GetNowTime() const {
		if (!isStart) {
			return timeFormStart.zero();
		}
		return timeFormStart;
	}

	inline uint64_t GetFrameCount() const {
		return frameCount;
	}

public:
	inline explicit operator bool() const noexcept {
		return isStart;
	}

	inline bool operator!() const noexcept {
		return !isStart;
	}

public:
	std::function<void(void)> func;

private:
	bool isStart;

	std::chrono::steady_clock::time_point startTime;
	std::chrono::milliseconds timeFormStart;

	uint64_t frameCount;
};