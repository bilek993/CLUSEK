#include "Renderer.h"
#include "../Utils/Logger.h"
#include "AdapterReader.h"
#include <WICTextureLoader.h>

bool Renderer::Initialize(const HWND hwnd, const int width, const int height, const int fullscreen, const int syncIntervals, const int selectedAdapterId,
	const int refreshRateNumerator, const int refreshRateDenominator, const int multisamplesCount, const int multisamplesQuality)
{
	Logger::Debug("Renderer initialization...");

	SyncIntervals = syncIntervals;
	WindowWidth = width;
	WindowHeight = height;

	if (!InitializeDirectX(	hwnd,
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

void Renderer::RenderFrame()
{
	float bgColor[] = { 0.0f, 0.75f, 1.0f };
	DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), bgColor);
	DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DeviceContext->IASetInputLayout(UberVertexShader.GetInputLayout());
	DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DeviceContext->RSSetState(RasterizerState.Get());
	DeviceContext->OMSetDepthStencilState(DepthStencilState.Get(), 0);
	
	DeviceContext->PSSetSamplers(0, 1, SamplerState.GetAddressOf());
	DeviceContext->VSSetShader(UberVertexShader.GetShader(), nullptr, 0);
	DeviceContext->PSSetShader(UberPixelShader.GetShader(), nullptr, 0);

	UINT offset = 0;

	auto worldMatrix = DirectX::XMMatrixIdentity();
	auto eyePosition = DirectX::XMVectorSet(0.0f, -4.0f, -2.0f, 0.0f);
	auto lookAtPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	auto upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	auto viewMatrix = DirectX::XMMatrixLookAtLH(eyePosition, lookAtPosition, upVector);
	auto fovDegrees = 90.0f;
	auto fovRadians = (fovDegrees / 360.0f) * DirectX::XM_2PI;
	auto aspectRatio = static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight);
	auto nearZ = 0.1f;
	auto farZ = 1000.0f;
	auto projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);

	UberShaderConstantBuffer.Data.Mat = worldMatrix * viewMatrix * projectionMatrix;
	UberShaderConstantBuffer.Data.Mat = DirectX::XMMatrixTranspose(UberShaderConstantBuffer.Data.Mat);
	if (!UberShaderConstantBuffer.ApplyChanges())
		return;

	DeviceContext->VSSetConstantBuffers(0, 1, UberShaderConstantBuffer.GetAddressOf());
	DeviceContext->PSSetShaderResources(0, 1, ExampleTexture.GetAddressOf());
	DeviceContext->IASetVertexBuffers(0, 1, TriangleVertexBuffer.GetAddressOf(), TriangleVertexBuffer.StridePtr(), &offset);
	DeviceContext->IASetIndexBuffer(TriangleIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	
	DeviceContext->DrawIndexed(6, 0, 0);

	SwapChain->Present(SyncIntervals, 0);
}

bool Renderer::InitializeDirectX(const HWND hwnd, const int fullscreen, const int selectedAdapterId,
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
	scd.Windowed = -fullscreen+1;
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

	Logger::Debug("DirectX initialized successfully.");
	return true;
}

bool Renderer::InitializeShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	const auto hr = DirectX::CreateWICTextureFromFile(Device.Get(), L"Data/Textures/example_texture.png", nullptr, ExampleTexture.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Couldn't load example texture from file!");
		return false;
	}

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
		Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f),
		Vertex(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f),
		Vertex(0.5f, 0.5f, 0.0f, 1.0f, 0.0f),
		Vertex(0.5f, -0.5f, 0.0f, 1.0f, 1.0f),
	};

	DWORD indices[] =
	{
		0, 1, 2,
		0, 2, 3,
	};
	
	auto hr = TriangleVertexBuffer.Initialize(Device.Get(), v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		Logger::Error("Failed to create vertex buffer.");
		return false;
	}

	hr = TriangleIndexBuffer.Initialize(Device.Get(), indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		Logger::Error("Failed to create index buffer.");
		return false;
	}

	hr = UberShaderConstantBuffer.Initialize(Device.Get(), DeviceContext.Get());
	if (FAILED(hr))
	{
		Logger::Error("Failed to create constant buffer.");
		return false;
	}

	Logger::Debug("Scene initialization succeeded...");
	return true;
}
