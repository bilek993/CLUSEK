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
#include "../../Renderer/PostProcessing/CopyToBackBufferPostProcessing.h"
#include "../../Renderer/RenderTexture.h"
#include "../Components/TransformComponent.h"
#include "../../Renderer/ConstantBufferTypes/CameraBuffer.h"
#include "../../Renderer/PbrResources.h"
#include "../../Renderer/PostProcessing/MultisamplingPostProcessing.h"
#include "../../Renderer/RenderDepthStencil.h"
#include "../../Renderer/ConstantBufferTypes/ShadowBuffer.h"

class RenderSystem final : public BaseSystem
{
public:
	void Start() override;
	void Update(float deltaTime) override;
	void RenderFrameBegin() const;
	void RenderFrameEnd() const;

	ID3D11Device* GetPointerToDevice() const;
	ID3D11DeviceContext* GetPointerToDeviceContext() const;
	ID3D11ShaderResourceView* GetPointerToShadowShaderResourceView() const;
private:
	bool InitializeDirectX();
	bool InitializeShaders();
	bool InitializePbrResources();
	void InitializeLightSettings() const;
	void InitializeClearColorSettings() const;
	void InitializeConstantBuffers();
	void InitializePostProcessing();
	void ShowLoadingScreen();
	void InitializeSkyboxComponent();
	void InitializeModelRenderComponent();

	void ChangeShader(const VertexShader& vertexShader, const PixelShader& pixelShader) const;

	template <class T>
	void Draw(const VertexBuffer<T>& vertexBuffer, const IndexBuffer& indexBuffer, UINT& offset) const;

	CameraComponent& GetMainCamera() const;
	TransformComponent& GetMainCameraTransform() const;

	void RenderShadows();
	void RenderScene(const CameraComponent &cameraComponent);

	void RenderSceneForShadows();

	void RenderSkyBoxComponents(const CameraComponent &cameraComponent);
	void RenderModelRenderComponents(const CameraComponent &cameraComponent);

	void PerformPostProcessing() const;

	int WindowWidth = 0;
	int WindowHeight = 0;

	int SyncIntervals = 1;

	std::unique_ptr<MultisamplingPostProcessing> MultisamplingPostProcessingInstance;
	std::unique_ptr<CopyToBackBufferPostProcessing> CopyToBackBufferPostProcessingInstance;

	std::unique_ptr<ReusablePostProcessing> ShadowRemapperPostProcessingInstance;

	Microsoft::WRL::ComPtr<ID3D11Device> Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> BackBufferRenderTargetView;

	RenderTexture IntermediateRenderTexture;

	VertexShader UberVertexShader;
	PixelShader UberPixelShader;

	VertexShader SkyVertexShader;
	PixelShader SkyPixelShader;

	VertexShader LoadingLogoVertexShader;
	PixelShader LoadingLogoPixelShader;

	VertexShader ShadowVertexShader;
	PixelShader ShadowPixelShader;

	ConstantBuffer<FatPerObjectBuffer> FatPerObjectBufferInstance;
	ConstantBuffer<LightAndAlphaBuffer> LightAndAlphaBufferInstance;
	ConstantBuffer<CameraBuffer> CameraBufferInstance;
	ConstantBuffer<SimplePerObjectBuffer> SimplePerObjectBufferInstance;
	ConstantBuffer<ShadowBuffer> ShadowBufferInstance;

	PbrResource PbrResourceInstance{};

	RenderDepthStencil SceneRenderDepthStencil{};
	RenderDepthStencil ShadowRenderDepthStencil{};

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> BlendState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> DefaultWrapSamplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> DefaultClampSamplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> BrdfSamplerState;

	inline static const float BACKGROUND_COLOR[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

template <class T>
void RenderSystem::Draw(const VertexBuffer<T>& vertexBuffer, const IndexBuffer& indexBuffer, UINT& offset) const
{
	DeviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->DrawIndexed(indexBuffer.GetIndexCount(), 0, 0);
}
