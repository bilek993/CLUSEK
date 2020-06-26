#include "EntityViewerWindow.h"
#include "../../ECS/Components/CameraComponent.h"
#include "../../ECS/Components/ModelRenderComponent.h"
#include "../../ECS/Components/PhysicsMaterialComponent.h"
#include "../../ECS/Components/RigidbodyDynamicBoxComponent.h"
#include "../../ECS/Components/RigidbodyDynamicCapsuleComponent.h"
#include "../../ECS/Components/RigidbodyDynamicSphereComponent.h"
#include "../../ECS/Components/RigidbodyStaticBoxComponent.h"
#include "../../ECS/Components/RigidbodyStaticCapsuleComponent.h"
#include "../../ECS/Components/RigidbodyStaticPlaneComponent.h"
#include "../../ECS/Components/RigidbodyStaticSphereComponent.h"
#include "../../ECS/Components/SkyboxComponent.h"
#include "../../ECS/Components/TransformComponent.h"
#include "../../ECS/Components/VehicleComponent.h"
#include "../../ECS/Components/VehiclePlayerControllerComponent.h"
#include "../../ECS/Components/TerrainComponent.h"
#include "../../ECS/Components/RigidbodyStaticHeightFieldsComponent.h"
#include "../../ECS/Components/CameraTargetComponent.h"
#include "../../ECS/Components/CameraVehicleAnimatedTargetComponent.h"
#include "../../ECS/Components/RigidbodyDynamicCylinderComponent.h"
#include "../../ECS/Components/RigidbodyStaticCylinderComponent.h"
#include "../../ECS/Components/CommentComponent.h"

void EntityViewerWindow::Draw()
{
	ImGui::Begin("Entity Viewer", &IsEnabled);

	ImGui::Text("Entities list:");
	ImGui::Separator();

	DrawList();

	ImGui::End();
}

void EntityViewerWindow::DrawList()
{
	auto counter = 0;

	Registry->each([this, &counter](entt::entity entity)
	{
		ImGui::BulletText("Entity %i", counter++);
		ImGui::SameLine();
		if (ImGui::SmallButton(("Select##" + std::to_string(counter)).c_str()))
		{
			*SelectedEntity = entity;
			*EntitySelected = true;
		}

		DrawComponent<CameraComponent>(entity, "Camera Component");
		DrawComponent<CameraTargetComponent>(entity, "Camera Target Component");
		DrawComponent<CameraVehicleAnimatedTargetComponent>(entity, "Camera Vehicle Animated Target Component");
		DrawComponent<ModelRenderComponent>(entity, "Model Render Component");
		DrawComponent<PhysicsMaterialComponent>(entity, "Physics Material Component");
		DrawComponent<RigidbodyDynamicBoxComponent>(entity, "Rigidbody Dynamic Box Component");
		DrawComponent<RigidbodyDynamicCapsuleComponent>(entity, "Rigidbody Dynamic Capsule Component");
		DrawComponent<RigidbodyDynamicCylinderComponent>(entity, "Rigidbody Dynamic Cylinder Component");
		DrawComponent<RigidbodyDynamicSphereComponent>(entity, "Rigidbody Dynamic Sphere Component");
		DrawComponent<RigidbodyStaticBoxComponent>(entity, "Rigidbody Static Box Component");
		DrawComponent<RigidbodyStaticCapsuleComponent>(entity, "Rigidbody Static Capsule Component");
		DrawComponent<RigidbodyStaticCylinderComponent>(entity, "Rigidbody Static Cylinder Component");
		DrawComponent<RigidbodyStaticHeightFieldsComponent>(entity, "Rigidbody Static Height Fields Component");
		DrawComponent<RigidbodyStaticPlaneComponent>(entity, "Rigidbody Static Plane Component");
		DrawComponent<RigidbodyStaticSphereComponent>(entity, "Rigidbody Static Sphere Component");
		DrawComponent<SkyboxComponent>(entity, "Skybox Component");
		DrawComponent<TerrainComponent>(entity, "Terrain Component");
		DrawComponent<TransformComponent>(entity, "Transform Component");
		DrawComponent<VehicleComponent>(entity, "Vehicle Component");
		DrawComponent<VehiclePlayerControllerComponent>(entity, "Vehicle Player Controller Component");
		DrawComponent<WheelComponent>(entity, "Wheel Component");
		DrawComponent<CommentComponent>(entity, "Comment Component");
	});
}
