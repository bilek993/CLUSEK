#pragma once
#include <vector>
#include "../Renderer/Mesh.h"
#include <unordered_map>
#include "../Renderer/Materials/SkyShaderMaterial.h"
#include "../Renderer/Shaders/ComputeShader.h"
#include "ConfigData.h"
#include <mutex>

class MaterialLoader final
{
public:
	static void LoadResource(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& path, const std::string
	                         & resourceId,
	                         const std::string& convertLatLongToCubeMap, const ConfigData* config);
	static void SetResourceForMesh(ID3D11Device* device, Mesh& mesh, const std::string& albedoTextureId, 
		const std::string& normalTextureId, const std::string& metalicSmoothnessTextureId, 
		const std::string& occlusionTextureId, const std::string& emissionTextureId, float alpha);
	static void SetResourceForSkyMaterial(ID3D11Device* device, SkyShaderMaterial& material, const std::string& albedoTextureId);
	static void GetAndSetLoadingTexture(ID3D11Device* device, const std::string& path, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& resourceView);
	static void SetResourceForMeshGroup(ID3D11Device* device, std::vector<Mesh>& meshes, const std::string& pathToMaterial);
private:
	inline static const int THREAD_COUNT = 32;
	inline static const int CUBE_SIZE = 6;

	enum FallbackColor : unsigned int
	{
		DefaultAlbedo = 0xff9314ff, // Alpha = ff, Blue = 93, Green = 14, Red = ff
		DefaultNormal = 0xffff8080, // Alpha = ff, Blue = ff, Green = 80, Red = 80
		DefaultMetalicSmoothness = 0x80000000, // Alpha = 80, Blue = 0, Green = 0, Red = 0
		DefaultOcclusion = 0xffffffff, // Alpha = ff, Blue = ff, Green = ff, Red = ff
		DefaultEmission = 0xff000000, // Alpha = ff, Blue = 00, Green = 00, Red = 00
	};

	static inline std::mutex ResourcesMapMutex{};
	static inline std::mutex LatLongConverterMutex{};

	static std::unordered_map<std::string, std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>> TextureResources;

	static inline Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;

	static std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GetTextureById(ID3D11Device* device, const std::string& id, 
		FallbackColor fallbackColor);
	static void LoadTextureToMaterial(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResource, const std::string& path);
	static void SetDefaultTexture(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResource, FallbackColor fallbackColor);

	static std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> ConvertLatLongToCubeMap(ID3D11Device* device, 
		ID3D11DeviceContext* context, ID3D11ShaderResourceView* const* inputResourceView, int textureSize, bool compatibleMode);
	static void CreateSamplerStateIfNeeded(ID3D11Device* device);
};
