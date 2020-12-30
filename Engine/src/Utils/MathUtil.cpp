#include "MathUtil.h"
#include "Logger.h"

float MathUtil::Remap(const float value, const float start1, const float stop1, const float start2, const float stop2)
{
	const auto outgoing = start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));

	if (isnan(outgoing) || isinf(outgoing))
	{
		Logger::Error("Remapping failed!");
		return value;
	}

	return outgoing;
}
