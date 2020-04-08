#include "PhysicsMaterialManager.h"
#include "../Utils/Logger.h"

PhysicsMaterialManager::PhysicsMaterialManager(physx::PxPhysics* physics, const float defaultStaticFriction, const float defaultDynamicFriction,
	const float defaultRestitution)
{
	Logger::Debug("Physics material manager initialization...");

	DefaultMaterial = physics->createMaterial(defaultStaticFriction, defaultDynamicFriction, defaultRestitution);
}

void PhysicsMaterialManager::AddMaterial(physx::PxPhysics* physics, const float staticFriction, const float dynamicFriction,
                                         const float restitution, const std::string& name)
{
	Logger::Debug("Adding new physics material (" + name + ").");

	const auto material = physics->createMaterial(	staticFriction,
													dynamicFriction,
													restitution);

	PhysicsMaterial.emplace_back(material);
	PhysicsMaterialName.emplace_back(name);
}

int PhysicsMaterialManager::GetMaterialCount() const
{
	return PhysicsMaterial.size();
}

physx::PxMaterial* PhysicsMaterialManager::GetMaterialByName(const std::string& name)
{
	if (GetMaterialCount() == 0)
		Logger::Error("No physics materials!");

	const auto foundValues = std::find(PhysicsMaterialName.begin(), PhysicsMaterialName.end(), name);

	if (foundValues == PhysicsMaterialName.end())
	{
		Logger::Warning("Material not found! Returning default material.");
		return DefaultMaterial;
	}

	const auto id = std::distance(PhysicsMaterialName.begin(), foundValues);
	return PhysicsMaterial[id];
}

physx::PxMaterial* PhysicsMaterialManager::GetMaterialById(const int id) const
{
	if (GetMaterialCount() == 0)
		Logger::Error("No physics materials!");

	if (id >= GetMaterialCount())
		Logger::Error("Physics material id out of bounds!");

	return PhysicsMaterial[id];
}

physx::PxMaterial** PhysicsMaterialManager::GetPointerToAllMaterials()
{
	return PhysicsMaterial.data();
}
