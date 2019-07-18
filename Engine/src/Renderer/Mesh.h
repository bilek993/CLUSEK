#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Vertex.h"
#include "Materials/UberShaderMaterial.h"

struct Mesh final
{
	std::string Name;
	VertexBuffer<Vertex> RenderVertexBuffer;
	IndexBuffer RenderIndexBuffer;
	UberShaderMaterial Material;
};
