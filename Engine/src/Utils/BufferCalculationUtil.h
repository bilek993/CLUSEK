#pragma once
#include <d3d11.h>

class BufferCalculationUtil final
{
public:
	// For buffer alignment data must be packed into 4-byte boundaries (more info: https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules).
	static UINT CalculateByteWidthWithPadding(UINT objectSize);
};