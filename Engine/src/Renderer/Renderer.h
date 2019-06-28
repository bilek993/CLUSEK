#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "AdapterReader.h"

class Renderer final
{
public:
	bool Initialize(HWND hwnd, int width, int height);
private:
	bool InitializeDirectX(HWND hwnd, int width, int height);

	Microsoft::WRL::ComPtr<ID3D11Device> Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;
};