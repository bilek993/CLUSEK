#include "DebugUserInterface.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include "../Utils/Logger.h"
#include "Windows/SystemsManagerWindow.h"
#include "../Renderer/PostProcessingSettings.h"
#include "../Utils/RayUtil.h"
#include "../Utils/CameraLocator.h"

void DebugUserInterface::Initialize(const HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ConfigData *configData,
	const std::function<void()> &functionCloseEngine)
{
	Logger::Debug("Preparing to initialize ImGui...");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, deviceContext);

	SetupStyle(configData);

	FunctionCloseEngine = functionCloseEngine;

	IsEnabled = configData->EnableImGuiOnStart == 1;
}

void DebugUserInterface::BeforeUpdate() const
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

void DebugUserInterface::Update(const float deltaTime, ConfigData *configData, IOData *ioData, 
	std::vector<SystemHolder> *systems, const int renderSystemId, int physicsSystemId, 
	DynamicRenderSettings *dynamicRenderSettings, PostProcessingSettings *postProcessingSettings, entt::registry *registry)
{
	const auto renderSystem = dynamic_cast<RenderSystem*>((*systems)[renderSystemId].System.get());

	BeforeUpdate();

	HandleKeyboardEvents(ioData);
	if (!IsEnabled)
	{
		AfterUpdate(renderSystem);
		return;
	}

	HandleMainDockingArea();

	HandleClickingOnObjects(ioData, registry, configData);

	UPDATE_USER_INTERFACE(ConfigurationWindowInstance);
	UPDATE_USER_INTERFACE(CameraSpeedWindowInstance);
	UPDATE_USER_INTERFACE(SystemsManagerWindowInstance);
	UPDATE_USER_INTERFACE(FpsTimerWindowInstance);
	UPDATE_USER_INTERFACE(LightingWindowInstance);
	UPDATE_USER_INTERFACE(BackBufferWindowInstance);
	UPDATE_USER_INTERFACE(MouseInputWindowInstance);
	UPDATE_USER_INTERFACE(KeyboardInputWindowInstance);
	UPDATE_USER_INTERFACE(GamePadInputWindowInstance);
	UPDATE_USER_INTERFACE(PostProcessingWindowInstance);
	UPDATE_USER_INTERFACE(PhysicsStatisticsWindowInstance);
	UPDATE_USER_INTERFACE(EntityCreatorWindowInstance);
	UPDATE_USER_INTERFACE(EntityViewerWindowInstance);
	UPDATE_USER_INTERFACE(PerSystemPerformanceWindowInstance);
	UPDATE_USER_INTERFACE(ShadowPreviewerWindowInstance);
	UPDATE_USER_INTERFACE(FogWindowInstance);
	UPDATE_USER_INTERFACE(TextureCompressionWindowInstance);
	UPDATE_USER_INTERFACE(LodDebugWindowInstance);
	UPDATE_USER_INTERFACE(EntityEditorWindowInstance);
	UPDATE_USER_INTERFACE(PhysicsMaterialsWindowInstance);
	UPDATE_USER_INTERFACE(GrassWindowInstance);
	UPDATE_USER_INTERFACE(EntitySelectorLockWindowInstance);

	AfterUpdate(renderSystem);
}

void DebugUserInterface::AfterUpdate(RenderSystem *renderSystem) const
{
	renderSystem->PrepareForGuiRender();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DebugUserInterface::DrawMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			DrawMenuItemForWindow("Edit configuration", ConfigurationWindowInstance);
			DrawMenuItemForFunction("Exit", [this]
			{
				FunctionCloseEngine();
			});
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Camera"))
		{
			DrawMenuItemForWindow("Camera speed", CameraSpeedWindowInstance);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("ECS"))
		{
			DrawMenuItemForWindow("Entity selector lock", EntitySelectorLockWindowInstance);
			DrawMenuItemForWindow("Entity creator", EntityCreatorWindowInstance);
			DrawMenuItemForWindow("Entity viewer", EntityViewerWindowInstance);
			DrawMenuItemForWindow("Entity editor", EntityEditorWindowInstance);
			DrawMenuItemForWindow("Systems manager", SystemsManagerWindowInstance);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Renderer"))
		{
			DrawMenuItemForWindow("Post Processing settings", PostProcessingWindowInstance);
			DrawMenuItemForWindow("Lighting settings", LightingWindowInstance);
			DrawMenuItemForWindow("Fog settings", FogWindowInstance);
			DrawMenuItemForWindow("Grass settings", GrassWindowInstance);
			DrawMenuItemForWindow("Back Buffer setting", BackBufferWindowInstance);
			DrawMenuItemForWindow("Shadow Previewer", ShadowPreviewerWindowInstance);
			DrawMenuItemForWindow("LOD Debug", LodDebugWindowInstance);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Physics"))
		{
			DrawMenuItemForWindow("Physics statistics", PhysicsStatisticsWindowInstance);
			DrawMenuItemForWindow("Physics materials", PhysicsMaterialsWindowInstance);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("I/O Devices"))
		{
			DrawMenuItemForWindow("Mouse input", MouseInputWindowInstance);
			DrawMenuItemForWindow("Keyboard input", KeyboardInputWindowInstance);
			DrawMenuItemForWindow("GamePad input", GamePadInputWindowInstance);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			DrawMenuItemForWindow("Texture compression", TextureCompressionWindowInstance);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Performance"))
		{
			DrawMenuItemForWindow("FPS Timer", FpsTimerWindowInstance);
			DrawMenuItemForWindow("Per System Performance Window", PerSystemPerformanceWindowInstance);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void DebugUserInterface::DrawMenuItemForWindow(const std::string& label, BaseWindow& window) const
{
	const auto isWindowEnabled = window.GetIsEnabled();

	if (ImGui::MenuItem(label.c_str(), nullptr, isWindowEnabled))
		window.SetIsEnabled(!isWindowEnabled);
}

void DebugUserInterface::DrawMenuItemForFunction(const std::string& label, const std::function<void()>& function) const
{
	if (ImGui::MenuItem(label.c_str()))
		function();
}

void DebugUserInterface::HandleClickingOnObjects(IOData* ioData, entt::registry* registry, ConfigData *configData)
{
	if (LockSelecting)
		return;
	
	if (ioData->MouseTracker.leftButton != DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED)
		return;

	if (ioData->MouseState.positionMode == DirectX::Mouse::MODE_RELATIVE)
	{
		Logger::Debug("Skipping handling mouse for UI due to relative mouse mode!");
		return;
	}

	auto& mainCameraComponent = CameraLocator::GetMainCamera(registry);

	DirectX::XMVECTOR rayOrigin{};
	DirectX::XMVECTOR rayDirection{};

	RayUtil::MousePositionToRayOriginAndDirection(	ioData->MouseState.x,
													ioData->MouseState.y,
													configData->WindowWidth,
													configData->WindowHeight,
													mainCameraComponent.ViewMatrix,
													mainCameraComponent.ProjectionMatrix,
													rayOrigin,
													rayDirection);

	auto selectedObjectDistance = std::numeric_limits<float>::max();

	registry->each([this, registry, configData, &rayOrigin, &rayDirection, &selectedObjectDistance](const entt::entity entity)
	{
		if (!registry->has<TransformComponent, ModelRenderComponent>(entity))
			return;

		const auto& transformComponent = registry->get<TransformComponent>(entity);
		const auto& modelRenderComponent = registry->get<ModelRenderComponent>(entity);

		auto currentObjectDistance = 0.0f;

		for (const auto& mesh : *modelRenderComponent.Meshes)
		{
			if (RayUtil::TestObb(	rayOrigin,
									rayDirection,
									mesh.FrustumPoints,
									transformComponent.WorldMatrix,
									configData->MainCameraFarZ - configData->MainCameraNearZ,
									&currentObjectDistance))
			{
				if (currentObjectDistance < selectedObjectDistance)
				{
					selectedObjectDistance = currentObjectDistance;
					SelectedEntity = entity;
					EntitySelected = true;
				}
			}
		}
	});
}

DebugUserInterface::~DebugUserInterface()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void DebugUserInterface::SetupStyle(const ConfigData *configData) const
{
	Logger::Debug("Preparing to change ImGui style...");

	ImGui::StyleColorsDark();
	auto& style = ImGui::GetStyle();

	style.ChildRounding = 3.0f;
	style.WindowRounding = 0.0f;
	style.GrabRounding = 0.0f;
	style.PopupRounding = 0.0f;
	style.FrameRounding = 3.0f;
	style.ScrollbarRounding = 2.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

	style.Colors[ImGuiCol_Separator]			= ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
	style.Colors[ImGuiCol_SeparatorHovered]		= ImVec4(0.75f, 0.75f, 0.75f, 0.78f);
	style.Colors[ImGuiCol_SeparatorActive]		= ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
	style.Colors[ImGuiCol_Tab]					= ImVec4(0.35f, 0.35f, 0.35f, 0.86f);
	style.Colors[ImGuiCol_TabHovered]			= ImVec4(0.78f, 0.78f, 0.78f, 0.80f);
	style.Colors[ImGuiCol_TabActive]			= ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused]			= ImVec4(0.15f, 0.15f, 0.15f, 0.97f);
	style.Colors[ImGuiCol_TabUnfocusedActive]	= ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
	style.Colors[ImGuiCol_Text]					= ImVec4(0.8f, 0.8f, 0.8f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled]			= ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg]				= ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
	style.Colors[ImGuiCol_PopupBg]				= ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_Border]				= ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_BorderShadow]			= ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_FrameBg]				= ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered]		= ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive]		= ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_TitleBg]				= ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed]		= ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive]		= ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg]			= ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg]			= ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab]		= ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered]	= ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive]	= ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_CheckMark]			= ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab]			= ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive]		= ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
	style.Colors[ImGuiCol_Button]				= ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered]		= ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive]			= ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	style.Colors[ImGuiCol_Header]				= ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered]		= ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive]			= ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip]			= ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
	style.Colors[ImGuiCol_ResizeGripHovered]	= ImVec4(1.00f, 1.00f, 1.00f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive]		= ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
	style.Colors[ImGuiCol_PlotLines]			= ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered]		= ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram]		= ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered]	= ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg]		= ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ModalWindowDarkening]	= ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
	style.Colors[ImGuiCol_DockingPreview]		= ImVec4(0.26f, 0.26f, 0.26f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight]			= ImVec4(0.82f, 0.82f, 0.82f, 1.00f);

	auto& io = ImGui::GetIO();
	if (!configData->PathToImGuiFont.empty())
		io.Fonts->AddFontFromFileTTF(configData->PathToImGuiFont.c_str(), 14.0f);

	Logger::Debug("ImGui style changed!");
}

void DebugUserInterface::HandleKeyboardEvents(const IOData *ioData)
{
	if (ioData->KeyboardTracker.released.OemTilde)
		IsEnabled = !IsEnabled;
}

void DebugUserInterface::HandleMainDockingArea()
{
	const auto windowFlags =	ImGuiWindowFlags_MenuBar | 
								ImGuiWindowFlags_NoDocking | 
								ImGuiWindowFlags_NoTitleBar | 
								ImGuiWindowFlags_NoCollapse | 
								ImGuiWindowFlags_NoResize | 
								ImGuiWindowFlags_NoMove | 
								ImGuiWindowFlags_NoBringToFrontOnFocus | 
								ImGuiWindowFlags_NoNavFocus |
								ImGuiWindowFlags_NoBackground;

	const auto dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode | 
								ImGuiDockNodeFlags_NoDockingInCentralNode;

	const auto viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin(DOCKSPACE_AREA_NAME.c_str(), &IsDockingEnabled, windowFlags);
	ImGui::PopStyleVar(3);

	DrawMenuBar();

	const auto mainDockspaceId = ImGui::GetID(DOCKSPACE_AREA_NAME.c_str());
	ImGui::DockSpace(mainDockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

	ImGui::End();
}