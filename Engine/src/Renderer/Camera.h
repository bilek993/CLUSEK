#pragma once
#include <DirectXMath.h>

class Camera final
{
public:
	Camera();
	void SetCameraSettings(float fov, float aspectRatio, float nearZ, float farZ);

	const DirectX::XMMATRIX &GetViewMatrix() const;
	const DirectX::XMMATRIX &GetProjectionMatrix() const;

	const DirectX::XMVECTOR &GetPositionVector() const;
	const DirectX::XMVECTOR &GetRotationVector() const;

	const DirectX::XMVECTOR &GetForwardVector() const;
	const DirectX::XMVECTOR &GetRightVector() const;

	DirectX::XMFLOAT3 GetPositionFloat3() const;
	DirectX::XMFLOAT3 GetRotationFloat3() const;

	void SetPosition(const DirectX::XMVECTOR &pos);
	void SetPosition(float x, float y, float z);
	void SetRotation(const DirectX::XMVECTOR &rot);
	void SetRotation(float x, float y, float z);

	void AdjustPosition(const DirectX::XMVECTOR &pos);
	void AdjustPosition(float x, float y, float z);
	void AdjustRotation(const DirectX::XMVECTOR &rot);
	void AdjustRotation(float x, float y, float z);

	void LookAt(DirectX::XMFLOAT3 targetPosition);
private:
	void UpdateViewMatrix();

	DirectX::XMMATRIX ViewMatrix;
	DirectX::XMMATRIX ProjectionMatrix;

	DirectX::XMVECTOR PositionVector;
	DirectX::XMVECTOR RotationVector;

	DirectX::XMVECTOR VectorForward;
	DirectX::XMVECTOR VectorRight;

	static const DirectX::XMVECTOR FORWARD_VECTOR;
	static const DirectX::XMVECTOR UP_VECTOR;
	static const DirectX::XMVECTOR RIGHT_VECTOR;
};
