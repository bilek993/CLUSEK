#pragma once
#include <PxPhysicsAPI.h>
#include <vector>

class VehicleResourcesGenerator final
{
public:
	static physx::PxConvexMesh* CreateWheelMesh(physx::PxPhysics& physics, const physx::PxCooking& cooking,
		float width, float radius, int halfVerticesCount = 16);
private:
	static physx::PxConvexMesh* CreateConvexMesh(physx::PxPhysics& physics, 
		const physx::PxCooking& cooking, const std::vector<physx::PxVec3>& vertices, int verticesCount);
};
