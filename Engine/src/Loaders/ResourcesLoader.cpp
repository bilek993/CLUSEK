#include "ResourcesLoader.h"
#include "../Utils/Logger.h"
#include "MaterialLoader.h"
#include "ModelLoader.h"

void ResourcesLoader::Load(ID3D11Device* device, const std::string& pathToResourceFile)
{
	Logger::Debug("Preparing to load resource...");

	ModelLoader::LoadResource(device, "Data/Models/Nanosuit/nanosuit.fbx", "Nanosuit");
	MaterialLoader::LoadResource(device, "Data/Textures/Nanosuit/glass_dif.png", "NanosuitGlassDif");
	MaterialLoader::LoadResource(device, "Data/Textures/Nanosuit/leg_dif.png", "NanosuitLegDif");
	MaterialLoader::LoadResource(device, "Data/Textures/Nanosuit/hand_dif.png", "NanosuitHandDif");
	MaterialLoader::LoadResource(device, "Data/Textures/Nanosuit/helmet_diff.png", "NanosuitHelmetDiff");
	MaterialLoader::LoadResource(device, "Data/Textures/Nanosuit/body_dif.png", "NanosuitBodyDif");
	MaterialLoader::LoadResource(device, "Data/Textures/Nanosuit/arm_dif.png", "NanosuitArmDif");
}
