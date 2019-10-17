#include "VehicleResourcesGenerator.h"

physx::PxVehicleDrive4W* VehicleResourcesGenerator::Create4WheelVehicle(physx::PxPhysics* physics,
	const physx::PxCooking* cooking, const VehicleComponent& vehicleComponent, const PhysicsMaterialComponent& vehicleMaterialComponent)
{
	const physx::PxVec3 vehicleMomentOfInertia(
		(vehicleComponent.DimensionY * vehicleComponent.DimensionY + vehicleComponent.DimensionZ * vehicleComponent.DimensionZ) * vehicleComponent.Mass / 12.0f,
		(vehicleComponent.DimensionX * vehicleComponent.DimensionX + vehicleComponent.DimensionZ * vehicleComponent.DimensionZ) * 0.8f * vehicleComponent.Mass / 12.0f,
		(vehicleComponent.DimensionX * vehicleComponent.DimensionX + vehicleComponent.DimensionY * vehicleComponent.DimensionY) * vehicleComponent.Mass / 12.0f
	);
	const physx::PxVec3 vehicleCenterOfMassOffset(vehicleComponent.CenterOfMassOffsetX, vehicleComponent.CenterOfMassOffsetY, vehicleComponent.CenterOfMassOffsetZ);
	const physx::PxVec3 vehicleDimensions(vehicleComponent.DimensionX, vehicleComponent.DimensionY, vehicleComponent.DimensionZ);

	return nullptr;
}
