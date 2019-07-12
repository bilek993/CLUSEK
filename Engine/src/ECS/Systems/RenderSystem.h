#pragma once
#include "BaseSystem.h"
#include <wrl/client.h>
#include <d3d11.h>
#include "../../Renderer/Shaders/VertexShader.h"
#include "../../Renderer/Shaders/PixelShader.h"

class RenderSystem final : public BaseSystem
{
public:
	void Start(entt::registry& registry, const HWND &hwnd, const ConfigData& configData) override;
	void Update(float deltaTime, entt::registry& registry, IOData& ioData, IODevices& ioDevices,
		RenderWindow& window, ConfigData& configData) override;
private:
	bool InitializeDirectX(HWND hwnd, int fullscreen, int selectedAdapterId, int refreshRateNumerator,
		int refreshRateDenominator, int multisamplesCount, int multisamplesQuality);
	bool InitializeShaders();
	void InitializeImGui(HWND hwnd) const;
	void RenderFrameBegin();
	void RenderFrameEnd();

	int WindowWidth = 0;
	int WindowHeight = 0;

	int SyncIntervals = 1;

	Microsoft::WRL::ComPtr<ID3D11Device> Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;

	VertexShader UberVertexShader;
	PixelShader UberPixelShader;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;
};
