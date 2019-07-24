#pragma once
#include "BaseSystem.h"
#include <wrl/client.h>
#include <d3d11.h>
#include "../../Renderer/Shaders/VertexShader.h"
#include "../../Renderer/Shaders/PixelShader.h"

class RenderSystem final : public BaseSystem
{
public:
	void Start(entt::registry& registry, const RenderWindow &window, const ConfigData& configData) override;
	void Update(float deltaTime, entt::registry& registry, IOData& ioData, IODevices& ioDevices,
		RenderWindow& window, ConfigData& configData) override;
	void RenderFrameBegin();
	void RenderFrameEnd() const;

	ID3D11Device* GetPointerToDevice() const;
	ID3D11DeviceContext* GetPointerToDeviceContext() const;
private:
	bool InitializeDirectX(HWND hwnd, int fullscreen, int selectedAdapterId, int refreshRateNumerator,
		int refreshRateDenominator, int multisamplesCount, int multisamplesQuality, int wireframeMode, 
		const std::string& cullMode);
	bool InitializeShaders();

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
