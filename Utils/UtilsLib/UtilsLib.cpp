#include "UtilsLib.h"
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector3.h"
#include <random>


namespace UtilsLib {
	template<>
	float Random<float>(float min, float max) {
		static std::random_device seed;
		static std::mt19937_64 rnd(seed());
		std::uniform_real_distribution<float> dist{ min, max };

		return dist(rnd);
	}
	template<>
	double Random<double>(double min, double max) {
		static std::random_device seed;
		static std::mt19937_64 rnd(seed());
		std::uniform_real_distribution<double> dist{ min, max };

		return dist(rnd);
	}
	template<>
	Vector2 Random<Vector2>(Vector2 min, Vector2 max) {
		Vector2 result;
		result.x = Random<float>(min.x, max.x);
		result.y = Random<float>(min.y, max.y);

		return result;
	}
	template<>
	Vector3 Random<Vector3>(Vector3 min, Vector3 max) {
		Vector3 result;
		result.x = Random<float>(min.x, max.x);
		result.y = Random<float>(min.y, max.y);
		result.z = Random<float>(min.z, max.z);

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