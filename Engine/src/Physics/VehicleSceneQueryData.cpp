#include "VehicleSceneQueryData.h"
#include "SurfaceFilterTypes.h"

VehicleSceneQueryData* VehicleSceneQueryData::Allocate(const int maxNumVehicles, const int maxNumWheelsPerVehicle,
	const int maxNumHitPointsPerWheel, const int numVehiclesInBatch, physx::PxAllocatorCallback& allocator)
{
	const auto sqDataSize = ((sizeof(VehicleSceneQueryData) + 15) & ~15);

	const auto maxNumWheels = maxNumVehicles * maxNumWheelsPerVehicle;
	const auto raycastResultSize = ((sizeof(physx::PxRaycastQueryResult)*maxNumWheels + 15) & ~15);
	const auto sweepResultSize = ((sizeof(physx::PxSweepQueryResult)*maxNumWheels + 15) & ~15);

	const auto maxNumHitPoints = maxNumWheels * maxNumHitPointsPerWheel;
	const auto raycastHitSize = ((sizeof(physx::PxRaycastHit)*maxNumHitPoints + 15) & ~15);
	const auto sweepHitSize = ((sizeof(physx::PxSweepHit)*maxNumHitPoints + 15) & ~15);

	const auto size = sqDataSize + raycastResultSize + raycastHitSize + sweepResultSize + sweepHitSize;
	auto buffer = static_cast<physx::PxU8*>(allocator.allocate(size, nullptr, nullptr, 0));

	const auto sceneQueryData = new(buffer) VehicleSceneQueryData();
	sceneQueryData->NumberOfQueriesPerBatch = numVehiclesInBatch * maxNumWheelsPerVehicle;
	sceneQueryData->NumberOfHitsResultsPerQuery = maxNumHitPointsPerWheel;
	buffer += sqDataSize;

	sceneQueryData->SweepResults = reinterpret_cast<physx::PxSweepQueryResult*>(buffer);
	buffer += sweepResultSize;

	sceneQueryData->SweepHitBuffer = reinterpret_cast<physx::PxSweepHit*>(buffer);
	buffer += sweepHitSize;

	for (auto i = 0; i < maxNumWheels; i++)
		new(sceneQueryData->SweepResults + i) physx::PxSweepQueryResult();

	for (auto i = 0; i < maxNumHitPoints; i++)
		new(sceneQueryData->SweepHitBuffer + i) physx::PxSweepHit();

	sceneQueryData->PreFilterShader = [](physx::PxFilterData filterData0, const physx::PxFilterData filterData1, 
		const void* constantBlock, physx::PxU32 constantBlockSize, physx::PxHitFlags& queryFlags)
	{
		return (0 == (filterData1.word3 & Drivable)) ? physx::PxQueryHitType::eNONE : physx::PxQueryHitType::eTOUCH;
	};
	sceneQueryData->PostFilterShader = [](physx::PxFilterData filterData0, physx::PxFilterData filterData1, 
		const void* constantBlock, physx::PxU32 constantBlockSize, const physx::PxQueryHit& hit)
	{ 
		return static_cast<const physx::PxSweepHit&>(hit).hadInitialOverlap() ? physx::PxQueryHitType::eNONE : physx::PxQueryHitType::eTOUCH;
	};

	return sceneQueryData;
}

void VehicleSceneQueryData::Free(physx::PxAllocatorCallback& allocator)
{
	allocator.deallocate(this);
}

physx::PxBatchQuery* VehicleSceneQueryData::SetUpBatchedSceneQuery(const int batchId,
	const VehicleSceneQueryData& vehicleSceneQueryData, physx::PxScene* scene)
{
	const auto maxNumQueriesInBatch = vehicleSceneQueryData.NumberOfQueriesPerBatch;
	const auto maxNumHitResultsInBatch = vehicleSceneQueryData.NumberOfQueriesPerBatch * vehicleSceneQueryData.NumberOfHitsResultsPerQuery;

	physx::PxBatchQueryDesc sqDesc(maxNumQueriesInBatch, maxNumQueriesInBatch, 0);

	sqDesc.queryMemory.userSweepResultBuffer = vehicleSceneQueryData.SweepResults + batchId * maxNumQueriesInBatch;
	sqDesc.queryMemory.userSweepTouchBuffer = vehicleSceneQueryData.SweepHitBuffer + batchId * maxNumHitResultsInBatch;
	sqDesc.queryMemory.sweepTouchBufferSize = maxNumHitResultsInBatch;

	sqDesc.preFilterShader = vehicleSceneQueryData.PreFilterShader;
	sqDesc.postFilterShader = vehicleSceneQueryData.PostFilterShader;

	return scene->createBatchQuery(sqDesc);
}

physx::PxSweepQueryResult* VehicleSceneQueryData::GetSweepResultBuffer(const int batchId) const
{
	return SweepResults + batchId * NumberOfQueriesPerBatch;
}

int VehicleSceneQueryData::GetSweepResultSize() const
{
	return NumberOfQueriesPerBatch;
}
