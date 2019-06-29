#include "Renderer.h"
#include "../Utils/Logger.h"
#include "AdapterReader.h"

bool Renderer::Initialize(const HWND hwnd, const int width, const int height, const int fullscreen, const int syncIntervals, const int selectedAdapterId,
	const int refreshRateNumerator, const int refreshRateDenominator, const int multisamplesCount, const int multisamplesQuality)
{
	Logger::Debug("Renderer initialization...");
	SyncIntervals = syncIntervals;
	if (!InitializeDirectX(	hwnd, 
							width, 
							height, 
							fullscreen, 
							selectedAdapterId, 
							refreshRateNumerator, 
							refreshRateDenominator,
							multisamplesCount, 
							multisamplesQuality))
	{
		return false;
	}

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;

	return true;
}

void Renderer::RenderFrame() const
{
	float bgColor[] = { 0.0f, 0.75f, 1.0f };
	DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), bgColor);

	DeviceContext->IASetInputLayout(UberVertexShader.GetInputLayout());
	DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DeviceContext->RSSetState(RasterizerState.Get());

	DeviceContext->VSSetShader(UberVertexShader.GetShader(), nullptr, 0);
	DeviceContext->PSSetShader(UberPixelShader.GetShader(), nullptr, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);

	DeviceContext->Draw(3, 0);

	SwapChain->Present(SyncIntervals, 0);
}

bool Renderer::InitializeDirectX(const HWND hwnd, const int width, const int height, const int fullscreen, const int selectedAdapterId,
	const int refreshRateNumerator, const int refreshRateDenominator, const int multisamplesCount, const int multisamplesQuality)
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

	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
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
	scd.Windowed = -fullscreen+1;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	auto hr = D3D11CreateDeviceAndSwapChain(adapters[selectedAdapterId].Adapter,
											D3D_DRIVER_TYPE_UNKNOWN,
											nullptr,
											0,
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

	DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), nullptr);
	Logger::Debug("Binding render target output merge successfully.");

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);

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
	Logger::Debug("Rasterizer state is set successfully.");

	Logger::Debug("DirectX initialized successfully.");

	return true;
}

bool Renderer::InitializeShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	if(!UberVertexShader.Initialize(Device, L"uber_vertex_shader.cso", layout, ARRAYSIZE(layout)))
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

bool Renderer::InitializeScene()
{
	Logger::Debug("Preparing to initialize scene...");

	Vertex v[] = 
	{
		Vertex(0.0f, 0.5f, 1.0f, 0.0f, 0.0f),
		Vertex(0.5f, -0.5f, 0.0f, 1.0f, 0.0f),
		Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f),
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;

	const auto hr = Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, VertexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Failed to create vertex buffer.");
		return false;
	}

	Logger::Debug("Scene initialization succeeded...");
	return true;
}
