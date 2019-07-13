#include "RenderSystem.h"
#include "../Components/RenderComponent.h"
#include <WICTextureLoader.h>
#include "../Components/CameraComponent.h"
#include "../../Renderer/AdapterReader.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

void RenderSystem::Start(entt::registry& registry, const HWND &hwnd, const ConfigData& configData)
{
	SyncIntervals = configData.SyncIntervals;
	WindowWidth = configData.WindowWidth;
	WindowHeight = configData.WindowHeight;

	if (!InitializeDirectX(hwnd,
		configData.Fullscreen,
		configData.SelectedAdapterId,
		configData.RefreshRateNumerator,
		configData.RefreshRateDenominator,
		configData.MultisamplesCount,
		configData.MultisamplesQuality))
	{
		Logger::Error("DirectX initialization failed");
	}

	if (!InitializeShaders())
		Logger::Error("Shaders initialization failed");

	InitializeImGui(hwnd);

	registry.view<RenderComponent>().each([this](RenderComponent &renderComponent)
	{
		Vertex v[] =
		{
			Vertex(-0.5f,  -0.5f, -0.5f, 0.0f, 1.0f),
			Vertex(-0.5f,   0.5f, -0.5f, 0.0f, 0.0f),
			Vertex(0.5f,   0.5f, -0.5f, 1.0f, 0.0f),
			Vertex(0.5f,  -0.5f, -0.5f, 1.0f, 1.0f),
			Vertex(-0.5f,  -0.5f, 0.5f, 0.0f, 1.0f),
			Vertex(-0.5f,   0.5f, 0.5f, 0.0f, 0.0f),
			Vertex(0.5f,   0.5f, 0.5f, 1.0f, 0.0f),
			Vertex(0.5f,  -0.5f, 0.5f, 1.0f, 1.0f),
		};

		DWORD indices[] =
		{
			0, 1, 2,
			0, 2, 3,
			4, 7, 6,
			4, 6, 5,
			3, 2, 6,
			3, 6, 7,
			4, 5, 1,
			4, 1, 0,
			1, 5, 6,
			1, 6, 2,
			0, 3, 7,
			0, 7, 4,
		};

		auto hr = renderComponent.RenderVertexBuffer.Initialize(Device.Get(), v, ARRAYSIZE(v));
		if (FAILED(hr))
			Logger::Error("Failed to create vertex buffer.");

		hr = renderComponent.RenderIndexBuffer.Initialize(Device.Get(), indices, ARRAYSIZE(indices));
		if (FAILED(hr))
			Logger::Error("Failed to create index buffer.");

		hr = DirectX::CreateWICTextureFromFile(Device.Get(), L"Data/Textures/example_texture.png", nullptr, renderComponent.TextureResourceView.GetAddressOf());
		if (FAILED(hr))
			Logger::Error("Couldn't load example texture from file!");

		hr = renderComponent.UberShaderConstantBuffer.Initialize(Device.Get(), DeviceContext.Get());
		if (FAILED(hr))
			Logger::Error("Failed to create constant buffer.");
	});
}

void RenderSystem::Update(float deltaTime, entt::registry& registry, IOData& ioData, IODevices& ioDevices, RenderWindow& window, ConfigData& configData)
{
	auto view = registry.view<CameraComponent>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one main render camera found!");
		else
			Logger::Error("Main render camera not found!");
	}

	auto &cameraComponent = view.raw()[0];

	RenderFrameBegin();
	registry.view<RenderComponent>().each([this, &cameraComponent](RenderComponent &renderComponent)
	{
		renderComponent.UberShaderConstantBuffer.Data.MatModelViewProjection = renderComponent.ModelMatrix * (cameraComponent.ViewMatrix * cameraComponent.ProjectionMatrix);
		renderComponent.UberShaderConstantBuffer.Data.MatModelViewProjection = DirectX::XMMatrixTranspose(renderComponent.UberShaderConstantBuffer.Data.MatModelViewProjection);
		renderComponent.UberShaderConstantBuffer.ApplyChanges();

		DeviceContext->VSSetConstantBuffers(0, 1, renderComponent.UberShaderConstantBuffer.GetAddressOf());

		UINT offset = 0;

		DeviceContext->PSSetShaderResources(0, 1, renderComponent.TextureResourceView.GetAddressOf());
		DeviceContext->IASetVertexBuffers(0, 1, renderComponent.RenderVertexBuffer.GetAddressOf(), renderComponent.RenderVertexBuffer.StridePtr(), &offset);
		DeviceContext->IASetIndexBuffer(renderComponent.RenderIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		DeviceContext->DrawIndexed(renderComponent.RenderIndexBuffer.GetBufferSize(), 0, 0);
	});
	RenderFrameEnd();
}

RenderSystem::~RenderSystem()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool RenderSystem::InitializeDirectX(HWND hwnd, int fullscreen, int selectedAdapterId, int refreshRateNumerator,
	int refreshRateDenominator, int multisamplesCount, int multisamplesQuality)
{
	auto adapters = AdapterReader::GetData();

	if (adapters.empty())
		Logger::Error("No adapters found!");

	if (selectedAdapterId != 0)
		Logger::Warning("Selected adapter with not [0] id. This might not be optimal choice.");

	if (selectedAdapterId >= adapters.size())
		Logger::Error("Adapter with id [" + std::to_string(selectedAdapterId) + "] not found!");

	Logger::Debug("Selected adapter [" + std::to_string(selectedAdapterId) + "].");

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SAMPLE_DESC));

	scd.BufferDesc.Width = WindowWidth;
	scd.BufferDesc.Height = WindowHeight;
	scd.BufferDesc.RefreshRate.Numerator = refreshRateNumerator;
	scd.BufferDesc.RefreshRate.Denominator = refreshRateDenominator;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = multisamplesCount;
	scd.SampleDesc.Quality = multisamplesQuality;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hwnd;
	scd.Windowed = -fullscreen + 1;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

#ifdef _DEBUG
	auto softwareLayers = D3D11_CREATE_DEVICE_DEBUG;
	Logger::Warning("D3D11_DEVICE in DEVICE_DEBUG mode! Rendering will be much slower!");
#else
	auto softwareLayers = 0;
#endif

	auto hr = D3D11CreateDeviceAndSwapChain(adapters[selectedAdapterId].Adapter,
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
	depthStencilTextureDesc.SampleDesc.Count = multisamplesCount;
	depthStencilTextureDesc.SampleDesc.Quality = multisamplesQuality;
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

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;

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
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
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

void RenderSystem::InitializeImGui(HWND hwnd) const
{
	Logger::Debug("Preparing to initialize ImGui...");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(Device.Get(), DeviceContext.Get());
	ImGui::StyleColorsDark();
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

void RenderSystem::RenderFrameEnd()
{
	// START OF TMP CODE
	// Remove this as fast as possible after creating proper imgui system.
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Test");
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	// END OF TMP CODE

	SwapChain->Present(SyncIntervals, 0);
}
