#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Shaders/VertexShader.h"
#include "Shaders/PixelShader.h"

class Renderer final
{
public:
	bool Initialize(HWND hwnd, int width, int height, int fullscreen, int syncIntervals, int selectedAdapterId, 
		int refreshRateNumerator, int refreshRateDenominator, int multisamplesCount, int multisamplesQuality);
	void RenderFrame() const;
private:
	bool InitializeDirectX(HWND hwnd, int width, int height, int fullscreen, int selectedAdapterId,
		int refreshRateNumerator, int refreshRateDenominator, int multisamplesCount, int multisamplesQuality);
	bool InitializeShaders();

	int SyncIntervals = 1;

	Microsoft::WRL::ComPtr<ID3D11Device> Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;

	VertexShader UberVertexShader;
	PixelShader UberPixelShader;
};
