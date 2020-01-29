#include "TerrainUtil.h"
#include "stb_image.h"
#include <vector>


void TerrainUtil::GenerateTerrainMesh(TerrainComponent& terrainComponent, ID3D11Device* device)
{
	auto width = 0;
	auto height = 0;
	auto numberOfChannels = 0;

	const auto data = stbi_load_16(terrainComponent.PathToHeightmap.c_str(), &width, &height, &numberOfChannels, 1);
	if (data == nullptr)
	{
		Logger::Error("Couldn't open heightmap file!");
		Logger::Debug("Clearing memory after heightmap...");
		stbi_image_free(data);
		return;
	}

	if (numberOfChannels != 1)
		Logger::Warning("Number of channels for heigtmap is equal to" + std::to_string(numberOfChannels) + ". It might be very problematic!");

	std::vector<PositionAndUvVertex> vertices;
	std::vector<DWORD> indices;

	Logger::Debug("Calculating vertices buffer for terrain...");

	for (auto y = 0; y < height; y += terrainComponent.QualityDivider)
	{
		for (auto x = 0; x < width; x += terrainComponent.QualityDivider)
		{
			const auto pixelOffset = data + ((y * width) + x) * numberOfChannels;
			const auto terrainHeight = static_cast<float>(pixelOffset[0]) / std::numeric_limits<stbi_us>::max() * terrainComponent.MaxHeight;

			PositionAndUvVertex vertex{};
			vertex.Position = DirectX::XMFLOAT3(x * terrainComponent.ScaleXZ, terrainHeight, y * terrainComponent.ScaleXZ);
			// vertex.TextureCoord // TODO: Add texture coord

			vertices.emplace_back(vertex);
		}
	}

	Logger::Debug("Calculating indieces buffer for terrain...");

	for (auto y = 0; y < (height / terrainComponent.QualityDivider) - 1; y++)
	{
		const auto scaledWidth = (width / terrainComponent.QualityDivider);

		for (auto x = 0; x < scaledWidth - 1; x++)
		{
			indices.emplace_back((y * scaledWidth) + x);
			indices.emplace_back((y * scaledWidth) + (x + 1));
			indices.emplace_back(((y + 1) * scaledWidth) + x);
			indices.emplace_back(((y + 1) * scaledWidth) + (x + 1));
		}
	}

	Logger::Debug("Preparing to create vertex buffer and index buffer...");

	auto hr = terrainComponent.RenderVertexBuffer.Initialize(device, vertices.data(), vertices.size());
	if (FAILED(hr))
		Logger::Error("Couldn't create terrain vertex buffer.");
	hr = terrainComponent.RenderIndexBuffer.Initialize(device, indices.data(), indices.size());
	if (FAILED(hr))
		Logger::Error("Couldn't create terrain index buffer.");

	Logger::Debug("Clearing memory after heightmap...");
	stbi_image_free(data);
}
