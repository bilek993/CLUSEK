#pragma once
#include <vector>
#include "../Utils/SystemHolder.h"

class DebugUserInterface final
{
public:
	void Update(const IOData& ioData, std::vector<SystemHolder>& systems);
private:
	void HandleKeyboardEvents(const IOData& ioData);

	void DrawSystemBrowser(std::vector<SystemHolder>& systems) const;

	bool IsEnabled = false;
};
