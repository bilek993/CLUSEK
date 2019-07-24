#pragma once
#include "../../Renderer/LightSettings.h"

class LightingWindow final
{
public:
	inline static bool IsEnabled = false;
	static void Draw(LightSettings& lightSettings);
};
