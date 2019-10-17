#include "PhysicsMeshGenerator.h"
#include "../Utils/Logger.h"

physx::PxConvexMesh* PhysicsMeshGenerator::CreateCylinder(physx::PxPhysics& physics, const physx::PxCooking& cooking,
	const float width, const float radius, const int halfVerticesCount)
{
	std::vector<physx::PxVec3> points;
	points.resize(2 * halfVerticesCount);

	for (auto i = 0; i < halfVerticesCount; i++)
	{
		const auto cosTheta = std::cos(i * physx::PxPi * 2.0f / static_cast<float>(halfVerticesCount));
		const auto sinTheta = std::sin(i * physx::PxPi * 2.0f / static_cast<float>(halfVerticesCount));

		const auto y = radius * cosTheta;
		const auto z = radius * sinTheta;

		const auto position = 2 * i;

		points[position] = physx::PxVec3(-width / 2.0f, y, z);
		points[position + 1] = physx::PxVec3(width / 2.0f, y, z);
	}

	return CreateConvexMesh(physics, cooking, points, halfVerticesCount * 2);
}

physx::PxConvexMesh* PhysicsMeshGenerator::CreateCustomBox(physx::PxPhysics& physics, const physx::PxCooking& cooking,
	const physx::PxVec3& dimensions)
{
	const auto x = dimensions.x * 0.5f;
	const auto y = dimensions.x * 0.5f;
	const auto z = dimensions.x * 0.5f;

	std::vector<physx::PxVec3> vertices =
	{
		physx::PxVec3(x,y,-z),
		physx::PxVec3(x,y,z),
		physx::PxVec3(x,-y,z),
		physx::PxVec3(x,-y,-z),
		physx::PxVec3(-x,y,-z),
		physx::PxVec3(-x,y,z),
		physx::PxVec3(-x,-y,z),
		physx::PxVec3(-x,-y,-z)
	};

	return CreateConvexMesh(physics, cooking, vertices, vertices.size());
}

physx::PxConvexMesh* PhysicsMeshGenerator::CreateConvexMesh(physx::PxPhysics& physics, const physx::PxCooking& cooking,
	const std::vector<physx::PxVec3>& vertices, const int verticesCount)
{
	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = verticesCount;
	convexDesc.points.stride = sizeof(physx::PxVec3);
	convexDesc.points.data = vertices.data();
	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	physx::PxConvexMesh* convexMesh = nullptr;
	physx::PxDefaultMemoryOutputStream buffer;

	if (cooking.cookConvexMesh(convexDesc, buffer))
	{
		physx::PxDefaultMemoryInputData id(buffer.getData(), buffer.getSize());
		convexMesh = physics.createConvexMesh(id);
	}
	else
	{
		Logger::Error("Error when cooking a convex mesh!");
	}

	return  convexMesh;
}
