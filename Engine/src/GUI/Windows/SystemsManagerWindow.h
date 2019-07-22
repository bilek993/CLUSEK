#pragma once
#include <vector>
#include "../../Utils/SystemHolder.h"
#include "BaseDebugWindow.h"

struct SystemsManagerWindow final : public BaseDebugWindow
{
	static void Draw(std::vector<SystemHolder>& systems);
};
