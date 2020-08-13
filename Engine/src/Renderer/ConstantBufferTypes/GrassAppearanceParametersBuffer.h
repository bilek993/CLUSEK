#pragma once

struct GrassAppearanceParametersBuffer final
{
	float AlphaDiscardPoint;
	float AlbedoInterpolationRatio;
	float NormalInterpolationRatio;
	float RoughnessInterpolationTarget;
	float RoughnessInterpolationRatio;
	float MetalicInterpolationTarget;
	float MetalicInterpolationRatio;
	float OcclusionValue;
};