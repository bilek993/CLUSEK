#pragma once
#include "PxPhysicsAPI.h"
#include <DirectXMath.h>

class PhysicsUnitConversion final
{
public:
	static physx::PxQuat DirectEulerToPhysicsQuaternion(DirectX::XMFLOAT3 rotation);
};
