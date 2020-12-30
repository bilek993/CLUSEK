#include "MathUtil.h"
#include "Logger.h"

float MathUtil::Remap(const float value, const float start1, const float stop1, const float start2, const float stop2)
{
	const auto outgoing = start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));

	if (isnan(outgoing))
		Logger::Error("Result of remapping is not a number!");
	else if (isinf(outgoing))
		Logger::Error("Result of remapping is infinity or -infinity!");

	return outgoing;
}
