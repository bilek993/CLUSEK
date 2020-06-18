#include "FloatingPointComparator.h"
#include <cmath>

bool FloatingPointComparator::IsAlmostEqual(const float v1, const float v2, const float epsilon)
{
	return fabs(v1 - v2) < epsilon;
}

bool FloatingPointComparator::IsAlmostEqual(const double v1, const double v2, const double epsilon)
{
	return fabs(v1 - v2) < epsilon;
}
