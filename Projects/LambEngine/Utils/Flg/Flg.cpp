#include "Utils/Flg/Flg.h"

namespace Lamb {
	Flg::Flg() :
		flg_(false),
		preFlg_(false)
	{}

	Flg::Flg(bool right) :
		flg_(right),
		preFlg_(false)
	{}

	void Flg::Update() {
		preFlg_ = flg_;
	}
}