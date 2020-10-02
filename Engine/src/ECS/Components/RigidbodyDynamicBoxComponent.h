#pragma once
#include <PxPhysicsAPI.h>

struct RigidbodyDynamicBoxComponent final
{
	float Width;
	float Height;
	float Depth;
	float Density;
	DirectX::XMFLOAT3 PositionOffsetVector;
	DirectX::XMFLOAT3 RotationOffsetVector;

	physx::PxRigidDynamic* Body;
};
