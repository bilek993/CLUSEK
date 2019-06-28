#pragma once
#include "AdapterData.h"
#include <vector>

class AdapterReader final
{
public:
	static std::vector<AdapterData> GetData();
private:
	static void PrintAdapterData(DXGI_ADAPTER_DESC description, int id);
};
