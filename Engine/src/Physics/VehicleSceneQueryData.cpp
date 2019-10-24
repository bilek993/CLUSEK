#include "VehicleSceneQueryData.h"

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

	auto sqData = new(buffer) VehicleSceneQueryData();
	sqData->NumberOfQueriesPerBatch = numVehiclesInBatch * maxNumWheelsPerVehicle;
	sqData->NumberOfHitsResultsPerQuery = maxNumHitPointsPerWheel;
	buffer += sqDataSize;

	sqData->SweepResults = reinterpret_cast<physx::PxSweepQueryResult*>(buffer);
	buffer += sweepResultSize;

	sqData->SweepHitBuffer = reinterpret_cast<physx::PxSweepHit*>(buffer);
	buffer += sweepHitSize;

	for (auto i = 0; i < maxNumWheels; i++)
		new(sqData->SweepResults + i) physx::PxSweepQueryResult();

	for (auto i = 0; i < maxNumHitPoints; i++)
		new(sqData->SweepHitBuffer + i) physx::PxSweepHit();

	return sqData;
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
