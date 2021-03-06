#include "VehicleResourcesGenerator.h"
#include "PhysicsMeshGenerator.h"
#include "../Renderer/TransformLogic.h"
#include "PhysicsUnitConversion.h"
#include "PhysicsFilterHelper.h"
#include "../Loaders/ModelLoader.h"
#include "../Utils/Logger.h"

physx::PxVehicleDrive4W* VehicleResourcesGenerator::Create4WheelVehicle(physx::PxPhysics* physics,
	const physx::PxCooking* cooking, const VehicleComponent& vehicleComponent, const PhysicsMaterialComponent& vehicleMaterialComponent,
	const TransformComponent& vehicleTransformComponent)
{
	Logger::Debug("Preparing to create vehicle...");

	const auto wheelsCount = std::size(vehicleComponent.Wheels);

	const physx::PxVec3 vehicleMomentOfInertia(
		(vehicleComponent.DimensionY * vehicleComponent.DimensionY + vehicleComponent.DimensionZ * vehicleComponent.DimensionZ) * vehicleComponent.Mass / 12.0f,
		(vehicleComponent.DimensionX * vehicleComponent.DimensionX + vehicleComponent.DimensionZ * vehicleComponent.DimensionZ) * 0.8f * vehicleComponent.Mass / 12.0f,
		(vehicleComponent.DimensionX * vehicleComponent.DimensionX + vehicleComponent.DimensionY * vehicleComponent.DimensionY) * vehicleComponent.Mass / 12.0f
	);
	const physx::PxVec3 vehicleCenterOfMassOffset(vehicleComponent.CenterOfMassOffsetX, vehicleComponent.CenterOfMassOffsetY, vehicleComponent.CenterOfMassOffsetZ);
	const physx::PxVec3 vehicleDimensions(vehicleComponent.DimensionX, vehicleComponent.DimensionY, vehicleComponent.DimensionZ);

	physx::PxVehicleChassisData chassisData;
	chassisData.mMOI = vehicleMomentOfInertia;
	chassisData.mMass = vehicleComponent.Mass;
	chassisData.mCMOffset = vehicleCenterOfMassOffset;

	const auto vehicleMesh = vehicleComponent.ConvexModelId.empty() ? 
								PhysicsMeshGenerator::CreateCustomBox(*physics, *cooking, vehicleDimensions) :
								ModelLoader::GetConvexResource(vehicleComponent.ConvexModelId);

	const auto vehicleActor = Create4WheelVehicleActor(	physics, 
														cooking, 
														vehicleComponent, 
														vehicleMaterialComponent,
														vehicleMesh, 
														chassisData,
														wheelsCount);

	const auto wheelsOffsets = GenerateWheelsOffsets(	vehicleTransformComponent, 
														vehicleComponent,
														wheelsCount);

	const auto wheelsSimData = CreateWheelsSimData(	vehicleComponent, 
													wheelsCount, 
													vehicleCenterOfMassOffset,
													wheelsOffsets);

	const auto driveSimData = CreateDriveSimData(	vehicleComponent,
													wheelsSimData);

	auto vehicleDrive4Wheels = physx::PxVehicleDrive4W::allocate(wheelsCount);
	vehicleDrive4Wheels->setup(	physics,
								vehicleActor,
								*wheelsSimData, 
								driveSimData,
								wheelsCount - 4);

	SetInitialTransform(vehicleTransformComponent, vehicleDrive4Wheels);

	wheelsSimData->free();

	return vehicleDrive4Wheels;
}

physx::PxVehicleDrivableSurfaceToTireFrictionPairs* VehicleResourcesGenerator::CreateFrictionPairs(
	physx::PxPhysics& physics, PhysicsMaterialManager* physicsMaterialManager)
{
	Logger::Debug("Preparing to create friction pairs...");

	const auto tireTypesCount = 1; // Currently one on tire type is supported
	const auto surfaceTypesCount = physicsMaterialManager->GetMaterialCount();

	std::vector<physx::PxVehicleDrivableSurfaceType> surfaceTypes;
	for (auto i = 0; i < physicsMaterialManager->GetMaterialCount(); i++)
	{
		physx::PxVehicleDrivableSurfaceType drivableSurfaceType{};
		drivableSurfaceType.mType = i;
		surfaceTypes.emplace_back(drivableSurfaceType);
	}

	auto surfaceTirePairs =	physx::PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(tireTypesCount, surfaceTypesCount);

	surfaceTirePairs->setup(tireTypesCount, surfaceTypesCount, const_cast<const physx::PxMaterial**>(physicsMaterialManager->GetPointerToAllMaterials()), surfaceTypes.data());

	for (auto surfaceId = 0; surfaceId < surfaceTypesCount; surfaceId++)
	{
		for (auto tireId = 0; tireId < tireTypesCount; tireId++)
		{
			surfaceTirePairs->setTypePairFriction(surfaceId, tireId, physicsMaterialManager->GetTireFrictionById(surfaceId));
		}
	}

	return surfaceTirePairs;
}

float VehicleResourcesGenerator::CalculateWheelMOI(const float mass, const float radius)
{
	return 0.5f * mass * radius * radius;
}

physx::PxRigidDynamic* VehicleResourcesGenerator::Create4WheelVehicleActor(physx::PxPhysics* physics,
	const physx::PxCooking* cooking, const VehicleComponent& vehicleComponent, const PhysicsMaterialComponent& vehicleMaterialComponent,
	physx::PxConvexMesh* vehicleMesh, const physx::PxVehicleChassisData& chassisData, const int wheelsCount)
{
	std::vector<physx::PxConvexMesh*> wheelMeshes;
	for (auto i = 0; i < wheelsCount; i++)
	{
		wheelMeshes.emplace_back(PhysicsMeshGenerator::CreateCylinder(*physics, *cooking,
			vehicleComponent.Wheels[i]->Width, vehicleComponent.Wheels[i]->Radius));
	}

	const auto vehicleActor = physics->createRigidDynamic(physx::PxTransform(physx::PxIdentity));

	physx::PxFilterData wheelFilterData;
	PhysicsFilterHelper::SetSimulationFilter(wheelFilterData, CollisionWheel, CollisionWheelAgainst);

	physx::PxFilterData wheelQueryFilterData;
	PhysicsFilterHelper::SetSurfaceFilter(wheelQueryFilterData, Undrivable);

	for (auto i = 0; i < wheelsCount; i++)
	{
		physx::PxConvexMeshGeometry geometry(wheelMeshes[i]);
		auto wheelShape = physx::PxRigidActorExt::createExclusiveShape(*vehicleActor, geometry, *vehicleComponent.WheelsMaterials[i]->Material);
		wheelShape->setQueryFilterData(wheelQueryFilterData);
		wheelShape->setSimulationFilterData(wheelFilterData);
		wheelShape->setLocalPose(physx::PxTransform(physx::PxIdentity));
	}

	const physx::PxConvexMeshGeometry vehicleGeometry(vehicleMesh);
	auto vehicleShape = physx::PxRigidActorExt::createExclusiveShape(*vehicleActor, vehicleGeometry, *vehicleMaterialComponent.Material);
	vehicleShape->setLocalPose(CalculateChassisOffset(vehicleComponent));

	vehicleActor->setMass(chassisData.mMass);
	vehicleActor->setMassSpaceInertiaTensor(chassisData.mMOI);
	vehicleActor->setCMassLocalPose(physx::PxTransform(chassisData.mCMOffset, physx::PxQuat(physx::PxIdentity)));

	return vehicleActor;
}

std::vector<physx::PxVec3> VehicleResourcesGenerator::GenerateWheelsOffsets(const TransformComponent& vehicleTransformComponent, 
	const VehicleComponent& vehicleComponent, const int wheelsCount)
{
	std::vector<physx::PxVec3> offsets;

	for (auto i = 0; i < wheelsCount; i++)
	{
		const auto wheelPosition = TransformLogic::GetPosition(*vehicleComponent.WheelTransform[i]);
		const auto vehiclePosition = TransformLogic::GetPosition(vehicleTransformComponent);

		physx::PxVec3 offset(
			wheelPosition.x - vehiclePosition.x,
			wheelPosition.y - vehiclePosition.y,
			wheelPosition.z - vehiclePosition.z
		);

		offsets.emplace_back(offset);
	}

	return offsets;
}

physx::PxVehicleWheelsSimData* VehicleResourcesGenerator::CreateWheelsSimData(const VehicleComponent& vehicleComponent, 
	const int wheelsCount, const physx::PxVec3& vehicleCenterOfMassOffset, const std::vector<physx::PxVec3>& wheelsOffset)
{
	auto wheelsSimData = physx::PxVehicleWheelsSimData::allocate(wheelsCount);

	std::vector<physx::PxVehicleWheelData> wheels;
	for (auto i = 0; i < wheelsCount; i++)
	{
		wheels.emplace_back(physx::PxVehicleWheelData());
		wheels[i].mMass = vehicleComponent.Wheels[i]->Mass;
		wheels[i].mMOI = CalculateWheelMOI(vehicleComponent.Wheels[i]->Mass, vehicleComponent.Wheels[i]->Radius);
		wheels[i].mRadius = vehicleComponent.Wheels[i]->Radius;
		wheels[i].mWidth = vehicleComponent.Wheels[i]->Width;
		wheels[i].mMaxBrakeTorque = vehicleComponent.MaxBrakeTorque;
	}

	wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = vehicleComponent.MaxHandBrakeTorque;
	wheels[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = vehicleComponent.MaxHandBrakeTorque;
	wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = vehicleComponent.MaxSteer;
	wheels[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = vehicleComponent.MaxSteer;

	std::vector<physx::PxVehicleTireData> tires;
	for (auto i = 0; i < wheelsCount; i++)
	{
		tires.emplace_back(physx::PxVehicleTireData());

		tires[i].mType = vehicleComponent.Wheels[i]->TireType;
		tires[i].mLatStiffX *= vehicleComponent.Wheels[i]->LatStiffXMultiplier;
		tires[i].mLatStiffY *= vehicleComponent.Wheels[i]->LatStiffYMultiplier;
	}

	std::vector<physx::PxVehicleSuspensionData> suspensions;

	std::vector<float> suspensionSprungMasses(wheelsCount);
	PxVehicleComputeSprungMasses(wheelsCount, wheelsOffset.data(), vehicleCenterOfMassOffset, 
		vehicleComponent.Mass, 1, suspensionSprungMasses.data());

	for (auto i = 0; i < wheelsCount; i++)
	{
		suspensions.emplace_back(physx::PxVehicleSuspensionData());
		suspensions[i].mMaxCompression = vehicleComponent.Wheels[i]->SuspensionMaxCompression;
		suspensions[i].mMaxDroop = vehicleComponent.Wheels[i]->SuspensionMaxDroop;
		suspensions[i].mSpringStrength = vehicleComponent.Wheels[i]->SuspensionSpringStrength;
		suspensions[i].mSpringDamperRate = vehicleComponent.Wheels[i]->SuspensionSpringDamperRate;
		suspensions[i].mSprungMass = suspensionSprungMasses[i];
	}

	for (auto i = 0; i < wheelsCount; i+=2)
	{
		suspensions[i].mCamberAtRest = vehicleComponent.Wheels[i]->CamberAngleAtRest;
		suspensions[i].mCamberAtMaxDroop = vehicleComponent.Wheels[i]->CamberAngleAtMaxDroop;
		suspensions[i].mCamberAtMaxCompression = vehicleComponent.Wheels[i]->CamberAngleAtMaxCompression;

		suspensions[i+1].mCamberAtRest = -vehicleComponent.Wheels[i]->CamberAngleAtRest;
		suspensions[i+1].mCamberAtMaxDroop = -vehicleComponent.Wheels[i]->CamberAngleAtMaxDroop;
		suspensions[i+1].mCamberAtMaxCompression = -vehicleComponent.Wheels[i]->CamberAngleAtMaxCompression;
	}

	std::vector<physx::PxVec3> suspensionTravelDirections(wheelsCount);
	std::vector<physx::PxVec3> wheelCentreCMOffsets(wheelsCount);
	std::vector<physx::PxVec3> suspensionForceAppCMOffsets(wheelsCount);
	std::vector<physx::PxVec3> tireForceAppCMOffsets(wheelsCount);

	for (auto i = 0; i < wheelsCount; i++)
	{
		suspensionTravelDirections[i] = physx::PxVec3(0.0f, -1.0f, 0.0f);
		wheelCentreCMOffsets[i] = wheelsOffset[i] - vehicleCenterOfMassOffset;
		suspensionForceAppCMOffsets[i] = physx::PxVec3(wheelCentreCMOffsets[i].x, vehicleComponent.Wheels[i]->SuspensionForceApplicationPoinOffset, wheelCentreCMOffsets[i].z);
		tireForceAppCMOffsets[i] = physx::PxVec3(wheelCentreCMOffsets[i].x, vehicleComponent.Wheels[i]->TireForceApplicationPoinOffset, wheelCentreCMOffsets[i].z);
	}

	physx::PxFilterData filterData;
	PhysicsFilterHelper::SetSurfaceFilter(filterData, Undrivable);

	for (auto i = 0; i < wheelsCount; i++)
	{
		wheelsSimData->setWheelData(i, wheels[i]);
		wheelsSimData->setTireData(i, tires[i]);
		wheelsSimData->setSuspensionData(i, suspensions[i]);
		wheelsSimData->setSuspTravelDirection(i, suspensionTravelDirections[i]);
		wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
		wheelsSimData->setSuspForceAppPointOffset(i, suspensionForceAppCMOffsets[i]);
		wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
		wheelsSimData->setSceneQueryFilterData(i, filterData);
		wheelsSimData->setWheelShapeMapping(i, physx::PxI32(i));
	}

	physx::PxVehicleAntiRollBarData antiRollBarFront;
	antiRollBarFront.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT;
	antiRollBarFront.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT;
	antiRollBarFront.mStiffness = vehicleComponent.AntiRollBarStiffness;
	wheelsSimData->addAntiRollBarData(antiRollBarFront);

	physx::PxVehicleAntiRollBarData antiRollBarRear;
	antiRollBarRear.mWheel0 = physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT;
	antiRollBarRear.mWheel1 = physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT;
	antiRollBarRear.mStiffness = vehicleComponent.AntiRollBarStiffness;
	wheelsSimData->addAntiRollBarData(antiRollBarRear);

	return wheelsSimData;
}

physx::PxVehicleDriveSimData4W VehicleResourcesGenerator::CreateDriveSimData(const VehicleComponent& vehicleComponent,
	const physx::PxVehicleWheelsSimData* wheelsSimData)
{
	physx::PxVehicleDriveSimData4W driveSimData;

	physx::PxVehicleDifferential4WData differential;
	if (vehicleComponent.DifferentialType == "LIMITED_SLIP_4W")
		differential.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
	else if (vehicleComponent.DifferentialType == "LIMITED_SLIP_FRONT")
		differential.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_FRONTWD;
	else if (vehicleComponent.DifferentialType == "LIMITED_SLIP_REAR")
		differential.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_REARWD;
	else if (vehicleComponent.DifferentialType == "OPEN_4W")
		differential.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_OPEN_4WD;
	else if (vehicleComponent.DifferentialType == "OPEN_FRONT")
		differential.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_OPEN_FRONTWD;
	else if (vehicleComponent.DifferentialType == "OPEN_REAR")
		differential.mType = physx::PxVehicleDifferential4WData::eDIFF_TYPE_OPEN_REARWD;
	driveSimData.setDiffData(differential);

	physx::PxVehicleEngineData engine;
	engine.mPeakTorque = vehicleComponent.PeakTorque;
	engine.mMaxOmega = vehicleComponent.MaxOmega;
	engine.mMOI = vehicleComponent.EngineMomentOfInertia;
	driveSimData.setEngineData(engine);

	physx::PxVehicleGearsData gears;
	gears.mSwitchTime = vehicleComponent.GearsSwitchTime;
	driveSimData.setGearsData(gears);

	physx::PxVehicleClutchData clutch;
	if (vehicleComponent.ClutchAccuracyMode == "BEST_POSSIBLE")
		clutch.mAccuracyMode = physx::PxVehicleClutchAccuracyMode::eBEST_POSSIBLE;
	else if (vehicleComponent.ClutchAccuracyMode == "ESTIMATE")
		clutch.mAccuracyMode = physx::PxVehicleClutchAccuracyMode::eESTIMATE;
	clutch.mStrength = vehicleComponent.ClutchStrength;
	driveSimData.setClutchData(clutch);

	physx::PxVehicleAckermannGeometryData ackermann;
	ackermann.mAccuracy = vehicleComponent.AckermannAccuracy;
	ackermann.mAxleSeparation =
		wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z - 
		wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT).z;
	ackermann.mFrontWidth =
		wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x -
		wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
	ackermann.mRearWidth =
		wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
		wheelsSimData->getWheelCentreOffset(physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
	driveSimData.setAckermannGeometryData(ackermann);

	return driveSimData;
}

void VehicleResourcesGenerator::SetInitialTransform(const TransformComponent& vehicleTransformComponent,
	physx::PxVehicleDrive4W* vehicle)
{
	const auto directPosition = TransformLogic::GetPosition(vehicleTransformComponent);
	const auto directRotation = TransformLogic::GetRotationEuler(vehicleTransformComponent);

	const auto position = physx::PxVec3(directPosition.x, directPosition.y, directPosition.z);
	const auto rotation = PhysicsUnitConversion::DirectEulerToPhysicsQuaternion(directRotation);

	vehicle->getRigidDynamicActor()->setGlobalPose(physx::PxTransform(position, rotation));
	vehicle->getRigidDynamicActor()->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
}

physx::PxTransform VehicleResourcesGenerator::CalculateChassisOffset(const VehicleComponent& vehicleComponent)
{
	const physx::PxVec3 position(	vehicleComponent.ConvexPositionOffsetVector.x, 
									vehicleComponent.ConvexPositionOffsetVector.y,
									vehicleComponent.ConvexPositionOffsetVector.z);
	
	const auto rotation = PhysicsUnitConversion::DirectEulerToPhysicsQuaternion(DirectX::XMFLOAT3(	vehicleComponent.ConvexRotationOffsetVector.x,
																									vehicleComponent.ConvexRotationOffsetVector.y,
																									vehicleComponent.ConvexRotationOffsetVector.z));
	
	return physx::PxTransform(position, rotation);
}
