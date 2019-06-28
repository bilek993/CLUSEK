#include "Renderer.h"
#include "../Utils/Logger.h"

bool Renderer::Initialize(HWND hwnd, int width, int height)
{
	Logger::Debug("Renderer initialization...");
	return InitializeDirectX(hwnd, width, height);
}

bool Renderer::InitializeDirectX(HWND hwnd, int width, int height)
{
	auto adapters = AdapterReader::GetData();

	return true;
}
