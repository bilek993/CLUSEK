#pragma once
#include <DirectXMath.h>
#include "LodMode.h"

struct DynamicRenderSettings final
{
	DirectX::XMFLOAT3 DirectionalLightColor;
	float DirectionalLightStrength;
	DirectX::XMFLOAT3 DirectionalLightDirection;

	float ClearColor[4] = { 0.0f, 0.75f, 1.0f, 1.0f };

	DirectX::XMFLOAT3 FogColor;
	float FogDensity;
	DirectX::XMFLOAT3 FogLightColor;
	float FogLightPower;
	float FogMinDistance;
	float SkyConstantValue;

	LodMode LevelOfDetailsMode = Automatic;
	float ForcedLodPercentage = 0.75f;

	bool GrassEnabled = true;
	float GrassGeneratorSurfaceWidth = 500.0f;
	float GrassGeneratorSurfaceHeight = 500.0f;
	float GrassGeneratorMaxCameraDistance = 100.0f;
	float MinGrassTranslationX = 0.0f;
	float MinGrassTranslationY = 0.0f;
	float MaxGrassTranslationX = 0.0f;
	float MaxGrassTranslationY = 0.0f;
	float GrassEndCascade0 = 0.0f;
	float GrassEndCascade1 = 0.0f;
	float GrassEnd = 0.0f;
};
