#include "RenderSystem.h"
#include "../Components/RenderComponent.h"
#include <WICTextureLoader.h>
#include "../Components/CameraComponent.h"

void RenderSystem::Start(entt::registry& registry, const ConfigData& configData, Renderer &renderer)
{
	registry.view<RenderComponent>().each([&renderer](RenderComponent &renderComponent)
	{
		Vertex v[] =
		{
			Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f),
			Vertex(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f),
			Vertex(0.5f, 0.5f, 0.0f, 1.0f, 0.0f),
			Vertex(0.5f, -0.5f, 0.0f, 1.0f, 1.0f),
		};

		DWORD indices[] =
		{
			0, 1, 2,
			0, 2, 3,
		};

		auto hr = renderComponent.RenderVertexBuffer.Initialize(renderer.GetDevice().Get(), v, ARRAYSIZE(v));
		if (FAILED(hr))
			Logger::Error("Failed to create vertex buffer.");

		hr = renderComponent.RenderIndexBuffer.Initialize(renderer.GetDevice().Get(), indices, ARRAYSIZE(indices));
		if (FAILED(hr))
			Logger::Error("Failed to create index buffer.");

		hr = DirectX::CreateWICTextureFromFile(renderer.GetDevice().Get(), L"Data/Textures/example_texture.png", nullptr, renderComponent.TextureResourceView.GetAddressOf());
		if (FAILED(hr))
			Logger::Error("Couldn't load example texture from file!");

		hr = renderComponent.UberShaderConstantBuffer.Initialize(renderer.GetDevice().Get(), renderer.GetDeviceContext().Get());
		if (FAILED(hr))
		{
			Logger::Error("Failed to create constant buffer.");
			return false;
		}
	});
}

void RenderSystem::Update(float deltaTime, entt::registry& registry, IOData& ioData, IODevices& ioDevices,
	Renderer& renderer, RenderWindow& window, ConfigData& configData)
{
	auto view = registry.view<CameraComponent>();
	if (view.size() != 1)
	{
		if (view.size() > 1)
			Logger::Error("More than one main render camera found!");
		else
			Logger::Error("Main render camera not found!");
	}

	auto &cameraComponent = view.raw()[0];

	renderer.RenderFrameBegin();
	registry.view<RenderComponent>().each([&renderer, &cameraComponent](RenderComponent &renderComponent)
	{
		renderComponent.UberShaderConstantBuffer.Data.Mat = cameraComponent.ViewMatrix * cameraComponent.ProjectionMatrix;
		renderComponent.UberShaderConstantBuffer.Data.Mat = DirectX::XMMatrixTranspose(renderComponent.UberShaderConstantBuffer.Data.Mat);
		if (!renderComponent.UberShaderConstantBuffer.ApplyChanges())
			return;

		renderer.GetDeviceContext()->VSSetConstantBuffers(0, 1, renderComponent.UberShaderConstantBuffer.GetAddressOf());

		UINT offset = 0;

		renderer.GetDeviceContext()->PSSetShaderResources(0, 1, renderComponent.TextureResourceView.GetAddressOf());
		renderer.GetDeviceContext()->IASetVertexBuffers(0, 1, renderComponent.RenderVertexBuffer.GetAddressOf(), renderComponent.RenderVertexBuffer.StridePtr(), &offset);
		renderer.GetDeviceContext()->IASetIndexBuffer(renderComponent.RenderIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		renderer.GetDeviceContext()->DrawIndexed(6, 0, 0);
	});
	renderer.RenderFrameEnd();
}
