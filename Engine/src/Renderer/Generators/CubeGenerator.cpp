#include "CubeGenerator.h"
#include "../IndexBuffer.h"

void CubeGenerator::Generate(ID3D11Device* device, VertexBuffer<PositionVertex>& vertexBufferOut, IndexBuffer& indexBufferOut)
{
	Logger::Debug("Preparing cube generator...");

	PositionVertex vertex[] =
	{
		PositionVertex(-0.5f,  -0.5f, -0.5f), 
		PositionVertex(-0.5f,   0.5f, -0.5f),
		PositionVertex(0.5f,   0.5f, -0.5f),
		PositionVertex(0.5f,  -0.5f, -0.5f),
		PositionVertex(-0.5f,  -0.5f, 0.5f),
		PositionVertex(-0.5f,   0.5f, 0.5f),
		PositionVertex(0.5f,   0.5f, 0.5f),
		PositionVertex(0.5f,  -0.5f, 0.5f),
	};

	Logger::Debug("Preparing to generate vertex buffer...");
	auto hr = vertexBufferOut.Initialize(device, vertex, ARRAYSIZE(vertex));
	if (hr)
		Logger::Error("Vertex buffer creation error for generated cube!");

	DWORD indices[] =
	{
		0, 1, 2,
		0, 2, 3,
		4, 7, 6, 
		4, 6, 5,
		3, 2, 6,
		3, 6, 7,
		4, 5, 1,
		4, 1, 0,
		1, 5, 6,
		1, 6, 2,
		0, 3, 7,
		0, 7, 4,
	};

	Logger::Debug("Preparing to generate indices buffer...");
	hr = indexBufferOut.Initialize(device, indices, ARRAYSIZE(indices));
	if (hr)
		Logger::Error("Index buffer creation error for generated cube!");

	Logger::Debug("Cube generator done!");
}
