#pragma once
#include <vector>
#include "../../Renderer/GrassMesh.h"

struct GrassComponent final
{
	float AlphaDiscardPoint = 0.5f;
	float AlbedoInterpolationRatio = 0.5f;
	float NormalInterpolationRatio = 0.5f;
	float RoughnessInterpolationTarget = 0.5f;
	float RoughnessInterpolationRatio = 0.5f;
	float MetalicInterpolationTarget = 0.0f;
	float MetalicInterpolationRatio = 0.5f;
	float OcclusionValue = 1.0f;
	float WindVertexAffectHeight = 0.5f;
	float GlobalWindSpeed = 0.001f;
	float GlobalWindScale = 0.5f;
	float LocalWindSpeed = 0.001f;
	DirectX::XMFLOAT3 LocalWindScale{0.5f, 0.175f, 0.5f};
	std::string ModelId;
	std::string MaterialId;

	std::shared_ptr<std::vector<GrassMesh>> Meshes;
};
