#pragma once
#include <PxPhysicsAPI.h>
#include <vector>

class PhysicsMeshGenerator final
{
public:
	static physx::PxConvexMesh* CreateCylinder(physx::PxPhysics& physics, const physx::PxCooking& cooking,
		float width, float radius, int halfVerticesCount = 16);
	static physx::PxConvexMesh* CreateCustomBox(physx::PxPhysics& physics, const physx::PxCooking& cooking,
	                                            const physx::PxVec3& dimensions);
private:
	static physx::PxConvexMesh* CreateConvexMesh(physx::PxPhysics& physics,
		const physx::PxCooking& cooking, const std::vector<physx::PxVec3>& vertices, int verticesCount);
};