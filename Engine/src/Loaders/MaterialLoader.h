#pragma once
#include <vector>
#include "../Renderer/Mesh.h"
#include <unordered_map>
#include "../Renderer/Materials/SkyShaderMaterial.h"
#include "../Renderer/Shaders/ComputeShader.h"
#include "ConfigData.h"
#include <mutex>

#include "../Renderer/GrassMesh.h"
#include "../Renderer/Materials/TerrainShaderMaterial.h"

class MaterialLoader final
{
public:
	static void LoadResource(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& path, const std::string& resourceId, 
		const std::string& convertLatLongToCubeMap, const std::string& srgbMode, const std::string& mipMaps, const ConfigData* config);
	
	static void SetResourceForMesh(ID3D11Device* device, Mesh& mesh, const std::string& albedoTextureId, 
		const std::string& normalTextureId, const std::string& metalicSmoothnessTextureId, 
		const std::string& occlusionTextureId, const std::string& emissionTextureId, float alpha, float thresholdAlpha,
		bool twoSided, float hightWindSpeed, float hightWindScale, float hightWindBase, float localWindSpeed, 
		float localWindScale, bool hightWindEnabled, bool localWindEnabled);
	static void SetResourceForMeshGroup(ID3D11Device* device, std::vector<Mesh>& meshes, const std::string& pathToMaterial);
	static void SetResourceForSingleMesh(ID3D11Device* device, Mesh& mesh, const std::string& pathToMaterial);

	static void SetResourceForGrassMesh(ID3D11Device* device, GrassMesh& mesh, const std::string& albedoTextureId);
	static void SetResourceForGrassMeshGroup(ID3D11Device* device, std::vector<GrassMesh>& meshes, const std::string& pathToMaterial);
	
	static void SetResourceForSkyMaterial(ID3D11Device* device, SkyShaderMaterial& material, const std::string& albedoTextureId);
	static void SetResourceForTerrainMaterial(ID3D11Device* device, TerrainShaderMaterial& material, const std::string& pathToMaterial);
	static void GetAndSetLoadingTexture(ID3D11Device* device, const std::string& path, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& resourceView);
private:
	inline static const int THREAD_COUNT = 32;
	inline static const int CUBE_SIZE = 6;

	enum FallbackColor : unsigned int
	{
		DefaultAlbedo = 0xff9314ff, // Alpha = ff, Blue = 93, Green = 14, Red = ff
		DefaultNormal = 0xffff8080, // Alpha = ff, Blue = ff, Green = 80, Red = 80
		DefaultMetalicSmoothness = 0x00000000, // Alpha = 0, Blue = 0, Green = 0, Red = 0
		DefaultOcclusion = 0xffffffff, // Alpha = ff, Blue = ff, Green = ff, Red = ff
		DefaultEmission = 0xff000000, // Alpha = ff, Blue = 00, Green = 00, Red = 00
	};

	static inline std::mutex ResourcesMapMutex{};
	static inline std::mutex ContextMutex{};

	static std::unordered_map<std::string, std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>> TextureResources;

	static inline Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;

	static std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GetTextureById(ID3D11Device* device, const std::string& id, 
		FallbackColor fallbackColor);
	static void LoadTextureToMaterial(ID3D11Device* device, ID3D11DeviceContext* context, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResource, 
		const std::string& path, bool forceSrgb, bool generateMipMaps);
	static void SetDefaultTexture(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResource, FallbackColor fallbackColor);

	static std::shared_ptr<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> ConvertLatLongToCubeMap(ID3D11Device* device, 
		ID3D11DeviceContext* context, ID3D11ShaderResourceView* const* inputResourceView, int textureSize, bool compatibleMode);
	static void CreateSamplerStateIfNeeded(ID3D11Device* device);
};
