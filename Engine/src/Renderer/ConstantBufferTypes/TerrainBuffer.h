#pragma once
#include <DirectXMath.h>

struct TerrainBuffer final
{
	DirectX::XMVECTOR FrustumPlanes[6];
};
