#include "RenderSystem.h"
#include "../Components/RenderComponent.h"
#include "../Components/CameraComponent.h"
#include "../../Renderer/AdapterReader.h"
#include "../../Loaders/ModelLoader.h"
#include "../../Loaders/MaterialLoader.h"
#include "../../Loaders/ResourcesLoader.h"

void RenderSystem::Start()
{
	Logger::Debug("Staring render system...");

	SyncIntervals = ConfigurationData->SyncIntervals;
	WindowWidth = ConfigurationData->WindowWidth;
	WindowHeight = ConfigurationData->WindowHeight;

	InitializeLightSettings();

	if (!InitializeDirectX())
	{
		Logger::Error("DirectX initialization failed");
	}

	if (!InitializeShaders())
		Logger::Error("Shaders initialization failed");

	auto hr = UberShaderVertexShaderConstantBuffer.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create constant buffer.");

	hr = UberShaderPixelShaderLightConstantBuffer.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
		Logger::Error("Failed to create constant buffer.");

	ResourcesLoader::Load(Device.Get(), ConfigurationData->PathToResources);

	Registry->view<RenderComponent>().each([this](RenderComponent &renderComponent)
	{
		renderComponent.Meshes = ModelLoader::GetResource(renderComponent.ModelId);
		MaterialLoader::SetResourceForMeshGroup(Device.Get(), *renderComponent.Meshes, renderComponent.MaterialId);
	});
}

void RenderSystem::Update(const float deltaTime)
{
	auto view = Registry->view<CameraComponent>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one main render camera found!");
		else
			Logger::Error("Main render camera not found!");
	}

	auto &cameraComponent = view.raw()[0];

	Registry->view<RenderComponent>().each([this, &cameraComponent](RenderComponent &renderComponent)
	{
		UberShaderVertexShaderConstantBuffer.Data.WorldViewProjectionMat = 
			XMMatrixTranspose(renderComponent.WorldMatrix * (cameraComponent.ViewMatrix * cameraComponent.ProjectionMatrix));
		UberShaderVertexShaderConstantBuffer.Data.WorldMatrix = XMMatrixTranspose(renderComponent.WorldMatrix);
		UberShaderVertexShaderConstantBuffer.ApplyChanges();

		DeviceContext->VSSetConstantBuffers(0, 1, UberShaderVertexShaderConstantBuffer.GetAddressOf());

		UberShaderPixelShaderLightConstantBuffer.Data.AmbientLightColor = CurrentRenderSettings->AmbientLightColor;
		UberShaderPixelShaderLightConstantBuffer.Data.AmbientLightStrength = CurrentRenderSettings->AmbientLightStrength;
		UberShaderPixelShaderLightConstantBuffer.Data.DirectionalLightColor = CurrentRenderSettings->DirectionalLightColor;
		UberShaderPixelShaderLightConstantBuffer.Data.DirectionalLightStrength = CurrentRenderSettings->DirectionalLightStrength;
		UberShaderPixelShaderLightConstantBuffer.Data.DirectionalLightDirection = CurrentRenderSettings->DirectionalLightDirection;
		UberShaderPixelShaderLightConstantBuffer.ApplyChanges();

		DeviceContext->PSSetConstantBuffers(0, 1, UberShaderPixelShaderLightConstantBuffer.GetAddressOf());

		UINT offset = 0;

		for (const auto& mesh : *renderComponent.Meshes) 
		{
			DeviceContext->PSSetShaderResources(0, 1, mesh.Material.MainTexture->GetAddressOf());
			DeviceContext->IASetVertexBuffers(0, 1, mesh.RenderVertexBuffer.GetAddressOf(), mesh.RenderVertexBuffer.StridePtr(), &offset);
			DeviceContext->IASetIndexBuffer(mesh.RenderIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			DeviceContext->DrawIndexed(mesh.RenderIndexBuffer.GetIndexCount(), 0, 0);
		}
	});
}

void RenderSystem::RenderFrameBegin()
{
	float bgColor[] = { 0.0f, 0.75f, 1.0f };
	DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), bgColor);
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DeviceContext->IASetInputLayout(UberVertexShader.GetInputLayout());
	DeviceContext->OMSetDepthStencilState(DepthStencilState.Get(), 0);

	DeviceContext->PSSetSamplers(0, 1, SamplerState.GetAddressOf());
	DeviceContext->VSSetShader(UberVertexShader.GetShader(), nullptr, 0);
	DeviceContext->PSSetShader(UberPixelShader.GetShader(), nullptr, 0);
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

bool RenderSystem::InitializeDirectX()
{
	auto adapters = AdapterReader::GetData();

	if (adapters.empty())
		Logger::Error("No adapters found!");

	if (ConfigurationData->SelectedAdapterId != 0)
		Logger::Warning("Selected adapter with not [0] id. This might not be optimal choice.");

	if (ConfigurationData->SelectedAdapterId >= adapters.size())
		Logger::Error("Adapter with id [" + std::to_string(ConfigurationData->SelectedAdapterId) + "] not found!");

	Logger::Debug("Selected adapter [" + std::to_string(ConfigurationData->SelectedAdapterId) + "].");

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SAMPLE_DESC));

	scd.BufferDesc.Width = WindowWidth;
	scd.BufferDesc.Height = WindowHeight;
	scd.BufferDesc.RefreshRate.Numerator = ConfigurationData->RefreshRateNumerator;
	scd.BufferDesc.RefreshRate.Denominator = ConfigurationData->RefreshRateDenominator;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = ConfigurationData->MultisamplesCount;
	scd.SampleDesc.Quality = ConfigurationData->MultisamplesQuality;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = Window->GetHandle();
	scd.Windowed = -ConfigurationData->Fullscreen + 1;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

#ifdef _DEBUG
	auto softwareLayers = D3D11_CREATE_DEVICE_DEBUG;
	Logger::Warning("D3D11_DEVICE in DEVICE_DEBUG mode! Rendering will be much slower!");
#else
	auto softwareLayers = 0;
#endif

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

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		Logger::Error("Error getting back buffer!");
		return false;
	}

	hr = Device->CreateRenderTargetView(backBuffer.Get(), nullptr, RenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Error creating render target view!");
		return false;
	}

	D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
	depthStencilTextureDesc.Width = WindowWidth;
	depthStencilTextureDesc.Height = WindowHeight;
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.ArraySize = 1;
	depthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilTextureDesc.SampleDesc.Count = ConfigurationData->MultisamplesCount;
	depthStencilTextureDesc.SampleDesc.Quality = ConfigurationData->MultisamplesQuality;
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

	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
	Logger::Debug("Binding render target output merge successfully.");

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

	DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Logger::Debug("DirectX initialized successfully.");
	return true;
}

bool RenderSystem::InitializeShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

	};

	if (!UberVertexShader.Initialize(Device, L"uber_vertex_shader.cso", layout, ARRAYSIZE(layout)))
	{
		Logger::Error("UberVertexShader not initialized due to critical problem!");
		return false;
	}

	if (!UberPixelShader.Initialize(Device, L"uber_pixel_shader.cso"))
	{
		Logger::Error("UberPixelShader not initialized due to critical problem!");
		return false;
	}

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
