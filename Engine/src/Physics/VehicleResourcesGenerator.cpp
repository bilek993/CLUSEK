#include "VehicleResourcesGenerator.h"
#include "PhysicsMeshGenerator.h"

physx::PxVehicleDrive4W* VehicleResourcesGenerator::Create4WheelVehicle(physx::PxPhysics* physics,
	const physx::PxCooking* cooking, const VehicleComponent& vehicleComponent, const PhysicsMaterialComponent& vehicleMaterialComponent)
{
	auto vehicleAuto = Create4WheelVehicleActor(physics, cooking, vehicleComponent, vehicleMaterialComponent);

	return nullptr;
}

physx::PxRigidDynamic* VehicleResourcesGenerator::Create4WheelVehicleActor(physx::PxPhysics* physics,
	const physx::PxCooking* cooking, const VehicleComponent& vehicleComponent, const PhysicsMaterialComponent& vehicleMaterialComponent)
{
	const auto wheelsCount = std::size(vehicleComponent.Wheels);

	const physx::PxVec3 vehicleMomentOfInertia(
		(vehicleComponent.DimensionY * vehicleComponent.DimensionY + vehicleComponent.DimensionZ * vehicleComponent.DimensionZ) * vehicleComponent.Mass / 12.0f,
		(vehicleComponent.DimensionX * vehicleComponent.DimensionX + vehicleComponent.DimensionZ * vehicleComponent.DimensionZ) * 0.8f * vehicleComponent.Mass / 12.0f,
		(vehicleComponent.DimensionX * vehicleComponent.DimensionX + vehicleComponent.DimensionY * vehicleComponent.DimensionY) * vehicleComponent.Mass / 12.0f
	);
	const physx::PxVec3 vehicleCenterOfMassOffset(vehicleComponent.CenterOfMassOffsetX, vehicleComponent.CenterOfMassOffsetY, vehicleComponent.CenterOfMassOffsetZ);
	const physx::PxVec3 vehicleDimensions(vehicleComponent.DimensionX, vehicleComponent.DimensionY, vehicleComponent.DimensionZ);

	std::vector<physx::PxConvexMesh*> wheelMeshes;
	for (auto i = 0; i < wheelsCount; i++)
	{
		wheelMeshes.emplace_back(PhysicsMeshGenerator::CreateCylinder(*physics, *cooking,
			vehicleComponent.Wheels[i]->Width, vehicleComponent.Wheels[i]->Radius));
	}

	const auto vehicleMesh = PhysicsMeshGenerator::CreateCustomBox(*physics, *cooking, vehicleDimensions);

	physx::PxVehicleChassisData rigidbodyData;
	rigidbodyData.mMOI = vehicleMomentOfInertia;
	rigidbodyData.mMass = vehicleComponent.Mass;
	rigidbodyData.mCMOffset = vehicleCenterOfMassOffset;

	const auto vehicleActor = physics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));

	for (auto i = 0; i < wheelsCount; i++)
	{
		physx::PxConvexMeshGeometry geometry(wheelMeshes[i]);
		auto wheelShape = physx::PxRigidActorExt::createExclusiveShape(*vehicleActor, geometry, *vehicleComponent.WheelsMaterials[i]->Material);
		wheelShape->setLocalPose(physx::PxTransform(physx::PxIdentity));
	}

	physx::PxConvexMeshGeometry vehicleGeometry(vehicleMesh);
	auto vehicleShape = physx::PxRigidActorExt::createExclusiveShape(*vehicleActor, vehicleGeometry, *vehicleMaterialComponent.Material);
	vehicleShape->setLocalPose(physx::PxTransform(physx::PxIdentity));

	vehicleActor->setMass(rigidbodyData.mMass);
	vehicleActor->setMassSpaceInertiaTensor(rigidbodyData.mMOI);
	vehicleActor->setCMassLocalPose(physx::PxTransform(rigidbodyData.mCMOffset, physx::PxQuat(physx::PxIdentity)));

	return vehicleActor;
}
