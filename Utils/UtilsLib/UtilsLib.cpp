#include "UtilsLib.h"
#include <random>


namespace UtilsLib {
	int32_t Random(int32_t min, int32_t max) {
		static std::random_device seed;
		static std::mt19937_64 rnd(seed());
		std::uniform_int_distribution<> dist{ min, max };

		return dist(rnd);
	}

	float Random(float min, float max) {
		static std::random_device seed;
		static std::mt19937_64 rnd(seed());
		std::uniform_real_distribution<float> dist{ min, max };

		return dist(rnd);
	}

	Flg::Flg():
		flg_(false),
		preFlg_(false)
	{}

	void Flg::Update() {
		preFlg_ = flg_;
	}
}