#pragma once
#include <DirectXMath.h>
#include <geometry/PxHeightFieldSample.h>
#include <geometry/PxHeightField.h>

struct RigidbodyStaticHeightFieldsComponent final
{
	physx::PxHeightFieldSample* HeightFieldSample;
	physx::PxHeightField* HeightField;
	physx::PxShape* Shape;
};
