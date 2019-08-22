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

void RenderSystem::Start()
{
	Logger::Debug("Staring render system...");

	SyncIntervals = ConfigurationData->SyncIntervals;
	WindowWidth = ConfigurationData->WindowWidth;
	WindowHeight = ConfigurationData->WindowHeight;

	InitializeLightSettings();
	InitializeClearColorSettings();

	if (!InitializeDirectX())
		Logger::Error("DirectX initialization failed");

	if (!InitializeShaders())
		Logger::Error("Shaders initialization failed");

	InitializeConstantBuffers();

	ResourcesLoader::Load(Device.Get(), ConfigurationData->PathToResources);

	// Skybox Component

	Registry->view<SkyboxComponent>().each([this](SkyboxComponent &skyboxComponent)
	{
		CubeGenerator::Generate(Device.Get(), skyboxComponent.RenderVertexBuffer, skyboxComponent.RenderIndexBuffer);
		MaterialLoader::SetResourceForManuallyForSkyMaterial(Device.Get(), skyboxComponent.Material, skyboxComponent.SkyboxTextureId);
	});

	// Model Render Component

	Registry->view<ModelRenderComponent>().each([this](ModelRenderComponent &modelRenderComponent)
	{
		modelRenderComponent.Meshes = ModelLoader::GetResource(modelRenderComponent.ModelId);
		MaterialLoader::SetResourceForMeshGroup(Device.Get(), *modelRenderComponent.Meshes, modelRenderComponent.MaterialId);
	});
}

void RenderSystem::Update(const float deltaTime)
{
	auto& mainCameraComponent = GetMainCamera();

	RenderSkyBoxComponents(mainCameraComponent);
	RenderModelRenderComponents(mainCameraComponent);
}

void RenderSystem::RenderFrameBegin() const
{
	DeviceContext->OMSetRenderTargets(1, IntermediateRenderTargetView.GetAddressOf(), DepthStencilView.Get());
	DeviceContext->ClearRenderTargetView(IntermediateRenderTargetView.Get(), CurrentRenderSettings->ClearColor);

	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DeviceContext->OMSetDepthStencilState(DepthStencilState.Get(), 0);

	DeviceContext->PSSetSamplers(0, 1, SamplerState.GetAddressOf());
}

void RenderSystem::RenderFrameEnd()
{
	PerformPostProcessing();
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

	// Intermediate texture map initialization

	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = WindowWidth;
	textureDesc.Height = WindowHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	hr = Device->CreateTexture2D(&textureDesc, nullptr, IntermediateRenderTexture.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating intermediate texture map!");
		return false;
	}

	// Intermediate render target view initialization

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	hr = Device->CreateRenderTargetView(IntermediateRenderTexture.Get(), &renderTargetViewDesc, IntermediateRenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating intermediate render target view!");
		return false;
	}

	// Intermediate shader resource view initialization

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	hr = Device->CreateShaderResourceView(IntermediateRenderTexture.Get(), &shaderResourceViewDesc, IntermediateShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating intermediate shader resource view!");
		return false;
	}

	// Depth stencil initialization

	D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
	depthStencilTextureDesc.Width = WindowWidth;
	depthStencilTextureDesc.Height = WindowHeight;
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.ArraySize = 1;
	depthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilTextureDesc.SampleDesc.Count = 1;
	depthStencilTextureDesc.SampleDesc.Quality = 0;
	depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilTextureDesc.CPUAccessFlags = 0;
	depthStencilTextureDesc.MiscFlags = 0;

	hr = Device->CreateTexture2D(&depthStencilTextureDesc, nullptr, DepthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating depth stencil buffer!");
		return false;
	}

	hr = Device->CreateDepthStencilView(DepthStencilBuffer.Get(), nullptr, DepthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating depth stencil view!");
		return false;
	}

	Logger::Debug("Successfully created depth stencil buffer and view.");

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = Device->CreateDepthStencilState(&depthStencilDesc, DepthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating depth stencil state!");
		return false;
	}

	Logger::Debug("Successfully created depth stencil state.");

	// Viewport initialization

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(WindowWidth);
	viewport.Height = static_cast<float>(WindowHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	DeviceContext->RSSetViewports(1, &viewport);
	Logger::Debug("Viewport is set successfully.");

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

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

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

	blendDesc.RenderTarget[0] = renderTargetBlendDesc;

	hr = Device->CreateBlendState(&blendDesc, BlendState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating blend state!");
		return false;
	}

	Logger::Debug("Blend state is set successfully.");

	// Sampler state initialization

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = Device->CreateSamplerState(&samplerDesc, SamplerState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating sampler state!");
		return false;
	}
	Logger::Debug("Sampler state is set successfully.");

	// Topology initialization

	DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Logger::Debug("DirectX initialized successfully.");
	return true;
}

bool RenderSystem::InitializeShaders()
{
	// Uber shader

	if (!UberVertexShader.Initialize(Device, L"uber_vertex_shader.cso", FatVertex::Layout, FatVertex::LayoutSize))
	{
		Logger::Error("UberVertexShader not initialized due to critical problem!");
		return false;
	}

	if (!UberPixelShader.Initialize(Device, L"uber_pixel_shader.cso"))
	{
		Logger::Error("UberPixelShader not initialized due to critical problem!");
		return false;
	}

	// Sky shader
	
	if (!SkyVertexShader.Initialize(Device, L"sky_vertex_shader.cso", PositionVertex::Layout, PositionVertex::LayoutSize))
	{
		Logger::Error("SkyVertexShader not initialized due to critical problem!");
		return false;
	}

	if (!SkyPixelShader.Initialize(Device, L"sky_pixel_shader.cso"))
	{
		Logger::Error("SkyPixelShader not initialized due to critical problem!");
		return false;
	}

	// Copy shader

	if (!CopyVertexShader.Initialize(Device, L"copy_vertex_shader.cso", PositionVertex::Layout, PositionVertex::LayoutSize))
	{
		Logger::Error("CopyVertexShader not initialized due to critical problem!");
		return false;
	}

	if (!CopyPixelShader.Initialize(Device, L"copy_pixel_shader.cso"))
	{
		Logger::Error("CopyPixelShader not initialized due to critical problem!");
		return false;
	}

	// Generic code

	Logger::Debug("All shaders successfully initialized.");
	return true;
}

void RenderSystem::InitializeLightSettings() const
{
	CurrentRenderSettings->AmbientLightColor =
		DirectX::XMFLOAT3(ConfigurationData->AmbientLightColorRed, ConfigurationData->AmbientLightColorGreen, ConfigurationData->AmbientLightColorBlue);
	CurrentRenderSettings->AmbientLightStrength = ConfigurationData->AmbientLightStrength;

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

	hr = SimplePerObjectBufferInstance.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create 'SimplePerObjectBufferInstance' constant buffer.");
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

void RenderSystem::RenderModelRenderComponents(const CameraComponent& cameraComponent)
{
	UINT offset = 0;
	ChangeShader(UberVertexShader, UberPixelShader);

	Registry->view<ModelRenderComponent>().each([this, &cameraComponent, &offset](ModelRenderComponent &modelRenderComponent)
	{
		FatPerObjectBufferInstance.Data.WorldViewProjectionMat =
			XMMatrixTranspose(modelRenderComponent.WorldMatrix * (cameraComponent.ViewMatrix * cameraComponent.ProjectionMatrix));
		FatPerObjectBufferInstance.Data.WorldMatrix = XMMatrixTranspose(modelRenderComponent.WorldMatrix);
		FatPerObjectBufferInstance.ApplyChanges();

		DeviceContext->VSSetConstantBuffers(0, 1, FatPerObjectBufferInstance.GetAddressOf());

		LightAndAlphaBufferInstance.Data.AmbientLightColor = CurrentRenderSettings->AmbientLightColor;
		LightAndAlphaBufferInstance.Data.AmbientLightStrength = CurrentRenderSettings->AmbientLightStrength;
		LightAndAlphaBufferInstance.Data.DirectionalLightColor = CurrentRenderSettings->DirectionalLightColor;
		LightAndAlphaBufferInstance.Data.DirectionalLightStrength = CurrentRenderSettings->DirectionalLightStrength;
		LightAndAlphaBufferInstance.Data.DirectionalLightDirection = CurrentRenderSettings->DirectionalLightDirection;
		LightAndAlphaBufferInstance.Data.Alpha = 1.0f;
		LightAndAlphaBufferInstance.ApplyChanges();

		DeviceContext->PSSetConstantBuffers(0, 1, LightAndAlphaBufferInstance.GetAddressOf());

		for (const auto& mesh : *modelRenderComponent.Meshes)
		{
			if (mesh.Material.Alpha < 1.0f)
				continue;

			DeviceContext->PSSetShaderResources(0, 1, mesh.Material.AlbedoTexture->GetAddressOf());
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
			Draw(mesh.RenderVertexBuffer, mesh.RenderIndexBuffer, offset);
		}

		DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	});
}

void RenderSystem::PerformPostProcessing()
{
	UINT offset = 0;

	DeviceContext->OMSetRenderTargets(1, BackBufferRenderTargetView.GetAddressOf(), DepthStencilView.Get());
	ChangeShader(CopyVertexShader, CopyPixelShader);

	VertexBuffer<PositionVertex> vertexBuffer;
	IndexBuffer indexBuffer;
	CubeGenerator::Generate(Device.Get(), vertexBuffer, indexBuffer);

	Draw(vertexBuffer, indexBuffer, offset);
}
