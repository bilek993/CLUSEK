#pragma once
#include <vector>
#include <PxMaterial.h>
#include <PxPhysics.h>

class PhysicsMaterialManager final
{
public:
	PhysicsMaterialManager(	physx::PxPhysics* physics, float defaultStaticFriction, float defaultDynamicFriction,
							float defaultRestitution);

	void AddMaterial(	physx::PxPhysics* physics, float staticFriction, float dynamicFriction, 
						float tireFriction, float restitution, const std::string& name);

	int GetMaterialCount() const;

	std::vector<std::string> GetPhysicsMaterialNames() const;

	physx::PxMaterial* GetMaterialByName(const std::string& name);
	physx::PxMaterial* GetMaterialById(int id) const;
	physx::PxMaterial** GetPointerToAllMaterials();

	float GetTireFrictionById(int id) const;

	void UpdateTireFrictionById(int id, float friction);

	void ReleaseAllMaterials();

private:
	physx::PxMaterial* DefaultMaterial = nullptr;

	std::vector<physx::PxMaterial*> PhysicsMaterials;
	std::vector<float> PhysicsTireFrictions;
	std::vector<std::string> PhysicsMaterialNames;
};
