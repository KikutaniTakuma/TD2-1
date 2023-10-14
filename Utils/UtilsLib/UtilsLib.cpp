#include "UtilsLib.h"
#include <algorithm>
#include <random>

namespace UtilsLib {
	int8_t Random(int8_t min, int8_t max) {
		static std::random_device seed;
		static std::mt19937_64 rnd(seed());
		if (max < min) {
			std::swap(min, max);
		}

		std::uniform_int_distribution<> dist{ min, max };

		return static_cast<int8_t>(dist(rnd));
	}
	uint8_t Random(uint8_t min, uint8_t max) {
		static std::random_device seed;
		static std::mt19937_64 rnd(seed());
		if (max < min) {
			std::swap(min, max);
		}
		std::uniform_int_distribution<> dist{ min, max };

		return static_cast<uint8_t>(dist(rnd));
	}

	int16_t Random(int16_t min, int16_t max) {
		static std::random_device seed;
		static std::mt19937_64 rnd(seed());
		if (max < min) {
			std::swap(min, max);
		}
		std::uniform_int_distribution<> dist{ min, max };

		return static_cast<int16_t>(dist(rnd));
	}

	uint16_t Random(uint16_t min, uint16_t max) {
		static std::random_device seed;
		static std::mt19937_64 rnd(seed());
		if (max < min) {
			std::swap(min, max);
		}
		std::uniform_int_distribution<> dist{ min, max };

		return static_cast<uint16_t>(dist(rnd));
	}

	int32_t Random(int32_t min, int32_t max) {
		static std::random_device seed;
		static std::mt19937_64 rnd(seed());
		if (max < min) {
			std::swap(min, max);
		}
		std::uniform_int_distribution<> dist{ min, max };

		return static_cast<int32_t>(dist(rnd));
	}

	uint32_t Random(uint32_t min, uint32_t max) {
		static std::random_device seed;
		static std::mt19937_64 rnd(seed());
		if (max < min) {
			std::swap(min, max);
		}
		std::uniform_int_distribution<> dist{ int32_t(min), int32_t(max) };

		return static_cast<uint32_t>(dist(rnd));
	}

	float Random(float min, float max) {
		static std::random_device seed;
		static std::mt19937_64 rnd(seed());
		if (max < min) {
			std::swap(min, max);
		}
		std::uniform_real_distribution<float> dist{ min, max };

		return static_cast<float>(dist(rnd));
	}

	double Random(double min, double max) {
		static std::random_device seed;
		static std::mt19937_64 rnd(seed());
		if (max < min) {
			std::swap(min, max);
		}
		std::uniform_real_distribution<double> dist{ min, max };

		return static_cast<double>(dist(rnd));
	}


	Vector2 Random(const Vector2& min, const Vector2& max) {
		Vector2 result;
		result.x = Random(min.x, max.x);
		result.y = Random(min.y, max.y);

		return result;
	}

	Vector3 Random(const Vector3& min, const Vector3& max) {
		Vector3 result;
		result.x = Random(min.x, max.x);
		result.y = Random(min.y, max.y);
		result.z = Random(min.z, max.z);

		return result;
	}


	Flg::Flg():
		flg_(false),
		preFlg_(false)
	{}

	void Flg::Update() {
		preFlg_ = flg_;
	}
}