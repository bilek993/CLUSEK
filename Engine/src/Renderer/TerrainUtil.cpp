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

	terrainComponent.TexelSize = 1.0f / ((width + height) / 2.0f);

	std::vector<PositionAndUvVertex> vertices;
	std::vector<DWORD> renderIndices;
	std::vector<DWORD> shadowIndices;

	if (!async)
	{
		vertices = GenerateVertices(width,
									height,
									numberOfChannels,
									terrainComponent.QualityDivider,
									terrainComponent.ScaleXZ,
									terrainComponent.MaxHeight,
									data);
		renderIndices = GenerateRenderIndices(width, height, terrainComponent.QualityDivider);
		shadowIndices = GenerateShadowIndices(width, height, terrainComponent.QualityDivider);
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
		auto futureRenderIndies = std::async(std::launch::async, GenerateRenderIndices, width, height, terrainComponent.QualityDivider);
		auto futureShadowIndices = std::async(std::launch::async, GenerateShadowIndices, width, height, terrainComponent.QualityDivider);

		vertices = futureVerticies.get();
		renderIndices = futureRenderIndies.get();
		shadowIndices = futureShadowIndices.get();
	}

	Logger::Debug("Preparing to create vertex buffer and index buffer...");

	auto hr = terrainComponent.RenderVertexBuffer.Initialize(device, vertices.data(), vertices.size());
	if (FAILED(hr))
		Logger::Error("Couldn't create terrain vertex buffer.");

	hr = terrainComponent.RenderIndexBuffer.Initialize(device, renderIndices.data(), renderIndices.size());
	if (FAILED(hr))
		Logger::Error("Couldn't create terrain index buffer.");

	hr = terrainComponent.ShadowIndexBuffer.Initialize(device, shadowIndices.data(), shadowIndices.size());
	if (FAILED(hr))
		Logger::Error("Couldn't create terrain shadow index buffer.");

	Logger::Debug("Clearing memory after heightmap...");
	stbi_image_free(data);
}

std::vector<PositionAndUvVertex> TerrainUtil::GenerateVertices(const int width, const int height, const int numberOfChannels,
	const int qualityDivider, const float scaleXZ, const float maxHeight, stbi_us* data)
{
	Logger::Debug("Calculating vertices buffer for terrain...");
	std::vector<PositionAndUvVertex> vertices;

	for (auto y = 0; y < height; y += qualityDivider)
	{
		for (auto x = 0; x < width; x += qualityDivider)
		{
			const auto pixelOffset = CalculateOffset(data, x, y, width, numberOfChannels);
			const auto terrainHeight = GetHeight(pixelOffset, maxHeight);

			PositionAndUvVertex vertex{};
			vertex.Position = DirectX::XMFLOAT3(y * scaleXZ, terrainHeight, x * scaleXZ);
			vertex.TextureCoord = DirectX::XMFLOAT2(static_cast<float>(x) / static_cast<float>(width), static_cast<float>(y) / static_cast<float>(height));

			if (y < height - 1 && x < width - 1)
				CalculateBoundsY(&vertex, data, width, numberOfChannels, maxHeight, x, y, x + qualityDivider, y + qualityDivider);

			vertices.emplace_back(vertex);
		}
	}

	Logger::Debug("Vertices buffer calculated!");
	return vertices;
}

std::vector<DWORD> TerrainUtil::GenerateRenderIndices(const int width, const int height, const int qualityDivider)
{
	Logger::Debug("Calculating indices buffer (rendering) for terrain...");
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

	Logger::Debug("Indices buffer (rendering) calculated!");
	return indices;
}

std::vector<DWORD> TerrainUtil::GenerateShadowIndices(const int width, const int height, const int qualityDivider)
{
	Logger::Debug("Calculating indices buffer (shadows) for terrain...");
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
			indices.emplace_back(((y + 1) * scaledWidth) + x);
			indices.emplace_back((y * scaledWidth) + (x + 1));
		}
	}

	Logger::Debug("Indices buffer (shadows) calculated!");
	return indices;
}

void TerrainUtil::CalculateBoundsY(PositionAndUvVertex* vertex, stbi_us* data, const int width, const int numberOfChannels, 
	const float maxHeight, const int currentX, const int currentY, const int nextX, const int nextY)
{
	auto min = std::numeric_limits<float>::max();
	auto max = -std::numeric_limits<float>::max();

	for (auto y = currentY; y < nextY; y++)
	{
		for (auto x = currentX; x < nextX; x++)
		{
			const auto pixelOffset = CalculateOffset(data, x, y, width, numberOfChannels);
			const auto terrainHeight = GetHeight(pixelOffset, maxHeight);

			min = std::min(min, terrainHeight);
			max = std::max(max, terrainHeight);
		}
	}

	vertex->BoundsY = DirectX::XMFLOAT2(min, max);
}

stbi_us* TerrainUtil::CalculateOffset(stbi_us* data, const int x, const int y, const int width, const int numberOfChannels)
{
	return data + ((y * width) + x) * numberOfChannels;;
}

float TerrainUtil::GetHeight(const stbi_us* offsetData, const float maxHeight)
{
	return static_cast<float>(offsetData[0]) / std::numeric_limits<stbi_us>::max() * maxHeight;
}
