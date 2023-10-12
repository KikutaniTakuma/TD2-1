#pragma once
#include <cstdint>
namespace UtilsLib {
	template<typename T>
	T Random(T min, T max) {
		static std::random_device seed;
		static std::mt19937_64 rnd(seed());
		std::uniform_int_distribution<> dist{ min, max };

		return dist(rnd);
	}

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