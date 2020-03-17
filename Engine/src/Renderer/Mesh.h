#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Materials/UberShaderMaterial.h"
#include "Vertex/FatVertex.h"
#include "array"

struct Mesh final
{
	std::string Name;
	std::array<DirectX::XMFLOAT3, 8> FrustumPoints;
	VertexBuffer<FatVertex> RenderVertexBuffer;
	IndexBuffer RenderIndexBuffer;
	UberShaderMaterial Material;
};
