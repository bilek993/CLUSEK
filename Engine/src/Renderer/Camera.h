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
	DirectX::XMFLOAT3 GetPositionFloat3() const;
	const DirectX::XMVECTOR &GetRotationVector() const;
	DirectX::XMFLOAT3 GetRotationFloat() const;

	void SetPosition(const DirectX::XMVECTOR &pos);
	void SetPosition(float x, float y, float z);
	void SetRotation(const DirectX::XMVECTOR &rot);
	void SetRotation(float x, float y, float z);
private:
	void UpdateViewMatrix();

	DirectX::XMMATRIX ViewMatrix;
	DirectX::XMMATRIX ProjectionMatrix;

	DirectX::XMVECTOR PositionVector;
	DirectX::XMVECTOR RotationVector;

	const DirectX::XMVECTOR ForwardVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR UpVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
};
