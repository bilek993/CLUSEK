#pragma once
#include <DirectXMath.h>
#include <vector>
#include <PxPhysicsAPI.h>
#include "../../Renderer/Mesh.h"

struct RoadComponent final
{
	std::vector<DirectX::XMFLOAT2> MeshVertices{};
	std::vector<DirectX::XMFLOAT3> Points{};
	int Resolution = 10000;
	float SplitDistance = 10.0f;
	std::string MaterialId;
	float TextureScaling = 1.0f;
	DirectX::XMFLOAT3 PositionOffsetVector;
	DirectX::XMFLOAT3 RotationOffsetVector;

	bool InitializedOnce = true;
	std::vector<DirectX::XMVECTOR> CalculatedSupportPoints;
	Mesh Mesh;
	physx::PxRigidStatic* PhysicsBody;
};
