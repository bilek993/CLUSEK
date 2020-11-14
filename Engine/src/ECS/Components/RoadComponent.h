#pragma once
#include <DirectXMath.h>
#include <vector>

struct RoadComponent final
{
	std::vector<DirectX::XMFLOAT2> MeshVertices{};
	std::vector<DirectX::XMFLOAT3> Points{};
	int Resolution = 10000;
	float SplitDistance = 10.0f;
};
