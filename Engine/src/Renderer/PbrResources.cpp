#include "PbrResources.h"
#include "../Utils/Logger.h"
#include "../Utils/StringUtil.h"
#include <WICTextureLoader.h>
#include "Shaders/ComputeShader.h"
#include "ConstantBufferTypes/RadianceBuffer.h"
#include "ConstantBuffer.h"
#include "Generators/ResourcesGenerator.h"
#include "../../IrradianceBuffer.h"

bool PbrResource::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, const ConfigData* config,
	ID3D11ShaderResourceView* const* skyResourceView)
{
	Logger::Debug("Preparing PBR resources...");

	if (!InitializeSamplerState(device))
		return false;
	if (!LoadBrdfLutFile(device, context, config->BrdfLutTextureSize))
		return false;
	if (!GenerateIrradiance(device, context, skyResourceView, config->IrradianceTextureSize, config->IrradianceSampleDelta))
		return false;
	if (!GenerateRadiance(device, context, skyResourceView, config->RadianceTextureSize))
		return false;

	CleanUp(context);

	Logger::Debug("All PBR resources initialized!");
	return true;
}

ID3D11ShaderResourceView** PbrResource::GetAddressOfBrdfLutResourceTexture()
{
	return BrdfLutTexture.ShaderResourceView.GetAddressOf();
}

ID3D11ShaderResourceView** PbrResource::GetAddressOfIrradianceResourceTexture()
{
	return IrradianceTexture.ShaderResourceView.GetAddressOf();
}

ID3D11ShaderResourceView** PbrResource::GetAddressOfRadianceResourceTexture()
{
	return RadianceTexture.ShaderResourceView.GetAddressOf();
}

bool PbrResource::InitializeSamplerState(ID3D11Device* device)
{
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	const auto hr = device->CreateSamplerState(&samplerDesc, SamplerState.GetAddressOf());
	if (FAILED(hr))
	{
		Logger::Error("Couldn't create sampler state for PBR Resource!");
		return false;
	}

	return true;
}

bool PbrResource::LoadBrdfLutFile(ID3D11Device* device, ID3D11DeviceContext* context,
	const int textureSize)
{
	Logger::Debug("Preparing to generate BRDF Lut...");

	ComputeShader brdfLutComputeShader;

	if (!brdfLutComputeShader.Initialize(device, L"brdf_lut_compute_shader.cso"))
	{
		Logger::Error("BrdfLutComputeShader not initialized due to critical problem!");
		return false;
	}

	BrdfLutTexture = ResourcesGenerator::CreateFlatTexture(device, textureSize, textureSize, DXGI_FORMAT_R16G16_FLOAT, false);
	ResourcesGenerator::CreateUnorderedAccessView(device, BrdfLutTexture);

	context->CSSetShader(brdfLutComputeShader.GetShader(), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, BrdfLutTexture.UnorderedAccessView.GetAddressOf(), nullptr);
	context->Dispatch(BrdfLutTexture.Width / THREAD_COUNT, BrdfLutTexture.Height / THREAD_COUNT, CUBE_SIZE);

	return true;
}

bool PbrResource::GenerateIrradiance(ID3D11Device* device, ID3D11DeviceContext* context,
	ID3D11ShaderResourceView* const* skyResourceView, const int textureSize,
	float sampleDelta)
{
	Logger::Debug("Preparing to generate irradiance map...");

	ComputeShader irradianceComputeShader;

	if (!irradianceComputeShader.Initialize(device, L"irradiance_compute_shader.cso"))
	{
		Logger::Error("IrradianceComputeShader not initialized due to critical problem!");
		return false;
	}

	ConstantBuffer<IrradianceBuffer> constantBuffer;
	const auto hr = constantBuffer.Initialize(device, context);
	if (FAILED(hr))
		Logger::Error("Failed to create 'IrradianceBuffer' constant buffer.");

	constantBuffer.Data.SampleDelta = sampleDelta;
	constantBuffer.ApplyChanges();

	IrradianceTexture = ResourcesGenerator::CreateCubeTexture(device, textureSize, textureSize, DXGI_FORMAT_R16G16B16A16_FLOAT, false);
	ResourcesGenerator::CreateUnorderedAccessView(device, IrradianceTexture);

	context->CSSetShader(irradianceComputeShader.GetShader(), nullptr, 0);
	context->CSSetShaderResources(0, 1, skyResourceView);
	context->CSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	context->CSSetUnorderedAccessViews(0, 1, IrradianceTexture.UnorderedAccessView.GetAddressOf(), nullptr);
	context->CSSetSamplers(0, 1, SamplerState.GetAddressOf());
	context->Dispatch(IrradianceTexture.Width / THREAD_COUNT, IrradianceTexture.Height / THREAD_COUNT, CUBE_SIZE);

	return true;
}

bool PbrResource::GenerateRadiance(ID3D11Device* device, ID3D11DeviceContext* context,
	ID3D11ShaderResourceView* const* skyResourceView, const int textureSize)
{
	Logger::Debug("Preparing to generate radiance map...");

	ComputeShader radianceComputeShader;

	if (!radianceComputeShader.Initialize(device, L"radiance_compute_shader.cso"))
	{
		Logger::Error("RadianceComputeShader not initialized due to critical problem!");
		return false;
	}

	RadianceTexture = ResourcesGenerator::CreateCubeTexture(device, textureSize, textureSize, DXGI_FORMAT_R16G16B16A16_FLOAT, true);

	context->CSSetShader(radianceComputeShader.GetShader(), nullptr, 0);
	context->CSSetSamplers(0, 1, SamplerState.GetAddressOf());
	context->CSSetShaderResources(0, 1, skyResourceView);

	ConstantBuffer<RadianceBuffer> constantBuffer;
	const auto hr = constantBuffer.Initialize(device, context);
	if (FAILED(hr))
		Logger::Error("Failed to create 'RadianceBuffer' constant buffer.");

	const auto deltaRoughness = 1.0f / std::max(1.0f, static_cast<float>(RadianceTexture.Levels - 1));
	auto size = RadianceTexture.Width;

	for (auto level = 0; level < RadianceTexture.Levels; level++)
	{
		const auto threadGroupCount = std::max(1, size / 32);
		ResourcesGenerator::CreateUnorderedAccessView(device, RadianceTexture, level);

		constantBuffer.Data.Roughness = level * deltaRoughness;
		constantBuffer.ApplyChanges();

		context->CSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
		context->CSSetUnorderedAccessViews(0, 1, RadianceTexture.UnorderedAccessView.GetAddressOf(), nullptr);
		context->Dispatch(threadGroupCount, threadGroupCount, CUBE_SIZE);

		size /= 2;
	}

	return true;
}

void PbrResource::CleanUp(ID3D11DeviceContext* context)
{
	Logger::Debug("Preparing clean up mechanism, after PBR Resources initialization...");

	ID3D11UnorderedAccessView* const nullView[] = { nullptr };
	context->CSSetUnorderedAccessViews(0, 1, nullView, nullptr);

	BrdfLutTexture.UnorderedAccessView.Reset();
	IrradianceTexture.UnorderedAccessView.Reset();
	RadianceTexture.UnorderedAccessView.Reset();
}