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
	std::string ModelId;
	std::string MaterialId;

	std::shared_ptr<std::vector<GrassMesh>> Meshes;
};
