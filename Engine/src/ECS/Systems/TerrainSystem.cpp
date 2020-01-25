#include "TerrainSystem.h"

void TerrainSystem::Start()
{
	Logger::Debug("Preparing terrain system...");

	Registry->view<TerrainComponent>().each([this](TerrainComponent &terrainComponent)
	{
		GenerateTerrainMesh(terrainComponent);
	});
}

void TerrainSystem::Update(float deltaTime)
{
}

void TerrainSystem::GenerateTerrainMesh(TerrainComponent& terrainComponent) const
{

}
