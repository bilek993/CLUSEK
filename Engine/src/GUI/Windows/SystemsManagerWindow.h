#pragma once
#include <vector>
#include "../../Utils/SystemHolder.h"

class SystemsManagerWindow final
{
public:
	inline static bool IsEnabled = false;
	static void Draw(std::vector<SystemHolder>& systems);
};
