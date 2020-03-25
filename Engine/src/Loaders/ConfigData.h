#pragma once

struct ConfigData final
{
	// Logger settings
	int LoggerEnabled = 0;
	std::string LoggerDestination = "logs.txt";
	int LoggerLevel = 0;

	// Window settings
	std::string WindowClassName = "EngineClassName";
	std::string WindowTitle = "";
	int WindowWidth = 640;
	int WindowHeight = 480;

	// ImGui
	int EnableImGuiOnStart = 0;
	std::string PathToImGuiFont = "";

	// DirectX settings
	int Fullscreen = 0;
	int SyncIntervals = 1;
	int SelectedAdapterId = 0;
	int RefreshRateNumerator = 0;
	int RefreshRateDenominator = 0;
	int MultisamplesCount = 1;
	int MultisamplesQuality = 0;
	int WireframeMode = 0;
	std::string CullMode = "BACK";
	float ClearColorRed = 1.0f;
	float ClearColorGreen = 1.0f;
	float ClearColorBlue = 1.0f;

	// Loading screen settings
	int DisableLoadingScreen = 0;
	std::string PathToLoadingLogo = "";
	float LogoScale;

	// Main camera settings
	float MainCameraFov = 90.0f;
	float MaxRotationX = 90.0f;
	float MinRotationX = -90.0f;
	float MainCameraNearZ = 0.1f;
	float MainCameraFarZ = 1000.0f;
	float MaxCameraSpeed = 0.0125f;

	// Lighting settings
	float DirectionalLightColorRed = 1.0f;
	float DirectionalLightColorGreen = 1.0f;
	float DirectionalLightColorBlue = 1.0f;
	float DirectionalLightStrength = 0.75f;
	float DirectionalLightDirectionX = 0.25f;
	float DirectionalLightDirectionY = 0.5f;
	float DirectionalLightDirectionZ = -1.0f;

	// Fog settings
	float FogColorRed = 0.3f;
	float FogColorGreen = 0.6f;
	float FogColorBlue = 0.7f;
	float FogDensity = 0.0002f;
	float FogLightColorRed = 1.0f;
	float FogLightColorGreen = 0.9f;
	float FogLightColorBlue = 0.7f;
	float FogMinDistance = 0.0f;
	float SkyConstantValue = 3000.0f;

	// Shadows settings
	int ShadowsEnabled = 1;
	int EnableRealtimeShadowPreview = 0;
	int ShadowsTextureSize = 512;
	float ShadowAlphaThreshold = 0.95f;
	float ShadowCameraShiftNearZ = 50.0f;
	float CascadeEnd0 = 0.25f;
	float CascadeEnd1 = 0.5f;
	float CascadeEnd2 = 0.75f;
	float CascadeEnd3 = 1.0f;
	float CascadeBias0 = 0.01f;
	float CascadeBias1 = 0.01f;
	float CascadeBias2 = 0.01f;
	float CascadeBias3 = 0.01f;

	// Terrain settings
	int EnableAsyncTerrainGeneration = 1;
	float MinTerrainTessellationFactor = 0.0f;
	float MaxTerrainTessellationFactor = 6.0f;
	float MinTerrainTessellationDistance = 0.0f;
	float MaxTerrainTessellationDistance = 1000.0f;

	// Loaders settings
	int CubemapGeneratedSize = 1024;
	int EnableAsyncModelLoading = 1;
	int EnableAsyncTextureLoading = 1;

	// PBR settings
	int BrdfLutTextureSize = 128;
	int IrradianceTextureSize = 32;
	float IrradianceSampleDelta = 0.0125f;
	int RadianceTextureSize = 1024;

	// Physics settings
	std::string PvdSocketHost = "127.0.0.1";
	int PvdSocketPort = 5425;
	int PvdSocketTimeout = 25;
	float GravityX = 0.0f;
	float GravityY = -9.81f;
	float GravityZ = 0.0f;
	float PhysicsDeltaTime = 33.3f;
	std::string VehicleUpdateMode = "VELOCITY_CHANGE";
	float VehicleSweepPointRejectionAngle = 0.785f;
	float VehicleSweepNormalRejectionAngle = 0.785f;
	float VehicleMaxHitAcceleration = 50.0f;
	int VehicleMaxHitPointsPerWheel = 8;
	int VehicleHitsPerQuery = 8;
	float VehicleSweepWidthScale = 1.0f;
	float VehicleSweepRadiusScale = 1.01f;

	// Files responsible for world description
	std::string PathToResources = "";
	std::string PathToMap = "";
	std::string PathToPostProcessing = "";
};