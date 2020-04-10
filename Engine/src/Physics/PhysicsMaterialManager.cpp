#include "PhysicsMaterialManager.h"
#include "../Utils/Logger.h"
#include "../ECS/Systems/PhysicsSystem.h"

PhysicsMaterialManager::PhysicsMaterialManager(	physx::PxPhysics* physics, const float defaultStaticFriction, const float defaultDynamicFriction,
												const float defaultRestitution)
{
	Logger::Debug("Physics material manager initialization...");

	DefaultMaterial = physics->createMaterial(defaultStaticFriction, defaultDynamicFriction, defaultRestitution);
}

void PhysicsMaterialManager::AddMaterial(physx::PxPhysics* physics, const float staticFriction, const float dynamicFriction,
                                         const float tireFriction, const float restitution, const std::string& name)
{
	Logger::Debug("Adding new physics material (" + name + ").");

	const auto material = physics->createMaterial(	staticFriction,
													dynamicFriction,
													restitution);

	PhysicsMaterials.emplace_back(material);
	PhysicsTireFrictions.emplace_back(tireFriction);
	PhysicsMaterialNames.emplace_back(name);
}

int PhysicsMaterialManager::GetMaterialCount() const
{
	return PhysicsMaterials.size();
}

physx::PxMaterial* PhysicsMaterialManager::GetMaterialByName(const std::string& name)
{
	if (GetMaterialCount() == 0)
		Logger::Error("No physics materials!");

	const auto foundValues = std::find(PhysicsMaterialNames.begin(), PhysicsMaterialNames.end(), name);

	if (foundValues == PhysicsMaterialNames.end())
	{
		Logger::Warning("Material not found! Returning default material.");
		return DefaultMaterial;
	}

	const auto id = std::distance(PhysicsMaterialNames.begin(), foundValues);
	return PhysicsMaterials[id];
}

physx::PxMaterial* PhysicsMaterialManager::GetMaterialById(const int id) const
{
	if (GetMaterialCount() == 0)
		Logger::Error("No physics materials!");

	if (id >= GetMaterialCount())
		Logger::Error("Physics material id out of bounds!");

	return PhysicsMaterials[id];
}

physx::PxMaterial** PhysicsMaterialManager::GetPointerToAllMaterials()
{
	return PhysicsMaterials.data();
}

float PhysicsMaterialManager::GetTireFrictionById(const int id) const
{
	return PhysicsTireFrictions[id];
}

void PhysicsMaterialManager::ReleaseAllMaterials()
{
	for (auto& material : PhysicsMaterials)
		PX_RELEASE(material);
}
