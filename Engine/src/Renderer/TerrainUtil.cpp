#define STB_IMAGE_IMPLEMENTATION // Required here due to STB IMAGE library implementation
#include "TerrainUtil.h"
#include <future>

void TerrainUtil::GenerateTerrainMesh(TerrainComponent& terrainComponent, ID3D11Device* device, const bool async)
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

	if (!async)
	{
		vertices = GenerateVertices(width,
									height,
									numberOfChannels,
									terrainComponent.QualityDivider,
									terrainComponent.ScaleXZ,
									terrainComponent.MaxHeight,
									data);
		indices = GenerateIndices(width, height, terrainComponent.QualityDivider);
	}
	else
	{
		Logger::Debug("Preparing async mechanism for terrain utility...");

		auto futureVerticies = std::async(std::launch::async, GenerateVertices,	width,
																								height,
																								numberOfChannels, 
																								terrainComponent.QualityDivider,
																								terrainComponent.ScaleXZ,
																								terrainComponent.MaxHeight,
																								data);
		auto futureIndies = std::async(std::launch::async, GenerateIndices, width, height, terrainComponent.QualityDivider);

		vertices = futureVerticies.get();
		indices = futureIndies.get();
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

std::vector<PositionAndUvVertex> TerrainUtil::GenerateVertices(const int width, const int height, const int numberOfChannels,
	const int qualityDivider, const float scaleXZ, const float maxHeight, const stbi_us* data)
{
	Logger::Debug("Calculating vertices buffer for terrain...");
	std::vector<PositionAndUvVertex> vertices;

	for (auto y = 0; y < height; y += qualityDivider)
	{
		for (auto x = 0; x < width; x += qualityDivider)
		{
			const auto pixelOffset = data + ((y * width) + x) * numberOfChannels;
			const auto terrainHeight = static_cast<float>(pixelOffset[0]) / std::numeric_limits<stbi_us>::max() * maxHeight;

			PositionAndUvVertex vertex{};
			vertex.Position = DirectX::XMFLOAT3(x * scaleXZ, terrainHeight, y * scaleXZ);
			// vertex.TextureCoord // TODO: Add texture coord

			vertices.emplace_back(vertex);
		}
	}

	Logger::Debug("Vertices buffer calculated!");
	return vertices;
}

std::vector<DWORD> TerrainUtil::GenerateIndices(const int width, const int height, const int qualityDivider)
{
	Logger::Debug("Calculating indices buffer for terrain...");
	std::vector<DWORD> indices;

	for (auto y = 0; y < (height / qualityDivider) - 1; y++)
	{
		const auto scaledWidth = (width / qualityDivider);

		for (auto x = 0; x < scaledWidth - 1; x++)
		{
			indices.emplace_back((y * scaledWidth) + x);
			indices.emplace_back((y * scaledWidth) + (x + 1));
			indices.emplace_back(((y + 1) * scaledWidth) + x);
			indices.emplace_back(((y + 1) * scaledWidth) + (x + 1));
		}
	}

	Logger::Debug("Indices buffer calculated!");
	return indices;
}
