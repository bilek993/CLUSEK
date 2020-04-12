#define STB_IMAGE_IMPLEMENTATION // Required here due to STB IMAGE library implementation
#include "TerrainUtil.h"
#include <future>
#include <geometry/PxHeightFieldDesc.h>
#include <PxPhysics.h>
#include <extensions/PxRigidActorExt.h>
#include "../Renderer/Generators/ResourcesGenerator.h"

void TerrainUtil::GenerateTerrainMesh(TerrainComponent& terrainComponent, ID3D11Device* device, const bool async)
{
	auto width = 0;
	auto height = 0;
	auto numberOfChannels = 0;

	const auto data = OpenFile(terrainComponent.PathToHeightmap, &width, &height, &numberOfChannels);
	if (data == nullptr)
		return;

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

		auto futureVertices = std::async(std::launch::async, GenerateVertices,	width,
																						height,
																						numberOfChannels, 
																						terrainComponent.QualityDivider,
																						terrainComponent.ScaleXZ,
																						terrainComponent.MaxHeight,
																						data);
		auto futureRenderIndies = std::async(std::launch::async, GenerateIndices, width, height, terrainComponent.QualityDivider);

		vertices = futureVertices.get();
		indices = futureRenderIndies.get();
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

physx::PxHeightFieldGeometry TerrainUtil::GenerateTerrainForPhysx(physx::PxHeightFieldSample* heightFieldSample, physx::PxHeightField* heightField,
	physx::PxCooking* cooking, physx::PxPhysics* physics, physx::PxDefaultAllocator* allocator, const TerrainComponent& terrainComponent, 
	const physx::PxHeightFieldFormat::Enum format, const bool async)
{
	Logger::Debug("Preparing resources for PhysX terrain generation...");

	auto width = 0;
	auto height = 0;
	auto numberOfChannels = 0;

	const auto data = OpenFile(terrainComponent.PathToHeightmap, &width, &height, &numberOfChannels);
	if (data == nullptr)
		return physx::PxHeightFieldGeometry();

	heightFieldSample = static_cast<physx::PxHeightFieldSample*>(allocator->allocate(sizeof(physx::PxHeightFieldSample) * (width * height), nullptr, nullptr, 0));

	if (async)
	{
		Logger::Debug("Preparing async 'HeightField' generation.");

		std::vector<std::future<void>> asyncFutures;

		const auto numberOfThreads = std::thread::hardware_concurrency();
		const auto asyncStep = height / numberOfThreads;

		for (auto i = 0; i < height; i += asyncStep)
		{
			const auto start = i;
			const auto end = i + asyncStep >= height ? height : i + asyncStep;

			asyncFutures.emplace_back(std::async(	std::launch::async,
													CalculatePartOfHeightField,
													0,
													width,
													start,
													end,
													width,
													numberOfChannels,
													data,
													heightFieldSample));
		}
	}
	else
	{
		Logger::Debug("Preparing single threaded 'HeightField' generation.");

		CalculatePartOfHeightField(	0, 
									width, 
									0, 
									height, 
									width, 
									numberOfChannels, 
									data, 
									heightFieldSample);
	}

	Logger::Debug("Finalizing PhysX terrain generation...");

	physx::PxHeightFieldDesc desc;
	desc.format = format;
	desc.nbColumns = width;
	desc.nbRows = height;
	desc.samples.data = heightFieldSample;
	desc.samples.stride = sizeof(physx::PxHeightFieldSample);

	heightField = cooking->createHeightField(desc, physics->getPhysicsInsertionCallback());

	const auto heightScale = terrainComponent.MaxHeight / static_cast<float>(std::numeric_limits<stbi_us>::max());

	physx::PxHeightFieldGeometry geometry(heightField, physx::PxMeshGeometryFlags(), heightScale,
		terrainComponent.ScaleXZ, terrainComponent.ScaleXZ);

	Logger::Debug("Terrain generation for PhysX finished...");

	return geometry;
}

void TerrainUtil::OptimizeTerrain(TerrainComponent& terrainComponent, ID3D11Device* device, ID3D11DeviceContext* context)
{
	Logger::Debug("Preparing to optimize terrain materials.");

	ComputeShader optimizerComputeShader;
	if (!optimizerComputeShader.Initialize(device, L"terrain_material_optimizer_compute_shader.cso"))
	{
		Logger::Error("OptimizerComputeShader not initialized due to critical problem!");
		return;
	}

	int width, height;
	if (!CalculateOptimizedTexturesSize(terrainComponent, &width, &height))
	{
		Logger::Error("Incorrect terrain textures! Terrain textures should be the same size!");
		return;
	}

	InitializeOptimizationTextures(terrainComponent, width, height, device);

	ComputeTexturesOptimization(terrainComponent, width, height, optimizerComputeShader, context);

	GenerateMipMapsForOptimizedTextures(terrainComponent, context);
	CleanUpResourcesAfterOptimization(terrainComponent, context);

	Logger::Debug("Optimized terrain materials!");
}

void TerrainUtil::GenerateNormals(TerrainComponent& terrainComponent, ConstantBuffer<TerrainNormalBuffer>& terrainNormalBuffer, 
	ID3D11Device* device, ID3D11DeviceContext* context)
{
	Logger::Debug("Preparing to generate normals and tangents for the terrain.");

	const auto heightMapDescriptor = GetDescriptor(terrainComponent.Material.Heightmap->Get());
	const auto width = heightMapDescriptor.Width;
	const auto height = heightMapDescriptor.Height;

	terrainComponent.Material.CalculatedNormalTexture = ResourcesGenerator::CreateFlatTexture(device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, false);
	ResourcesGenerator::CreateUnorderedAccessView(device, terrainComponent.Material.CalculatedNormalTexture);

	terrainComponent.Material.CalculatedTangentTexture = ResourcesGenerator::CreateFlatTexture(device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, false);
	ResourcesGenerator::CreateUnorderedAccessView(device, terrainComponent.Material.CalculatedTangentTexture);

	ComputeShader normalGeneratorComputeShader;
	if (!normalGeneratorComputeShader.Initialize(device, L"terrain_normal_generator_compute_shader.cso"))
	{
		Logger::Error("TerrainNormalGeneratorComputeShader not initialized due to critical problem!");
		return;
	}

	terrainNormalBuffer.Data.WorldCellSpace = terrainComponent.ScaleXZ;
	terrainNormalBuffer.Data.MaxHeight = terrainComponent.MaxHeight;
	terrainNormalBuffer.ApplyChanges();

	context->CSSetShader(normalGeneratorComputeShader.GetShader(), nullptr, 0);

	context->CSSetConstantBuffers(0, 1, terrainNormalBuffer.GetAddressOf());

	context->CSSetShaderResources(0, 1, terrainComponent.Material.Heightmap->GetAddressOf());

	context->CSSetUnorderedAccessViews(0, 1, terrainComponent.Material.CalculatedNormalTexture.UnorderedAccessView.GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(1, 1, terrainComponent.Material.CalculatedTangentTexture.UnorderedAccessView.GetAddressOf(), nullptr);

	context->Dispatch(width / THREAD_COUNT, height / THREAD_COUNT, 1);

	ID3D11UnorderedAccessView* const nullView[] = { nullptr };
	context->CSSetUnorderedAccessViews(0, 1, nullView, nullptr);
	context->CSSetUnorderedAccessViews(1, 1, nullView, nullptr);

	terrainComponent.Material.CalculatedNormalTexture.UnorderedAccessView.Reset();
	terrainComponent.Material.CalculatedTangentTexture.UnorderedAccessView.Reset();

	Logger::Debug("Normals and tangents generation finished!");
}

stbi_us* TerrainUtil::OpenFile(const std::string& path, int* width, int* height, int* numberOfChannels)
{
	const auto data = stbi_load_16(path.c_str(), width, height, numberOfChannels, 1);

	if (data == nullptr)
	{
		Logger::Error("Couldn't open heightmap file!");
		Logger::Debug("Clearing memory after heightmap...");
		stbi_image_free(data);
		return nullptr;
	}

	if (*numberOfChannels != 1)
		Logger::Warning("Number of channels for heigtmap is equal to" + std::to_string(*numberOfChannels) + ". It might be very problematic!");

	return data;
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

std::vector<DWORD> TerrainUtil::GenerateIndices(const int width, const int height, const int qualityDivider)
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

void TerrainUtil::CalculatePartOfHeightField(const int startX, const int endX, const int startY, const int endY,
                                             const int width, const int numberOfChannels, stbi_us* data, physx::PxHeightFieldSample* heightFieldSample)
{
	Logger::Debug("Generating 'HeightField' for part (x:" + std::to_string(startX) + "-" + 
		std::to_string(endX) + ", y:" + std::to_string(startY) + "-" + std::to_string(endY) + ").");

	for (auto y = startY; y < endY; y++)
	{
		for (auto x = startX; x < endX; x++)
		{
			const auto pixelOffset = CalculateOffset(data, x, y, width, numberOfChannels);

			heightFieldSample[x + (y * width)].height = *pixelOffset;
			heightFieldSample[x + (y * width)].setTessFlag();
			heightFieldSample[x + (y * width)].materialIndex0 = 0;
			heightFieldSample[x + (y * width)].materialIndex1 = 0;
		}
	}
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
	return data + ((y * width) + x) * numberOfChannels;
}

float TerrainUtil::GetHeight(const stbi_us* offsetData, const float maxHeight)
{
	return static_cast<float>(offsetData[0]) / std::numeric_limits<stbi_us>::max() * maxHeight;
}

void TerrainUtil::InitializeOptimizationTextures(TerrainComponent& terrainComponent, const int width, const int height, ID3D11Device* device)
{
	terrainComponent.Material.OptimizedOcclusionTexture = ResourcesGenerator::CreateFlatTexture(device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, true);
	ResourcesGenerator::CreateUnorderedAccessView(device, terrainComponent.Material.OptimizedOcclusionTexture);

	terrainComponent.Material.OptimizedMetalicTexture = ResourcesGenerator::CreateFlatTexture(device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, true);
	ResourcesGenerator::CreateUnorderedAccessView(device, terrainComponent.Material.OptimizedMetalicTexture);

	terrainComponent.Material.OptimizedSmoothnessTexture = ResourcesGenerator::CreateFlatTexture(device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, true);
	ResourcesGenerator::CreateUnorderedAccessView(device, terrainComponent.Material.OptimizedSmoothnessTexture);
}

bool TerrainUtil::CalculateOptimizedTexturesSize(TerrainComponent& terrainComponent, int* width, int* height)
{
	const D3D11_TEXTURE2D_DESC listOfDesc[] = {
		GetDescriptor(terrainComponent.Material.BaseOcclusionTexture->Get()),
		GetDescriptor(terrainComponent.Material.RedOcclusionTexture->Get()),
		GetDescriptor(terrainComponent.Material.GreenOcclusionTexture->Get()),
		GetDescriptor(terrainComponent.Material.BlueOcclusionTexture->Get()),

		GetDescriptor(terrainComponent.Material.BaseMetalicSmoothnessTexture->Get()),
		GetDescriptor(terrainComponent.Material.RedMetalicSmoothnessTexture->Get()),
		GetDescriptor(terrainComponent.Material.GreenMetalicSmoothnessTexture->Get()),
		GetDescriptor(terrainComponent.Material.BlueMetalicSmoothnessTexture->Get()),
	};

	*width = listOfDesc[0].Width;
	*height = listOfDesc[0].Height;

	for (auto i = 1; i < 8; i++)
	{
		if (*width != listOfDesc[i].Width || *height != listOfDesc[i].Height)
			return false;
	}

	return true;
}

void TerrainUtil::ComputeTexturesOptimization(TerrainComponent& terrainComponent, const int width, const int height, ComputeShader& computeShader, ID3D11DeviceContext* context)
{
	context->CSSetShader(computeShader.GetShader(), nullptr, 0);

	context->CSSetShaderResources(0, 1, terrainComponent.Material.BaseMetalicSmoothnessTexture->GetAddressOf());
	context->CSSetShaderResources(1, 1, terrainComponent.Material.RedMetalicSmoothnessTexture->GetAddressOf());
	context->CSSetShaderResources(2, 1, terrainComponent.Material.GreenMetalicSmoothnessTexture->GetAddressOf());
	context->CSSetShaderResources(3, 1, terrainComponent.Material.BlueMetalicSmoothnessTexture->GetAddressOf());

	context->CSSetShaderResources(4, 1, terrainComponent.Material.BaseOcclusionTexture->GetAddressOf());
	context->CSSetShaderResources(5, 1, terrainComponent.Material.RedOcclusionTexture->GetAddressOf());
	context->CSSetShaderResources(6, 1, terrainComponent.Material.GreenOcclusionTexture->GetAddressOf());
	context->CSSetShaderResources(7, 1, terrainComponent.Material.BlueOcclusionTexture->GetAddressOf());

	context->CSSetUnorderedAccessViews(0, 1, terrainComponent.Material.OptimizedOcclusionTexture.UnorderedAccessView.GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(1, 1, terrainComponent.Material.OptimizedMetalicTexture.UnorderedAccessView.GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(2, 1, terrainComponent.Material.OptimizedSmoothnessTexture.UnorderedAccessView.GetAddressOf(), nullptr);

	context->Dispatch(width / THREAD_COUNT, height / THREAD_COUNT, 1);
}

void TerrainUtil::GenerateMipMapsForOptimizedTextures(TerrainComponent& terrainComponent, ID3D11DeviceContext* context)
{
	context->GenerateMips(terrainComponent.Material.OptimizedOcclusionTexture.ShaderResourceView.Get());
	context->GenerateMips(terrainComponent.Material.OptimizedMetalicTexture.ShaderResourceView.Get());
	context->GenerateMips(terrainComponent.Material.OptimizedSmoothnessTexture.ShaderResourceView.Get());
}

void TerrainUtil::CleanUpResourcesAfterOptimization(TerrainComponent& terrainComponent, ID3D11DeviceContext* context)
{
	ID3D11UnorderedAccessView* const nullView[] = { nullptr };
	context->CSSetUnorderedAccessViews(0, 1, nullView, nullptr);
	context->CSSetUnorderedAccessViews(1, 1, nullView, nullptr);
	context->CSSetUnorderedAccessViews(2, 1, nullView, nullptr);

	terrainComponent.Material.OptimizedOcclusionTexture.UnorderedAccessView.Reset();
	terrainComponent.Material.OptimizedMetalicTexture.UnorderedAccessView.Reset();
	terrainComponent.Material.OptimizedSmoothnessTexture.UnorderedAccessView.Reset();
}

D3D11_TEXTURE2D_DESC TerrainUtil::GetDescriptor(ID3D11ShaderResourceView* shaderResourceView)
{
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	D3D11_TEXTURE2D_DESC descriptor;

	shaderResourceView->GetResource(resource.GetAddressOf());
	resource->QueryInterface<ID3D11Texture2D>(texture.GetAddressOf());
	texture->GetDesc(&descriptor);

	return descriptor;
}
