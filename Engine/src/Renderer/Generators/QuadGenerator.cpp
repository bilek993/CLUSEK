#include "QuadGenerator.h"

void QuadGenerator::Generate(ID3D11Device* device, VertexBuffer<PositionVertex>& vertexBufferOut,
	IndexBuffer& indexBufferOut, const float minX, const float minY,  const float maxX, const float maxY)
{
	Logger::Debug("Preparing quad generator...");

	PositionVertex vertex[] =
	{
		PositionVertex(minX, minY, 0.0f),
		PositionVertex(minX, maxY, 0.0f),
		PositionVertex(maxX, minY, 0.0f),
		PositionVertex(maxX, maxY, 0.0f),
	};

	Logger::Debug("Preparing to generate vertex buffer...");
	auto hr = vertexBufferOut.Initialize(device, vertex, ARRAYSIZE(vertex));
	if (hr)
		Logger::Error("Vertex buffer creation error for generated cube!");

	DWORD indices[] =
	{
		1, 3, 2,
		2, 0, 1,
	};

	Logger::Debug("Preparing to generate indices buffer...");
	hr = indexBufferOut.Initialize(device, indices, ARRAYSIZE(indices));
	if (hr)
		Logger::Error("Index buffer creation error for generated quad!");

	Logger::Debug("Quad generator done!");
}
