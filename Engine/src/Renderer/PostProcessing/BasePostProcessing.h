#pragma once
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Vertex/PositionVertex.h"

class VertexShader;
class PixelShader;
class IndexBuffer;

class BasePostProcessing
{
public:
	BasePostProcessing(ID3D11DeviceContext* deviceContext, ID3D11Device* device);
protected:
	template <class T>
	void Draw(const VertexBuffer<T>& vertexBuffer, const IndexBuffer& indexBuffer, UINT& offset) const;
	void ChangeShader(const VertexShader& vertexShader, const PixelShader& pixelShader) const;

	ID3D11DeviceContext* DeviceContext = nullptr;
	ID3D11Device* Device = nullptr;

	VertexBuffer<PositionVertex> VertexBufferInstance;
	IndexBuffer IndexBufferInstance;
};

template <class T>
void BasePostProcessing::Draw(const VertexBuffer<T>& vertexBuffer, const IndexBuffer& indexBuffer, UINT& offset) const
{
	DeviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->DrawIndexed(indexBuffer.GetIndexCount(), 0, 0);
}
