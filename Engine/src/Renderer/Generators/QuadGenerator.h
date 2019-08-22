#pragma once
#include "../Vertex/PositionVertex.h"
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"

class QuadGenerator final
{
public:
	static void Generate(ID3D11Device* device, VertexBuffer<PositionVertex>& vertexBufferOut,
		IndexBuffer& indexBufferOut, float minX, float minY, float maxX, float maxY);
};
