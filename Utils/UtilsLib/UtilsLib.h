#pragma once
#include <cstdint>
#include <random>
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector3.h"

namespace UtilsLib {
	int8_t Random(int8_t min, int8_t max);
	uint8_t Random(uint8_t min, uint8_t max);

	int16_t Random(int16_t min, int16_t max);
	uint16_t Random(uint16_t min, uint16_t max);

	int32_t Random(int32_t min, int32_t max);
	uint32_t Random(uint32_t min, uint32_t max);

	float Random(float min, float max);
	double Random(double min, double max);

	Vector2 Random(Vector2 min, Vector2 max);
	Vector3 Random(Vector3 min, Vector3 max);

	class Flg {
	public:
		Flg();
		Flg(const Flg&) = default;
		Flg(Flg&&) = default;
		~Flg() = default;

	public:
		void Update();

		// trueになった瞬間
		bool OnEnter() const {
			return flg_ && !preFlg_;
		}

		// trueなっているとき
		bool OnStay() const {
			return flg_ && preFlg_;
		}

		// falseになった瞬間
		bool OnExit() const {
			return !flg_ && preFlg_;
		}

	public:
		bool flg_;
	private:
		bool preFlg_;
	};
}