#pragma once
#include <vector>
#include "../Renderer/Mesh.h"

class MaterialLoader final
{
public:
	static void LoadMaterialForMesh(ID3D11Device* device, Mesh& mesh);
	static void LoadMaterialForMeshGroup(ID3D11Device* device, std::vector<Mesh>& meshes);
};
