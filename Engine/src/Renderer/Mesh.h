#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Vertex.h"

struct Mesh final
{
	VertexBuffer<Vertex> RenderVertexBuffer;
	IndexBuffer RenderIndexBuffer;
};
