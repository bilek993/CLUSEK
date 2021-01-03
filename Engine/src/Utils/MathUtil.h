#pragma once

class MathUtil
{
public:
	static float Remap(float value, float start1, float stop1, float start2, float stop2);

	static bool IsAlmostEqual(float v1, float v2, float epsilon = 0.0001f);
	static bool IsAlmostEqual(double v1, double v2, double epsilon = 0.0000001);

	static bool IsAlmostZero(float value, float epsilon = 0.0001f);
	static bool IsAlmostZero(double value, double epsilon = 0.0000001);

	template <typename T>
	static bool SameSign(T v1, T v2);
};

template <typename T>
bool MathUtil::SameSign(T v1, T v2)
{
	return (v1 >= 0) ^ (v2 < 0);
}
