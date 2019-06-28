#include "Renderer.h"
#include "../Utils/Logger.h"
#include "AdapterReader.h"

bool Renderer::Initialize(const HWND hwnd, const int width, const int height, const int selectedAdapterId)
{
	Logger::Debug("Renderer initialization...");
	return InitializeDirectX(hwnd, width, height, selectedAdapterId);
}

void Renderer::RenderFrame()
{
	float bgColor[] = { 0.0f, 0.75f, 1.0f };
	DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), bgColor);
	SwapChain->Present(TRUE, 0); // Add vsync later
}

bool Renderer::InitializeDirectX(const HWND hwnd, const int width, const int height, const int selectedAdapterId)
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
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hwnd;
	scd.Windowed = TRUE;
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
	Logger::Debug("DirectX initialized successfully.");

	return true;
}
