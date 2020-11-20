#include <fstream>
#include <algorithm>
#include "ConfigLoader.h"
#include "../Utils/Logger.h"

ConfigData ConfigLoader::GetData()
{
	Logger::Debug("Preparing to open file '" + FilePath + "'...");
	std::ifstream cfgFile(FilePath);
	ConfigData data;

	if (cfgFile.is_open())
	{
		Logger::Debug("Reading configuration from file '" + FilePath + "'...");

		std::string line;

		while (getline(cfgFile, line)) 
		{
			line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

			if (line[0] == COMMENT_SIGN || line.empty())
				continue;

			const auto colonPosition = line.find(SEPARATOR_SIGN);
			auto key = line.substr(0, colonPosition);
			auto value = line.substr(colonPosition + 1);

			MAP_CONFIG_FIELD(key, LoggerEnabled, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, LoggerDestination, data, value);
			MAP_CONFIG_FIELD(key, LoggerLevel, data, std::stoi(value));

			MAP_CONFIG_FIELD(key, WindowClassName, data, value);
			MAP_CONFIG_FIELD(key, WindowTitle, data, value);
			MAP_CONFIG_FIELD(key, WindowWidth, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, WindowHeight, data, std::stoi(value));

			MAP_CONFIG_FIELD(key, ComInitializationMultithreaded, data, std::stoi(value));

			MAP_CONFIG_FIELD(key, EnableImGuiOnStart, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, PathToImGuiFont, data, value);

			MAP_CONFIG_FIELD(key, Fullscreen, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, SyncIntervals, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, SelectedAdapterId, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, RefreshRateNumerator, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, RefreshRateDenominator, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, MultisamplesCount, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, MultisamplesQuality, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, WireframeMode, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, ClearColorRed, data, std::stof(value));
			MAP_CONFIG_FIELD(key, ClearColorGreen, data, std::stof(value));
			MAP_CONFIG_FIELD(key, ClearColorBlue, data, std::stof(value));

			MAP_CONFIG_FIELD(key, DisableLoadingScreen, data, std::stoi(value))
			MAP_CONFIG_FIELD(key, PathToLoadingLogo, data, value);
			MAP_CONFIG_FIELD(key, LogoScale, data, std::stof(value));
			MAP_CONFIG_FIELD(key, DisableLoadingText, data, std::stoi(value))
			MAP_CONFIG_FIELD(key, PathToLoadingFont, data, value);
			MAP_CONFIG_FIELD(key, LoadingOffsetX, data, std::stof(value));
			MAP_CONFIG_FIELD(key, LoadingOffsetY, data, std::stof(value));

			MAP_CONFIG_FIELD(key, MainCameraFov, data, std::stof(value));
			MAP_CONFIG_FIELD(key, MaxRotationX, data, std::stof(value));
			MAP_CONFIG_FIELD(key, MinRotationX, data, std::stof(value));
			MAP_CONFIG_FIELD(key, MouseSpeedController, data, std::stof(value));
			MAP_CONFIG_FIELD(key, RotationSpeedController, data, std::stof(value));
			MAP_CONFIG_FIELD(key, MainCameraNearZ, data, std::stof(value));
			MAP_CONFIG_FIELD(key, MainCameraFarZ, data, std::stof(value));
			MAP_CONFIG_FIELD(key, MaxCameraSpeed, data, std::stof(value));

			MAP_CONFIG_FIELD(key, FogColorRed, data, std::stof(value));
			MAP_CONFIG_FIELD(key, FogColorGreen, data, std::stof(value));
			MAP_CONFIG_FIELD(key, FogColorBlue, data, std::stof(value));
			MAP_CONFIG_FIELD(key, FogDensity, data, std::stof(value));
			MAP_CONFIG_FIELD(key, FogLightColorRed, data, std::stof(value));
			MAP_CONFIG_FIELD(key, FogLightColorGreen, data, std::stof(value));
			MAP_CONFIG_FIELD(key, FogLightColorBlue, data, std::stof(value));
			MAP_CONFIG_FIELD(key, FogLightPower, data, std::stof(value));
			MAP_CONFIG_FIELD(key, FogMinDistance, data, std::stof(value));
			MAP_CONFIG_FIELD(key, SkyConstantValue, data, std::stof(value));

			MAP_CONFIG_FIELD(key, DirectionalLightColorRed, data, std::stof(value));
			MAP_CONFIG_FIELD(key, DirectionalLightColorGreen, data, std::stof(value));
			MAP_CONFIG_FIELD(key, DirectionalLightColorBlue, data, std::stof(value));
			MAP_CONFIG_FIELD(key, DirectionalLightStrength, data, std::stof(value));
			MAP_CONFIG_FIELD(key, DirectionalLightDirectionX, data, std::stof(value));
			MAP_CONFIG_FIELD(key, DirectionalLightDirectionY, data, std::stof(value));
			MAP_CONFIG_FIELD(key, DirectionalLightDirectionZ, data, std::stof(value));

			MAP_CONFIG_FIELD(key, ShadowsEnabled, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, EnableRealtimeShadowPreview, data, std::stoi(value))
			MAP_CONFIG_FIELD(key, ShadowsTextureSize, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, ShadowAlphaThreshold, data, std::stof(value));
			MAP_CONFIG_FIELD(key, ShadowCameraShiftNearZ, data, std::stof(value));
			MAP_CONFIG_FIELD(key, CascadeEnd0, data, std::stof(value));
			MAP_CONFIG_FIELD(key, CascadeEnd1, data, std::stof(value));
			MAP_CONFIG_FIELD(key, CascadeEnd2, data, std::stof(value));
			MAP_CONFIG_FIELD(key, CascadeEnd3, data, std::stof(value));
			MAP_CONFIG_FIELD(key, CascadeBias0, data, std::stof(value));
			MAP_CONFIG_FIELD(key, CascadeBias1, data, std::stof(value));
			MAP_CONFIG_FIELD(key, CascadeBias2, data, std::stof(value));
			MAP_CONFIG_FIELD(key, CascadeBias3, data, std::stof(value));

			MAP_CONFIG_FIELD(key, EnableAsyncTerrainGeneration, data, std::stoi(value));

			MAP_CONFIG_FIELD(key, GrassEnabled, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, MaxGrassRenderInstances, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, GrassGeneratorResolution, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, GrassGeneratorSurfaceWidth, data, std::stof(value));
			MAP_CONFIG_FIELD(key, GrassGeneratorSurfaceHeight, data, std::stof(value));
			MAP_CONFIG_FIELD(key, GrassGeneratorMaxCameraDistance, data, std::stof(value));
			MAP_CONFIG_FIELD(key, MinGrassTranslationX, data, std::stof(value));
			MAP_CONFIG_FIELD(key, MinGrassTranslationY, data, std::stof(value));
			MAP_CONFIG_FIELD(key, MaxGrassTranslationX, data, std::stof(value));
			MAP_CONFIG_FIELD(key, MaxGrassTranslationY, data, std::stof(value));
			MAP_CONFIG_FIELD(key, GrassEndCascade0, data, std::stof(value));
			MAP_CONFIG_FIELD(key, GrassEndCascade1, data, std::stof(value));
			MAP_CONFIG_FIELD(key, GrassEnd, data, std::stof(value));

			MAP_CONFIG_FIELD(key, CubemapGeneratedSize, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, EnableAsyncModelLoading, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, EnableAsyncTextureLoading, data, std::stoi(value));

			MAP_CONFIG_FIELD(key, BrdfLutTextureSize, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, IrradianceTextureSize, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, IrradianceSampleDelta, data, std::stof(value));
			MAP_CONFIG_FIELD(key, RadianceTextureSize, data, std::stoi(value));

			MAP_CONFIG_FIELD(key, PvdSocketHost, data, value);
			MAP_CONFIG_FIELD(key, PvdSocketPort, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, PvdSocketTimeout, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, GravityX, data, std::stof(value));
			MAP_CONFIG_FIELD(key, GravityY, data, std::stof(value));
			MAP_CONFIG_FIELD(key, GravityZ, data, std::stof(value));
			MAP_CONFIG_FIELD(key, PhysicsDeltaTime, data, std::stof(value));
			MAP_CONFIG_FIELD(key, VehicleUpdateMode, data, value);
			MAP_CONFIG_FIELD(key, VehicleSweepPointRejectionAngle, data, std::stof(value));
			MAP_CONFIG_FIELD(key, VehicleSweepNormalRejectionAngle, data, std::stof(value));
			MAP_CONFIG_FIELD(key, VehicleMaxHitAcceleration, data, std::stof(value));
			MAP_CONFIG_FIELD(key, VehicleMaxHitPointsPerWheel, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, VehicleHitsPerQuery, data, std::stoi(value));
			MAP_CONFIG_FIELD(key, VehicleSweepWidthScale, data, std::stof(value));
			MAP_CONFIG_FIELD(key, VehicleSweepRadiusScale, data, std::stof(value));
			MAP_CONFIG_FIELD(key, DefaultPhysicsMaterialStaticFriction, data, std::stof(value));
			MAP_CONFIG_FIELD(key, DefaultPhysicsMaterialDynamicFriction, data, std::stof(value));
			MAP_CONFIG_FIELD(key, DefaultPhysicsMaterialRestitution, data, std::stof(value));

			MAP_CONFIG_FIELD(key, PathToResources, data, value);
			MAP_CONFIG_FIELD(key, PathToMapExport, data, value);
			MAP_CONFIG_FIELD(key, PathToPostProcessing, data, value);
			MAP_CONFIG_FIELD(key, PathToPhysicsMaterials, data, value);
		}

		Logger::Debug("Configuration has been successfully loaded!");
	}
	else
	{
		Logger::Warning("File '" + FilePath + "' doesn't exist!");
		Logger::Warning("Using default values for engine configuration!");
	}

	return data;
}
