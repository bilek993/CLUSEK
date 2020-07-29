#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Materials/GrassShaderMaterial.h"
#include "Vertex/FatVertex.h"
#include "array"

struct GrassMesh final
{
	std::string Name;
	VertexBuffer<FatVertex> RenderVertexBuffer;
	IndexBuffer RenderIndexBuffer;
	GrassShaderMaterial Material;
};