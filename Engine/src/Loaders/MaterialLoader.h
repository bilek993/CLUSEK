#pragma once
#include <vector>
#include "../Renderer/Mesh.h"
#include <unordered_map>
#include "../Renderer/Materials/SkyShaderMaterial.h"

class MaterialLoader final
{
public:
	static void LoadResource(ID3D11Device* device, const std::string& path, const std::string& resourceId);
	static void SetResourceForMesh(ID3D11Device* device, Mesh& mesh, const std::string& albedoTextureId, 
		const std::string& normalTextureId, float alpha);
	static void SetResourceForManuallyForSkyMaterial(ID3D11Device* device, SkyShaderMaterial& material, const std::string& albedoTextureId);
	static void SetResourceForMeshGroup(ID3D11Device* device, std::vector<Mesh>& meshes, const std::string& pathToMaterial);
private:
	enum FallbackColor : unsigned int
	{
		DefaultAlbedo = 0xff9314ff, // Alpha = ff, Blue = 93, Green = 14, Red = ff
		DefaultNormal = 0xffff8080, // Alpha = ff, Blue = ff, Green = 80, Red = 80
	};

	static std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GetTextureById(ID3D11Device* device, const std::string& id, 
		FallbackColor fallbackColor);
	static void LoadTextureToMaterial(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResource, const std::string& path);
	static void SetDefaultTexture(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResource, FallbackColor fallbackColor);

	static std::unordered_map<std::string, std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>> TextureResources;
};
