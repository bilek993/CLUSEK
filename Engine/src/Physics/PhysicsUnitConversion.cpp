#include "PhysicsUnitConversion.h"

physx::PxQuat PhysicsUnitConversion::DirectEulerToPhysicsQuaternion(DirectX::XMFLOAT3 rotation)
{
	const auto directQuat = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	DirectX::XMFLOAT4 storedValue{};
	XMStoreFloat4(&storedValue, directQuat);

	return physx::PxQuat(storedValue.x, storedValue.y, storedValue.z, storedValue.w);
}
