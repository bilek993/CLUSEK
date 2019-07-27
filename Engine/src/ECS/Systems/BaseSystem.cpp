#include "BaseSystem.h"

void BaseSystem::Initialize(entt::registry* registry, RenderWindow* window, ConfigData* configData,
	LightSettings* lightSettings, IOData* ioData, IODevices* ioDevices)
{
	Registry = registry;
	Window = window;
	ConfigurationData = configData;
	LightingSettings = lightSettings;
	InputOutputData = ioData;
	InputOutputDevices = ioDevices;
}
