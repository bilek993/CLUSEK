#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "DefaultVertex.h"
#include "Materials/UberShaderMaterial.h"

struct Mesh final
{
	std::string Name;
	VertexBuffer<DefaultVertex> RenderVertexBuffer;
	IndexBuffer RenderIndexBuffer;
	UberShaderMaterial Material;
};
