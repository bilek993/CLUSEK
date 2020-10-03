#pragma once
#include <DirectXMath.h>
#include <PxPhysicsAPI.h>

struct RigidbodyDynamicCapsuleComponent final
{
	float Height;
	float Radius;
	float Density;
	DirectX::XMFLOAT3 PositionOffsetVector;
	DirectX::XMFLOAT3 RotationOffsetVector;

	physx::PxRigidDynamic* Body;
};
