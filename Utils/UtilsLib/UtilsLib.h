#pragma once
#include <cstdint>
namespace UtilsLib {
	int32_t Random(int32_t min, int32_t max);
	float Random(float min, float max);

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