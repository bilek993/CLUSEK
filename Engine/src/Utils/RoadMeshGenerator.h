#pragma once
#include "../ECS/Components/RoadComponent.h"
#include "../Renderer/Mesh.h"

class RoadMeshGenerator final
{
public:
	static Mesh GenerateRoadMesh(const ID3D11Device* device, RoadComponent &roadComponent);
};
