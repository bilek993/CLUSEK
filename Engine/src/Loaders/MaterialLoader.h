#pragma once
#include <vector>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include "../Renderer/Mesh.h"

class MaterialLoader final
{
public:
	static void LoadMaterialForMesh(ID3D11Device* device, Mesh& mesh, const std::string& pathToMainTexture);
	static void LoadMaterialForMeshGroup(ID3D11Device* device, std::vector<Mesh>& meshes, const std::string& pathToMaterial);
private:
	static void LoadTextureToMaterial(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResource, const std::string& path);
};
