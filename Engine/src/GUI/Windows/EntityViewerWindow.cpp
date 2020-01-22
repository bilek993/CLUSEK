#include "EntityViewerWindow.h"
#include "../../ECS/Components/CameraComponent.h"
#include "../../ECS/Components/ModelRenderComponent.h"
#include "../../ECS/Components/PhysicsMaterialComponent.h"
#include "../../ECS/Components/RigidbodyDynamicBoxComponent.h"
#include "../../ECS/Components/RigidbodyDynamicCapsuleComponent.h"
#include "../../ECS/Components/RigidbodyDynamicSphereComponent.h"
#include "../../ECS/Components/RigidbodyStaticBoxComponent.h"
#include "../../ECS/Components/RigidbodyStaticCapsuleComponent.h"
#include "../../Loaders/Components/RigidbodyStaticCylinderComponent.h"
#include "../../ECS/Components/RigidbodyStaticPlaneComponent.h"
#include "../../ECS/Components/RigidbodyStaticSphereComponent.h"
#include "../../ECS/Components/SkyboxComponent.h"
#include "../../ECS/Components/TransformComponent.h"
#include "../../ECS/Components/VehicleComponent.h"
#include "../../ECS/Components/VehiclePlayerControllerComponent.h"
#include "../../ECS/Components/TerrainComponent.h"

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

	Registry->each([this, &counter](auto entity)
	{
		ImGui::BulletText("Entity %i", counter++);

		DrawComponent<CameraComponent>(entity, "Camera Component");
		DrawComponent<ModelRenderComponent>(entity, "Model Render Component");
		DrawComponent<PhysicsMaterialComponent>(entity, "Physics Material Component");
		DrawComponent<RigidbodyDynamicBoxComponent>(entity, "Rigidbody Dynamic Box Component");
		DrawComponent<RigidbodyDynamicCapsuleComponent>(entity, "Rigidbody Dynamic Capsule Component");
		DrawComponent<RigidbodyDynamicSphereComponent>(entity, "Rigidbody Dynamic Sphere Component");
		DrawComponent<RigidbodyStaticBoxComponent>(entity, "Rigidbody Static Box Component");
		DrawComponent<RigidbodyStaticCapsuleComponent>(entity, "Rigidbody Static Capsule Component");
		DrawComponent<RigidbodyStaticCylinderComponent>(entity, "Rigidbody Static Cylinder Component");
		DrawComponent<RigidbodyStaticPlaneComponent>(entity, "Rigidbody Static Plane Component");
		DrawComponent<RigidbodyStaticSphereComponent>(entity, "Rigidbody Static Sphere Component");
		DrawComponent<SkyboxComponent>(entity, "Skybox Component");
		DrawComponent<TransformComponent>(entity, "Transform Component");
		DrawComponent<VehicleComponent>(entity, "Vehicle Component");
		DrawComponent<VehiclePlayerControllerComponent>(entity, "Vehicle Player Controller Component");
		DrawComponent<WheelComponent>(entity, "Wheel Component");
		DrawComponent<TerrainComponent>(entity, "Terrain Component");
	});
}
