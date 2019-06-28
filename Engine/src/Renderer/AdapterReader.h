#pragma once
#include "AdapterData.h"
#include <vector>

class AdapterReader final
{
public:
	static std::vector<AdapterData> GetData();
};
