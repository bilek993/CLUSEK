#pragma once
#include "PxPhysicsAPI.h"

class VehicleSceneQueryData final
{
public:
	static VehicleSceneQueryData* Allocate(int maxNumVehicles, int maxNumWheelsPerVehicle, 
		int maxNumHitPointsPerWheel, int numVehiclesInBatch, physx::PxAllocatorCallback& allocator);

	void Free(physx::PxAllocatorCallback& allocator);

	static physx::PxBatchQuery* SetUpBatchedSceneQuery(int batchId, const VehicleSceneQueryData& vehicleSceneQueryData, 
		physx::PxScene* scene);

	physx::PxSweepQueryResult* GetSweepResultBuffer(int batchId) const;
	int GetSweepResultSize() const;

private:
	int NumberOfQueriesPerBatch = 0;
	int NumberOfHitsResultsPerQuery = 0;

	physx::PxSweepQueryResult* SweepResults = nullptr;
	physx::PxSweepHit* SweepHitBuffer = nullptr;
};