#pragma once
#include <vector>
#include <PxMaterial.h>
#include <PxPhysics.h>

class PhysicsMaterialManager final
{
public:
	PhysicsMaterialManager(physx::PxPhysics* physics);

	void AddMaterial(physx::PxPhysics* physics, float staticFriction, float dynamicFriction, 
		float restitution, const std::string& name);

	int GetMaterialCount() const;

	physx::PxMaterial* GetMaterialByName(const std::string& name);
	physx::PxMaterial* GetMaterialById(int id) const;
	physx::PxMaterial** GetPointerToAllMaterials();

private:
	physx::PxMaterial* DefaultMaterial = nullptr;

	std::vector<physx::PxMaterial*> PhysicsMaterial;
	std::vector<std::string> PhysicsMaterialName;
};
