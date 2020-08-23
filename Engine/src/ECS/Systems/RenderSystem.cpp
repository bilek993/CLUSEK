#include "RenderSystem.h"
#include "../Components/ModelRenderComponent.h"
#include "../Components/CameraComponent.h"
#include "../../Renderer/AdapterReader.h"
#include "../../Loaders/ModelLoader.h"
#include "../../Loaders/MaterialLoader.h"
#include "../../Loaders/ResourcesLoader.h"
#include "../Components/SkyboxComponent.h"
#include "../../Renderer/Vertex/PositionVertex.h"
#include "../../Renderer/Generators/CubeGenerator.h"
#include "../../Tags.h"
#include "../../Renderer/PostProcessing/CopyToBackBufferPostProcessing.h"
#include "../../Loaders/PostProcessingLoader.h"
#include "../../Renderer/TransformLogic.h"
#include "../../Renderer/Generators/QuadGenerator.h"
#include "../Components/TerrainComponent.h"
#include "../../Utils/TerrainUtil.h"
#include "../../Renderer/FrustumUtil.h"
#include "../../Renderer/GrassCamera.h"
#include "../../Utils/StringUtil.h"
#include "../../Utils/CameraLocator.h"
#include "../Components/GrassComponent.h"

void RenderSystem::Start()
{
	Logger::Debug("Staring render system...");

	SyncIntervals = ConfigurationData->SyncIntervals;
	WindowWidth = ConfigurationData->WindowWidth;
	WindowHeight = ConfigurationData->WindowHeight;

	InitializeFogSettings();
	InitializeLightSettings();
	InitializeClearColorSettings();
	InitializeGrassSettings();

	if (!InitializeDirectX())
		Logger::Error("DirectX initialization failed!");

	if (!InitializeShaders())
		Logger::Error("Shaders initialization failed!");

	InitializeConstantBuffers();
	InitializeAppendBuffers();
	InitializeCustomizableBuffers();
	
	InitializePostProcessing();

	if (!ConfigurationData->DisableLoadingScreen)
		ShowLoadingScreen();

	ResourcesLoader::Load(Device.Get(), DeviceContext.Get(), ConfigurationData);

	Logger::Debug("Preparing to initialize components in renderer...");

	InitializeSkyboxComponent();
	InitializeGrassComponent();
	InitializeTerrainComponent();
	InitializeModelRenderComponent();
	InitializeShadowCameras();

	Logger::Debug("Finished components initialization in renderer.");

	ConfigureCascadeConstantBuffer();

	if (!InitializePbrResources())
		Logger::Error("PBR resources initialization failed!");
}

void RenderSystem::Update(const float deltaTime)
{
	auto& mainCameraComponent = CameraLocator::GetMainCamera(Registry);
	auto& mainCameraTransform = CameraLocator::GetMainCameraTransform(Registry);

	UpdateCameraBuffer(mainCameraTransform);
	
	UpdateTimeBuffer();

	if (ConfigurationData->ShadowsEnabled)
		RenderShadows(mainCameraComponent, mainCameraTransform);

	RenderScene(mainCameraComponent, mainCameraTransform);

	PerformPostProcessing();
}

void RenderSystem::PrepareForGuiRender()
{
	DeviceContext->RSSetState(SolidRasterizerState.Get());
	DeviceContext->RSSetViewports(1, &SceneViewport);
	DeviceContext->OMSetRenderTargets(1, BackBufferRenderTargetView.GetAddressOf(), nullptr);
}

void RenderSystem::RenderFrameBegin() const
{
	DeviceContext->OMSetRenderTargets(1, IntermediateRenderTexture.GetAddressOfRenderTargetView(), SceneRenderDepthStencil.GetDepthStencilViewPointer());

	DeviceContext->ClearRenderTargetView(IntermediateRenderTexture.GetRenderTargetViewPointer(), CurrentRenderSettings->ClearColor);
	DeviceContext->ClearRenderTargetView(BackBufferRenderTargetView.Get(), CurrentRenderSettings->ClearColor);

	DeviceContext->ClearDepthStencilView(SceneRenderDepthStencil.GetDepthStencilViewPointer(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DeviceContext->OMSetDepthStencilState(SceneRenderDepthStencil.GetDepthStencilStatePointer(), 0);

	DeviceContext->PSSetSamplers(0, 1, DefaultWrapSamplerState.GetAddressOf());
	DeviceContext->PSSetSamplers(1, 1, DefaultClampSamplerState.GetAddressOf());
	DeviceContext->PSSetSamplers(2, 1, BrdfSamplerState.GetAddressOf());
	DeviceContext->PSSetSamplers(3, 1, ShadowSamplerState.GetAddressOf());

	DeviceContext->DSSetSamplers(0, 1, TerrainSamplerState.GetAddressOf());
}

void RenderSystem::RenderFrameEnd() const
{
	SwapChain->Present(SyncIntervals, 0);
}

ID3D11Device* RenderSystem::GetPointerToDevice() const
{
	return Device.Get();
}

ID3D11DeviceContext* RenderSystem::GetPointerToDeviceContext() const
{
	return DeviceContext.Get();
}

ID3D11ShaderResourceView* RenderSystem::GetPointerToRemappedShadowShaderResourceView(const float bias, const int cascadeLevel) const
{
	DeviceContext->RSSetViewports(1, &ShadowViewport);

	ShadowRemapperPostProcessingInstance->UpdateBias(bias);
	const auto outputTexture = ShadowRemapperPostProcessingInstance->Process(ShadowRenderDepthStencils[cascadeLevel].GetAddressOfShaderResourceView());

	return outputTexture.Get();
}

bool RenderSystem::InitializeDirectX()
{
	// Adapters selection and initialization

	auto adapters = AdapterReader::GetData();

	if (adapters.empty())
		Logger::Error("No adapters found!");

	if (ConfigurationData->SelectedAdapterId != 0)
		Logger::Warning("Selected adapter with not [0] id. This might not be optimal choice.");

	if (ConfigurationData->SelectedAdapterId >= static_cast<int>(adapters.size()))
		Logger::Error("Adapter with id [" + std::to_string(ConfigurationData->SelectedAdapterId) + "] not found!");

	Logger::Debug("Selected adapter [" + std::to_string(ConfigurationData->SelectedAdapterId) + "].");

	// Swap chain initializtion

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SAMPLE_DESC));

	scd.BufferDesc.Width = WindowWidth;
	scd.BufferDesc.Height = WindowHeight;
	scd.BufferDesc.RefreshRate.Numerator = ConfigurationData->RefreshRateNumerator;
	scd.BufferDesc.RefreshRate.Denominator = ConfigurationData->RefreshRateDenominator;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = Window->GetHandle();
	scd.Windowed = -ConfigurationData->Fullscreen + 1;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Setting device mode

#ifdef _DEBUG
	auto softwareLayers = D3D11_CREATE_DEVICE_DEBUG;
	Logger::Warning("D3D11_DEVICE in DEVICE_DEBUG mode! Rendering will be much slower!");
#else
	auto softwareLayers = 0;
#endif

	// Creating device and swap chain

	auto hr = D3D11CreateDeviceAndSwapChain(adapters[ConfigurationData->SelectedAdapterId].Adapter,
											D3D_DRIVER_TYPE_UNKNOWN,
											nullptr,
											softwareLayers,
											nullptr,
											0,
											D3D11_SDK_VERSION,
											&scd,
											SwapChain.GetAddressOf(),
											Device.GetAddressOf(),
											nullptr,
											DeviceContext.GetAddressOf());

	if (FAILED(hr))
	{
		Logger::Error("Failed to create device and swapchain!");
		return false;
	}

	// Initialize profiler

#ifdef _DEBUG
	Profiler = std::make_unique<ProfilerAnnotations>(DeviceContext.Get(), true);
#else
	Profiler = std::make_unique<ProfilerAnnotations>(DeviceContext.Get(), false);
#endif

	// Back buffer initialization

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		Logger::Error("Error getting back buffer!");
		return false;
	}

	// Back buffer render target initialization

	hr = Device->CreateRenderTargetView(backBuffer.Get(), nullptr, BackBufferRenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating render target view!");
		return false;
	}

	// Intermediate render texture initialization

	if (!IntermediateRenderTexture.Initialize(Device.Get(), WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT,
		ConfigurationData->MultisamplesCount, ConfigurationData->MultisamplesQuality))
	{
		Logger::Error("Error creating Intermediate render texture!");
		return false;
	}

	// Scene render depth stencil initialization

	if (!SceneRenderDepthStencil.Initialize(Device.Get(),
											WindowWidth,
											WindowHeight,
											DXGI_FORMAT_D24_UNORM_S8_UINT,
											DXGI_FORMAT_D24_UNORM_S8_UINT,
											DXGI_FORMAT_UNKNOWN,
											ConfigurationData->MultisamplesCount,
											ConfigurationData->MultisamplesQuality,
											false,
											true))
	{
		Logger::Error("Error creating scene render depth stencil!");
		return false;
	}

	Logger::Debug("Successfully created scene render depth stencil.");

	// Render shadow depth stencil initialization

	if (ConfigurationData->ShadowsEnabled)
	{
		for (auto i = 0; i < 4; i++)
		{
			if (!ShadowRenderDepthStencils[i].Initialize(	Device.Get(),
															ConfigurationData->ShadowsTextureSize,
															ConfigurationData->ShadowsTextureSize,
															DXGI_FORMAT_R32_TYPELESS,
															DXGI_FORMAT_D32_FLOAT,
															DXGI_FORMAT_R32_FLOAT,
															1,
															0,
															true,
															false))
			{
				Logger::Error("Error creating shadow render depth stencil!");
				return false;
			}

			DeviceContext->ClearDepthStencilView(ShadowRenderDepthStencils[i].GetDepthStencilViewPointer(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			Logger::Debug("Initialial clearing shadow render depth stencil...");
		}
	

		Logger::Debug("Successfully created shadow render depth stencil.");

	}

	// Grass render depth stencil initialization

	if (!GrassRenderDepthStencil.Initialize(Device.Get(),
											ConfigurationData->GrassGeneratorResolution,
											ConfigurationData->GrassGeneratorResolution,
											DXGI_FORMAT_D24_UNORM_S8_UINT,
											DXGI_FORMAT_D24_UNORM_S8_UINT,
											DXGI_FORMAT_UNKNOWN,
											1,
											0,
											false,
											false))
	{
		Logger::Error("Error creating grass render depth stencil!");
		return false;
	}

	Logger::Debug("Successfully created grass render depth stencil.");

	// Scene viewport initialization

	SceneViewport.TopLeftX = 0;
	SceneViewport.TopLeftY = 0;
	SceneViewport.Width = static_cast<float>(WindowWidth);
	SceneViewport.Height = static_cast<float>(WindowHeight);
	SceneViewport.MinDepth = 0.0f;
	SceneViewport.MaxDepth = 1.0f;

	Logger::Debug("Scene viewport is created successfully.");

	// Shadow viewport initialization

	if (ConfigurationData->ShadowsEnabled)
	{
		ShadowViewport.TopLeftX = 0;
		ShadowViewport.TopLeftY = 0;
		ShadowViewport.Width = static_cast<float>(ConfigurationData->ShadowsTextureSize);
		ShadowViewport.Height = static_cast<float>(ConfigurationData->ShadowsTextureSize);
		ShadowViewport.MinDepth = 0.0f;
		ShadowViewport.MaxDepth = 1.0f;

		Logger::Debug("Shadow viewport is created successfully.");
	}

	// Grass viewport initialization

	GrassViewport.TopLeftX = 0;
	GrassViewport.TopLeftY = 0;
	GrassViewport.Width = ConfigurationData->GrassGeneratorResolution;
	GrassViewport.Height = ConfigurationData->GrassGeneratorResolution;
	GrassViewport.MinDepth = 0.0f;
	GrassViewport.MaxDepth = 1.0f;

	Logger::Debug("Grass viewport is created successfully.");

	// Set default viewport

	DeviceContext->RSSetViewports(1, &SceneViewport);
	Logger::Debug("Default viewport has been set.");

	// Main rasterizer initialization

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	if (ConfigurationData->WireframeMode == 1)
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	else
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;

	if (ConfigurationData->CullMode == "BACK")
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
	else if (ConfigurationData->CullMode == "FRONT")
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	else if (ConfigurationData->CullMode == "NONE")
		rasterizerDesc.CullMode = D3D11_CULL_NONE;

	hr = Device->CreateRasterizerState(&rasterizerDesc, MainRasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating main rasterizer state!");
		return false;
	}

	// Grass rasterizer initialization

	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	if (ConfigurationData->WireframeMode == 1)
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	else
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;

	rasterizerDesc.CullMode = D3D11_CULL_NONE;

	hr = Device->CreateRasterizerState(&rasterizerDesc, GrassRasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating grass rasterizer state!");
		return false;
	}

	// Post processing rasterizer initialization
	
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;

	hr = Device->CreateRasterizerState(&rasterizerDesc, SolidRasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating solid rasterizer state!");
		return false;
	}

	// Setting initial rasterizer state

	DeviceContext->RSSetState(SolidRasterizerState.Get());
	Logger::Debug("Initial rasterizer state is set successfully.");

	// Blend state initialization

	D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;
	ZeroMemory(&renderTargetBlendDesc, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));

	renderTargetBlendDesc.BlendEnable = true;
	renderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	renderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

	blendDesc.RenderTarget[0] = renderTargetBlendDesc;

	hr = Device->CreateBlendState(&blendDesc, BlendState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating blend state!");
		return false;
	}

	Logger::Debug("Blend state is set successfully.");

	// Default wrap sampler state initialization

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = Device->CreateSamplerState(&samplerDesc, DefaultWrapSamplerState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating default wrap sampler state!");
		return false;
	}
	Logger::Debug("Default wrap sampler state is set successfully.");

	// Default clamp sampler state initialization

	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = Device->CreateSamplerState(&samplerDesc, DefaultClampSamplerState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating default clamp sampler state!");
		return false;
	}
	Logger::Debug("Default clamp sampler state is set successfully.");

	// BRDF sampler state initialization

	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = Device->CreateSamplerState(&samplerDesc, BrdfSamplerState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating BRDF sampler state!");
		return false;
	}
	Logger::Debug("BRDF sampler state is set successfully.");

	// Shadow sampler state initialization

	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	samplerDesc.BorderColor[0] = D3D11_FLOAT32_MAX;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 0;

	hr = Device->CreateSamplerState(&samplerDesc, ShadowSamplerState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating shadow sampler state!");
		return false;
	}
	Logger::Debug("Shadow sampler state is set successfully.");

	// Terrain sampler state initialization

	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = Device->CreateSamplerState(&samplerDesc, TerrainSamplerState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating terrain sampler state!");
		return false;
	}
	Logger::Debug("Terrain sampler state is set successfully.");

	// Topology initialization

	DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Logger::Debug("DirectX initialized successfully.");
	return true;
}

bool RenderSystem::InitializeShaders()
{
	// Uber shader

	if (!UberVertexShader.Initialize(Device.Get(), L"uber_vertex_shader.cso", FatVertex::Layout, FatVertex::LayoutSize))
	{
		Logger::Error("UberVertexShader not initialized due to critical problem!");
		return false;
	}

	if (!UberPixelShader.Initialize(Device.Get(), L"uber_pixel_shader.cso"))
	{
		Logger::Error("UberPixelShader not initialized due to critical problem!");
		return false;
	}

	// Sky shader
	
	if (!SkyVertexShader.Initialize(Device.Get(), L"sky_vertex_shader.cso", PositionVertex::Layout, PositionVertex::LayoutSize))
	{
		Logger::Error("SkyVertexShader not initialized due to critical problem!");
		return false;
	}

	if (!SkyPixelShader.Initialize(Device.Get(), L"sky_pixel_shader.cso"))
	{
		Logger::Error("SkyPixelShader not initialized due to critical problem!");
		return false;
	}

	// Loading Logo shader

	if (!LoadingLogoVertexShader.Initialize(Device.Get(), L"loading_logo_vertex_shader.cso", PositionVertex::Layout, PositionVertex::LayoutSize))
	{
		Logger::Error("LoadingLogoVertexShader not initialized due to critical problem!");
		return false;
	}

	if (!LoadingLogoPixelShader.Initialize(Device.Get(), L"loading_logo_pixel_shader.cso"))
	{
		Logger::Error("LoadingLogoPixelShader not initialized due to critical problem!");
		return false;
	}

	// Terrain shader

	if (!TerrainVertexShader.Initialize(Device.Get(), L"terrain_vertex_shader.cso", PositionAndUvVertex::Layout, PositionAndUvVertex::LayoutSize))
	{
		Logger::Error("TerrainVertexShader not initialized due to critical problem!");
		return false;
	}

	if (!TerrainPixelShader.Initialize(Device.Get(), L"terrain_pixel_shader.cso"))
	{
		Logger::Error("TerrainPixelShader not initialized due to critical problem!");
		return false;
	}

	if (!TerrainHullShader.Initialize(Device.Get(), L"terrain_hull_shader.cso"))
	{
		Logger::Error("TerrainHullShader not initialized due to critical problem!");
		return false;
	}

	if (!TerrainDomainShader.Initialize(Device.Get(), L"terrain_domain_shader.cso"))
	{
		Logger::Error("TerrainDomainShader not initialized due to critical problem!");
		return false;
	}

	// Grass shader

	if (!GrassVertexShader.Initialize(Device.Get(), L"grass_vertex_shader.cso", FatVertex::Layout, FatVertex::LayoutSize))
	{
		Logger::Error("GrassVertexShader not initialized due to critical problem!");
		return false;
	}

	if (!GrassPixelShader.Initialize(Device.Get(), L"grass_pixel_shader.cso"))
	{
		Logger::Error("GrassPixelShader not initialized due to critical problem!");
		return false;
	}


	// Grass generator shader

	if (!GrassGeneratorPixelShader.Initialize(Device.Get(), L"grass_generator_pixel_shader.cso"))
	{
		Logger::Error("GrassGeneratorPixelShader not initialized due to critical problem!");
		return false;
	}

	// Prepare grass date for indirect rendering shader

	if (!PrepareGrassDateForIndirectRenderingComputeShader.Initialize(Device.Get(), L"prepare_grass_date_for_indirect_rendering_compute_shader.cso"))
	{
		Logger::Error("PrepareGrassDateForIndirectRenderingComputeShader not initialized due to critical problem!");
		return false;
	}

	// Shadow shaders

	if (ConfigurationData->ShadowsEnabled) {
		if (!ShadowVertexShader.Initialize(Device.Get(), L"shadow_vertex_shader.cso", FatVertex::Layout, FatVertex::LayoutSize))
		{
			Logger::Error("ShadowVertexShader not initialized due to critical problem!");
			return false;
		}

		if (!ShadowPixelShader.Initialize(Device.Get(), L"shadow_pixel_shader.cso"))
		{
			Logger::Error("ShadowPixelShader not initialized due to critical problem!");
			return false;
		}

		if (!ShadowTerrainVertexShader.Initialize(Device.Get(), L"shadow_terrain_vertex_shader.cso", PositionAndUvVertex::Layout, PositionAndUvVertex::LayoutSize))
		{
			Logger::Error("ShadowTerrainVertexShader not initialized due to critical problem!");
			return false;
		}

		if (!ShadowTerrainHullShader.Initialize(Device.Get(), L"shadow_terrain_hull_shader.cso"))
		{
			Logger::Error("ShadowTerrainHullShader not initialized due to critical problem!");
			return false;
		}

		if (!ShadowTerrainDomainShader.Initialize(Device.Get(), L"shadow_terrain_domain_shader.cso"))
		{
			Logger::Error("ShadowTerrainDomainShader not initialized due to critical problem!");
			return false;
		}
	}

	// Generic code

	Logger::Debug("All shaders successfully initialized.");
	return true;
}

bool RenderSystem::InitializePbrResources()
{
	auto view = Registry->view<SkyboxComponent>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Warning("More than one skybox component found! Engine will use first one.");
		else
			Logger::Error("No skybox component found!");
	}

	return PbrResourceInstance.Initialize(Device.Get(), DeviceContext.Get(), 
		ConfigurationData, view.raw()[0].Material.SkyMap->GetAddressOf());
}

void RenderSystem::InitializeFogSettings() const
{
	CurrentRenderSettings->FogColor =
		DirectX::XMFLOAT3(ConfigurationData->FogColorRed, ConfigurationData->FogColorGreen, ConfigurationData->FogColorBlue);
	CurrentRenderSettings->FogDensity = ConfigurationData->FogDensity;
	CurrentRenderSettings->FogLightColor =
		DirectX::XMFLOAT3(ConfigurationData->FogLightColorRed, ConfigurationData->FogLightColorGreen, ConfigurationData->FogLightColorBlue);
	CurrentRenderSettings->FogLightPower = ConfigurationData->FogLightPower;
	CurrentRenderSettings->FogMinDistance = ConfigurationData->FogMinDistance;
	CurrentRenderSettings->SkyConstantValue = ConfigurationData->SkyConstantValue;
}

void RenderSystem::InitializeLightSettings() const
{
	CurrentRenderSettings->DirectionalLightColor =
		DirectX::XMFLOAT3(ConfigurationData->DirectionalLightColorRed, ConfigurationData->DirectionalLightColorGreen, ConfigurationData->DirectionalLightColorBlue);
	CurrentRenderSettings->DirectionalLightStrength = ConfigurationData->DirectionalLightStrength;
	CurrentRenderSettings->DirectionalLightDirection =
		DirectX::XMFLOAT3(ConfigurationData->DirectionalLightDirectionX, ConfigurationData->DirectionalLightDirectionY, ConfigurationData->DirectionalLightDirectionZ);
}

void RenderSystem::InitializeClearColorSettings() const
{
	CurrentRenderSettings->ClearColor[0] = ConfigurationData->ClearColorRed;
	CurrentRenderSettings->ClearColor[1] = ConfigurationData->ClearColorGreen;
	CurrentRenderSettings->ClearColor[2] = ConfigurationData->ClearColorBlue;
}

void RenderSystem::InitializeGrassSettings() const
{
	CurrentRenderSettings->GrassEnabled = ConfigurationData->GrassEnabled;
	CurrentRenderSettings->GrassGeneratorSurfaceWidth = ConfigurationData->GrassGeneratorSurfaceWidth;
	CurrentRenderSettings->GrassGeneratorSurfaceHeight = ConfigurationData->GrassGeneratorSurfaceHeight;
	CurrentRenderSettings->GrassGeneratorMaxCameraDistance = ConfigurationData->GrassGeneratorMaxCameraDistance;
	CurrentRenderSettings->MinGrassTranslationX = ConfigurationData->MinGrassTranslationX;
	CurrentRenderSettings->MinGrassTranslationY = ConfigurationData->MinGrassTranslationY;
	CurrentRenderSettings->MaxGrassTranslationX = ConfigurationData->MaxGrassTranslationX;
	CurrentRenderSettings->MaxGrassTranslationY = ConfigurationData->MaxGrassTranslationY;
	CurrentRenderSettings->GrassEndCascade0 = ConfigurationData->GrassEndCascade0;
	CurrentRenderSettings->GrassEndCascade1 = ConfigurationData->GrassEndCascade1;
	CurrentRenderSettings->GrassEnd = ConfigurationData->GrassEnd;
}

void RenderSystem::InitializeConstantBuffers()
{
	Logger::Debug("Preparing to initialize constant buffers...");

	auto hr = FatPerObjectBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'FatPerObjectBufferInstance' constant buffer.");

	hr = LightAndAlphaBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'LightAndAlphaBufferInstance' constant buffer.");

	hr = CameraBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'CameraBufferInstance' constant buffer.");

	hr = SimplePerObjectBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'SimplePerObjectBufferInstance' constant buffer.");

	hr = ShadowBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'ShadowBufferInstance' constant buffer.");

	hr = CascadeLevelsBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'CascadeLevelsBuffer' constant buffer.");

	hr = TerrainBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'TerrainBuffer' constant buffer.");

	hr = TerrainSettingsBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'TerrainSettingsBufferInstance' constant buffer.");

	hr = TerrainHeightSamplingBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'TerrainHeightSamplingBufferInstance' constant buffer.");

	hr = TerrainNormalBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'TerrainNormalBufferInstance' constant buffer.");

	hr = TerrainUvBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'TerrainUvBufferInstance' constant buffer.");

	hr = WorldMatrixBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'WorldMatrixBufferInstance' constant buffer.");

	hr = FogBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'FogBufferInstance' constant buffer.");

	hr = LodTransitionBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'LodTransitionBufferInstance' constant buffer.");

	hr = GrassIndirectModelInfoBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'GrassIndirectModelInfoBufferInstance' constant buffer.");

	hr = GrassPerObjectBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'GrassPerObjectBufferInstance' constant buffer.");

	hr = GrassAppearanceParametersBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'GrassAppearanceParametersBufferInstance' constant buffer.");

	hr = GrassGeneratorParametersBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'GrassGeneratorParametersBufferInstance' constant buffer.");

	hr = TimeBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'TimeBufferInstance' constant buffer.");
}

void RenderSystem::InitializeAppendBuffers()
{
	Logger::Debug("Preparing to initialize append buffers...");

	const auto hr = GrassInstanceBufferInstance.Initialize(Device.Get(), ConfigurationData->MaxGrassRenderInstances);
	if (FAILED(hr))
		Logger::Error("Failed to create 'GrassInstanceBufferInstance' append buffer.");
}

void RenderSystem::InitializeCustomizableBuffers()
{
	// Initialization
	
	Logger::Debug("Preparing to initialize customizable buffers...");

	// Grass counter buffer

	D3D11_BUFFER_DESC grassCounterBufferDesc;
	ZeroMemory(&grassCounterBufferDesc, sizeof(D3D11_BUFFER_DESC));

	grassCounterBufferDesc.ByteWidth = sizeof(UINT);
	grassCounterBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	grassCounterBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	grassCounterBufferDesc.CPUAccessFlags = 0;
	grassCounterBufferDesc.MiscFlags = 0;
	grassCounterBufferDesc.StructureByteStride = 0;

	D3D11_UNORDERED_ACCESS_VIEW_DESC grassCounterUavDesc;
	ZeroMemory(&grassCounterUavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	grassCounterUavDesc.Format = DXGI_FORMAT_R32_UINT;
	grassCounterUavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	grassCounterUavDesc.Buffer.FirstElement = 0;
	grassCounterUavDesc.Buffer.Flags = 0;
	grassCounterUavDesc.Buffer.NumElements = 1;

	auto hr = GrassCounterBufferInstance.Initialize(Device.Get(), grassCounterBufferDesc, grassCounterUavDesc);
	if (FAILED(hr))
		Logger::Error("Failed to create grass counter customizable buffer.");

	// Grass draw index instance indirect args buffer

	D3D11_BUFFER_DESC grassDrawIndexInstanceIndirectArgsBufferDesc;
	ZeroMemory(&grassDrawIndexInstanceIndirectArgsBufferDesc, sizeof(D3D11_BUFFER_DESC));

	grassDrawIndexInstanceIndirectArgsBufferDesc.ByteWidth = sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS);
	grassDrawIndexInstanceIndirectArgsBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	grassDrawIndexInstanceIndirectArgsBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	grassDrawIndexInstanceIndirectArgsBufferDesc.CPUAccessFlags = 0;
	grassDrawIndexInstanceIndirectArgsBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	grassDrawIndexInstanceIndirectArgsBufferDesc.StructureByteStride = sizeof(float);

	D3D11_UNORDERED_ACCESS_VIEW_DESC grassDrawIndexInstanceIndirectArgsUavDesc;
	ZeroMemory(&grassDrawIndexInstanceIndirectArgsUavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	grassDrawIndexInstanceIndirectArgsUavDesc.Format = DXGI_FORMAT_R32_UINT;
	grassDrawIndexInstanceIndirectArgsUavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	grassDrawIndexInstanceIndirectArgsUavDesc.Buffer.FirstElement = 0;
	grassDrawIndexInstanceIndirectArgsUavDesc.Buffer.Flags = 0;
	grassDrawIndexInstanceIndirectArgsUavDesc.Buffer.NumElements = 5;

	hr = GrassDrawIndexInstanceIndirectArgsBufferInstance.Initialize(Device.Get(), grassDrawIndexInstanceIndirectArgsBufferDesc, grassDrawIndexInstanceIndirectArgsUavDesc);
	if (FAILED(hr))
		Logger::Error("Failed to create grass draw index instance indirect args customizable buffer.");

	// Finalization

	Logger::Debug("Customizable buffers initialized...");
}

void RenderSystem::InitializePostProcessing()
{
	// Screen space post processing

	CurrentPostProcessingSettings->Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	PostProcessingLoader::Load(ConfigurationData->PathToPostProcessing, CurrentPostProcessingSettings,
		DeviceContext.Get(), Device.Get(), WindowWidth, WindowHeight);

	MultisamplingPostProcessingInstance = std::make_unique<MultisamplingPostProcessing>(DeviceContext.Get(),
		Device.Get(), WindowWidth, WindowHeight, CurrentPostProcessingSettings->Format);

	CopyToBackBufferPostProcessingInstance = std::make_unique<CopyToBackBufferPostProcessing>(DeviceContext.Get(), 
		Device.Get(), BackBufferRenderTargetView.GetAddressOf());

	// Custom post processing

	if (ConfigurationData->ShadowsEnabled && ConfigurationData->EnableRealtimeShadowPreview)
	{
		ShadowRemapperPostProcessingInstance = std::make_unique<ShadowRemapperPostProcessing>(	DeviceContext.Get(),
																								Device.Get(), 
																								ConfigurationData->ShadowsTextureSize,
																								ConfigurationData->ShadowsTextureSize,
																								DXGI_FORMAT_R32G32B32A32_FLOAT,
																								ConfigurationData->MainCameraNearZ, 
																								ConfigurationData->MainCameraFarZ,
																								2.0f);
	}
}

void RenderSystem::ShowLoadingScreen()
{
	Logger::Debug("Preparing loading screen...");

	RenderFrameBegin();
	DeviceContext->ClearRenderTargetView(IntermediateRenderTexture.GetRenderTargetViewPointer(), BACKGROUND_COLOR);

	ChangeBasicShaders(LoadingLogoVertexShader, LoadingLogoPixelShader);

	const auto ratio = static_cast<float>(ConfigurationData->WindowWidth) / static_cast<float>(ConfigurationData->WindowHeight);
	Logger::Debug("Screen ratio: " + std::to_string(ratio) + ".");

	const auto logoScale = DirectX::XMMatrixScaling(ConfigurationData->LogoScale, 
		ConfigurationData->LogoScale, ConfigurationData->LogoScale);

	SimplePerObjectBufferInstance.Data.WorldViewProjectionMat =
		XMMatrixTranspose(logoScale * DirectX::XMMatrixOrthographicLH(ratio, 1, -1.0f, 1.0f));
	SimplePerObjectBufferInstance.ApplyChanges();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> loadingTextureResourceView;
	MaterialLoader::GetAndSetLoadingTexture(Device.Get(), ConfigurationData->PathToLoadingLogo, loadingTextureResourceView);

	DeviceContext->VSSetConstantBuffers(0, 1, SimplePerObjectBufferInstance.GetAddressOf());
	DeviceContext->PSSetShaderResources(0, 1, loadingTextureResourceView.GetAddressOf());

	VertexBuffer<PositionVertex> vertexBuffer;
	IndexBuffer indexBuffer;
	QuadGenerator::Generate(Device.Get(), vertexBuffer, indexBuffer, -0.5f, -0.5f, 0.5f, 0.5f);

	UINT offset = 0;
	Draw(vertexBuffer, indexBuffer, offset);

	if (!ConfigurationData->DisableLoadingText)
	{
		Logger::Debug("Preparing loading text...");

		auto loadingSpriteBatch = std::make_unique<DirectX::SpriteBatch>(DeviceContext.Get());
		const auto loadingSpriteFont = std::make_unique<DirectX::SpriteFont>(Device.Get(), StringUtil::StringToWide(ConfigurationData->PathToLoadingFont).c_str());

		loadingSpriteBatch->Begin();
		loadingSpriteFont->DrawString(	loadingSpriteBatch.get(), 
										L"Loading...", 
										DirectX::XMFLOAT2(ConfigurationData->WindowWidth - ConfigurationData->LoadingOffsetX, ConfigurationData->WindowHeight - ConfigurationData->LoadingOffsetY));
		loadingSpriteBatch->End();
	}

	PerformPostProcessing(true);
	RenderFrameEnd();

	Logger::Debug("Showed loading screen!");
}

void RenderSystem::InitializeSkyboxComponent()
{
	Registry->view<SkyboxComponent>().each([this](SkyboxComponent &skyboxComponent)
	{
		CubeGenerator::Generate(Device.Get(), skyboxComponent.RenderVertexBuffer, skyboxComponent.RenderIndexBuffer);
		MaterialLoader::SetResourceForSkyMaterial(Device.Get(), skyboxComponent.Material, skyboxComponent.SkyboxTextureId);
	});
}

void RenderSystem::InitializeGrassComponent()
{
	Registry->view<GrassComponent>().each([this](GrassComponent &grassComponent)
	{
		grassComponent.Meshes = ModelLoader::GetGrassResource(grassComponent.ModelId);
		MaterialLoader::SetResourceForGrassMeshGroup(Device.Get(), *grassComponent.Meshes, grassComponent.MaterialId);
	});
}

void RenderSystem::InitializeTerrainComponent()
{
	Registry->view<TerrainComponent>().each([this](TerrainComponent &terrainComponent)
	{
		MaterialLoader::SetResourceForTerrainMaterial(Device.Get(), terrainComponent.Material, terrainComponent.MaterialId);
		TerrainUtil::GenerateTerrainMesh(terrainComponent, Device.Get(), ConfigurationData->EnableAsyncTerrainGeneration);
		TerrainUtil::OptimizeTerrain(terrainComponent, Device.Get(), DeviceContext.Get());
		TerrainUtil::GenerateNormals(terrainComponent, TerrainNormalBufferInstance, Device.Get(), DeviceContext.Get());
	});
}

void RenderSystem::InitializeModelRenderComponent()
{
	Registry->view<ModelRenderComponent>().each([this](ModelRenderComponent &modelRenderComponent)
	{
		modelRenderComponent.Meshes = ModelLoader::GetResource(modelRenderComponent.ModelId);
		MaterialLoader::SetResourceForMeshGroup(Device.Get(), *modelRenderComponent.Meshes, modelRenderComponent.MaterialId);

		if (!modelRenderComponent.LowPolyModelId.empty())
		{
			modelRenderComponent.LowPolyMeshes = ModelLoader::GetResource(modelRenderComponent.LowPolyModelId);
			MaterialLoader::SetResourceForMeshGroup(Device.Get(), *modelRenderComponent.LowPolyMeshes, modelRenderComponent.MaterialId);
		}
	});
}

void RenderSystem::InitializeShadowCameras()
{
	for (auto i = 0; i < ShadowCameras.size(); i++)
	{
		ShadowCameras[i].UpdateShadowResolution(ConfigurationData->ShadowsTextureSize);
		ShadowCameras[i].UpdateNearZShift(ConfigurationData->ShadowCameraShiftNearZ);
		ShadowCameras[i].UpdateMainCameraProjectionMatrix(	i,
															ConfigurationData->MainCameraFov,
															ConfigurationData->WindowWidth,
															ConfigurationData->WindowHeight,
															ConfigurationData->MainCameraNearZ,
															{
																ConfigurationData->MainCameraFarZ * ConfigurationData->CascadeEnd0,
																ConfigurationData->MainCameraFarZ * ConfigurationData->CascadeEnd1,
																ConfigurationData->MainCameraFarZ * ConfigurationData->CascadeEnd2,
																ConfigurationData->MainCameraFarZ * ConfigurationData->CascadeEnd3 });
	}
}

void RenderSystem::ChangeBasicShaders(const VertexShader& vertexShader, const PixelShader& pixelShader) const
{
	DeviceContext->IASetInputLayout(vertexShader.GetInputLayout());
	DeviceContext->VSSetShader(vertexShader.GetShader(), nullptr, 0);
	DeviceContext->PSSetShader(pixelShader.GetShader(), nullptr, 0);
}

void RenderSystem::ChangeTessellationShaders(const HullShader& hullShader, const DomainShader& domainShader) const
{
	DeviceContext->HSSetShader(hullShader.GetShader(), nullptr, 0);
	DeviceContext->DSSetShader(domainShader.GetShader(), nullptr, 0);
}

void RenderSystem::ResetTessellationShaders() const
{
	DeviceContext->HSSetShader(nullptr, nullptr, 0);
	DeviceContext->DSSetShader(nullptr, nullptr, 0);
}

void RenderSystem::RenderShadows(const CameraComponent &mainCameraComponent, const TransformComponent &mainCameraTransformComponent)
{
	Profiler->BeginEvent("Shadows");

	DeviceContext->RSSetViewports(1, &ShadowViewport);

	auto& mainCamera = CameraLocator::GetMainCamera(Registry);

	for (auto& camera : ShadowCameras)
	{
		camera.UpdateLightDirection(	CurrentRenderSettings->DirectionalLightDirection.x,
										CurrentRenderSettings->DirectionalLightDirection.y,
										CurrentRenderSettings->DirectionalLightDirection.z);
		camera.UpdateShadowMapLocation(mainCamera.ViewMatrix);
	}

	RenderSceneForShadows(mainCameraComponent, mainCameraTransformComponent);

	Profiler->EndEvent();
}

void RenderSystem::RenderScene(const CameraComponent &cameraComponent, const TransformComponent &mainCameraTransformComponent)
{
	Profiler->BeginEvent("Scene");

	DeviceContext->RSSetState(MainRasterizerState.Get());
	DeviceContext->RSSetViewports(1, &SceneViewport);
	DeviceContext->OMSetRenderTargets(1, IntermediateRenderTexture.GetAddressOfRenderTargetView(), SceneRenderDepthStencil.GetDepthStencilViewPointer());

	UpdateFogBuffer();
	UpdateLightAndAlphaBuffer();

	SetShadowResourcesForShadowCascades(14);
	SetPbrResources();

	RenderModelRenderComponents(cameraComponent, mainCameraTransformComponent, Solid);

	if (CurrentRenderSettings->GrassEnabled)
		RenderGrass(cameraComponent, mainCameraTransformComponent);

	RenderTerrain(cameraComponent, mainCameraTransformComponent);
	RenderSkyBoxComponents(cameraComponent);
	RenderModelRenderComponents(cameraComponent, mainCameraTransformComponent, Transparent);

	ClearShadowResourcesForShadowCascades(14);

	Profiler->EndEvent();
}

void RenderSystem::RenderSceneForShadows(const CameraComponent &mainCameraComponent, const TransformComponent &mainCameraTransformComponent)
{
	UINT offset = 0;

	// Model Render Component - CB

	DeviceContext->VSSetConstantBuffers(0, 1, ShadowBufferInstance.GetAddressOf());

	// Terrain Component - CB

	DeviceContext->VSSetConstantBuffers(1, 1, WorldMatrixBufferInstance.GetAddressOf());

	DeviceContext->HSSetConstantBuffers(0, 1, TerrainBufferInstance.GetAddressOf());
	DeviceContext->HSSetConstantBuffers(1, 1, CameraBufferInstance.GetAddressOf());
	DeviceContext->HSSetConstantBuffers(2, 1, TerrainSettingsBufferInstance.GetAddressOf());

	DeviceContext->DSSetConstantBuffers(0, 1, ShadowBufferInstance.GetAddressOf());
	DeviceContext->DSSetConstantBuffers(1, 1, TerrainHeightSamplingBufferInstance.GetAddressOf());

	for (auto i = 0; i < ShadowCamera::SHADOW_CASCADES_COUNT; i++)
	{
		Profiler->BeginEvent("Cascade " + std::to_string(i));

		DeviceContext->OMSetRenderTargets(0, nullptr, ShadowRenderDepthStencils[i].GetDepthStencilViewPointer());
		DeviceContext->ClearDepthStencilView(ShadowRenderDepthStencils[i].GetDepthStencilViewPointer(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ChangeBasicShaders(ShadowVertexShader, ShadowPixelShader);

		Registry->view<ModelRenderComponent, TransformComponent>().each([this, &mainCameraTransformComponent, &offset, i](ModelRenderComponent &modelRenderComponent, TransformComponent &transformComponent)
		{
			auto shadowBufferInstanceSet = false;

			std::shared_ptr<std::vector<Mesh>> currentMeshes;
			const auto distanceFromCamera = CalculateDistanceFromCamera(transformComponent, mainCameraTransformComponent);

			if (modelRenderComponent.LowPolyDistance < 0.0f || modelRenderComponent.LowPolyDistance >= distanceFromCamera)
				currentMeshes = modelRenderComponent.Meshes;
			else
				currentMeshes = modelRenderComponent.LowPolyMeshes;

			for (const auto& mesh : *currentMeshes)
			{
				if (!FrustumUtil::Test(FrustumUtil::RecalculateAABBForWorld(mesh.FrustumPoints, transformComponent.WorldMatrix), ShadowCameras[i].GetFrustumPlanes()))
					continue;

				if (!shadowBufferInstanceSet)
				{
					shadowBufferInstanceSet = true;

					ShadowBufferInstance.Data.WorldLightMatrix = XMMatrixTranspose(transformComponent.WorldMatrix * ShadowCameras[i].CalculateCameraMatrix());
					ShadowBufferInstance.ApplyChanges();
				}

				if (mesh.Material.Alpha < ConfigurationData->ShadowAlphaThreshold)
					continue;

				Draw(mesh.RenderVertexBuffer, mesh.RenderIndexBuffer, offset);
			}
		});

		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

		ChangeBasicShaders(ShadowTerrainVertexShader, ShadowPixelShader);
		ChangeTessellationShaders(ShadowTerrainHullShader, ShadowTerrainDomainShader);

		const auto shadowCameraFrustumPlanes = ShadowCameras[i].GetFrustumPlanes();
		for (auto j = 0; j < 6; j++)
			TerrainBufferInstance.Data.FrustumPlanes[j] = shadowCameraFrustumPlanes[j];
		TerrainBufferInstance.ApplyChanges();

		Registry->view<TerrainComponent, TransformComponent>().each([this, &offset, i](TerrainComponent &terrainComponent, TransformComponent &transformComponent)
		{
			WorldMatrixBufferInstance.Data.WorldMatrix = XMMatrixTranspose(transformComponent.WorldMatrix);
			WorldMatrixBufferInstance.ApplyChanges();

			ShadowBufferInstance.Data.WorldLightMatrix = XMMatrixTranspose(transformComponent.WorldMatrix * ShadowCameras[i].CalculateCameraMatrix());
			ShadowBufferInstance.ApplyChanges();

			TerrainHeightSamplingBufferInstance.Data.MaxHeight = terrainComponent.MaxHeight;
			TerrainHeightSamplingBufferInstance.ApplyChanges();

			Draw(terrainComponent.RenderVertexBuffer, terrainComponent.RenderIndexBuffer, offset);
		});

		ResetTessellationShaders();
		DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Profiler->EndEvent();
	}
}

void RenderSystem::RenderSkyBoxComponents(const CameraComponent& cameraComponent)
{
	Profiler->BeginEvent("Skybox");

	UINT offset = 0;
	ChangeBasicShaders(SkyVertexShader, SkyPixelShader);

	DeviceContext->VSSetConstantBuffers(0, 1, SimplePerObjectBufferInstance.GetAddressOf());

	DeviceContext->PSSetConstantBuffers(0, 1, FogBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(1, 1, LightAndAlphaBufferInstance.GetAddressOf());

	Registry->view<SkyboxComponent, TransformComponent>().each([this, &cameraComponent, &offset](SkyboxComponent &skyboxComponent, TransformComponent &transformComponent)
	{
		SimplePerObjectBufferInstance.Data.WorldViewProjectionMat =
			XMMatrixTranspose(transformComponent.WorldMatrix * (cameraComponent.ViewMatrix * cameraComponent.ProjectionMatrix));
		SimplePerObjectBufferInstance.ApplyChanges();

		DeviceContext->PSSetShaderResources(0, 1, skyboxComponent.Material.SkyMap->GetAddressOf());

		Draw(skyboxComponent.RenderVertexBuffer, skyboxComponent.RenderIndexBuffer, offset);
	});

	Profiler->EndEvent();
}

void RenderSystem::RenderGrass(const CameraComponent& mainCameraComponent, const TransformComponent& mainCameraTransformComponent)
{
	// Preparation

	Profiler->BeginEvent("Grass");

	DeviceContext->RSSetState(GrassRasterizerState.Get());
	
	UINT offset = 0;

	// STAGE 1
	// Generate grass

	ChangeBasicShaders(TerrainVertexShader, GrassGeneratorPixelShader);

	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	ChangeTessellationShaders(TerrainHullShader, TerrainDomainShader);

	DeviceContext->RSSetViewports(1, &GrassViewport);

	UINT initialCountsUav = 0;
	DeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(	0, 
																nullptr, 
																GrassRenderDepthStencil.GetDepthStencilViewPointer(),
																0, 
																1, 
																GrassInstanceBufferInstance.GetAddressOfUnorderedAccessView(), 
																&initialCountsUav);

	DeviceContext->ClearDepthStencilView(	GrassRenderDepthStencil.GetDepthStencilViewPointer(), 
											D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
											1.0f, 
											0);

	for (auto i = 0; i < 6; i++)
		TerrainBufferInstance.Data.FrustumPlanes[i] = mainCameraComponent.FrustumPlanes[i];

	TerrainBufferInstance.ApplyChanges();

	DeviceContext->VSSetConstantBuffers(0, 1, FatPerObjectBufferInstance.GetAddressOf());

	DeviceContext->HSSetConstantBuffers(0, 1, TerrainBufferInstance.GetAddressOf());
	DeviceContext->HSSetConstantBuffers(1, 1, CameraBufferInstance.GetAddressOf());
	DeviceContext->HSSetConstantBuffers(2, 1, TerrainSettingsBufferInstance.GetAddressOf());

	DeviceContext->DSSetConstantBuffers(0, 1, FatPerObjectBufferInstance.GetAddressOf());
	DeviceContext->DSSetConstantBuffers(1, 1, TerrainHeightSamplingBufferInstance.GetAddressOf());

	DeviceContext->PSSetConstantBuffers(0, 1, TerrainUvBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(1, 1, GrassGeneratorParametersBufferInstance.GetAddressOf());

	Registry->view<TerrainComponent, TransformComponent>().each([this, &offset, &mainCameraComponent, &mainCameraTransformComponent](TerrainComponent &terrainComponent, TransformComponent &transformComponent)
	{
		TerrainSettingsBufferInstance.Data.MinTessellationFactor = terrainComponent.MinTerrainTessellationFactor;
		TerrainSettingsBufferInstance.Data.MaxTessellationFactor = terrainComponent.MaxTerrainTessellationFactor;
		TerrainSettingsBufferInstance.Data.MinTessellationDistance = terrainComponent.MinTerrainTessellationDistance;
		TerrainSettingsBufferInstance.Data.MaxTessellationDistance = terrainComponent.MaxTerrainTessellationDistance;
		TerrainSettingsBufferInstance.ApplyChanges();

		FatPerObjectBufferInstance.Data.WorldViewProjectionMat =
			XMMatrixTranspose(transformComponent.WorldMatrix * GrassCamera::GenerateCameraMatrix(	mainCameraTransformComponent, 
																									CurrentRenderSettings->GrassGeneratorMaxCameraDistance, 
																									CurrentRenderSettings->GrassGeneratorSurfaceWidth,
																									CurrentRenderSettings->GrassGeneratorSurfaceHeight,
																									ConfigurationData->GrassGeneratorResolution));
		FatPerObjectBufferInstance.Data.WorldMatrix = XMMatrixTranspose(transformComponent.WorldMatrix);
		for (auto i = 0; i < 4; i++)
			FatPerObjectBufferInstance.Data.LightSpaceMatrix[i] = XMMatrixTranspose(ShadowCameras[i].CalculateCameraMatrix());
		FatPerObjectBufferInstance.ApplyChanges();

		TerrainHeightSamplingBufferInstance.Data.MaxHeight = terrainComponent.MaxHeight;
		TerrainHeightSamplingBufferInstance.ApplyChanges();

		TerrainUvBufferInstance.Data.TexturesScale = terrainComponent.Material.TexturesScale;
		TerrainUvBufferInstance.ApplyChanges();

		GrassGeneratorParametersBufferInstance.Data.GrassGeneratorResolution = ConfigurationData->GrassGeneratorResolution;
		GrassGeneratorParametersBufferInstance.Data.GrassPlacementThreshold = terrainComponent.GrassPlacementThreshold;
		GrassGeneratorParametersBufferInstance.Data.MinGrassTranslationX = CurrentRenderSettings->MinGrassTranslationX;
		GrassGeneratorParametersBufferInstance.Data.MinGrassTranslationY = CurrentRenderSettings->MinGrassTranslationY;
		GrassGeneratorParametersBufferInstance.Data.MaxGrassTranslationX = CurrentRenderSettings->MaxGrassTranslationX;
		GrassGeneratorParametersBufferInstance.Data.MaxGrassTranslationY = CurrentRenderSettings->MaxGrassTranslationY;
		GrassGeneratorParametersBufferInstance.Data.GrassEndCascade0 = CurrentRenderSettings->GrassEndCascade0;
		GrassGeneratorParametersBufferInstance.Data.GrassEndCascade1 = CurrentRenderSettings->GrassEndCascade1;
		GrassGeneratorParametersBufferInstance.Data.GrassEnd = CurrentRenderSettings->GrassEnd;
		GrassGeneratorParametersBufferInstance.ApplyChanges();

		DeviceContext->DSSetShaderResources(0, 1, terrainComponent.Material.Heightmap->GetAddressOf());

		DeviceContext->PSSetShaderResources(0, 1, terrainComponent.Material.Splatmap->GetAddressOf());
		DeviceContext->PSSetShaderResources(1, 1, terrainComponent.Material.GrassPlacement->GetAddressOf());

		DeviceContext->PSSetShaderResources(2, 1, terrainComponent.Material.BaseAlbedoTexture->GetAddressOf());
		DeviceContext->PSSetShaderResources(3, 1, terrainComponent.Material.RedAlbedoTexture->GetAddressOf());
		DeviceContext->PSSetShaderResources(4, 1, terrainComponent.Material.GreenAlbedoTexture->GetAddressOf());
		DeviceContext->PSSetShaderResources(5, 1, terrainComponent.Material.BlueAlbedoTexture->GetAddressOf());

		DeviceContext->PSSetShaderResources(6, 1, terrainComponent.Material.OptimizedMetalicTexture.ShaderResourceView.GetAddressOf());
		DeviceContext->PSSetShaderResources(7, 1, terrainComponent.Material.OptimizedSmoothnessTexture.ShaderResourceView.GetAddressOf());

		DeviceContext->PSSetShaderResources(8, 1, terrainComponent.Material.CalculatedNormalTexture.ShaderResourceView.GetAddressOf());

		Draw(terrainComponent.RenderVertexBuffer, terrainComponent.RenderIndexBuffer, offset);
	});

	DeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(	0, 
																nullptr, 
																nullptr,
																0, 
																1, 
																&NullUnorderedAccessView, 
																&initialCountsUav);
	ResetTessellationShaders();
	DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Stage 2
	// Compute, prepare data and draw grass 
	
	LightAndAlphaBufferInstance.Data.Alpha = 1.0;
	LightAndAlphaBufferInstance.ApplyChanges();

	DeviceContext->VSSetConstantBuffers(0, 1, GrassPerObjectBufferInstance.GetAddressOf());

	DeviceContext->PSSetConstantBuffers(0, 1, LightAndAlphaBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(1, 1, CameraBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(2, 1, CascadeLevelsBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(3, 1, FogBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(4, 1, GrassAppearanceParametersBufferInstance.GetAddressOf());

	DeviceContext->VSSetShaderResources(0, 1, GrassInstanceBufferInstance.GetAddressOfShaderResourceView());
	DeviceContext->VSSetConstantBuffers(1, 1, TimeBufferInstance.GetAddressOf());

	DeviceContext->PSSetShaderResources(0, 1, GrassInstanceBufferInstance.GetAddressOfShaderResourceView());
	
	Registry->view<GrassComponent>().each([this, &offset, &mainCameraComponent](GrassComponent &grassComponent)
	{
		for (const auto& mesh : *grassComponent.Meshes)
		{
			// Compute and prepare data

			GrassIndirectModelInfoBufferInstance.Data.IndexCountPerInstance = mesh.RenderIndexBuffer.GetIndexCount();
			GrassIndirectModelInfoBufferInstance.Data.MaxNumberOfInstances = ConfigurationData->MaxGrassRenderInstances;
			GrassIndirectModelInfoBufferInstance.ApplyChanges();

			DeviceContext->CopyStructureCount(GrassCounterBufferInstance.GetBuffer(), 0, GrassInstanceBufferInstance.GetUnorderedAccessView());

			DeviceContext->CSSetShader(PrepareGrassDateForIndirectRenderingComputeShader.GetShader(), nullptr, 0);

			DeviceContext->CSSetUnorderedAccessViews(0, 1, GrassCounterBufferInstance.GetAddressOfUnorderedAccessView(), nullptr);
			DeviceContext->CSSetUnorderedAccessViews(1, 1, GrassDrawIndexInstanceIndirectArgsBufferInstance.GetAddressOfUnorderedAccessView(), nullptr);

			DeviceContext->CSSetConstantBuffers(0, 1, GrassIndirectModelInfoBufferInstance.GetAddressOf());

			DeviceContext->Dispatch(1, 1, 1);

			// Draw grass

			ChangeBasicShaders(GrassVertexShader, GrassPixelShader);

			DeviceContext->RSSetViewports(1, &SceneViewport);
			DeviceContext->OMSetRenderTargets(1, IntermediateRenderTexture.GetAddressOfRenderTargetView(), SceneRenderDepthStencil.GetDepthStencilViewPointer());

			GrassPerObjectBufferInstance.Data.ViewProjectionMatrix = XMMatrixTranspose(mainCameraComponent.ViewMatrix * mainCameraComponent.ProjectionMatrix);
			for (auto i = 0; i < 4; i++)
				GrassPerObjectBufferInstance.Data.LightSpaceMatrix[i] = XMMatrixTranspose(ShadowCameras[i].CalculateCameraMatrix());
			GrassPerObjectBufferInstance.ApplyChanges();

			GrassAppearanceParametersBufferInstance.Data.AlphaDiscardPoint = grassComponent.AlphaDiscardPoint;
			GrassAppearanceParametersBufferInstance.Data.AlbedoInterpolationRatio = grassComponent.AlbedoInterpolationRatio;
			GrassAppearanceParametersBufferInstance.Data.NormalInterpolationRatio = grassComponent.NormalInterpolationRatio;
			GrassAppearanceParametersBufferInstance.Data.RoughnessInterpolationTarget = grassComponent.RoughnessInterpolationTarget;
			GrassAppearanceParametersBufferInstance.Data.RoughnessInterpolationRatio = grassComponent.RoughnessInterpolationRatio;
			GrassAppearanceParametersBufferInstance.Data.MetalicInterpolationTarget = grassComponent.MetalicInterpolationTarget;
			GrassAppearanceParametersBufferInstance.Data.MetalicInterpolationRatio = grassComponent.MetalicInterpolationRatio;
			GrassAppearanceParametersBufferInstance.Data.OcclusionValue = grassComponent.OcclusionValue;
			GrassAppearanceParametersBufferInstance.ApplyChanges();

			DeviceContext->PSSetShaderResources(1, 1, mesh.Material.AlbedoTexture->GetAddressOf());

			DeviceContext->IASetVertexBuffers(0, 1, mesh.RenderVertexBuffer.GetAddressOf(), mesh.RenderVertexBuffer.StridePtr(), &offset);
			DeviceContext->IASetIndexBuffer(mesh.RenderIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			DeviceContext->DrawIndexedInstancedIndirect(GrassDrawIndexInstanceIndirectArgsBufferInstance.GetBuffer(), 0);
		}
	});

	// Finalization

	DeviceContext->VSSetShaderResources(0, 1, &NullShaderResourceView);
	DeviceContext->RSSetState(MainRasterizerState.Get());
	DeviceContext->RSSetViewports(1, &SceneViewport);
	DeviceContext->OMSetRenderTargets(1, IntermediateRenderTexture.GetAddressOfRenderTargetView(), SceneRenderDepthStencil.GetDepthStencilViewPointer());

	Profiler->EndEvent();
}

void RenderSystem::RenderTerrain(const CameraComponent &mainCameraComponent, const TransformComponent &mainCameraTransformComponent)
{
	Profiler->BeginEvent("Terrain");

	UINT offset = 0;
	ChangeBasicShaders(TerrainVertexShader, TerrainPixelShader);

	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	ChangeTessellationShaders(TerrainHullShader, TerrainDomainShader);

	for (auto i = 0; i < 6; i++)
		TerrainBufferInstance.Data.FrustumPlanes[i] = mainCameraComponent.FrustumPlanes[i];

	TerrainBufferInstance.ApplyChanges();

	LightAndAlphaBufferInstance.Data.Alpha = 1.0;
	LightAndAlphaBufferInstance.ApplyChanges();

	DeviceContext->VSSetConstantBuffers(0, 1, FatPerObjectBufferInstance.GetAddressOf());

	DeviceContext->HSSetConstantBuffers(0, 1, TerrainBufferInstance.GetAddressOf());
	DeviceContext->HSSetConstantBuffers(1, 1, CameraBufferInstance.GetAddressOf());
	DeviceContext->HSSetConstantBuffers(2, 1, TerrainSettingsBufferInstance.GetAddressOf());

	DeviceContext->DSSetConstantBuffers(0, 1, FatPerObjectBufferInstance.GetAddressOf());
	DeviceContext->DSSetConstantBuffers(1, 1, TerrainHeightSamplingBufferInstance.GetAddressOf());

	DeviceContext->PSSetConstantBuffers(0, 1, TerrainUvBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(1, 1, LightAndAlphaBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(2, 1, CameraBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(3, 1, CascadeLevelsBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(4, 1, FogBufferInstance.GetAddressOf());

	Registry->view<TerrainComponent, TransformComponent>().each([this, &offset, &mainCameraComponent](TerrainComponent &terrainComponent, TransformComponent &transformComponent)
	{
		TerrainSettingsBufferInstance.Data.MinTessellationFactor = terrainComponent.MinTerrainTessellationFactor;
		TerrainSettingsBufferInstance.Data.MaxTessellationFactor = terrainComponent.MaxTerrainTessellationFactor;
		TerrainSettingsBufferInstance.Data.MinTessellationDistance = terrainComponent.MinTerrainTessellationDistance;
		TerrainSettingsBufferInstance.Data.MaxTessellationDistance = terrainComponent.MaxTerrainTessellationDistance;
		TerrainSettingsBufferInstance.ApplyChanges();

		FatPerObjectBufferInstance.Data.WorldViewProjectionMat =
			XMMatrixTranspose(transformComponent.WorldMatrix * (mainCameraComponent.ViewMatrix * mainCameraComponent.ProjectionMatrix));
		FatPerObjectBufferInstance.Data.WorldMatrix = XMMatrixTranspose(transformComponent.WorldMatrix);
		for (auto i = 0; i < 4; i++)
			FatPerObjectBufferInstance.Data.LightSpaceMatrix[i] = XMMatrixTranspose(ShadowCameras[i].CalculateCameraMatrix());
		FatPerObjectBufferInstance.ApplyChanges();

		TerrainHeightSamplingBufferInstance.Data.MaxHeight = terrainComponent.MaxHeight;
		TerrainHeightSamplingBufferInstance.ApplyChanges();

		TerrainUvBufferInstance.Data.TexturesScale = terrainComponent.Material.TexturesScale;
		TerrainUvBufferInstance.ApplyChanges();

		DeviceContext->DSSetShaderResources(0, 1, terrainComponent.Material.Heightmap->GetAddressOf());

		DeviceContext->PSSetShaderResources(0, 1, terrainComponent.Material.Splatmap->GetAddressOf());

		DeviceContext->PSSetShaderResources(1, 1, terrainComponent.Material.BaseAlbedoTexture->GetAddressOf());
		DeviceContext->PSSetShaderResources(2, 1, terrainComponent.Material.RedAlbedoTexture->GetAddressOf());
		DeviceContext->PSSetShaderResources(3, 1, terrainComponent.Material.GreenAlbedoTexture->GetAddressOf());
		DeviceContext->PSSetShaderResources(4, 1, terrainComponent.Material.BlueAlbedoTexture->GetAddressOf());

		DeviceContext->PSSetShaderResources(5, 1, terrainComponent.Material.BaseNormalTexture->GetAddressOf());
		DeviceContext->PSSetShaderResources(6, 1, terrainComponent.Material.RedNormalTexture->GetAddressOf());
		DeviceContext->PSSetShaderResources(7, 1, terrainComponent.Material.GreenNormalTexture->GetAddressOf());
		DeviceContext->PSSetShaderResources(8, 1, terrainComponent.Material.BlueNormalTexture->GetAddressOf());

		DeviceContext->PSSetShaderResources(9, 1, terrainComponent.Material.OptimizedOcclusionTexture.ShaderResourceView.GetAddressOf());
		DeviceContext->PSSetShaderResources(10, 1, terrainComponent.Material.OptimizedMetalicTexture.ShaderResourceView.GetAddressOf());
		DeviceContext->PSSetShaderResources(11, 1, terrainComponent.Material.OptimizedSmoothnessTexture.ShaderResourceView.GetAddressOf());

		DeviceContext->PSSetShaderResources(12, 1, terrainComponent.Material.CalculatedNormalTexture.ShaderResourceView.GetAddressOf());
		DeviceContext->PSSetShaderResources(13, 1, terrainComponent.Material.CalculatedTangentTexture.ShaderResourceView.GetAddressOf());

		Draw(terrainComponent.RenderVertexBuffer, terrainComponent.RenderIndexBuffer, offset);
	});

	ResetTessellationShaders();
	DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Profiler->EndEvent();
}

void RenderSystem::RenderModelRenderComponents(const CameraComponent &mainCameraComponent, const TransformComponent &mainTransformComponent, 
	const TransparencyRenderType renderMode)
{
	Profiler->BeginEvent(std::string(renderMode == Solid ? "Solid" : "Non-soild") + " Model");

	UINT offset = 0;
	ChangeBasicShaders(UberVertexShader, UberPixelShader);

	DeviceContext->VSSetConstantBuffers(0, 1, FatPerObjectBufferInstance.GetAddressOf());
	
	DeviceContext->PSSetConstantBuffers(0, 1, LightAndAlphaBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(1, 1, CameraBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(2, 1, CascadeLevelsBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(3, 1, FogBufferInstance.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(4, 1, LodTransitionBufferInstance.GetAddressOf());

	if (renderMode == Transparent)
		DeviceContext->OMSetBlendState(BlendState.Get(), nullptr, 0xffffffff);

	Registry->view<ModelRenderComponent, TransformComponent>().each([this, &mainCameraComponent, &mainTransformComponent, 
		&offset, renderMode](ModelRenderComponent &modelRenderComponent, TransformComponent &transformComponent)
	{
		if (modelRenderComponent.Invisible)
			return;

		auto fatPerObjectBufferSet = false;

		const auto distanceFromCamera = CalculateDistanceFromCamera(transformComponent, mainTransformComponent);
		const auto coverageLod = CalculateLodCoverage(modelRenderComponent, distanceFromCamera);

		if (modelRenderComponent.LowPolyDistance < 0.0f || 
			modelRenderComponent.LowPolyDistance >= distanceFromCamera - modelRenderComponent.LodTransitionDistance || 
			CurrentRenderSettings->LevelOfDetailsMode == Manual)
		{
			LodTransitionBufferInstance.Data.PercentageCoverage = coverageLod;
			LodTransitionBufferInstance.Data.InvertedCoverage = false;
			LodTransitionBufferInstance.ApplyChanges();

			for (const auto& mesh : *modelRenderComponent.Meshes)
			{
				RenderMesh(mesh, transformComponent, mainCameraComponent, offset, fatPerObjectBufferSet, renderMode);
			}
		}
		if (modelRenderComponent.LowPolyDistance >= 0.0f && 
			(modelRenderComponent.LowPolyDistance < distanceFromCamera + modelRenderComponent.LodTransitionDistance || CurrentRenderSettings->LevelOfDetailsMode == Manual))
		{
			LodTransitionBufferInstance.Data.PercentageCoverage = coverageLod;
			LodTransitionBufferInstance.Data.InvertedCoverage = true;
			LodTransitionBufferInstance.ApplyChanges();

			for (const auto& mesh : *modelRenderComponent.LowPolyMeshes)
			{
				RenderMesh(mesh, transformComponent, mainCameraComponent, offset, fatPerObjectBufferSet, renderMode);
			}
		}
	});

	if (renderMode == Transparent)
		DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

	Profiler->EndEvent();
}

void RenderSystem::RenderMesh(const Mesh& mesh, TransformComponent &transformComponent, const CameraComponent &mainCameraComponent,
	UINT& offset, bool& fatPerObjectBufferSet, const TransparencyRenderType renderMode)
{
	if (renderMode == Solid && mesh.Material.Alpha < 1.0f)
		return;

	if (renderMode == Transparent && mesh.Material.Alpha >= 1.0f)
		return;

	if (!FrustumUtil::Test(FrustumUtil::RecalculateAABBForWorld(mesh.FrustumPoints, transformComponent.WorldMatrix), mainCameraComponent.FrustumPlanes))
		return;

	if (!fatPerObjectBufferSet)
	{
		fatPerObjectBufferSet = true;

		FatPerObjectBufferInstance.Data.WorldViewProjectionMat =
			XMMatrixTranspose(transformComponent.WorldMatrix * (mainCameraComponent.ViewMatrix * mainCameraComponent.ProjectionMatrix));
		FatPerObjectBufferInstance.Data.WorldMatrix = XMMatrixTranspose(transformComponent.WorldMatrix);
		for (auto i = 0; i < 4; i++)
			FatPerObjectBufferInstance.Data.LightSpaceMatrix[i] = XMMatrixTranspose(ShadowCameras[i].CalculateCameraMatrix());
		FatPerObjectBufferInstance.ApplyChanges();
	}

	LightAndAlphaBufferInstance.Data.Alpha = mesh.Material.Alpha;
	LightAndAlphaBufferInstance.ApplyChanges();

	DeviceContext->PSSetShaderResources(0, 1, mesh.Material.AlbedoTexture->GetAddressOf());
	DeviceContext->PSSetShaderResources(1, 1, mesh.Material.NormalTexture->GetAddressOf());
	DeviceContext->PSSetShaderResources(2, 1, mesh.Material.MetalicSmoothnessTexture->GetAddressOf());
	DeviceContext->PSSetShaderResources(3, 1, mesh.Material.OcclusionTexture->GetAddressOf());
	DeviceContext->PSSetShaderResources(4, 1, mesh.Material.EmissionTexture->GetAddressOf());

	Draw(mesh.RenderVertexBuffer, mesh.RenderIndexBuffer, offset);
}

void RenderSystem::SetShadowResourcesForShadowCascades(const int firstCascadeId)
{
	for (auto i = 0; i < ShadowRenderDepthStencils.size(); i++)
		DeviceContext->PSSetShaderResources(firstCascadeId + i, 1, ShadowRenderDepthStencils[i].GetAddressOfShaderResourceView());
}

void RenderSystem::ClearShadowResourcesForShadowCascades(const int firstCascadeId) const
{
	for (auto i = 0; i < ShadowRenderDepthStencils.size(); i++)
		DeviceContext->PSSetShaderResources(firstCascadeId + i, 1, &NullShaderResourceView);
}

void RenderSystem::SetPbrResources()
{
	DeviceContext->PSSetShaderResources(18, 1, PbrResourceInstance.GetAddressOfIrradianceResourceTexture());
	DeviceContext->PSSetShaderResources(19, 1, PbrResourceInstance.GetAddressOfRadianceResourceTexture());
	DeviceContext->PSSetShaderResources(20, 1, PbrResourceInstance.GetAddressOfBrdfLutResourceTexture());
}

void RenderSystem::ConfigureCascadeConstantBuffer()
{
	CascadeLevelsBufferInstance.Data.CascadeEnds[0] = ConfigurationData->MainCameraFarZ * ConfigurationData->CascadeEnd0;
	CascadeLevelsBufferInstance.Data.CascadeEnds[1] = ConfigurationData->MainCameraFarZ * ConfigurationData->CascadeEnd1;
	CascadeLevelsBufferInstance.Data.CascadeEnds[2] = ConfigurationData->MainCameraFarZ * ConfigurationData->CascadeEnd2;
	CascadeLevelsBufferInstance.Data.CascadeEnds[3] = ConfigurationData->MainCameraFarZ * ConfigurationData->CascadeEnd3;
	CascadeLevelsBufferInstance.Data.Biases[0] = ConfigurationData->CascadeBias0;
	CascadeLevelsBufferInstance.Data.Biases[1] = ConfigurationData->CascadeBias1;
	CascadeLevelsBufferInstance.Data.Biases[2] = ConfigurationData->CascadeBias2;
	CascadeLevelsBufferInstance.Data.Biases[3] = ConfigurationData->CascadeBias3;
	CascadeLevelsBufferInstance.ApplyChanges();
}

void RenderSystem::UpdateTimeBuffer()
{
	TimeBufferInstance.Data.Time = MainTimer.GetDelta();
	TimeBufferInstance.ApplyChanges();
}

void RenderSystem::UpdateFogBuffer()
{
	FogBufferInstance.Data.FogColor = CurrentRenderSettings->FogColor;
	FogBufferInstance.Data.FogDensity = CurrentRenderSettings->FogDensity;
	FogBufferInstance.Data.FogLightColor = CurrentRenderSettings->FogLightColor;
	FogBufferInstance.Data.FogLightPower = CurrentRenderSettings->FogLightPower;
	FogBufferInstance.Data.FogMinDistance = CurrentRenderSettings->FogMinDistance;
	FogBufferInstance.Data.SkyConstantValue = CurrentRenderSettings->SkyConstantValue;
	FogBufferInstance.ApplyChanges();
}

void RenderSystem::UpdateLightAndAlphaBuffer()
{
	LightAndAlphaBufferInstance.Data.DirectionalLightColor = CurrentRenderSettings->DirectionalLightColor;
	LightAndAlphaBufferInstance.Data.DirectionalLightStrength = CurrentRenderSettings->DirectionalLightStrength;
	LightAndAlphaBufferInstance.Data.DirectionalLightDirection = CurrentRenderSettings->DirectionalLightDirection;
	LightAndAlphaBufferInstance.ApplyChanges();
}

void RenderSystem::UpdateCameraBuffer(const TransformComponent &mainCameraTransformComponent)
{
	CameraBufferInstance.Data.CameraPosition = TransformLogic::GetPosition(mainCameraTransformComponent);
	CameraBufferInstance.ApplyChanges();
}

void RenderSystem::PerformPostProcessing(const bool minimal) const
{
	Profiler->BeginEvent("PostProcessing");

	DeviceContext->RSSetState(SolidRasterizerState.Get());

	auto currentInput = IntermediateRenderTexture.GetShaderResourceView();

	Profiler->BeginEvent("Multisampling");
	if (ConfigurationData->MultisamplesCount > 1)
		currentInput = MultisamplingPostProcessingInstance->Process(currentInput.GetAddressOf());
	Profiler->EndEvent();

	if (!minimal)
	{
		for (auto& postProcessingEffect : CurrentPostProcessingSettings->List)
		{
			Profiler->BeginEvent(postProcessingEffect->GetName());
			currentInput = postProcessingEffect->Process(currentInput.GetAddressOf());
			Profiler->EndEvent();
		}
	}

	Profiler->BeginEvent("Copy To Back Buffer");
	CopyToBackBufferPostProcessingInstance->Process(currentInput.GetAddressOf());
	Profiler->EndEvent();

	Profiler->EndEvent();
}

float RenderSystem::CalculateDistanceFromCamera(const TransformComponent& transformComponent, const TransformComponent& cameraTransformComponent) const
{
	const auto objectPosition = TransformLogic::GetPosition(transformComponent);
	const auto cameraPosition = TransformLogic::GetPosition(cameraTransformComponent);

	const auto objectPositionVector = XMLoadFloat3(&objectPosition);
	const auto cameraPositionVector = XMLoadFloat3(&cameraPosition);

	const auto vectorDiff = DirectX::XMVectorSubtract(objectPositionVector, cameraPositionVector);

	return DirectX::XMVectorGetX(DirectX::XMVector3Length(vectorDiff));
}

float RenderSystem::CalculateLodCoverage(ModelRenderComponent& modelRenderComponent, const float distanceFromCamera) const
{
	if (CurrentRenderSettings->LevelOfDetailsMode == Manual)
		return CurrentRenderSettings->ForcedLodPercentage;

	if (modelRenderComponent.LowPolyDistance < 0.0f)
		return 1.0f;

	return std::clamp((modelRenderComponent.LowPolyDistance - distanceFromCamera) / modelRenderComponent.LodTransitionDistance, 0.0f, 1.0f);
}
