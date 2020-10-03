#pragma once
#include <DirectXMath.h>
#include <PxPhysicsAPI.h>

struct RigidbodyDynamicCylinderComponent final
{
	float Width;
	float Radius;
	float Density;
	DirectX::XMFLOAT3 PositionOffsetVector;
	DirectX::XMFLOAT3 RotationOffsetVector;

	physx::PxRigidDynamic* Body;
};
