#pragma once
#include <DirectXMath.h>
#include <PxPhysicsAPI.h>
#include <string>

struct RigidbodyStaticConvexComponent final
{
	std::string ConvexModelId;
	DirectX::XMFLOAT3 PositionOffsetVector;
	DirectX::XMFLOAT3 RotationOffsetVector;

	physx::PxRigidStatic* Body;
};
