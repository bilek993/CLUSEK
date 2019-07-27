#pragma once
#include "../../Renderer/DynamicRenderSettings.h"

class BackBufferWindow final
{
public:
	static void Draw(DynamicRenderSettings& renderSettings);
	inline static bool IsEnabled = false;
};
