#pragma once
#include "../../Renderer/Mesh.h"
#include "../../Renderer/Vertex/PositionVertex.h"
#include "../../Renderer/Materials/SkyShaderMaterial.h"

struct SkyboxComponent final
{
	std::string SkyboxTextureId;
	VertexBuffer<PositionVertex> RenderVertexBuffer;
	IndexBuffer RenderIndexBuffer;
	SkyShaderMaterial Material;
};
