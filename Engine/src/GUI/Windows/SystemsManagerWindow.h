#pragma once
#include <vector>
#include "../../Utils/SystemHolder.h"

struct SystemsManagerWindow final
{
	static bool IsEnabled;
	static void Draw(std::vector<SystemHolder>& systems);
};
