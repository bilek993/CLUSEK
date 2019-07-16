#pragma once
#include <vector>
#include "../Utils/SystemHolder.h"

class DebugUserInterface final
{
public:
	void Update(std::vector<SystemHolder>& systems) const;
private:
	void DrawSystemBrowser(std::vector<SystemHolder>& systems) const;
};
