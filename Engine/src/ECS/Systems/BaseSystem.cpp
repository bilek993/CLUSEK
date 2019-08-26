#include "BaseSystem.h"
#include "../../Renderer/PostProcessingSettings.h"

void BaseSystem::Initialize(entt::registry* registry, RenderWindow* window, ConfigData* configData,
	DynamicRenderSettings *renderSettings, IOData* ioData, IODevices* ioDevices, PostProcessingSettings *postProcessingSettings)
{
	Registry = registry;
	Window = window;
	ConfigurationData = configData;
	CurrentRenderSettings = renderSettings;
	InputOutputData = ioData;
	InputOutputDevices = ioDevices;
	CurrentPostProcessingSettings = postProcessingSettings;
}
