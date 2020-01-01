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
#include "../../Renderer/ModelViewLogic.h"

void RenderSystem::Start()
{
	Logger::Debug("Staring render system...");

	SyncIntervals = ConfigurationData->SyncIntervals;
	WindowWidth = ConfigurationData->WindowWidth;
	WindowHeight = ConfigurationData->WindowHeight;

	InitializeLightSettings();
	InitializeClearColorSettings();

	if (!InitializeDirectX())
		Logger::Error("DirectX initialization failed!");

	if (!InitializeShaders())
		Logger::Error("Shaders initialization failed!");

	InitializeConstantBuffers();
	InitializePostProcessing();

	if (!ConfigurationData->DisableLoadingScreen)
		ShowLoadingScreen();

	ResourcesLoader::Load(Device.Get(), DeviceContext.Get(), ConfigurationData);

	InitializeSkyboxComponent();
	InitializeModelRenderComponent();

	if (!InitializePbrResources())
		Logger::Error("PBR resources initialization failed!");
}

void RenderSystem::Update(const float deltaTime)
{
	auto& mainCameraComponent = GetMainCamera();

	if (ConfigurationData->ShadowsEnabled)
		RenderShadows();

	RenderScene(mainCameraComponent);

	PerformPostProcessing();
}

void RenderSystem::PrepareForGuiRender()
{
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

ID3D11ShaderResourceView* RenderSystem::GetPointerToRemappedShadowShaderResourceView(const float bias) const
{
	DeviceContext->RSSetViewports(1, &ShadowViewport);

	ShadowRemapperPostProcessingInstance->UpdateBias(bias);
	const auto outputTexture = ShadowRemapperPostProcessingInstance->Process(ShadowRenderDepthStencil.GetAddressOfShaderResourceView());

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

	// Depth scene render stencil initialization

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

	// Depth shadow render stencil initialization

	if (!ShadowRenderDepthStencil.Initialize(	Device.Get(),
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

	Logger::Debug("Successfully created shadow render depth stencil.");

	DeviceContext->ClearDepthStencilView(ShadowRenderDepthStencil.GetDepthStencilViewPointer(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	Logger::Debug("Initialial clearing shadow render depth stencil...");

	// Scene viewport initialization

	SceneViewport.TopLeftX = 0;
	SceneViewport.TopLeftY = 0;
	SceneViewport.Width = static_cast<float>(WindowWidth);
	SceneViewport.Height = static_cast<float>(WindowHeight);
	SceneViewport.MinDepth = 0.0f;
	SceneViewport.MaxDepth = 1.0f;

	Logger::Debug("Scene viewport is created successfully.");

	// Shadow viewport initialization

	ShadowViewport.TopLeftX = 0;
	ShadowViewport.TopLeftY = 0;
	ShadowViewport.Width = static_cast<float>(ConfigurationData->ShadowsTextureSize);
	ShadowViewport.Height = static_cast<float>(ConfigurationData->ShadowsTextureSize);
	ShadowViewport.MinDepth = 0.0f;
	ShadowViewport.MaxDepth = 1.0f;

	Logger::Debug("Shadow viewport is created successfully.");

	DeviceContext->RSSetViewports(1, &SceneViewport);
	Logger::Debug("Default viewport has been set.");

	// Rasterization initialization

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

	hr = Device->CreateRasterizerState(&rasterizerDesc, RasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating rasterizer state!");
		return false;
	}
	DeviceContext->RSSetState(RasterizerState.Get());
	Logger::Debug("Rasterizer state is set successfully.");

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

	// Shadow shader

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

	ShadowRemapperPostProcessingInstance = std::make_unique<ShadowRemapperPostProcessing>(	DeviceContext.Get(),
																							Device.Get(), 
																							ConfigurationData->ShadowsTextureSize,
																							ConfigurationData->ShadowsTextureSize,
																							DXGI_FORMAT_R32G32B32A32_FLOAT,
																							ConfigurationData->MainCameraNearZ, 
																							ConfigurationData->MainCameraFarZ,
																							2.0f);
}

void RenderSystem::ShowLoadingScreen()
{
	Logger::Debug("Preparing loading screen...");

	RenderFrameBegin();
	DeviceContext->ClearRenderTargetView(IntermediateRenderTexture.GetRenderTargetViewPointer(), BACKGROUND_COLOR);

	ChangeShader(LoadingLogoVertexShader, LoadingLogoPixelShader);

	const auto ratio = static_cast<float>(ConfigurationData->WindowWidth) / static_cast<float>(ConfigurationData->WindowHeight);
	Logger::Debug("Screen ratio: " + std::to_string(ratio) + ".");

	const auto logoScale = DirectX::XMMatrixScaling(ConfigurationData->LogoScale, 
		ConfigurationData->LogoScale, ConfigurationData->LogoScale);

	SimplePerObjectBufferInstance.Data.WorldViewProjectionMat =
		XMMatrixTranspose(logoScale * ModelViewLogic::GenerateOrthographicProjectionMatrix(ratio));
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

	PerformPostProcessing();
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

void RenderSystem::InitializeModelRenderComponent()
{
	Registry->view<ModelRenderComponent>().each([this](ModelRenderComponent &modelRenderComponent)
	{
		modelRenderComponent.Meshes = ModelLoader::GetResource(modelRenderComponent.ModelId);
		MaterialLoader::SetResourceForMeshGroup(Device.Get(), *modelRenderComponent.Meshes, modelRenderComponent.MaterialId);
	});
}

void RenderSystem::ChangeShader(const VertexShader& vertexShader, const PixelShader& pixelShader) const
{
	DeviceContext->IASetInputLayout(vertexShader.GetInputLayout());
	DeviceContext->VSSetShader(vertexShader.GetShader(), nullptr, 0);
	DeviceContext->PSSetShader(pixelShader.GetShader(), nullptr, 0);
}

CameraComponent& RenderSystem::GetMainCamera() const
{
	auto view = Registry->view<CameraComponent, entt::tag<Tags::MAIN_CAMERA>>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one main render camera found!");
		else
			Logger::Error("Main render camera not found!");
	}

	return view.raw<CameraComponent>()[0];
}

TransformComponent& RenderSystem::GetMainCameraTransform() const
{
	auto view = Registry->view<CameraComponent, TransformComponent, entt::tag<Tags::MAIN_CAMERA>>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one main render camera found!");
		else
			Logger::Error("Main render camera not found!");
	}

	return view.raw<TransformComponent>()[0];
}

void RenderSystem::RenderShadows()
{
	DeviceContext->RSSetViewports(1, &ShadowViewport);
	DeviceContext->OMSetRenderTargets(0, nullptr, ShadowRenderDepthStencil.GetDepthStencilViewPointer());
	DeviceContext->ClearDepthStencilView(ShadowRenderDepthStencil.GetDepthStencilViewPointer(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ShadowCameraInstance.UpdateLightDirection(	CurrentRenderSettings->DirectionalLightDirection.x,
												CurrentRenderSettings->DirectionalLightDirection.y,
												CurrentRenderSettings->DirectionalLightDirection.z);
	ShadowCameraInstance.UpdateShadowMapLocation();

	RenderSceneForShadows();
}

void RenderSystem::RenderScene(const CameraComponent &cameraComponent)
{
	DeviceContext->RSSetViewports(1, &SceneViewport);
	DeviceContext->OMSetRenderTargets(1, IntermediateRenderTexture.GetAddressOfRenderTargetView(), SceneRenderDepthStencil.GetDepthStencilViewPointer());

	const auto lightSpaceMatrix = XMMatrixTranspose(ShadowCameraInstance.CalculateCameraMatrix());

	RenderSkyBoxComponents(cameraComponent);
	RenderModelRenderComponents(cameraComponent, lightSpaceMatrix);
}

void RenderSystem::RenderSceneForShadows()
{
	UINT offset = 0;
	ChangeShader(ShadowVertexShader, ShadowPixelShader);

	DeviceContext->VSSetConstantBuffers(0, 1, ShadowBufferInstance.GetAddressOf());

	Registry->view<ModelRenderComponent>().each([this, &offset](ModelRenderComponent &modelRenderComponent)
	{
		ShadowBufferInstance.Data.WorldLightMatrix = DirectX::XMMatrixTranspose(modelRenderComponent.WorldMatrix * ShadowCameraInstance.CalculateCameraMatrix());
		ShadowBufferInstance.ApplyChanges();

		for (const auto& mesh : *modelRenderComponent.Meshes)
		{
			if (mesh.Material.Alpha < ConfigurationData->ShadowAlphaThreshold)
				continue;

			Draw(mesh.RenderVertexBuffer, mesh.RenderIndexBuffer, offset);
		}
	});
}

void RenderSystem::RenderSkyBoxComponents(const CameraComponent& cameraComponent)
{
	UINT offset = 0;
	ChangeShader(SkyVertexShader, SkyPixelShader);

	Registry->view<SkyboxComponent>().each([this, &cameraComponent, &offset](SkyboxComponent &skyboxComponent)
	{
		SimplePerObjectBufferInstance.Data.WorldViewProjectionMat =
			XMMatrixTranspose(skyboxComponent.WorldMatrix * (cameraComponent.ViewMatrix * cameraComponent.ProjectionMatrix));
		SimplePerObjectBufferInstance.ApplyChanges();

		DeviceContext->VSSetConstantBuffers(0, 1, SimplePerObjectBufferInstance.GetAddressOf());
		DeviceContext->PSSetShaderResources(0, 1, skyboxComponent.Material.SkyMap->GetAddressOf());

		Draw(skyboxComponent.RenderVertexBuffer, skyboxComponent.RenderIndexBuffer, offset);
	});
}

void RenderSystem::RenderModelRenderComponents(const CameraComponent& cameraComponent, const DirectX::XMMATRIX& lightSpaceMatrix)
{
	auto& mainCameraTransform = GetMainCameraTransform();

	UINT offset = 0;
	ChangeShader(UberVertexShader, UberPixelShader);

	DeviceContext->PSSetShaderResources(8, 1, ShadowRenderDepthStencil.GetAddressOfShaderResourceView());

	Registry->view<ModelRenderComponent>().each([this, &cameraComponent, &offset, &mainCameraTransform, &lightSpaceMatrix](ModelRenderComponent &modelRenderComponent)
	{
		FatPerObjectBufferInstance.Data.WorldViewProjectionMat =
			XMMatrixTranspose(modelRenderComponent.WorldMatrix * (cameraComponent.ViewMatrix * cameraComponent.ProjectionMatrix));
		FatPerObjectBufferInstance.Data.WorldMatrix = XMMatrixTranspose(modelRenderComponent.WorldMatrix);
		FatPerObjectBufferInstance.Data.LightSpaceMatrix = lightSpaceMatrix;
		FatPerObjectBufferInstance.Data.InvertedWorldMatrix = XMMatrixInverse(nullptr, modelRenderComponent.WorldMatrix);
		FatPerObjectBufferInstance.ApplyChanges();

		DeviceContext->VSSetConstantBuffers(0, 1, FatPerObjectBufferInstance.GetAddressOf());

		LightAndAlphaBufferInstance.Data.DirectionalLightColor = CurrentRenderSettings->DirectionalLightColor;
		LightAndAlphaBufferInstance.Data.DirectionalLightStrength = CurrentRenderSettings->DirectionalLightStrength;
		LightAndAlphaBufferInstance.Data.DirectionalLightDirection = CurrentRenderSettings->DirectionalLightDirection;
		LightAndAlphaBufferInstance.Data.Alpha = 1.0f;
		LightAndAlphaBufferInstance.ApplyChanges();

		DeviceContext->PSSetConstantBuffers(0, 1, LightAndAlphaBufferInstance.GetAddressOf());

		CameraBufferInstance.Data.CameraPosition = TransformLogic::GetPosition(mainCameraTransform);
		CameraBufferInstance.ApplyChanges();

		DeviceContext->PSSetConstantBuffers(1, 1, CameraBufferInstance.GetAddressOf());

		for (const auto& mesh : *modelRenderComponent.Meshes)
		{
			if (mesh.Material.Alpha < 1.0f)
				continue;

			DeviceContext->PSSetShaderResources(0, 1, mesh.Material.AlbedoTexture->GetAddressOf());
			DeviceContext->PSSetShaderResources(1, 1, mesh.Material.NormalTexture->GetAddressOf());
			DeviceContext->PSSetShaderResources(2, 1, mesh.Material.MetalicSmoothnessTexture->GetAddressOf());
			DeviceContext->PSSetShaderResources(3, 1, mesh.Material.OcclusionTexture->GetAddressOf());
			DeviceContext->PSSetShaderResources(4, 1, mesh.Material.EmissionTexture->GetAddressOf());
			DeviceContext->PSSetShaderResources(5, 1, PbrResourceInstance.GetAddressOfIrradianceResourceTexture());
			DeviceContext->PSSetShaderResources(6, 1, PbrResourceInstance.GetAddressOfRadianceResourceTexture());
			DeviceContext->PSSetShaderResources(7, 1, PbrResourceInstance.GetAddressOfBrdfLutResourceTexture());

			Draw(mesh.RenderVertexBuffer, mesh.RenderIndexBuffer, offset);
		}

		DeviceContext->OMSetBlendState(BlendState.Get(), nullptr, 0xffffffff);

		for (const auto& mesh : *modelRenderComponent.Meshes)
		{
			if (mesh.Material.Alpha >= 1.0f)
				continue;

			LightAndAlphaBufferInstance.Data.Alpha = mesh.Material.Alpha;
			LightAndAlphaBufferInstance.ApplyChanges();
			DeviceContext->PSSetConstantBuffers(0, 1, LightAndAlphaBufferInstance.GetAddressOf());

			DeviceContext->PSSetShaderResources(0, 1, mesh.Material.AlbedoTexture->GetAddressOf());
			DeviceContext->PSSetShaderResources(1, 1, mesh.Material.NormalTexture->GetAddressOf());
			DeviceContext->PSSetShaderResources(2, 1, mesh.Material.MetalicSmoothnessTexture->GetAddressOf());
			DeviceContext->PSSetShaderResources(3, 1, mesh.Material.OcclusionTexture->GetAddressOf());
			DeviceContext->PSSetShaderResources(4, 1, mesh.Material.EmissionTexture->GetAddressOf());
			DeviceContext->PSSetShaderResources(5, 1, PbrResourceInstance.GetAddressOfIrradianceResourceTexture());
			DeviceContext->PSSetShaderResources(6, 1, PbrResourceInstance.GetAddressOfRadianceResourceTexture());
			DeviceContext->PSSetShaderResources(7, 1, PbrResourceInstance.GetAddressOfBrdfLutResourceTexture());

			Draw(mesh.RenderVertexBuffer, mesh.RenderIndexBuffer, offset);
		}

		DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	});

	DeviceContext->PSSetShaderResources(8, 1, &NullShaderResourceView);
}

void RenderSystem::PerformPostProcessing() const
{
	auto currentInput = IntermediateRenderTexture.GetShaderResourceView();

	if (ConfigurationData->MultisamplesCount > 1)
		currentInput = MultisamplingPostProcessingInstance->Process(currentInput.GetAddressOf());

	for (auto& postProcessingEffect : CurrentPostProcessingSettings->List)
		currentInput = postProcessingEffect->Process(currentInput.GetAddressOf());

	CopyToBackBufferPostProcessingInstance->Process(currentInput.GetAddressOf());
}