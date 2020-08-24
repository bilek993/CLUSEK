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
	float WindVertexAffectHeight;
	float GlobalWindSpeed;
	float GlobalWindScale;
	float LocalWindSpeed;
	DirectX::XMFLOAT3 LocalWindScale;
};