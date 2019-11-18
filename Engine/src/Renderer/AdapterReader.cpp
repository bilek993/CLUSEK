#include "AdapterReader.h"
#include <d3d11.h>
#include <wrl/client.h>
#include "../Utils/Logger.h"

std::vector<AdapterData> AdapterReader::GetData()
{
	Logger::Debug("Getting adapters data...");

	Microsoft::WRL::ComPtr<IDXGIFactory> factory;

	const auto hrFactory = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.GetAddressOf()));
	if (FAILED(hrFactory))
	{
		Logger::Error("Creating DXGI Factory failed.");
		exit(-1);
	}

	std::vector<AdapterData> adapters;
	IDXGIAdapter *adapter;
	UINT index = 0;
	while (SUCCEEDED(factory->EnumAdapters(index, &adapter)))
	{
		AdapterData data;
		data.Adapter = adapter;
		const auto hrAdapter = adapter->GetDesc(&data.Description);

		PrintAdapterData(data.Description, index);

		if (FAILED(hrAdapter))
			Logger::Error("Failed to get adapter description");

		adapters.push_back(data);
		++index;
	}

	return adapters;
}

void AdapterReader::PrintAdapterData(DXGI_ADAPTER_DESC description, int id)
{
	char charsDesc[128];
	auto defChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, description.Description, -1, charsDesc, 128, &defChar, nullptr);
	const std::string convertedDescription(charsDesc);

	Logger::Debug("[" + std::to_string(id) + "] " + convertedDescription + " (" + std::to_string(description.DedicatedVideoMemory / 1000000) + "MB)");
}
