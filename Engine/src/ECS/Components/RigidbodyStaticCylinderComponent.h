#pragma once
#include <DirectXMath.h>
#include <PxPhysicsAPI.h>

struct RigidbodyStaticCylinderComponent final
{
	float Width;
	float Radius;
	DirectX::XMFLOAT3 PositionOffsetVector;
	DirectX::XMFLOAT3 RotationOffsetVector;
	
	physx::PxRigidStatic* Body;
};
