#include "PhysicsFilterHelper.h"
#include <vector>

void PhysicsFilterHelper::SetQueryFilterDataForActor(physx::PxRigidActor& actor, const physx::PxFilterData& filterData)
{
	const auto numberOfShapes = actor.getNbShapes();

	std::vector<physx::PxShape*> shapes(numberOfShapes);
	actor.getShapes(shapes.data(), numberOfShapes);

	for (auto& shape : shapes)
	{
		shape->setQueryFilterData(filterData);
	}
}

void PhysicsFilterHelper::SetSimulationFilterDataForActor(physx::PxRigidActor& actor, const physx::PxFilterData& filterData)
{
	const auto numberOfShapes = actor.getNbShapes();

	std::vector<physx::PxShape*> shapes(numberOfShapes);
	actor.getShapes(shapes.data(), numberOfShapes);

	for (auto& shape : shapes)
	{
		shape->setSimulationFilterData(filterData);
	}
}

void PhysicsFilterHelper::SetSurfaceFilter(physx::PxFilterData& filterData, const SurfaceFilterTypes type)
{
	filterData.word3 = type;
}

void PhysicsFilterHelper::SetSurfaceFilter(physx::PxFilterData& filterData, const std::string& type)
{
	if (type == "Drivable")
		filterData.word3 = Drivable;
	else if (type == "Undrivable")
		filterData.word3 = Undrivable;
}

void PhysicsFilterHelper::SetSimulationFilter(physx::PxFilterData& filterData, const CollisionFilterTypes collide,
	const CollisionFilterTypes collideAgainst)
{
	filterData.word0 = collide;
	filterData.word1 = collideAgainst;
}
