#pragma once
#include "PxPhysicsAPI.h"

struct RigidbodyStaticPlaneComponent final
{
	float NormalX;
	float NormalY;
	float NormalZ;
	float Distance;

	physx::PxRigidStatic* Body;
};
