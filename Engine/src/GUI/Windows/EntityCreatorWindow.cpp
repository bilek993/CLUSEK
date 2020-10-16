#include "EntityCreatorWindow.h"
#include <imgui.h>
#include "../../Tags.h"
#include "../../Utils/CameraLocator.h"
#include "../../Utils/ImGuiExtensions.h"
#include "../../Utils/RayUtil.h"
#include <Windows.h>

void EntityCreatorWindow::Draw()
{
	ImGui::Begin("Entity creator", &IsEnabled);

	DrawCore();
	ImGui::Separator();
	DrawSettings();

	if (Mode == Single && UpdateTransformComponentOnClick)
		HandleClicking();
	
	ImGui::End();
}

void EntityCreatorWindow::DrawCore()
{
	ImGui::Text(Mode == Single ? "Entity description:" : "Entities description:");
	
	ImGui::InputTextMultiline(	"##EntityJsonText", 
								const_cast<char*>(EntityJsonText.c_str()),
								EntityJsonText.capacity() + 1,
								ImVec2(-FLT_MIN, 350.0f),
								ImGuiInputTextFlags_CallbackResize,
								ImGuiExtensions::InputTextStringCallback,
								&EntityJsonText);
	
	if (ImGui::Button("Create"))
	{
		switch (Mode)
		{
		case Single:
			CreateEntityWithComponents();
			break;
		case Array:
			CreateEntitiesWithComponents();
			break;
		default:
			Logger::Error("Incorrect creator mode!");
		}
	}

	if (ImGui::Button("Copy"))
		CopyToClipboard();
}

void EntityCreatorWindow::DrawSettings()
{
	ImGui::Text("Mode:");
	ImGui::RadioButton("Single entity", reinterpret_cast<int*>(&Mode), Single);
	ImGui::RadioButton("Array of entities", reinterpret_cast<int*>(&Mode), Array);

	ImGui::Text("Other settings:");
	
	ImGui::Checkbox("Clear after creating", &ClearAfterCreating);

	if (Mode == Single)
	{
		ImGui::Checkbox("Update transform component on click (it will mess up JSON formatting)", &UpdateTransformComponentOnClick);

		if (UpdateTransformComponentOnClick)
		{
			ImGui::Checkbox("Random rotation on X axis", &RandomRotationX);
			ImGui::Checkbox("Random rotation on Y axis", &RandomRotationY);
			ImGui::Checkbox("Random rotation on Z axis", &RandomRotationZ);
		}
	}
}

void EntityCreatorWindow::CopyToClipboard() const
{
	ImGui::LogToClipboard();
	ImGui::LogText(EntityJsonText.c_str());
	ImGui::LogFinish();
}

void EntityCreatorWindow::HandleClicking()
{
	if (IoData->MouseTracker.leftButton != DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED)
		return;

	if (IoData->MouseState.positionMode == DirectX::Mouse::MODE_RELATIVE)
	{
		Logger::Debug("Skipping handling mouse for UI due to relative mouse mode!");
		return;
	}

	if (EntityJsonText.empty())
	{
		Logger::Warning("No data provided for parsing!");
		return;
	}
	
	auto jsonObject = nlohmann::json::parse(EntityJsonText);
	if (jsonObject.is_null())
	{
		Logger::Warning("Empty object has been passed!");
		return;
	}

	auto& jsonComponents = jsonObject["Components"];
	if (jsonComponents.is_null())
	{
		Logger::Warning("Components are not presented!");
		return;
	}

	auto& jsonTransformComponent = jsonComponents["TransformComponent"];
	if (jsonComponents.is_null())
	{
		Logger::Warning("Transform component is not presented!");
		return;
	}

	const auto hit = RayCastPhysics(100000.0f);
	if (!hit.hasBlock)
	{
		Logger::Debug("Ray didn't hit any object.");
		return;
	}
	
	jsonTransformComponent["PositionX"] = hit.block.position.x;
	jsonTransformComponent["PositionY"] = hit.block.position.y;
	jsonTransformComponent["PositionZ"] = hit.block.position.z;

	if (RandomRotationX)
		jsonTransformComponent["RotationX"] = RotationRandomDistribution(RandomGeneratorEngine);
	if (RandomRotationY)
		jsonTransformComponent["RotationY"] = RotationRandomDistribution(RandomGeneratorEngine);
	if (RandomRotationZ)
		jsonTransformComponent["RotationZ"] = RotationRandomDistribution(RandomGeneratorEngine);

	EntityJsonText = jsonObject.dump();
}

physx::PxRaycastBuffer EntityCreatorWindow::RayCastPhysics(const float maxDistance) const
{
	auto& mainCameraComponent = CameraLocator::GetMainCamera(Registry);
	
	DirectX::XMVECTOR rayOriginVector{};
	DirectX::XMVECTOR rayDirectionVector{};
	
	RayUtil::MousePositionToRayOriginAndDirection(	IoData->MouseState.x,
													IoData->MouseState.y,
													Config->WindowWidth,
													Config->WindowHeight,
													mainCameraComponent.ViewMatrix,
													mainCameraComponent.ProjectionMatrix,
													rayOriginVector,
													rayDirectionVector);

	DirectX::XMFLOAT3 rayOrigin{};
	DirectX::XMFLOAT3 rayDirection{};
	
	XMStoreFloat3(&rayOrigin, rayOriginVector);
	XMStoreFloat3(&rayDirection, rayDirectionVector);

	const auto origin = physx::PxVec3(rayOrigin.x, rayOrigin.y, rayOrigin.z);
	const auto unitDir = physx::PxVec3(rayDirection.x, rayDirection.y, rayDirection.z);
	physx::PxRaycastBuffer hit;
	
	const auto physicsSystem = dynamic_cast<PhysicsSystem*>((*Systems)[PhysicsSystemId].System.get());
	
	const auto scene = physicsSystem->GetPointerToScene();
	scene->raycast(origin, unitDir, maxDistance, hit);

	return hit;
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

	if (ClearAfterCreating)
	{
		Logger::Debug("Clearing entity text field...");
		EntityJsonText = "";
	}
}

void EntityCreatorWindow::CreateEntitiesWithComponents()
{
	Logger::Debug("Preparing to create entities...");
	auto jsonObject = nlohmann::json::parse(EntityJsonText);

	for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it)
	{
		Logger::Debug("Creating entity...");
		const auto entity = Registry->create();

		AddTags(it.value()["Tags"], entity);
		AddComponents(it.value()["Components"], entity);
	}

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
		MAP_COMPONENT_LOADERS(it, RigidbodyStaticConvexComponent, *Registry, entity);
		MAP_COMPONENT_LOADERS(it, RigidbodyDynamicConvexComponent, *Registry, entity);
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
