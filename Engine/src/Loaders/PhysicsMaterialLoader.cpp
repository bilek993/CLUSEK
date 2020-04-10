#include "PhysicsMaterialLoader.h"
#include "../Utils/Logger.h"
#include <fstream>

void PhysicsMaterialLoader::Load(const std::string& path, PhysicsMaterialManager* physicsMaterialMaterialManager, physx::PxPhysics* physics)
{
	if (path.empty())
	{
		Logger::Error("Incorrect path to physics material passed! Path to physics material file cannot be empty.");
		return;
	}

	nlohmann::json jsonObject;
	std::ifstream inputFile(path);
	inputFile >> jsonObject;

	Logger::Debug("Preparing to load physics materials...");

	for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it)
		ParseMaterial(it.value(), physicsMaterialMaterialManager, physics);

	Logger::Debug("Loaded all supported physics materials!");
}

void PhysicsMaterialLoader::ParseMaterial(nlohmann::json& jsonObject, PhysicsMaterialManager* physicsMaterialMaterialManager, physx::PxPhysics* physics)
{
	if (jsonObject["Name"].is_null())
	{
		Logger::Warning("Material with no name are not supported!");
		return;
	}

	const auto name = jsonObject["Name"].get<std::string>();
	const auto staticFriction = jsonObject["StaticFriction"].is_null() ? DEFAULT_STATIC_FRICTION : jsonObject["StaticFriction"].get<float>();
	const auto dynamicFriction = jsonObject["DynamicFriction"].is_null() ? DEFAULT_STATIC_FRICTION : jsonObject["DynamicFriction"].get<float>();
	const auto restitution = jsonObject["Restitution"].is_null() ? DEFAULT_STATIC_FRICTION : jsonObject["Restitution"].get<float>();

	physicsMaterialMaterialManager->AddMaterial(physics, staticFriction, dynamicFriction, restitution, name);
	Logger::Debug("Added new physics material named: '" + name + "'");
}
