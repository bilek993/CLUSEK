#include "BufferCalculationUtil.h"

UINT BufferCalculationUtil::CalculateByteWidthWithPadding(const UINT objectSize)
{
	if (objectSize % 16 != 0)
		return objectSize + (16 - (objectSize % 16));
	else
		return objectSize;
}
