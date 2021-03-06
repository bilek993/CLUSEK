#pragma once
#include "BaseSystem.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
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
#include "../../Renderer/PostProcessing/ShadowRemapperPostProcessing.h"
#include "../../Renderer/ShadowCamera.h"
#include "../../Renderer/ConstantBufferTypes/CascadeLevelsBuffer.h"
#include "../../Renderer/Shaders/HullShader.h"
#include "../../Renderer/Shaders/DomainShader.h"
#include "../../Renderer/ConstantBufferTypes/TerrainBuffer.h"
#include "../../Renderer/ConstantBufferTypes/TerrainSettingsBuffer.h"
#include "../../Renderer/ConstantBufferTypes/TerrainHeightSamplingBuffer.h"
#include "../../Renderer/ConstantBufferTypes/TerrainNormalBuffer.h"
#include "../../Renderer/ConstantBufferTypes/TerrainUvBuffer.h"
#include "../../Renderer/ConstantBufferTypes/WorldMatrixBuffer.h"
#include "../../Renderer/TransparencyRenderType.h"
#include "../../Renderer/ProfilerAnnotations.h"
#include "../../Renderer/ConstantBufferTypes/FogBuffer.h"
#include "../../Renderer/Mesh.h"
#include "../../Renderer/ConstantBufferTypes/LodTransitionBuffer.h"
#include "../Components/ModelRenderComponent.h"
#include "../../Renderer/AppendBuffer.h"
#include "../../Renderer/CustomizableBuffer.h"
#include "../../Renderer/AppendBufferTypes/GrassInstanceBuffer.h"
#include "../../Renderer/ConstantBufferTypes/DiscardPixelsBuffer.h"
#include "../../Renderer/ConstantBufferTypes/GrassAppearanceParametersBuffer.h"
#include "../../Renderer/ConstantBufferTypes/GrassGeneratorParametersBuffer.h"
#include "../../Renderer/ConstantBufferTypes/GrassIndirectModelInfoBuffer.h"
#include "../../Renderer/ConstantBufferTypes/GrassPerObjectBuffer.h"
#include "../../Renderer/ConstantBufferTypes/TimeBuffer.h"
#include "../../Renderer/ConstantBufferTypes/WindBuffer.h"

class RenderSystem final : public BaseSystem
{
public:
	void Start() override;
	void Rebuild() override;
	void Update(float deltaTime) override;

	void PrepareForGuiRender();

	void RenderFrameBegin() const;
	void RenderFrameEnd() const;

	ID3D11Device* GetPointerToDevice() const;
	ID3D11DeviceContext* GetPointerToDeviceContext() const;
	ID3D11ShaderResourceView* GetPointerToRemappedShadowShaderResourceView(float bias, int cascadeLevel) const;
private:
	bool InitializeDirectX();
	bool InitializeShaders();
	bool InitializePbrResources();
	void InitializeFogSettings() const;
	void InitializeLightSettings() const;
	void InitializeClearColorSettings() const;
	void InitializeGrassSettings() const;
	void InitializeConstantBuffers();
	void InitializeAppendBuffers();
	void InitializeCustomizableBuffers();
	void InitializePostProcessing();
	void ShowLoadingScreen();
	void InitializeSkyboxComponent();
	void InitializeGrassComponent();
	void InitializeTerrainComponent();
	void InitializeModelRenderComponent();
	void InitializeRoadComponent();
	void InitializeShadowCameras();

	void ChangeBasicShaders(const VertexShader& vertexShader, const PixelShader& pixelShader) const;
	void ChangeTessellationShaders(const HullShader& hullShader, const DomainShader& domainShader) const;
	void ResetTessellationShaders() const;

	template <class T>
	void Draw(const VertexBuffer<T>& vertexBuffer, const IndexBuffer& indexBuffer, UINT& offset) const;

	void RenderShadows(const CameraComponent &mainCameraComponent, const TransformComponent &mainCameraTransformComponent);
	void RenderScene(const CameraComponent &cameraComponent, const TransformComponent &mainCameraTransformComponent);

	void RenderSceneForShadows(const CameraComponent &mainCameraComponent, const TransformComponent &mainCameraTransformComponent);

	void RenderSkyBoxComponents(const CameraComponent &cameraComponent);
	void RenderGrass(const CameraComponent &mainCameraComponent, const TransformComponent &mainCameraTransformComponent);
	void RenderTerrain(const CameraComponent &mainCameraComponent, const TransformComponent &mainCameraTransformComponent);
	void RenderModelRenderComponents(const CameraComponent &mainCameraComponent, const TransformComponent &mainTransformComponent, TransparencyRenderType renderMode);
	void RenderRoadComponents(const CameraComponent &mainCameraComponent, const TransformComponent &mainCameraTransformComponent);

	void RenderMesh(const Mesh& mesh, TransformComponent &transformComponent, const CameraComponent &mainCameraComponent, 
		UINT& offset, bool& fatPerObjectBufferSet, const TransparencyRenderType renderMode);

	void SetShadowResourcesForShadowCascades(int firstCascadeId);
	void ClearShadowResourcesForShadowCascades(int firstCascadeId) const;

	void SetPbrResources();

	void ConfigureCascadeConstantBuffer();

	void UpdateTimeBuffer();
	void UpdateFogBuffer();
	void UpdateLightAndAlphaBuffer();
	void UpdateCameraBuffer(const TransformComponent &mainCameraTransformComponent);

	void PerformPostProcessing(bool minimal = false) const;

	float CalculateDistanceFromCamera(const TransformComponent& transformComponent, const TransformComponent& cameraTransformComponent) const;
	float CalculateLodCoverage(ModelRenderComponent& modelRenderComponent, float distanceFromCamera) const;

	Timer MainTimer;

	std::unique_ptr<ProfilerAnnotations> Profiler;

	int WindowWidth = 0;
	int WindowHeight = 0;

	int SyncIntervals = 1;

	std::unique_ptr<MultisamplingPostProcessing> MultisamplingPostProcessingInstance;
	std::unique_ptr<CopyToBackBufferPostProcessing> CopyToBackBufferPostProcessingInstance;

	std::unique_ptr<ShadowRemapperPostProcessing> ShadowRemapperPostProcessingInstance;

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

	VertexShader TerrainVertexShader;
	PixelShader TerrainPixelShader;
	HullShader TerrainHullShader;
	DomainShader TerrainDomainShader;

	VertexShader GrassVertexShader;
	PixelShader GrassPixelShader;

	PixelShader GrassGeneratorPixelShader;

	ComputeShader PrepareGrassDateForIndirectRenderingComputeShader;

	VertexShader ShadowVertexShader;
	PixelShader ShadowPixelShader;
	PixelShader ShadowPixelShaderWithAlpha;

	VertexShader ShadowTerrainVertexShader;
	HullShader ShadowTerrainHullShader;
	DomainShader ShadowTerrainDomainShader;

	std::array<ShadowCamera, ShadowCamera::SHADOW_CASCADES_COUNT> ShadowCameras;

	ConstantBuffer<FatPerObjectBuffer> FatPerObjectBufferInstance;
	ConstantBuffer<LightAndAlphaBuffer> LightAndAlphaBufferInstance;
	ConstantBuffer<CameraBuffer> CameraBufferInstance;
	ConstantBuffer<SimplePerObjectBuffer> SimplePerObjectBufferInstance;
	ConstantBuffer<ShadowBuffer> ShadowBufferInstance;
	ConstantBuffer<CascadeLevelsBuffer> CascadeLevelsBufferInstance;
	ConstantBuffer<TerrainBuffer> TerrainBufferInstance;
	ConstantBuffer<TerrainSettingsBuffer> TerrainSettingsBufferInstance;
	ConstantBuffer<TerrainHeightSamplingBuffer> TerrainHeightSamplingBufferInstance;
	ConstantBuffer<TerrainNormalBuffer> TerrainNormalBufferInstance;
	ConstantBuffer<TerrainUvBuffer> TerrainUvBufferInstance;
	ConstantBuffer<WorldMatrixBuffer> WorldMatrixBufferInstance;
	ConstantBuffer<FogBuffer> FogBufferInstance;
	ConstantBuffer<LodTransitionBuffer> LodTransitionBufferInstance;
	ConstantBuffer<GrassIndirectModelInfoBuffer> GrassIndirectModelInfoBufferInstance;
	ConstantBuffer<GrassPerObjectBuffer> GrassPerObjectBufferInstance;
	ConstantBuffer<GrassAppearanceParametersBuffer> GrassAppearanceParametersBufferInstance;
	ConstantBuffer<GrassGeneratorParametersBuffer> GrassGeneratorParametersBufferInstance;
	ConstantBuffer<TimeBuffer> TimeBufferInstance;
	ConstantBuffer<DiscardPixelsBuffer> DiscardPixelsBufferInstance;
	ConstantBuffer<WindBuffer> WindBufferInstance;

	AppendBuffer<GrassInstanceBuffer> GrassInstanceBufferInstance;

	CustomizableBuffer GrassCounterBufferInstance;
	CustomizableBuffer GrassDrawIndexInstanceIndirectArgsBufferInstance;

	PbrResource PbrResourceInstance{};
	bool IsPbrInitialized = false;

	RenderDepthStencil SceneRenderDepthStencil{};
	std::array<RenderDepthStencil, ShadowCamera::SHADOW_CASCADES_COUNT> ShadowRenderDepthStencils{};
	RenderDepthStencil GrassRenderDepthStencil{};

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> MainRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> MainTwoSidedRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> SolidRasterizerState;

	Microsoft::WRL::ComPtr<ID3D11BlendState> BlendState;

	D3D11_VIEWPORT SceneViewport{};
	D3D11_VIEWPORT ShadowViewport{};
	D3D11_VIEWPORT GrassViewport{};

	Microsoft::WRL::ComPtr<ID3D11SamplerState> DefaultWrapSamplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> DefaultClampSamplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> BrdfSamplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ShadowSamplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> TerrainSamplerState;

	ID3D11ShaderResourceView* NullShaderResourceView = nullptr;
	ID3D11UnorderedAccessView* NullUnorderedAccessView = nullptr;

	inline static const float BACKGROUND_COLOR[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

template <class T>
void RenderSystem::Draw(const VertexBuffer<T>& vertexBuffer, const IndexBuffer& indexBuffer, UINT& offset) const
{
	DeviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->DrawIndexed(indexBuffer.GetIndexCount(), 0, 0);
}
