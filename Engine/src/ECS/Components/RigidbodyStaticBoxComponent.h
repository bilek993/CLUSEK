#pragma once
#include <DirectXMath.h>
#include <PxPhysicsAPI.h>

struct RigidbodyStaticBoxComponent final
{
	float Width;
	float Height;
	float Depth;
	DirectX::XMFLOAT3 PositionOffsetVector;
	DirectX::XMFLOAT3 RotationOffsetVector;

	physx::PxRigidStatic* Body;
};
