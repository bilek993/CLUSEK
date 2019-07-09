#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Shaders/VertexShader.h"
#include "Shaders/PixelShader.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBufferTypes.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

class Renderer final
{
public:
	bool Initialize(HWND hwnd, int width, int height, int fullscreen, int syncIntervals, int selectedAdapterId, 
		int refreshRateNumerator, int refreshRateDenominator, int multisamplesCount, int multisamplesQuality,
		float mainCameraFov, float mainCameraNearZ, float mainCameraFarZ);
	void RenderFrame();
	std::shared_ptr<Camera> GetPointerToCamera() const;
private:
	bool InitializeDirectX(HWND hwnd, int fullscreen, int selectedAdapterId, int refreshRateNumerator, 
		int refreshRateDenominator, int multisamplesCount, int multisamplesQuality, float mainCameraFov, 
		float mainCameraNearZ, float mainCameraFarZ);
	bool InitializeShaders();
	void InitializeImGui(HWND hwnd) const;
	bool InitializeScene();

	int WindowWidth = 0;
	int WindowHeight = 0;

	int SyncIntervals = 1;

	Microsoft::WRL::ComPtr<ID3D11Device> Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView;

	VertexShader UberVertexShader;
	PixelShader UberPixelShader;
	ConstantBuffer<CB_VS_UberVertexShader> UberShaderConstantBuffer;

	VertexBuffer<Vertex> TriangleVertexBuffer;
	IndexBuffer TriangleIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ExampleTexture;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;

	std::shared_ptr<Camera> MainCamera;
};
