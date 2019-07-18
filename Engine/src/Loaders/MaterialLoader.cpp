#include "MaterialLoader.h"

void MaterialLoader::LoadMaterialForMesh(ID3D11Device* device, Mesh& mesh)
{
	const auto hr = DirectX::CreateWICTextureFromFile(device, L"Data/Textures/example_texture.png", nullptr, mesh.Material.MainTexture.GetAddressOf());
	if (FAILED(hr))
		Logger::Error("Couldn't load example texture from file!");
}

void MaterialLoader::LoadMaterialForMeshGroup(ID3D11Device* device, std::vector<Mesh>& meshes)
{
	for (auto& mesh : meshes)
	{
		LoadMaterialForMesh(device, mesh);
	}
}
