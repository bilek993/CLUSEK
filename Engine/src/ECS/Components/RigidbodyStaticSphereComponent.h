#pragma once
#include <PxPhysicsAPI.h>

struct RigidbodyStaticSphereComponent final
{
	float Radius;
	DirectX::XMFLOAT3 PositionOffsetVector;
	DirectX::XMFLOAT3 RotationOffsetVector;

	physx::PxRigidStatic* Body;
};
