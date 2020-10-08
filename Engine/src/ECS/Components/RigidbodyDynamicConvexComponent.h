#pragma once
#include <DirectXMath.h>
#include <PxPhysicsAPI.h>
#include <string>

struct RigidbodyDynamicConvexComponent final
{
	std::string ConvexModelId;
	float Density;
	DirectX::XMFLOAT3 PositionOffsetVector;
	DirectX::XMFLOAT3 RotationOffsetVector;

	physx::PxRigidDynamic* Body;
};
