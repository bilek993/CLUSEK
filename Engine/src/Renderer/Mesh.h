#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Materials/UberShaderMaterial.h"
#include "Vertex/FatVertex.h"

struct Mesh final
{
	std::string Name;
	VertexBuffer<FatVertex> RenderVertexBuffer;
	IndexBuffer RenderIndexBuffer;
	UberShaderMaterial Material;
};
