#include "PhysicsUnitConversion.h"
#include <cmath>

physx::PxQuat PhysicsUnitConversion::DirectEulerToPhysicsQuaternion(const DirectX::XMFLOAT3 rotationEuler)
{
	const auto directQuat = DirectX::XMQuaternionRotationRollPitchYaw(rotationEuler.x, rotationEuler.y, rotationEuler.z);
	DirectX::XMFLOAT4 storedValue{};
	XMStoreFloat4(&storedValue, directQuat);

	return physx::PxQuat(storedValue.x, storedValue.y, storedValue.z, storedValue.w);
}

DirectX::XMFLOAT3 PhysicsUnitConversion::PhysicsQuaternionToDirectEuler(const physx::PxQuat rotationQuat)
{
	const auto quatSquareX = rotationQuat.x * rotationQuat.x;
	const auto quatSquareY = rotationQuat.y * rotationQuat.y;
	const auto quatSquareZ = rotationQuat.z * rotationQuat.z;
	const auto quatSquareW = rotationQuat.w * rotationQuat.w;

	const auto roll = atan2f(	2.0f * (rotationQuat.w * rotationQuat.x + rotationQuat.y * rotationQuat.z),
								1.0 - 2.0f * (quatSquareX + quatSquareY));

	const auto prePitch =		2.0f * (rotationQuat.w * rotationQuat.y - rotationQuat.z * rotationQuat.x);
	const auto pitch = fabs(prePitch) >= 1 ? copysign(M_PI_2, prePitch) : asinf(prePitch);

	const auto yaw = atan2f(	2.0f * (rotationQuat.w * rotationQuat.z + rotationQuat.x * rotationQuat.y), 
								1.0f - 2.0f * (quatSquareY + quatSquareZ));

	return DirectX::XMFLOAT3(roll, pitch, yaw);
}
