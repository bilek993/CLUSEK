#include "TransformLogic.h"
#include <imgui.h>
#include <ImGuizmo.h>

void TransformLogic::SetMatrix(const physx::PxMat44& matrix, TransformComponent& transformComponent)
{
	transformComponent.WorldMatrix = DirectX::XMMATRIX(
		matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
		matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
		matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
		matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]
	);
}

void TransformLogic::SetPositionAndRotation(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, TransformComponent& transformComponent)
{
	const auto translationMatrix = DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&pos));
	const auto rotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rot));

	transformComponent.WorldMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);
}

void TransformLogic::SetPosition(const DirectX::XMVECTOR& pos, TransformComponent& transformComponent)
{
	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, transformComponent.WorldMatrix);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&worldMatrixFloats._11, matrixTranslation, matrixRotation, matrixScale);

	DirectX::XMFLOAT3 positionFloats{};
	XMStoreFloat3(&positionFloats, pos);

	matrixTranslation[0] = positionFloats.x;
	matrixTranslation[1] = positionFloats.y;
	matrixTranslation[2] = positionFloats.z;

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &worldMatrixFloats._11);
	transformComponent.WorldMatrix = XMLoadFloat4x4(&worldMatrixFloats);
}

void TransformLogic::SetPosition(const DirectX::XMFLOAT3& pos, TransformComponent& transformComponent)
{
	SetPosition(XMLoadFloat3(&pos), transformComponent);
}

void TransformLogic::SetPosition(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	auto position = DirectX::XMFLOAT3(x, y, z);
	SetPosition(XMLoadFloat3(&position), transformComponent);
}

void TransformLogic::SetRotationEuler(const DirectX::XMVECTOR& rot, TransformComponent& transformComponent)
{
	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, transformComponent.WorldMatrix);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&worldMatrixFloats._11, matrixTranslation, matrixRotation, matrixScale);

	DirectX::XMFLOAT3 rotationFloats{};
	XMStoreFloat3(&rotationFloats, rot);

	matrixRotation[0] = rotationFloats.x;
	matrixRotation[1] = rotationFloats.y;
	matrixRotation[2] = rotationFloats.z;

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &worldMatrixFloats._11);
	transformComponent.WorldMatrix = XMLoadFloat4x4(&worldMatrixFloats);
}

void TransformLogic::SetRotationEuler(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	auto rotation = DirectX::XMFLOAT3(x, y, z);
	SetRotationEuler(XMLoadFloat3(&rotation), transformComponent);
}

void TransformLogic::GetPosition(float* x, float* y, float* z, const TransformComponent& transformComponent)
{
	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, transformComponent.WorldMatrix);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&worldMatrixFloats._11, matrixTranslation, matrixRotation, matrixScale);

	if (x != nullptr)
		*x = matrixTranslation[0];
	if (y != nullptr)
		*y = matrixTranslation[1];
	if (z != nullptr)
		*z = matrixTranslation[2];
}

DirectX::XMFLOAT3 TransformLogic::GetPosition(const TransformComponent& transformComponent)
{
	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, transformComponent.WorldMatrix);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&worldMatrixFloats._11, matrixTranslation, matrixRotation, matrixScale);

	return DirectX::XMFLOAT3(matrixTranslation);
}

void TransformLogic::AdjustPosition(const DirectX::XMVECTOR& pos, TransformComponent& transformComponent)
{
	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, transformComponent.WorldMatrix);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&worldMatrixFloats._11, matrixTranslation, matrixRotation, matrixScale);

	DirectX::XMFLOAT3 positionFloats{};
	XMStoreFloat3(&positionFloats, pos);

	matrixTranslation[0] += positionFloats.x;
	matrixTranslation[1] += positionFloats.y;
	matrixTranslation[2] += positionFloats.z;
		
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &worldMatrixFloats._11);
	transformComponent.WorldMatrix = XMLoadFloat4x4(&worldMatrixFloats);
}

void TransformLogic::AdjustPosition(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 positionFloats(x, y, z);
	const auto pos = XMLoadFloat3(&positionFloats);

	AdjustPosition(pos, transformComponent);
}

void TransformLogic::AdjustRotation(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 rotationFloats(x, y, z);
	const auto rotation = XMLoadFloat3(&rotationFloats);
	AdjustRotationEuler(rotation, transformComponent);
}

void TransformLogic::AdjustRotationEuler(const DirectX::XMVECTOR& rot, TransformComponent& transformComponent)
{
	const auto currentRotationFloats = GetRotationEuler(transformComponent);
	auto currentRotation = XMLoadFloat3(&currentRotationFloats);

	currentRotation = DirectX::XMVectorAdd(currentRotation, rot);
	SetRotationEuler(currentRotation, transformComponent);
}

void TransformLogic::GetRotation(float* x, float* y, float* z, const TransformComponent& transformComponent)
{
	const auto rotation = GetRotationEuler(transformComponent);

	if (x != nullptr)
		*x = rotation.x;
	if (y != nullptr)
		*y = rotation.y;
	if (z != nullptr)
		*z = rotation.z;
}

void TransformLogic::GetRotation(float* x, float* y, float* z, float* w, const TransformComponent& transformComponent)
{
	const auto rotation = XMQuaternionRotationMatrix(transformComponent.WorldMatrix);

	DirectX::XMFLOAT4 storedValue{};
	XMStoreFloat4(&storedValue, rotation);

	if (x != nullptr)
		*x = storedValue.x;
	if (y != nullptr)
		*y = storedValue.y;
	if (z != nullptr)
		*z = storedValue.z;
	if (w != nullptr)
		*w = storedValue.w;
}

DirectX::XMFLOAT3 TransformLogic::GetRotationEuler(const TransformComponent& transformComponent)
{
	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, transformComponent.WorldMatrix);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&worldMatrixFloats._11, matrixTranslation, matrixRotation, matrixScale);

	return DirectX::XMFLOAT3(matrixRotation);
}

DirectX::XMFLOAT4 TransformLogic::GetRotationQuat(const TransformComponent& transformComponent)
{
	const auto rotation = XMQuaternionRotationMatrix(transformComponent.WorldMatrix);

	DirectX::XMFLOAT4 storedValue{};
	XMStoreFloat4(&storedValue, rotation);
	return storedValue;
}
