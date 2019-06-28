#pragma once
#include <d3d10sdklayers.h>
#include <wrl/client.h>

struct AdapterData final
{
	IDXGIAdapter *Adapter = nullptr;
	DXGI_ADAPTER_DESC Description;
};
