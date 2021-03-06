#pragma once
#include <DirectXMath.h>
#include <PxPhysicsAPI.h>

struct RigidbodyStaticCapsuleComponent final
{
	float Height;
	float Radius;
	DirectX::XMFLOAT3 PositionOffsetVector;
	DirectX::XMFLOAT3 RotationOffsetVector;

	physx::PxRigidStatic* Body;
};
