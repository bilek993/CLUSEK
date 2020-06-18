#pragma once

class FloatingPointComparator final
{
public:
	static bool IsAlmostEqual(float v1, float v2, float epsilon = 0.0001f);
	static bool IsAlmostEqual(double v1, double v2, double epsilon = 0.0000001);
};