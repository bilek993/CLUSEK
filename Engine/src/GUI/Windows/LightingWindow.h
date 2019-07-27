#pragma once
#include "../../Renderer/DynamicRenderSettings.h"

class LightingWindow final
{
public:
	inline static bool IsEnabled = false;
	static void Draw(DynamicRenderSettings& renderSettings);
};
