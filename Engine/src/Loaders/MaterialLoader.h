#pragma once
#include <vector>
#include "../Renderer/Mesh.h"

class MaterialLoader final
{
public:
	static void LoadMaterialForMesh(ID3D11Device* device, Mesh& mesh, const std::wstring& pathToMainTexture);
	static void LoadMaterialForMeshGroup(ID3D11Device* device, std::vector<Mesh>& meshes, const std::string& pathToMaterial);
};
