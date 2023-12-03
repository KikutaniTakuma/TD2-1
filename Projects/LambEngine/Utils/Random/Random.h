#pragma once

#include <cstdint>
#include "Utils/Math/Vector2.h"
#include "Utils/Math/Vector3.h"

namespace Lamb {
	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	int8_t Random(int8_t min, int8_t max);
	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	uint8_t Random(uint8_t min, uint8_t max);

	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	int16_t Random(int16_t min, int16_t max);
	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	uint16_t Random(uint16_t min, uint16_t max);

	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	int32_t Random(int32_t min, int32_t max);
	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	uint32_t Random(uint32_t min, uint32_t max);

	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	float Random(float min, float max);
	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	double Random(double min, double max);

	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	Vector2 Random(const Vector2& min, const Vector2& max);
	/// <summary>
	/// 最小値から最大値までで一様分布で乱数を生成
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>乱数</returns>
	Vector3 Random(const Vector3& min, const Vector3& max);
}