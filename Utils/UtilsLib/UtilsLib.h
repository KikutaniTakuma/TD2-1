#pragma once
#include <cstdint>
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

	Vector2 Random(const Vector2& min, const Vector2& max);
	Vector3 Random(const Vector3& min, const Vector3& max);

	class Flg {
	public:
		Flg();
		Flg(const Flg&) = default;
		Flg(Flg&&) = default;
		~Flg() = default;

		Flg& operator=(const Flg&) = default;
		Flg& operator=(Flg&&) = default;

		inline Flg& operator=(bool flg) {
			flg_ = flg;
			return *this;
		}

		/// <summary>
		/// 暗黙型定義
		/// </summary>
		inline explicit operator bool() const {
			return flg_;
		}

		bool operator!() const {
			return !flg_;
		}

		inline bool operator==(const Flg& other) const {
			return flg_ == other.flg_ && preFlg_ == other.preFlg_;
		}

		inline bool operator!=(const Flg& other) const {
			return flg_ != other.flg_ || preFlg_ != other.preFlg_;
		}

		inline bool* Data() {
			return &flg_;
		}

	public:
		/// <summary>
		/// アップデート(基本的に更新処理の一番最初か終わりにする)
		/// </summary>
		void Update();

		/// <summary>
		/// trueになった瞬間を返す
		/// </summary>
		/// <returns>trueになった瞬間にtrue</returns>
		bool OnEnter() const {
			return flg_ && !preFlg_;
		}

		/// <summary>
		/// trueになっている間
		/// </summary>
		/// <returns>trueになっている間はtrue</returns>
		bool OnStay() const {
			return flg_ && preFlg_;
		}

		/// <summary>
		/// falseになった瞬間
		/// </summary>
		/// <returns>falseになった瞬間にtrue</returns>
		bool OnExit() const {
			return !flg_ && preFlg_;
		}

	private:
		bool flg_;
		bool preFlg_;
	};
}