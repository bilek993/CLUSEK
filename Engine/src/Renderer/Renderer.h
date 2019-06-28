#pragma once
#include <d3d11.h>
#include <wrl/client.h>

class Renderer final
{
public:
	bool Initialize(HWND hwnd, int width, int height, int selectedAdapterId);
	void RenderFrame();
private:
	bool InitializeDirectX(HWND hwnd, int width, int height, int selectedAdapterId);

	Microsoft::WRL::ComPtr<ID3D11Device> Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;
};