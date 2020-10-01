#include "EntityCreatorWindow.h"
#include <imgui.h>
#include "../../Tags.h"
#include "../../ECS/Components/TransformComponent.h"
#include "../../Utils/ImGuiExtensions.h"

void EntityCreatorWindow::Draw()
{
	ImGui::Begin("Entity creator", &IsEnabled);

	ImGui::Text(Mode == 0 ? "Entity description:" : "Entities description:");
	
	ImGui::InputTextMultiline(	"##EntityJsonText", 
								const_cast<char*>(EntityJsonText.c_str()),
								EntityJsonText.capacity() + 1,
								ImVec2(-FLT_MIN, 350.0f),
								ImGuiInputTextFlags_CallbackResize,
								ImGuiExtensions::InputTextStringCallback,
								&EntityJsonText);
	
	if (ImGui::Button("Create"))
		CreateEntityWithComponents();

	ImGui::Separator();

	ImGui::Text("Mode:");
	ImGui::RadioButton("Single entity", &Mode, 0);
	ImGui::RadioButton("Array of entities", &Mode, 1);
	
	ImGui::End();
}

void EntityCreatorWindow::CreateEntityWithComponents()
{
	Logger::Debug("Creating entity...");
	const auto entity = Registry->create();

	auto jsonObject = nlohmann::json::parse(EntityJsonText);
	
	AddTags(jsonObject["Tags"], entity);
	AddComponents(jsonObject["Components"], entity);

	Logger::Debug("Rebuilding systems...");
	RebuildEntities();

	Logger::Debug("Clearing entity text field...");
	EntityJsonText = "";
}

void EntityCreatorWindow::AddTags(nlohmann::json& json, const entt::registry::entity_type &entity) const
{
	const auto tags = json.get<std::vector<std::string>>();
	Registry->assign<entt::tag<Tags::REQUIRES_REBUILD>>(entity);

	for (auto& tag : tags)
	{
		MAP_LOADER_TAGS(tag.c_str(), "Main Camera", Tags::MAIN_CAMERA, (*Registry), entity);
	}
}

void EntityCreatorWindow::AddComponents(nlohmann::json& json, const entt::registry::entity_type &entity) const
{
	for (auto it = json.begin(); it != json.end(); ++it)
	{
		MAP_COMPONENT_LOADERS(it, CameraComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, CameraTargetComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, CameraVehicleAnimatedTargetComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, TransformComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, ModelRenderComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, SkyboxComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, PhysicsMaterialComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticPlaneComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticBoxComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyDynamicBoxComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticSphereComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyDynamicSphereComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticCapsuleComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyDynamicCapsuleComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticCylinderComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyDynamicCylinderComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticHeightFieldsComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, VehicleComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, WheelComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, VehiclePlayerControllerComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, TerrainComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, CommentComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, GrassComponent, *Registry, entity);
	}
}

void EntityCreatorWindow::RebuildEntities() const
{
	for (auto &system : *Systems)
		system.System->Rebuild();
}
