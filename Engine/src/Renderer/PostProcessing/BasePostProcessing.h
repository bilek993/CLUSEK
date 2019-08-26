#pragma once
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Vertex/PositionVertex.h"
#include "../Shaders/VertexShader.h"
#include "../Shaders/PixelShader.h"

class BasePostProcessing
{
public:
	BasePostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device);
protected:
	template <class T>
	void Draw(const VertexBuffer<T>& vertexBuffer, const IndexBuffer& indexBuffer, UINT& offset, int usedShaderResourceViews) const;
	void ChangeShader(const VertexShader& vertexShader, const PixelShader& pixelShader) const;

	void InitializeOutputTextureMap(int windowWidth, int windowHeight, ID3D11Texture2D** renderTexture) const;
	void InitializeOutputRenderTargetView(DXGI_FORMAT renderTargetFormat, ID3D11Texture2D* renderTexture, ID3D11RenderTargetView** renderTargetView) const;
	void InitializeShaderResourceView(DXGI_FORMAT renderTargetFormat, ID3D11Texture2D* renderTexture, ID3D11ShaderResourceView** shaderResourceView) const;

	ID3D11DeviceContext* DeviceContext = nullptr;
	ID3D11Device* Device = nullptr;

	VertexBuffer<PositionVertex> VertexBufferInstance;
	IndexBuffer IndexBufferInstance;
private:
	// Fix for hazard problems (https://gamedev.stackexchange.com/questions/95591/directx-rendertargettexture-bound-to-both-rtv-and-srv)
	void UnbindShaderResourceViews(int usedShaderResourceViews) const;
};

template <class T>
void BasePostProcessing::Draw(const VertexBuffer<T>& vertexBuffer, const IndexBuffer& indexBuffer, UINT& offset, const int usedShaderResourceViews) const
{
	DeviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->DrawIndexed(indexBuffer.GetIndexCount(), 0, 0);

	UnbindShaderResourceViews(usedShaderResourceViews);
}
