#pragma once
#include <PxPhysicsAPI.h>

struct RigidbodyDynamicSphereComponent final
{
	float Radius;
	float Density;
	DirectX::XMFLOAT3 PositionOffsetVector;
	DirectX::XMFLOAT3 RotationOffsetVector;

	physx::PxRigidDynamic* Body;
};
