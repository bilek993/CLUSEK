#pragma once
#include <PxPhysicsAPI.h>

struct RigidbodyStaticPlaneComponent final
{
	float NormalX;
	float NormalY;
	float NormalZ;
	float Distance;
	DirectX::XMFLOAT3 PositionOffsetVector;
	DirectX::XMFLOAT3 RotationOffsetVector;

	physx::PxRigidStatic* Body;
};
