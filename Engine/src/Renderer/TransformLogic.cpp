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

void TransformLogic::SetPositionAndRotationEuler(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot, TransformComponent& transformComponent)
{
	const auto translationMatrix = DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&pos));
	const auto rotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rot));

	transformComponent.WorldMatrix = rotationMatrix * translationMatrix;
}

void TransformLogic::SetPosition(const DirectX::XMVECTOR& pos, TransformComponent& transformComponent)
{
	DirectX::XMVECTOR scaleVector{};
	DirectX::XMVECTOR rotationVector{};
	DirectX::XMVECTOR translationVector{};

	XMMatrixDecompose(&scaleVector, &rotationVector, &translationVector, transformComponent.WorldMatrix);

	translationVector = pos;

	const auto scaleMatrix = DirectX::XMMatrixScalingFromVector(scaleVector);
	const auto rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotationVector);
	const auto translationMatrix = DirectX::XMMatrixTranslationFromVector(translationVector);

	transformComponent.WorldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
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
	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, transformComponent.WorldMatrix);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&worldMatrixFloats._11, matrixTranslation, matrixRotation, matrixScale);

	matrixRotation[0] = x;
	matrixRotation[1] = y;
	matrixRotation[2] = z;

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &worldMatrixFloats._11);
	transformComponent.WorldMatrix = XMLoadFloat4x4(&worldMatrixFloats);
}

void TransformLogic::GetPosition(float* x, float* y, float* z, const TransformComponent& transformComponent)
{
	const auto position = GetPosition(transformComponent);

	if (x != nullptr)
		*x = position.x;
	if (y != nullptr)
		*y = position.y;
	if (z != nullptr)
		*z = position.z;
}

DirectX::XMFLOAT3 TransformLogic::GetPosition(const TransformComponent& transformComponent)
{
	DirectX::XMVECTOR scaleVector{};
	DirectX::XMVECTOR rotationVector{};
	DirectX::XMVECTOR translationVector{};

	XMMatrixDecompose(&scaleVector, &rotationVector, &translationVector, transformComponent.WorldMatrix);

	DirectX::XMFLOAT3 position{};
	XMStoreFloat3(&position, translationVector);

	return position;
}

void TransformLogic::AdjustPosition(const DirectX::XMVECTOR& pos, TransformComponent& transformComponent)
{
	DirectX::XMVECTOR scaleVector{};
	DirectX::XMVECTOR rotationVector{};
	DirectX::XMVECTOR translationVector{};

	XMMatrixDecompose(&scaleVector, &rotationVector, &translationVector, transformComponent.WorldMatrix);

	translationVector = DirectX::XMVectorAdd(translationVector, pos);

	const auto scaleMatrix = DirectX::XMMatrixScalingFromVector(scaleVector);
	const auto rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotationVector);
	const auto translationMatrix = DirectX::XMMatrixTranslationFromVector(translationVector);

	transformComponent.WorldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
}

void TransformLogic::AdjustPosition(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	DirectX::XMFLOAT3 positionFloats(x, y, z);
	const auto pos = XMLoadFloat3(&positionFloats);

	AdjustPosition(pos, transformComponent);
}

void TransformLogic::AdjustRotation(const float x, const float y, const float z, TransformComponent& transformComponent)
{
	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, transformComponent.WorldMatrix);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&worldMatrixFloats._11, matrixTranslation, matrixRotation, matrixScale);

	matrixRotation[0] += x;
	matrixRotation[1] += y;
	matrixRotation[2] += z;

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &worldMatrixFloats._11);
	transformComponent.WorldMatrix = XMLoadFloat4x4(&worldMatrixFloats);
}

void TransformLogic::AdjustRotationEuler(const DirectX::XMVECTOR& rot, TransformComponent& transformComponent)
{
	DirectX::XMFLOAT4X4 worldMatrixFloats{};
	XMStoreFloat4x4(&worldMatrixFloats, transformComponent.WorldMatrix);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&worldMatrixFloats._11, matrixTranslation, matrixRotation, matrixScale);

	DirectX::XMFLOAT3 rotationFloats{};
	XMStoreFloat3(&rotationFloats, rot);

	matrixRotation[0] += rotationFloats.x;
	matrixRotation[1] += rotationFloats.y;
	matrixRotation[2] += rotationFloats.z;

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &worldMatrixFloats._11);
	transformComponent.WorldMatrix = XMLoadFloat4x4(&worldMatrixFloats);
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
