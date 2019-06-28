#include "Renderer.h"
#include "../Utils/Logger.h"

bool Renderer::Initialize(const HWND hwnd, const int width, const int height, const int selectedAdapterId)
{
	Logger::Debug("Renderer initialization...");
	return InitializeDirectX(hwnd, width, height, selectedAdapterId);
}

bool Renderer::InitializeDirectX(const HWND hwnd, const int width, const int height, const int selectedAdapterId)
{
	auto adapters = AdapterReader::GetData();

	if (adapters.empty())
		Logger::Error("No adapters found!");

	if (selectedAdapterId >= adapters.size())
		Logger::Error("Adapter with id [" + std::to_string(selectedAdapterId) + "] not found!");

	//D3D11CreateDeviceAndSwapChain()

	return true;
}
