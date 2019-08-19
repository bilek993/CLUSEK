#pragma once
#include "BaseSystem.h"
#include <wrl/client.h>
#include <d3d11.h>
#include "../../Renderer/Shaders/VertexShader.h"
#include "../../Renderer/Shaders/PixelShader.h"
#include "../../Renderer/ConstantBuffer.h"
#include "../Components/CameraComponent.h"
#include "../../Renderer/VertexBuffer.h"
#include "../../Renderer/IndexBuffer.h"
#include "../../Renderer/ConstantBufferTypes/SimplePerObjectBuffer.h"
#include "../../Renderer/ConstantBufferTypes/FatPerObjectBuffer.h"
#include "../../Renderer/ConstantBufferTypes/LightAndAlphaBuffer.h"

class RenderSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
	void RenderFrameBegin() const;
	void RenderFrameEnd() const;

	ID3D11Device* GetPointerToDevice() const;
	ID3D11DeviceContext* GetPointerToDeviceContext() const;
private:
	bool InitializeDirectX();
	bool InitializeShaders();
	void InitializeLightSettings() const;
	void InitializeClearColorSettings() const;
	void InitializeConstantBuffers();

	void ChangeShader(const VertexShader& vertexShader, const PixelShader& pixelShader) const;

	template <class T>
	void Draw(const VertexBuffer<T>& vertexBuffer, const IndexBuffer& indexBuffer, UINT& offset) const;

	CameraComponent& GetMainCamera() const;
	void RenderSkyBoxComponents(const CameraComponent &cameraComponent);
	void RenderModelRenderComponents(const CameraComponent &cameraComponent);

	int WindowWidth = 0;
	int WindowHeight = 0;

	int SyncIntervals = 1;

	Microsoft::WRL::ComPtr<ID3D11Device> Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;

	VertexShader UberVertexShader;
	PixelShader UberPixelShader;

	VertexShader SkyVertexShader;
	PixelShader SkyPixelShader;

	ConstantBuffer<FatPerObjectBuffer> FatPerObjectBufferInstance;
	ConstantBuffer<LightAndAlphaBuffer> LightAndAlphaBufferInstance;
	ConstantBuffer<SimplePerObjectBuffer> SimplePerObjectBufferInstance;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> BlendState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;
};

template <class T>
void RenderSystem::Draw(const VertexBuffer<T>& vertexBuffer, const IndexBuffer& indexBuffer, UINT& offset) const
{
	DeviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->DrawIndexed(indexBuffer.GetIndexCount(), 0, 0);
}
