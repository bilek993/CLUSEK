#pragma once
#include <vector>
#include "../../Utils/SystemHolder.h"
#include "BaseDebugWindow.h"

class SystemsManagerWindow final : public BaseDebugWindow
{
public:
	static void Draw(std::vector<SystemHolder>& systems);
};
