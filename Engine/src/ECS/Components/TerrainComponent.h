#pragma once
#include "../../Renderer/Vertex/FatVertex.h"
#include "../../Renderer/VertexBuffer.h"
#include "../../Renderer/IndexBuffer.h"
#include "../../Renderer/Materials/TerrainShaderMaterial.h"

struct TerrainComponent final
{
	std::string PathToHeightmap;
	std::string MaterialId;
	int QualityDivider = 1;
	float MaxHeight = 500.0f;
	VertexBuffer<FatVertex> RenderVertexBuffer;
	IndexBuffer RenderIndexBuffer;
	TerrainShaderMaterial Material;
};
