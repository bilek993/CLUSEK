#pragma once
#include "BaseSystem.h"
#include <wrl/client.h>
#include <d3d11.h>
#include "../../Renderer/Shaders/VertexShader.h"
#include "../../Renderer/Shaders/PixelShader.h"
#include "../../Renderer/ConstantBufferTypes.h"
#include "../../Renderer/ConstantBuffer.h"

class RenderSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
	void RenderFrameBegin();
	void RenderFrameEnd() const;

	ID3D11Device* GetPointerToDevice() const;
	ID3D11DeviceContext* GetPointerToDeviceContext() const;
private:
	bool InitializeDirectX();
	bool InitializeShaders();
	void InitializeLightSettings() const;
	void InitializeClearColorSettings() const;

	int WindowWidth = 0;
	int WindowHeight = 0;

	int SyncIntervals = 1;

	Microsoft::WRL::ComPtr<ID3D11Device> Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;

	VertexShader UberVertexShader;
	PixelShader UberPixelShader;

	ConstantBuffer<CB_VS_PerObjectBuffer_UberVertexShader> UberShaderVertexShaderConstantBuffer;
	ConstantBuffer<CB_PS_Light_UberVertexShader> UberShaderPixelShaderLightConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;
};
