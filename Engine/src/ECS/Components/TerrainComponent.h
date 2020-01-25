#pragma once
#include "../../Renderer/Vertex/FatVertex.h"
#include "../../Renderer/VertexBuffer.h"
#include "../../Renderer/IndexBuffer.h"

struct TerrainComponent final
{
	std::string PathToHeightmap;
	VertexBuffer<FatVertex> RenderVertexBuffer;
	IndexBuffer RenderIndexBuffer;
};