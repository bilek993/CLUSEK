#include "TerrainSystem.h"
#include "stb_image.h"

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
	auto width = 0;
	auto height = 0;
	auto numberOfChannels = 0;

	const auto data = stbi_load(terrainComponent.PathToHeightmap.c_str(), &width, &height, &numberOfChannels, 1);
	if (data == nullptr)
	{
		Logger::Error("Couldn't open heightmap file!");
		Logger::Debug("Clearing memory after heightmap...");
		stbi_image_free(data);
		return;
	}

	if (numberOfChannels != 1)
		Logger::Warning("Number of channels for heigtmap is equal to" + std::to_string(numberOfChannels) + ". It might be very problematic!");

	std::vector<FatVertex> vertices;
	std::vector<DWORD> indices;

	Logger::Debug("Calculating vertices buffer for terrain...");

	for (auto y = 0; y < height; y++)
	{
		for (auto x = 0; x < width; x += numberOfChannels)
		{
			const auto pixelOffset = data + ((y * height) + x) * numberOfChannels;
			const auto terrainHeight = static_cast<int>(pixelOffset[0]);

			FatVertex vertex{};
			vertex.Position = DirectX::XMFLOAT3(x, terrainHeight, y);
			vertex.Normal = DirectX::XMFLOAT3(0, 1, 0); // TODO: Change this
			vertex.Tangent = DirectX::XMFLOAT3(1, 0, 0); // TODO: Change this
			vertex.TextureCoord = DirectX::XMFLOAT2(0, 0); // TODO: Change this

			vertices.emplace_back(vertex);
		}
	}

	Logger::Debug("Calculating indieces buffer for terrain...");

	for (auto y = 0; y < height - 1; y++)
	{
		for (auto x = 0; x < width - 1; x++)
		{
			indices.emplace_back((y * height) + x);
			indices.emplace_back((y * height) + (x + 1));
			indices.emplace_back(((y + 1) * height) + x);
			indices.emplace_back(((y + 1) * height) + (x + 1));
		}
	}

	Logger::Debug("Preparing to create vertex buffer and index buffer...");

	auto hr = terrainComponent.RenderVertexBuffer.Initialize(DirectXDevice, vertices.data(), vertices.size());
	if (FAILED(hr))
		Logger::Error("Couldn't create terrain vertex buffer.");
	hr = terrainComponent.RenderIndexBuffer.Initialize(DirectXDevice, indices.data(), indices.size());
	if (FAILED(hr))
		Logger::Error("Couldn't create terrain index buffer.");

	Logger::Debug("Clearing memory after heightmap...");
	stbi_image_free(data);
}
