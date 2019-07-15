#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Vertex.h"

struct Mesh final
{
	std::string Name;
	VertexBuffer<Vertex> RenderVertexBuffer;
	IndexBuffer RenderIndexBuffer;
};
