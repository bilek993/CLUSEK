#pragma once
#include "../../Renderer/VertexBuffer.h"
#include "../../Renderer/IndexBuffer.h"
#include "../../Renderer/Materials/TerrainShaderMaterial.h"
#include "../../Renderer/Vertex/PositionAndUvVertex.h"

struct TerrainComponent final
{
	std::string PathToHeightmap;
	std::string MaterialId;
	int QualityDivider = 1;
	float ScaleXZ = 1.0f;
	float MaxHeight = 500.0f;
	float TexelSize = 0.1f;
	VertexBuffer<PositionAndUvVertex> RenderVertexBuffer;
	IndexBuffer RenderIndexBuffer;
	IndexBuffer ShadowIndexBuffer;
	TerrainShaderMaterial Material;
	DirectX::XMMATRIX WorldMatrix;
};
