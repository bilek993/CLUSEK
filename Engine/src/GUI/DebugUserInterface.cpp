#include "DebugUserInterface.h"

#include "../Utils/Logger.h"

void DebugUserInterface::Initialize(const HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext) const
{
	Logger::Debug("Preparing to initialize ImGui...");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, deviceContext);
	ImGui::StyleColorsDark();
}

void DebugUserInterface::BeforeUpdate() const
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void DebugUserInterface::Update(const IOData& ioData, std::vector<SystemHolder>& systems)
{
	BeforeUpdate();

	HandleKeyboardEvents(ioData);
	if (!IsEnabled)
	{
		AfterUpdate();
		return;
	}

	DrawMenuBar();
	DrawSystemBrowser(systems);

	AfterUpdate();
}

void DebugUserInterface::AfterUpdate() const
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DebugUserInterface::DrawMenuBar() const
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Restart"))
			{
				// TODO: Implement logic
			}
			if (ImGui::MenuItem("Exit"))
			{
				// TODO: Implement logic
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

DebugUserInterface::~DebugUserInterface()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void DebugUserInterface::HandleKeyboardEvents(const IOData& ioData)
{
	if (ioData.KeyboardTracker.released.OemTilde)
		IsEnabled = !IsEnabled;
}

void DebugUserInterface::DrawSystemBrowser(std::vector<SystemHolder>& systems) const
{
	ImGui::Begin("Systems browser");
	for (auto& system : systems)
	{
		ImGui::Checkbox(system.Name.c_str(), &system.Enabled);
	}
	ImGui::End();
}
