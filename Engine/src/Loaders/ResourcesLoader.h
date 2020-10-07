#pragma once
#include <string>
#include <d3d11.h>
#include <json.hpp>
#include <PxPhysics.h>
#include <cooking/PxCooking.h>
#include "ConfigData.h"

class ResourcesLoader final
{
public:
	static void LoadFromRenderer(ID3D11Device* device, ID3D11DeviceContext* context, const ConfigData* config);
	static void LoadFromPhysics(physx::PxPhysics& physics, const physx::PxCooking& cooking, const ConfigData* config);
	
private:
	static void LoadModels(ID3D11Device* device, const nlohmann::json& json, const ConfigData* config);
	static void LoadGrassModels(ID3D11Device* device, const nlohmann::json& json, const ConfigData* config);
	static void LoadConvexModels(physx::PxPhysics& physics, const physx::PxCooking& cooking, const nlohmann::json& json, 
		const ConfigData* config);
	
	static void LoadTextures(ID3D11Device* device, ID3D11DeviceContext* context, const nlohmann::json& json,
		const ConfigData* config);
};
