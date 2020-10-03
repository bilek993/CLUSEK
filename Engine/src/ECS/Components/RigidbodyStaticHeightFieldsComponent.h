#pragma once
#include <DirectXMath.h>
#include <geometry/PxHeightFieldSample.h>
#include <geometry/PxHeightField.h>

struct RigidbodyStaticHeightFieldsComponent final
{
	DirectX::XMFLOAT3 PositionOffsetVector;
	DirectX::XMFLOAT3 RotationOffsetVector;
	physx::PxHeightFieldSample* HeightFieldSample;
	physx::PxHeightField* HeightField;
	physx::PxShape* Shape;
};
